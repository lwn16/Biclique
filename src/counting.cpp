#include "counting.h"

static void RelabelGraph(BiGraphWithColor &graph, std::vector<int> &left_pos, std::vector<int> &right_pos) {
    std::vector<int> id, pos;

    id.resize(graph.left_n_);
    pos.resize(graph.left_n_);
    iota(id.begin(), id.end(), 0);
    sort(id.begin(), id.end(), [&](int a, int b) {
        return graph.left_color_[a] < graph.left_color_[b];
    });
    for (int i = 0; i < id.size(); i++) {
        pos[id[i]] = i;
    }
    for (auto &[u, v] : graph.edge_) {
        u = pos[u];
    }
    std::sort(graph.left_color_.begin(), graph.left_color_.end());

    id.resize(graph.right_n_);
    pos.resize(graph.right_n_);
    iota(id.begin(), id.end(), 0);
    sort(id.begin(), id.end(), [&](int a, int b) {
        return graph.right_color_[a] < graph.right_color_[b];
    });
    for (int i = 0; i < id.size(); i++) {
        pos[id[i]] = i;
    }
    for (auto &[u, v] : graph.edge_) {
        v = pos[v];
    }
    std::sort(graph.right_color_.begin(), graph.right_color_.end());

    sort(graph.edge_.begin(), graph.edge_.end());

    left_pos.clear();
    std::fill(graph.left_neighbour_.begin(), graph.left_neighbour_.end(), std::vector<int>());
    for (int l = 0, r = 0; l < graph.edge_n_; l = r) {
        for ( ;
             r < graph.edge_n_ && graph.right_color_[graph.edge_[l].second] == graph.right_color_[graph.edge_[r].second];
             r++);
        for (int i = l; i < r; i++) {
            auto [u, v] = graph.edge_[i];
            left_pos.push_back(graph.left_neighbour_[u].size());
        }
        for (int i = l; i < r; i++) {
            auto [u, v] = graph.edge_[i];
            graph.left_neighbour_[u].push_back(v);
        }
    }
    
    right_pos.clear();
    std::fill(graph.right_neighbour_.begin(), graph.right_neighbour_.end(), std::vector<int>());
    for (int l = 0, r = 0; l < graph.edge_n_; l = r) {
        for ( ;
             r < graph.edge_n_ && graph.left_color_[graph.edge_[l].first] == graph.left_color_[graph.edge_[r].first];
             r++);
        for (int i = l; i < r; i++) {
            auto [u, v] = graph.edge_[i];
            right_pos.push_back(graph.right_neighbour_[v].size());
        }
        for (int i = l; i < r; i++) {
            auto [u, v] = graph.edge_[i];
            graph.right_neighbour_[v].push_back(u);
        }
    }
}

CountingIndex::CountingIndex(BiGraphWithColor &&graph, int p, int q)
    : graph_(std::move(graph)), p_(p), q_(q) {
    if (graph_.left_color_.empty() || graph_.right_color_.empty()) {
        throw "The graph has not been colored";
    }

    RelabelGraph(graph_, left_pos_, right_pos_);

    left_id_.resize(graph_.left_n_);
    right_id_.resize(graph_.right_n_);
    for (int i = 0; i < graph_.edge_n_; i++) {
        auto [u, v] = graph_.edge_[i];
        left_id_[u].push_back(i);
        right_id_[v].push_back(i);
    }

    length_ = p + q - 1;
    if (p == 1) {
        for (int i = 1; i < q; i++) {
            trans_type_list_.push_back(TransType::left);
        }
    } else if (q == 1) {
        for (int i = 1; i < p; i++) {
            trans_type_list_.push_back(TransType::right);
        }
    } else {
        for (int i = 1; i <= (p - 1) * (q - 1); i++) {
            if (i % (p - 1) == 0) {
                trans_type_list_.push_back(TransType::left);
            }
            if (i % (q - 1) == 0) {
                trans_type_list_.push_back(TransType::right);
            }
        }
    }

    dp_cnt_.resize(length_, std::vector<dp_t>(graph_.edge_n_));
//    dp_sum_.resize(length_ - 1);

    for (int i = 0; i < graph_.edge_n_; i++) {
        dp_cnt_[0][i] = 1;
    }
    
    for (int t = 0; t < length_ - 1; t++) {
        if (trans_type_list_[t] == TransType::left) {
            {
                std::vector<std::vector<dp_t>> tmp;
                swap(dp_sum_,tmp);
            }
            dp_sum_.resize(graph_.left_n_, std::vector<dp_t>{0});
            for (int i = 0; i < graph_.edge_n_; i++) {
                dp_cnt_[t + 1][i] = dp_sum_[graph_.edge_[i].first][left_pos_[i]];
                dp_sum_[graph_.edge_[i].first].push_back(dp_sum_[graph_.edge_[i].first].back() + dp_cnt_[t][i]);
            }
        } else {
            {
                std::vector<std::vector<dp_t>> tmp;
                swap(dp_sum_,tmp);
            }
            dp_sum_.resize(graph_.right_n_, std::vector<dp_t>{0});
            for (int i = 0; i < graph_.edge_n_; i++) {
                dp_cnt_[t + 1][i] = dp_sum_[graph_.edge_[i].second][right_pos_[i]];
                dp_sum_[graph_.edge_[i].second].push_back(dp_sum_[graph_.edge_[i].second].back() + dp_cnt_[t][i]);
            }
        }
    }

    total_dp_ = 0;
    for (auto i : dp_cnt_[length_ - 1]) {
        sample_indices_.push_back(total_dp_);
        total_dp_ += i;
    }
}