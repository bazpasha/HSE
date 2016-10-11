#include <iostream>
#include <vector>
#include <stdlib.h>
#include <cmath>

class Graph {
    int size;
    std::vector<std::vector<int>> siblings;

    bool connected(int i, int j) {
        for (int k: siblings[i]) {
            if (k == j)
                return true;
        }
        return false;
    }

 public:
    using Path = std::vector<int>;

    double energy(Path& p) {
        if (p.size() == 0)
            return 2;
        return 1.0 / p.size();
    }

    bool inPath(Path& p, int i) {
        for (int k : p) {
            if (k == i)
                return true;
        }
        return false;
    }

    void shrink(Path& p, std::vector<Path>& shrink) {
        shrink.clear();
        Path q = p;

        if (q.size() > 0) {
            q.erase(q.begin(), q.begin() + 1);
            shrink.push_back(q);
            q = p;
            q.erase(q.end() - 1, q.end());
            shrink.push_back(q);
        }
    }

    void expand(Path& p, std::vector<Path>& expand) {
        expand.clear();
        Path q;

        if (p.size() == 0) {
            for (int i = 0; i < size; ++i) {
                q = {i};
                expand.push_back(q);
            }
        } else {
            int s = *(p.begin());
            for (int i : siblings[s]) {
                if (!inPath(p, i)) {
                    q = p;
                    q.insert(q.begin(), i);
                    expand.push_back(q);
                }
            }

            int f = *(p.end() - 1);
            for (int i : siblings[f]) {
                if (!inPath(p, i)) {
                    q = p;
                    q.push_back(i);
                    expand.push_back(q);
                }
            }
        }
    }

    void read() {
        int V, E;
        std::cin >> V >> E;
        size = V;
        siblings.resize(static_cast<size_t>(V));
        int u, v;
        for (int i = 0; i < E; ++i) {
            std::cin >> u >> v;
            --u;
            --v;
            siblings[u].push_back(v);
            siblings[v].push_back(u);
        }
    }

    void writePath(Path& p) {
        std::cout << p.size() << "\n";
        for (size_t i = 0; i + 1 < p.size(); ++i) {
            std::cout << p[i] + 1 << " - ";
        }
        if (p.size() > 0)
            std::cout << *(p.end() - 1) + 1;
        std::cout << "\n";
    }

    void randomGraph(size_t V, size_t E, size_t minOptimal = 0) {
        size = V;
        siblings.resize(V);
        for (int i = 1; i <= minOptimal; ++i) {
            siblings[i - 1].push_back(i);
            siblings[i].push_back(i - 1);
        }
        E = E - minOptimal;
        int u, v;
        for (size_t i = 0; i < E; ++i) {
            do {
                u = rand() % V;
                v = rand() % V;
            } while (!connected(u, v));
            siblings[u].push_back(v);
            siblings[v].push_back(u);
        }
    }
};

double dRand() {
    return static_cast<double>(rand()) / RAND_MAX;
}

class GradientDescent {
    Graph* G;

 public:
    GradientDescent(Graph* G_):
            G(G_)
    {}

    Graph::Path solve() {
        Graph::Path p = {};
        std::vector<Graph::Path> expand;
        G->expand(p, expand);
        while (expand.size() > 0) {
            p = expand[rand() % expand.size()];
            G->expand(p, expand);
        }
        return p;
    }
};

class Metropolis {
    double T, pi;
    Graph* G;

 public:
    Metropolis(Graph* G_, double T_, double pi_):
            G(G_), T(T_), pi(pi_)
    {}

    Graph::Path choosePath(std::vector<Graph::Path>& expand, std::vector<Graph::Path>& shrink) {
        if (dRand() <= pi && expand.size() > 0 || shrink.size() == 0) {
            return expand[rand() % expand.size()];
        } else {
            return shrink[rand() % shrink.size()];
        }
    }

    double worseProbability(double dE) {
        return std::exp(-dE / T);
    }

    Graph::Path solve() {
        double E = 2;
        Graph::Path p = {}, q;
        std::vector<Graph::Path> expand, shrink;
        G->expand(p, expand);
        G->shrink(p, shrink);
        while (expand.size() > 0) {
            q = choosePath(expand, shrink);
            if (G->energy(q) < G->energy(p) || dRand() < worseProbability(G->energy(q) - G->energy(p))) {
                p = q;
            }
            G->expand(p, expand);
            G->shrink(p, shrink);
        }
        return p;
    }
};

class MetropolisAnnealing {
    double Tmax, Tmin, pi;
    Graph* G;

 public:
    MetropolisAnnealing(Graph* G_, double Tmax_, double Tmin_, double pi_):
            G(G_), Tmax(Tmax_), Tmin(Tmin_), pi(pi_)
    {}

    Graph::Path choosePath(std::vector<Graph::Path>& expand, std::vector<Graph::Path>& shrink) {
        if (dRand() <= pi && expand.size() > 0 || shrink.size() == 0) {
            return expand[rand() % expand.size()];
        } else {
            return shrink[rand() % shrink.size()];
        }
    }

    double worseProbability(double dE, double T) {
        return std::exp(-dE / T);
    }

    Graph::Path solve() {
        double T = Tmax;
        double E = 2;
        Graph::Path p = {}, q;
        std::vector<Graph::Path> expand, shrink;
        double iter = 1;
        while (T > Tmin) {
            G->expand(p, expand);
            G->shrink(p, shrink);
            q = choosePath(expand, shrink);
            if (G->energy(q) < G->energy(p) || dRand() < worseProbability(G->energy(q) - G->energy(p), T)) {
                p = q;
            }
            T = Tmax / iter;
            iter += 1;
        }
        return p;
    }
};

int main() {
    Graph G;
    size_t D = 0, M = 0, MA = 0;
    for (size_t i = 0; i < 10; ++i) {
        G.randomGraph(100, 4000, 60);

        GradientDescent gradient(&G);
        Metropolis metropolis(&G, 15000, 0.75);
        MetropolisAnnealing metropolis_annealing(&G, 15000, 1, 0.75);

        Graph::Path p = gradient.solve();
        D += p.size();

        Graph::Path q = metropolis.solve();
        M += q.size();

        Graph::Path r = metropolis_annealing.solve();
        MA += r.size();
    }

    std::cout << (float)D / 10 << '\n' << (float)M / 10 << '\n' << (float)MA / 10;

    return 0;
}