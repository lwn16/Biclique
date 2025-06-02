#include "counting.h"
#include "getArgs.hpp"

#include <iostream>
#include <fstream>
#include <queue>
#include <tuple>
#include <chrono>

#define TIMERSTART(tag)  auto tag##_start = std::chrono::steady_clock::now(),tag##_end = tag##_start
#define TIMEREND(tag)  tag##_end =  std::chrono::steady_clock::now()
#define DURATION_s(tag) printf("%s costs %ld s\n",#tag,std::chrono::duration_cast<std::chrono::seconds>(tag##_end - tag##_start).count())
#define DURATION_ms(tag) printf("%s costs %ld ms\n",#tag,std::chrono::duration_cast<std::chrono::milliseconds>(tag##_end - tag##_start).count());
#define DURATION_us(tag) printf("%s costs %ld us\n",#tag,std::chrono::duration_cast<std::chrono::microseconds>(tag##_end - tag##_start).count());
#define DURATION_ns(tag) printf("%s costs %ld ns\n",#tag,std::chrono::duration_cast<std::chrono::nanoseconds>(tag##_end - tag##_start).count());

int main(int argc, char **argv) {
    argsController * aC = new argsController(argc, argv);

    std::string filePath;
    if (aC->exist("-f")) filePath = aC->get("-f");
    else{
        std::cout << "-f filePath" << std::endl;
        exit(-1);
    }
    std::ifstream fin(filePath);
    int p = 5;
    if (aC->exist("-p")) p = atoi(aC->get("-p").c_str());
    int q = 5;
    if (aC->exist("-q")) q = atoi(aC->get("-q").c_str());
    int test_sets = 10;
    if (aC->exist("-test_sets")) test_sets = atoi(aC->get("-test_sets").c_str());
    int T = 100000;
    if (aC->exist("-T")) T = atoi(aC->get("-T").c_str());
    int split = 1;
    int color = 1;
    if (aC->exist("-color")) color = atoi(aC->get("-color").c_str());

    BiGraph g;
    g.Read(fin);
    g.Trim(p, q);
    TIMERSTART(init);

    uint64_t left_degree2 = 0, right_degree2 = 0;
    for (auto &i : g.left_neighbour_) {
        left_degree2 += static_cast<uint64_t>(i.size()) * i.size();
    }
    for (auto &i : g.right_neighbour_) {
        right_degree2 += static_cast<uint64_t>(i.size()) * i.size();
    }
    if (left_degree2 < right_degree2) {
        std::swap(p, q);
        g.SwapLeftAndRight();
    }
    //std::cerr << left_degree2 << ' ' << right_degree2 << std::endl;

    std::vector<CountingIndex> split_graphs;
    std::vector<std::pair<int,int>> kappa_list;
    if (split){
        auto first_split_graphs = SplitLeft(g, p, q);
        for (auto &g_i_wocolor : first_split_graphs) {
            BiGraphWithColor g_i(std::move(g_i_wocolor));
            auto res = color ? g_i.SetColor(p - 1, q, color) : g_i.SetDefaultColor(p - 1, q);
            g_i.ShuffleColor();
            CountingIndex index(BiGraphWithColor(g_i), p - 1, q);
            split_graphs.push_back(index);
            kappa_list.emplace_back(res);
        }
    }
    else{
        {
            BiGraphWithColor g_i(std::move(g));
            auto res = color ? g_i.SetColor(p, q, color) : g_i.SetDefaultColor(p, q);
            g_i.ShuffleColor();
            CountingIndex index(BiGraphWithColor(g_i), p, q);
            split_graphs.push_back(index);
            kappa_list.emplace_back(res);
        }
    }

    std::vector<dp_t> sample_indices;
    dp_t sum = 0;
    for (auto &i : split_graphs) {
        sample_indices.push_back(sum);
        sum += i.total_dp_;
        //std::cerr << i.p_ << ' ' << i.q_ << ' ' << i.graph_.left_n_ << ' ' << i.graph_.right_n_ << ' ' << i.total_dp_ << std::endl;
    }
    // std::cout << "sum: " << sum << std::endl;

    // std::cerr << "num_split:" << (int)kappa_list.size() << std::endl;
    // int mxkL=0;
    // int mxkR=0;
    // int sumkL=0;
    // int sumkR=0;
    // for (auto [kappa_U,kappa_V]:kappa_list){
    //     std::cerr << "kL,kR: "<< kappa_U << " " << kappa_V << std::endl;
    //     mxkL=std::max(mxkL,kappa_U);
    //     mxkR=std::max(mxkR,kappa_V);
    //     sumkL+=kappa_U;
    //     sumkR+=kappa_V;
    // }
    // std::cerr << (int)kappa_list.size() << std::endl;
    // std::cerr << mxkL << " " << mxkR << " " << sumkL << " " << sumkR << std::endl;

    TIMEREND(init);
    DURATION_ms(init);
    
    std::vector<double> results;

    for (int test_id = 0; test_id < test_sets; test_id++) {
        TIMERSTART(sample);

        
        std::vector<int> left_buffer(g.left_n_);
        std::vector<int> right_buffer(g.right_n_);
        std::vector<int> sample_times(split_graphs.size());

        double total = 0;
        if (!split_graphs.empty()) {
            for (int i = 0; i < T; i++) {
                dp_t target = Random(sum);
                int index = std::prev(std::upper_bound(
                    sample_indices.begin(),
                    sample_indices.end(),
                    target)) - sample_indices.begin();
                sample_times[index]++;
            }
            
            for (int i = 0; i < split_graphs.size(); i++) {
                while (sample_times[i]--) {
                    auto cur = split_graphs[i].CountBySample(left_buffer, right_buffer);
                    total += cur / static_cast<double>(split_graphs[i].total_dp_);
                }
            }
        }
        total *= static_cast<double>(sum);
        
        TIMEREND(sample);
        DURATION_ms(sample);

        std::cout << "Runs " << test_id << ": ";
        std::cout << std::fixed << total / T << std::endl << std::endl;
        results.push_back(total / T);
    }
    double average = 0, average_error = 0;
    for (auto i : results) {
        average += i;
    }
    average /= results.size();
    for (auto i : results) {
        average_error += fabs(i - average) / average;
    }
    average_error /= results.size();
    std::cout << "average count: " << average << std::endl;
    std::cout << "relative error to C_bar: " << average_error * 100 << std::endl;
}