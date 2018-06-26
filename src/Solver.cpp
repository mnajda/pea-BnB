#include "Solver.hpp"

#include <algorithm>
#include <iostream>
#include <chrono>

Solver::Solver(std::vector<std::vector<int>> costMatrix) : instance(std::move(costMatrix))
{
}

void Solver::run()
{
    std::vector<int> path;
    path.reserve(instance.size);
    auto begin = std::chrono::system_clock::now();

    path.push_back(0);
    Node root{ path, 0 };
    branchAndBound(root);

    auto end = std::chrono::system_clock::now();
    elapsedTime = end - begin;
}

void Solver::printSolution()
{
    std::cout << std::endl;
    std::cout << "Cost: " << instance.minCost << std::endl;
    std::cout << std::endl;

    instance.bestPath.push_back(0);
    std::cout << "Path: ";

    for (const auto& e : instance.bestPath)
    {
        std::cout << e << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(elapsedTime).count() << "ns ";
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() << "ms ";
    std::cout << std::chrono::duration_cast<std::chrono::seconds>(elapsedTime).count() << "s" << std::endl;
}

std::vector<bool> Solver::getVisited(const std::vector<int>& path) const
{
    std::vector<bool> visited(instance.size, false);
    for (auto i = 0; i < instance.size; ++i)
    {
        if (find(path.begin(), path.end(), i) != path.end())
        {
            visited[i] = true;
        }
    }
    return visited;
}

std::vector<int> Solver::getValues(const std::vector<int>& path) const
{
    std::vector<int> values;
    std::vector<bool> visited = getVisited(path);

    for (auto i = 0; i < instance.size; ++i)
    {
        auto min = std::numeric_limits<int>::max();
        if (!visited[i])
        {
            for (int j = 0; j < instance.size; ++j)
            {
                if (!visited[j])
                {
                    min = std::min(min, instance[i][j]);
                }
            }
            values.push_back(min);
        }
    }

    std::sort(values.begin(), values.end());
    return values;
}

int Solver::getLowerBound(const std::vector<int>& path, int cost) const
{
    int lowerBound = cost;
    auto minCosts = getValues(path);
    for (const auto minCost : minCosts)
    {
        lowerBound += minCost;
        if (lowerBound > instance.minCost)
        {
            return lowerBound;
        }
    }
    return lowerBound;
}

void Solver::branchAndBound(Node& node)
{
    if (node.currentPath.size() == instance.size)
    {
        node.cost += instance[node.currentPath.back()][node.currentPath.front()];
        if (node.cost < instance.minCost)
        {
            instance.minCost = node.cost;
            instance.bestPath = node.currentPath;
        }
        return;
    }
    if (getLowerBound(node.currentPath, node.cost) < instance.minCost)
    {
        std::vector<Node> nodes;
        nodes.reserve(instance.size - 1);

        for (auto i = 1; i < instance.size; ++i)
        {
            if (std::find(node.currentPath.begin(), node.currentPath.end(), i) == node.currentPath.end())
            {
                int cost = node.cost + instance[node.currentPath.back()][i];
                if (cost < instance.minCost)
                {
                    nodes.emplace_back(node.currentPath, i);
                    nodes.back().currentPath.push_back(i);
                    nodes.back().cost = cost;
                }
            }
        }
        for (auto& next : nodes)
        {
            branchAndBound(next);
        }
    }

}
