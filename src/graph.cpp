#include "graph.h"

#include <algorithm>

void BiGraph::LoadFromEdges(std::vector<std::pair<int, int>> &&new_edge) {
    edge_.clear();
    edge_.shrink_to_fit();
    edge_ = std::move(new_edge);
    left_neighbour_.clear();
    left_neighbour_.shrink_to_fit();
    right_neighbour_.clear();
    right_neighbour_.shrink_to_fit();
    left_n_ = 0;
    right_n_ = 0;
    for (auto [u, v] : edge_) {
        left_n_ = std::max(left_n_, u + 1);
        right_n_ = std::max(right_n_, v + 1);
    }

    edge_n_ = edge_.size();
    left_neighbour_.resize(left_n_);
    right_neighbour_.resize(right_n_);

    for (auto [u, v] : edge_) {
        left_neighbour_[u].push_back(v);
    }
    for (int u = left_n_; u --> 0; ) {
        for (auto v : left_neighbour_[u]) {
            right_neighbour_[v].push_back(u);
        }
    }
    std::fill(left_neighbour_.begin(), left_neighbour_.end(), std::vector<int>());
    for (int v = right_n_; v --> 0; ) {
        for (auto u : right_neighbour_[v]) {
            left_neighbour_[u].push_back(v);
        }
    }
}

void BiGraph::Read(std::ifstream &fin) {
    std::vector<std::pair<int, int>> edge;
    int u, v;
    while (fin >> u >> v) {
        edge.push_back({u, v});
    }
    LoadFromEdges(std::move(edge));
}

void BiGraph::SwapLeftAndRight() {
    std::swap(left_n_, right_n_);
    std::swap(left_neighbour_, right_neighbour_);
    for (auto &[u, v] : edge_) {
        std::swap(u, v);
    }
}

void BiGraph::Trim(int p, int q) {
    std::vector<int> left_degree(left_n_);
    std::vector<int> right_degree(right_n_);

    std::vector<std::pair<int, int>> queue;

    for (int i = 0; i < left_n_; i++) {
        left_degree[i] = left_neighbour_[i].size();
        if (left_degree[i] < q) {
            queue.push_back({0, i});
        }
    }
    for (int i = 0; i < right_n_; i++) {
        right_degree[i] = right_neighbour_[i].size();
        if (right_degree[i] < p) {
            queue.push_back({1, i});
        }
    }

    for (int i = 0; i < queue.size(); i++) {
        auto [type, id] = queue[i];
        if (type == 0) {
            for (auto v : left_neighbour_[id]) {
                if (right_degree[v]-- == p) {
                    queue.push_back({1, v});
                }
            }
        } else {
            for (auto v : right_neighbour_[id]) {
                if (left_degree[v]-- == q) {
                    queue.push_back({0, v});
                }
            }
        }
    }

    std::vector<int> left_id(left_n_);
    std::vector<int> right_id(right_n_);
    std::vector<int> cnt, buc;

    cnt.resize(right_n_ + 1);
    std::fill(cnt.begin(), cnt.end(), 0);
    for (int i = 0; i < left_n_; i++) {
        if (left_degree[i] >= q) {
            cnt[left_degree[i]]++;
        }
    }
    for (int i = 1; i <= right_n_; i++) cnt[i] += cnt[i - 1];
    buc.resize(cnt[right_n_]);
    for (int i = 0; i < left_n_; i++) {
        if (left_degree[i] >= q) {
            buc[--cnt[left_degree[i]]] = i;
        }
    }
    for (int i = 0; i < buc.size(); i++) {
        left_id[buc[i]] = i;
    }


    cnt.resize(left_n_ + 1);
    std::fill(cnt.begin(), cnt.end(), 0);
    for (int i = 0; i < right_n_; i++) {
        if (right_degree[i] >= p) {
            cnt[right_degree[i]]++;
        }
    }
    for (int i = 1; i <= left_n_; i++) cnt[i] += cnt[i - 1];
    buc.resize(cnt[left_n_]);
    for (int i = 0; i < right_n_; i++) {
        if (right_degree[i] >= p) {
            buc[--cnt[right_degree[i]]] = i;
        }
    }
    for (int i = 0; i < buc.size(); i++) {
        right_id[buc[i]] = i;
    }

    std::vector<std::pair<int, int>> new_edge;
    for (auto [u, v] : edge_) {
        if (left_degree[u] >= q && right_degree[v] >= p) {
            new_edge.emplace_back(left_id[u], right_id[v]);
        }
    }

    LoadFromEdges(std::move(new_edge));
}


void RelabelEdges(std::vector<std::pair<int, int>> &edge,
                  std::vector<int> &left_buffer,
                  std::vector<int> &right_buffer) {
    std::vector<int> ids;

    int cnt = 0;
    for (auto [u, v] : edge) {
        if (left_buffer[u] >= 0) {
            left_buffer[u] = --cnt;
        }
    }
    for (auto &[u, v] : edge) {
        u = -left_buffer[u] + 1;
    }

    cnt = 0;
    for (auto [u, v] : edge) {
        if (right_buffer[v] >= 0) {
            right_buffer[v] = --cnt;
        }
    }
    for (auto &[u, v] : edge) {
        v = -right_buffer[v] + 1;
    }
}

static auto SplitLeftImpl(const BiGraph &origin_graph, int p, int q, int i,
                          std::vector<int> &left_buffer,
                          std::vector<int> &right_buffer) -> BiGraph {
    std::vector<std::pair<int, int>> edge;
    for (auto j : origin_graph.left_neighbour_[i]) {
        for (auto k : origin_graph.right_neighbour_[j]) {
            if (k <= i) {
                break;
            }
            left_buffer[k]++;
            right_buffer[j]++;
        }
    }
    for (auto j : origin_graph.left_neighbour_[i]) {
        for (auto k : origin_graph.right_neighbour_[j]) {
            if (k <= i) {
                break;
            }
            if (left_buffer[k] >= q && right_buffer[j] >= p - 1) {
                edge.emplace_back(k, j);
            }
        }
    }
    RelabelEdges(edge, left_buffer, right_buffer);
    for (auto j : origin_graph.left_neighbour_[i]) {
        for (auto k : origin_graph.right_neighbour_[j]) {
            if (k <= i) {
                break;
            }
            left_buffer[k] = 0;
            right_buffer[j] = 0;
        }
    }
    BiGraph g_i;
    g_i.LoadFromEdges(std::move(edge));
    g_i.Trim(p - 1, q);
    return g_i;
}

auto SplitLeft(const BiGraph &origin_graph, int p, int q) -> std::vector<BiGraph> {
    std::vector<BiGraph> result;
    std::vector<int> left_buffer(origin_graph.left_n_);
    std::vector<int> right_buffer(origin_graph.right_n_);
    for (int i = 0; i < origin_graph.left_n_; i++) {
        auto g_i = SplitLeftImpl(origin_graph, p, q, i, left_buffer, right_buffer);
        if (g_i.left_n_ == 0 || g_i.right_n_ == 0) {
            continue;
        }
        result.push_back(g_i);
    }
    return result;
}