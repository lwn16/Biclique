#include <vector>
#include <iostream>
#include <fstream>

class BiGraph {
public:
    BiGraph(): left_n_(0), right_n_(0) {}
    BiGraph(const BiGraph &) = default;
    BiGraph(BiGraph &&) = default;
    BiGraph& operator = (const BiGraph &) = default;
    BiGraph& operator = (BiGraph &&) = default;
    ~BiGraph() = default;

    virtual void LoadFromEdges(std::vector<std::pair<int, int>> &&new_edge);
    virtual void Read(std::ifstream &fin);
    virtual void SwapLeftAndRight();

    virtual void Trim(int p, int q);

public:
    int left_n_;
    int right_n_;
    int edge_n_;
    std::vector<std::vector<int>> left_neighbour_;
    std::vector<std::vector<int>> right_neighbour_;
    std::vector<std::pair<int, int>> edge_;
};

void RelabelEdges(std::vector<std::pair<int, int>> &edge);

auto SplitLeft(const BiGraph &origin_graph, int p, int q) -> std::vector<BiGraph>;