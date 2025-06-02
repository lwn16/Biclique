#include "random.h"
#include "coloring.h"

#include <numeric>

void BiGraphWithColor::ClearColor() {
    left_color_.clear();
    right_color_.clear();
}

void BiGraphWithColor::SwapLeftAndRight() {
    BiGraph::SwapLeftAndRight();
    std::swap(left_color_, right_color_);
}

static auto CalcColor(int left_n,
                      int right_n,
                      int q,
                      const std::vector<std::vector<int>> &left_neighbour,int color_mode=1) -> std::vector<int> {
    std::vector<int> result(left_n, -1), remain_id(left_n);
    std::iota(remain_id.begin(), remain_id.end(), 0);
    if (color_mode==2){
        std::sort(remain_id.begin(), remain_id.end(),
            [&](int x,int y){
                return left_neighbour[x].size() < left_neighbour[y].size();
            }
        );
    }
    else if (color_mode==3){
        std::sort(remain_id.begin(), remain_id.end(),
            [&](int x,int y){
                return left_neighbour[x].size() > left_neighbour[y].size();
            }
        );
    }
    for (int col = 0; !remain_id.empty(); col++) {
        std::vector<int> rest_cnt(left_n, q - 1);
        std::vector<int> right_status(right_n, -1); // -1: no neighbour, -2: black, otherwise: the only black neighbour
        std::vector<int> nxt_remain_id;

        for (auto l : remain_id) {
            bool tag = true;
            int revert_r = -1;
            for (auto r : left_neighbour[l]) {
                if (right_status[r] == -1) {
                    continue;
                }
                rest_cnt[l]--;
                if (right_status[r] >= 0) {
                    if (rest_cnt[right_status[r]]-- == 0) {
                        tag = false;
                    }
                }
                if (rest_cnt[l] < 0 || !tag) {
                    revert_r = r;
                    break;
                }
            }

            if (rest_cnt[l] < 0 || !tag) {
                for (auto r : left_neighbour[l]) {
                    if (right_status[r] >= 0) {
                        rest_cnt[right_status[r]]++;
                    }
                    if (r == revert_r) {
                        break;
                    }
                }
                nxt_remain_id.push_back(l);
                continue;
            }

            result[l] = col;
            for (auto r : left_neighbour[l]) {
                if (right_status[r] == -2) {
                    continue;
                }
                if (right_status[r] == -1) {
                    right_status[r] = l;
                    continue;
                }
                right_status[r] = -2;
            }
        }

        if (color_mode<=1) Shuffle(nxt_remain_id);
        remain_id = nxt_remain_id;
    }

    return result;
}

auto BiGraphWithColor::SetColor(int p, int q,int color_mode) -> std::pair<int, int> {
    left_color_ = CalcColor(left_n_, right_n_, q, left_neighbour_,color_mode);
    right_color_ = CalcColor(right_n_, left_n_, p, right_neighbour_,color_mode);

    return {*std::max_element(left_color_.begin(), left_color_.end()) + 1,
            *std::max_element(right_color_.begin(), right_color_.end()) + 1};
}

auto BiGraphWithColor::SetDefaultColor(int p, int q) -> std::pair<int, int> {
    left_color_.resize(left_n_);
    right_color_.resize(right_n_);
    std::iota(left_color_.begin(), left_color_.end(), 0);
    std::iota(right_color_.begin(), right_color_.end(), 0);

    return {*std::max_element(left_color_.begin(), left_color_.end()) + 1,
            *std::max_element(right_color_.begin(), right_color_.end()) + 1};
}


void BiGraphWithColor::ShuffleColor() {
    if (left_color_.empty() || right_color_.empty()) {
        throw "The graph has not been colored";
    }

    std::vector<int> permutation(*std::max_element(left_color_.begin(), left_color_.end()) + 1);
    std::iota(permutation.begin(), permutation.end(), 0);
    Shuffle(permutation);
    for (auto &i : left_color_) i = permutation[i];

    permutation.resize(*std::max_element(right_color_.begin(), right_color_.end()) + 1);
    std::iota(permutation.begin(), permutation.end(), 0);
    Shuffle(permutation);
    for (auto &i : right_color_) i = permutation[i];
}