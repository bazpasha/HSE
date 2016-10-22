#include <iostream>
#include <random>
#include <vector>

class Graph {
    struct Edge {
        int begin, end;
        Edge(int begin, int end):
                begin(begin), end(end)
        {}
        Edge():
                begin(0), end(0)
        {}
    };
    std::vector<Edge> edges;
    size_t V, E;
 public:
    size_t vertexNumber() {
        return V;
    }

    size_t edgesNumber() {
        return E;
    }

    void read() {
        std::cin >> V >> E;
        int u, v;
        for (size_t i = 0; i < E; ++i) {
            std::cin >> u >> v;
            edges.push_back(Edge(u, v));
        }
    }

    std::vector<Edge> maximumMatch() {
        std::vector<Edge> match, randomEdges = edges;
        std::vector<bool> vertex(V + 1, false);
        Edge current;
        size_t index = 0;
        while (randomEdges.size()) {
            index = std::rand() % randomEdges.size();
            current = randomEdges[index];
            randomEdges.erase(randomEdges.begin() + index, randomEdges.begin() + index + 1);
            if (!vertex[current.begin] && !vertex[current.end]) {
                match.push_back(current);
                vertex[current.begin] = true;
                vertex[current.end] = true;
            }
        }
        return match;
    }

    void printMatch() {
        std::vector<Edge> match = maximumMatch();
        for (Edge& e : match) {
            std::cout << e.begin << ' ' << e.end << '\n';
        }
    }
};

int main() {
    Graph G;
    G.read();
    G.printMatch();
    return 0;
}