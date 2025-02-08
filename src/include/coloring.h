#include "graph.h"

class BiGraphWithColor: public BiGraph {
public:
    BiGraphWithColor(const BiGraph &graph): BiGraph(graph) {}
    BiGraphWithColor(BiGraph &&graph): BiGraph(std::move(graph)) {}
    BiGraphWithColor(const BiGraphWithColor &) = default;
    BiGraphWithColor(BiGraphWithColor &&) = default;
    BiGraphWithColor& operator = (const BiGraphWithColor &) = default;
    BiGraphWithColor& operator = (BiGraphWithColor &&) = default;

    void ClearColor();
    auto SetColor(int p, int q) -> std::pair<int, int>;
    auto SetDefaultColor(int p, int q) -> std::pair<int, int>;
    void ShuffleColor();

    virtual void SwapLeftAndRight() override;
public:
    std::vector<int> left_color_;
    std::vector<int> right_color_;
};
