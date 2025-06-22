#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <ranges>
#include <algorithm>

//Лабораторная работа 4. Программирование алгоритмов на графах
//Написать программу поиска (в глубину) лексикографически первого пути на графе

using Graph = std::map<std::string, std::set<std::string>>;

void add_edge(Graph& graph, const std::string& u, const std::string& v) {
    graph[u].insert(v);
    graph[v].insert(u); 
}

std::vector<std::string> lexicographical_dfs(const Graph& graph, const std::string& start, const std::string& target) {
    std::vector<std::string> path;
    std::stack<std::pair<std::string, std::vector<std::string>>> stack;
    std::set<std::string> visited;

    stack.emplace(start, std::vector<std::string>{start});

    while (!stack.empty()) {
        auto [current, current_path] = stack.top();
        stack.pop();

        if (visited.contains(current)) continue;

        visited.insert(current);
        path = current_path;

        if (current == target) return path;

        if (graph.contains(current)) {
            for (const auto& neighbor : std::ranges::reverse_view(graph.at(current))) {
                if (!visited.contains(neighbor)) {
                    auto new_path = current_path;
                    new_path.push_back(neighbor);
                    stack.emplace(neighbor, new_path);
                }
            }
        }
    }

    return {}; 
}

int main() {
    Graph graph;
    
    add_edge(graph, "A", "B");
    add_edge(graph, "A", "C");
    add_edge(graph, "B", "D");
    add_edge(graph, "C", "D");
    add_edge(graph, "B", "E");
    add_edge(graph, "D", "E");

    std::string start = "A";
    std::string target = "E";

    auto path = lexicographical_dfs(graph, start, target);

    if (!path.empty()) {
        std::cout << "Founded path is: ";
        for (const auto& node : path) {
            std::cout << node << " ";
        }
    }
    else {
        std::cout << "Path not found";
    }

    return 0;
}