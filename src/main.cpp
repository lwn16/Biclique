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

    // g.TrimButterfly(p, q, 0);

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
    std::cerr << left_degree2 << ' ' << right_degree2 << std::endl;

    // class CountingIndexCmp {
    // public:
    //     auto operator()(const std::pair<CountingIndex, BiGraph>& a, const std::pair<CountingIndex, BiGraph> &b) -> bool {
    //         return a.first.total_dp_ < b.first.total_dp_;
    //     }
    // };
    // std::priority_queue<std::pair<CountingIndex, BiGraph>, std::vector<std::pair<CountingIndex, BiGraph>>, CountingIndexCmp> graph_queue;
    std::vector<CountingIndex> split_graphs;
    // dp_t total_sample_weight = 0, certain_sample_weight = 0;
    std::vector<std::pair<int,int>> kappa_list;
    if (split){
        auto first_split_graphs = SplitLeft(g, p, q);
        // std::vector<dp_t> dp_t_list;
        for (auto &g_i_wocolor : first_split_graphs) {
            BiGraphWithColor g_i(std::move(g_i_wocolor));
            auto res = color ? g_i.SetColor(p - 1, q) : g_i.SetDefaultColor(p - 1, q);
            g_i.ShuffleColor();
            CountingIndex index(BiGraphWithColor(g_i), p - 1, q);
            // total_sample_weight += index.total_dp_;
            // graph_queue.emplace(std::move(index), g_i);
            split_graphs.push_back(index);
            // dp_t_list.push_back(index.total_dp_);
            kappa_list.emplace_back(res);
        }
    }
    else{
        {
            BiGraphWithColor g_i(std::move(g));
            auto res = color ? g_i.SetColor(p, q) : g_i.SetDefaultColor(p, q);
            g_i.ShuffleColor();
            CountingIndex index(BiGraphWithColor(g_i), p, q);
            split_graphs.push_back(index);
            kappa_list.emplace_back(res);
        }
    }

    // std::sort(dp_t_list.begin(),dp_t_list.end());
    // double lambda = 0.9;
    // int lambda_pos = std::max(std::min((int)(dp_t_list.size()*lambda),(int)dp_t_list.size()-1),0);
    // double lambda_bound = lambda_pos < dp_t_list.size() ? dp_t_list[lambda_pos] : 1e18;
    // // std::cerr << "lambda : " << lambda_pos << " " << lambda_bound << std::endl;

    // {
    //     std::vector<CountingIndex> tmp;
    //     for (int i = 0; i < (int)split_graphs.size(); i++){
    //         if (split_graphs[i].total_dp_ > lambda_bound){
    //             split_graphs[i].graph_.TrimButterfly(p - 1, q , 0);
    //             if (split_graphs[i].graph_.edge_n_==0) continue;
    //             split_graphs[i].graph_.SetColor(p - 1, q);
    //             split_graphs[i].graph_.ShuffleColor();
    //             CountingIndex index(std::move(split_graphs[i].graph_), p - 1, q);
    //             tmp.push_back(std::move(index));
    //         }
    //         else{
    //             tmp.push_back(std::move(split_graphs[i]));
    //         }
    //     }
    //     std::swap(tmp,split_graphs);
    // }

    // while (!graph_queue.empty() && certain_sample_weight * 2 < total_sample_weight) {
    //     auto index = graph_queue.top().first;
    //     auto graph = graph_queue.top().second;
    //     auto p = index.p_;
    //     auto q = index.q_;
    //     auto total_dp = index.total_dp_;
    //     graph_queue.pop();

    //     if (p == 1) {
    //         certain_sample_weight += total_dp;
    //         split_graphs.push_back(std::move(index));
    //         continue;
    //     }
    //     total_sample_weight -= total_dp;

    //     graph.SwapLeftAndRight();
    //     std::swap(p, q);

    //     auto split_results = SplitLeft(graph, p, q);
    //     int sum_edge = 0;
    //     for (auto &i : split_results) {
    //         sum_edge += i.edge_n_;
    //     }

    //     for (auto &g_i_wocolor : split_results) {
    //         BiGraphWithColor g_i(std::move(g_i_wocolor));
    //         auto res = g_i.SetColor(p - 1, q);
    //         g_i.ShuffleColor();
    //         CountingIndex index(BiGraphWithColor(g_i), p - 1, q);
    //         if (index.total_dp_ == 0) {
    //             continue;
    //         }
    //         total_sample_weight += index.total_dp_;
    //         if (sum_edge > graph.edge_n_) {
    //             certain_sample_weight += index.total_dp_;
    //             split_graphs.push_back(std::move(index));
    //         } else {
    //             graph_queue.emplace(std::move(index), std::move(g_i));
    //         }
    //     }

    //     std::cerr << total_dp << ' ' << certain_sample_weight << ' ' << total_sample_weight << std::endl;
    // }

    // while (!graph_queue.empty()) {
    //     split_graphs.push_back(graph_queue.top().first);
    //     graph_queue.pop();
    // }

    // BiGraphWithColor g_color(g);
    // g_color.SetColor(p, q);
    // g_color.ShuffleColor();
    // CountingIndex index(std::move(g_color), p, q);

    std::vector<dp_t> sample_indices;
    dp_t sum = 0;
    for (auto &i : split_graphs) {
        sample_indices.push_back(sum);
        sum += i.total_dp_;
        std::cerr << i.p_ << ' ' << i.q_ << ' ' << i.graph_.left_n_ << ' ' << i.graph_.right_n_ << ' ' << i.total_dp_ << std::endl;
    }
    std::cout << "sum: " << sum << std::endl;

    std::cerr << "num_split:" << (int)kappa_list.size() << std::endl;
    int mxkL=0;
    int mxkR=0;
    int sumkL=0;
    int sumkR=0;
    for (auto [kappa_U,kappa_V]:kappa_list){
        std::cerr << "kL,kR: "<< kappa_U << " " << kappa_V << std::endl;
        mxkL=std::max(mxkL,kappa_U);
        mxkR=std::max(mxkR,kappa_V);
        sumkL+=kappa_U;
        sumkR+=kappa_V;
    }
    std::cerr << (int)kappa_list.size() << std::endl;
    std::cerr << mxkL << " " << mxkR << " " << sumkL << " " << sumkR << std::endl;

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

        std::cout << std::fixed << total / T << std::endl;
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
    std::cout << average << ' ' << average_error * 100 << std::endl;
}