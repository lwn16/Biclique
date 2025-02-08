#include "common.h"
#include "coloring.h"
#include "random.h"

enum TransType {
    left,
    right
};

class CountingIndex {
public:
    CountingIndex(BiGraphWithColor &&graph, int p, int q);
    CountingIndex(const CountingIndex &) = default;
    CountingIndex(CountingIndex &&) = default;
    CountingIndex& operator = (const CountingIndex &) = default;
    CountingIndex& operator = (CountingIndex &&) = default;

    auto CountBySample(std::vector<int> &left_buffer, std::vector<int> &right_buffer) -> double;

public:
    BiGraphWithColor graph_;
    int p_;
    int q_;

    std::vector<TransType> trans_type_list_;
    std::vector<std::vector<dp_t>> dp_cnt_;
//  std::vector<std::vector<std::vector<dp_t>>> dp_sum_;
    std::vector<std::vector<dp_t>> dp_sum_;
    std::vector<int> left_pos_;
    std::vector<int> right_pos_;
    std::vector<std::vector<int>> left_id_;
    std::vector<std::vector<int>> right_id_;
    int length_;

    dp_t total_dp_;

    std::vector<dp_t> sample_indices_;
};
