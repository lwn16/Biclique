#include "sampling.h"

auto CountingIndex::CountBySample(std::vector<int> &left_buffer, std::vector<int> &right_buffer) -> double {
    if (total_dp_ == 0) {
        return 0;
    }
    auto target = Random(total_dp_);
    int start_id = prev(std::upper_bound(
        sample_indices_.begin(),
        sample_indices_.end(),
        target)) - sample_indices_.begin();

    int left_cnt = 1;
    int right_cnt = 1;
    std::vector<int> left_ids{graph_.edge_[start_id].first};
    std::vector<int> right_ids{graph_.edge_[start_id].second};
    for (auto v : graph_.left_neighbour_[graph_.edge_[start_id].first]) {
        right_buffer[v]++;
    }
    for (auto u : graph_.right_neighbour_[graph_.edge_[start_id].second]) {
        left_buffer[u]++;
    }

    auto cur_id = start_id;
    double ans = total_dp_;
    for (int i = length_ - 2; i >= 0; i--) {
        auto [cur_u, cur_v] = graph_.edge_[cur_id];
        std::vector<dp_t> sample_list;
        std::vector<int> nxt_id;
        dp_t tmp = 0;
        if (trans_type_list_[i] == TransType::left) {
            for (int j = 0; j < graph_.left_neighbour_[cur_u].size(); j++) {
                auto v = graph_.left_neighbour_[cur_u][j];
                if (v >= cur_v || graph_.right_color_[v] == graph_.right_color_[cur_v]) {
                    break;
                }
                if (right_buffer[v] != left_cnt) {
                    continue;
                }
                nxt_id.push_back(left_id_[cur_u][j]);
                sample_list.push_back(tmp);
                tmp += dp_cnt_[i][left_id_[cur_u][j]];
            }
        } else {
            for (int j = 0; j < graph_.right_neighbour_[cur_v].size(); j++) {
                auto u = graph_.right_neighbour_[cur_v][j];
                if (u >= cur_u || graph_.left_color_[u] == graph_.left_color_[cur_u]) {
                    break;
                }
                if (left_buffer[u] != right_cnt) {
                    continue;
                }
                nxt_id.push_back(right_id_[cur_v][j]);
                sample_list.push_back(tmp);
                tmp += dp_cnt_[i][right_id_[cur_v][j]];
            }
        }
        if (tmp == 0) {
            ans = 0;
            break;
        }
        ans *= static_cast<double>(tmp) / static_cast<double>(dp_cnt_[i + 1][cur_id]);
        auto target = Random(tmp);
        cur_id = nxt_id[prev(std::upper_bound(
            sample_list.begin(),
            sample_list.end(),
            target)) - sample_list.begin()];
        if (trans_type_list_[i] == TransType::left) {
            auto v = graph_.edge_[cur_id].second;
            for (auto u : graph_.right_neighbour_[v]) {
                left_buffer[u]++;
            }
            right_ids.push_back(v);
            right_cnt++;
        } else {
            auto u = graph_.edge_[cur_id].first;
            for (auto v : graph_.left_neighbour_[u]) {
                right_buffer[v]++;
            }
            left_ids.push_back(u);
            left_cnt++;
        }
    }

    for (auto u : left_ids) {
        for (auto v : graph_.left_neighbour_[u]) {
            right_buffer[v] = 0;
        }
    }
    for (auto v : right_ids) {
        for (auto u : graph_.right_neighbour_[v]) {
            left_buffer[u] = 0;
        }
    }

    return ans;
}