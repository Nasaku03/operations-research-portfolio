/*
MSDG_sum_coloring.cpp

Implementation of the MSDG (Modular Synergistic Decomposition-Guided) metaheuristic
for the Sum Coloring Problem (chromatic sum) assuming the modular decomposition tree
is provided as input.

- Input format (example):
  1) Graph file (edge list):
       n m
       u v
       ... (0-based vertex indices)
  2) Modules file:
       k
       t size v1 v2 ... vt   (for each module: module_type size vertices...)
       module_type: 0=prime,1=series,2=parallel  (we primarily treat modules as units)

Usage: compile and run; program reads the two file paths from command line.

Note: This is a complete single-file prototype implementing:
  - module-level representation
  - greedy module coloring initialization
  - AVND-based intra-module local search (vertex swaps, recolorings)
  - inter-module coordination by shifting module color offsets
  - structural mutation: merge similar modules
  - main loop with time / iteration stopping criteria

This implementation focuses on clarity and a working prototype rather than extreme
micro-optimizations. It should compile with a modern C++17 compiler.

Author: ChatGPT (MSDG proposal implemented)
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <limits>
#include <map>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "ModuleMog.h"

using namespace std;

// ----------------------------- Utility types ---------------------------------
enum ModuleType { PRIME = 0, SERIES = 1, PARALLEL = 2 };
struct Module {
    int id;
    vector<int> vertices;       // vertex indices
    ModuleType type;                // 0=prime,1=series,2=parallel (informational)
    int baseOffset = 0;      // color offset applied to module-level colors
    vector<int> localColor;  // local color assignment for vertices (1-based colors)
};

struct Graph {
    int n;
    vector<vector<int>> adj;
    Graph(int n = 0) : n(n), adj(n) {}
    void addEdge(int u, int v) { adj[u].push_back(v); adj[v].push_back(u); }
};

// ----------------------------- Parameters ------------------------------------
struct Params {
    int maxIters = 2000;
    double timeLimitSec = 30.0;    // fallback time limit
    int avndRounds = 5;
    int populationSize = 10;       // population of module-level solutions
    int mutationFreq = 20;        // iterations between structural mutations
    unsigned seed = (unsigned)chrono::system_clock::now().time_since_epoch().count();
};

// ------------------------ Global random utilities ----------------------------
static std::mt19937 rng;
int randint(int a, int b) { uniform_int_distribution<int> dist(a, b); return dist(rng); }
double rand01() { uniform_real_distribution<double> d(0.0, 1.0); return d(rng); }

// ------------------------ Coloring helpers -----------------------------------
// Evaluate sum for a full coloring (colors are positive integers)
long long coloringSum(const vector<int>& color) {
    long long s = 0;
    for (int c : color) s += c;
    return s;
}

// Verify proper coloring
bool isProper(const Graph& G, const vector<int>& color) {
    int n = G.n;
    for (int u = 0; u < n; u++) {
        for (int v : G.adj[u]) if (u < v) {
            if (color[u] == color[v]) return false;
        }
    }
    return true;
}

// Greedy coloring on induced subgraph (vertices listed in verts)
// returns local colors (1-based) sized |verts|
vector<int> greedyColorModule(const Graph& G, const vector<int>& verts) {
    int k = verts.size();
    vector<int> color(k, 0);
    unordered_map<int, int> pos;
    for (int i = 0; i < k; i++) pos[verts[i]] = i;
    for (int i = 0; i < k; i++) {
        int u = verts[i];
        vector<char> used(k + 2, false);
        for (int v : G.adj[u]) {
            auto it = pos.find(v);
            if (it != pos.end()) {
                int idx = it->second;
                if (color[idx] > 0) used[color[idx]] = true;
            }
        }
        int c = 1; while (used[c]) ++c;
        color[i] = c;
    }
    return color;
}

// Map module-level solution to full coloring vector
vector<int> modulesToFullColor(const vector<Module>& modules, int n) {
    vector<int> color(n, 0);
    for (const Module& M : modules) {
        int k = M.vertices.size();
        for (int i = 0; i < k; i++) {
            int v = M.vertices[i];
            int local = M.localColor[i];
            color[v] = local + M.baseOffset; // baseOffset >= 0
            if (color[v] <= 0) color[v] = 1;
        }
    }
    return color;
}

// ---------------------- Intra-module local search (AVND) ---------------------
// We implement a small variable neighborhood descent: try recolor single vertex,
// try swap colors between two vertices, and try small reassignments.

void AVND_improve_module(const Graph& G, Module& M, int maxRounds) {
    // operate on induced adjacency and positions
    int k = M.vertices.size();
    if (k <= 1) return;
    unordered_map<int, int> pos;
    for (int i = 0; i < k; i++) pos[M.vertices[i]] = i;
    // compute adjacency within module
    vector<vector<int>> ladj(k);
    for (int i = 0; i < k; i++) {
        int u = M.vertices[i];
        for (int v : G.adj[u]) {
            auto it = pos.find(v);
            if (it != pos.end()) ladj[i].push_back(it->second);
        }
    }
    // localColor expected non-zero
    for (int round = 0; round < maxRounds; ++round) {
        bool improved = false;
        // neighborhood 1: try to recolor a vertex to a smaller color allowed
        for (int i = 0; i < k; i++) {
            int cur = M.localColor[i];
            vector<char> used(k + 2, false);
            for (int nb : ladj[i]) used[M.localColor[nb]] = true;
            // find smallest feasible color
            int c = 1; while (c <= k && used[c]) ++c;
            if (c < cur) {
                M.localColor[i] = c; improved = true; break;
            }
        }
        if (improved) continue;
        // neighborhood 2: try swapping colors of two vertices if reduces sum
        long long curSum = 0; for (int x : M.localColor) curSum += x;
        bool swapDone = false;
        for (int i = 0; i < k && !swapDone; i++) {
            for (int j = i + 1; j < k; j++) {
                if (M.localColor[i] == M.localColor[j]) continue;
                // swap if both remain proper
                int ci = M.localColor[i], cj = M.localColor[j];
                bool ok = true;
                // check i gets cj
                for (int nb : ladj[i]) if (nb != j) { if (M.localColor[nb] == cj) { ok = false; break; } }
                if (!ok) continue;
                // check j gets ci
                for (int nb : ladj[j]) if (nb != i) { if (M.localColor[nb] == ci) { ok = false; break; } }
                if (!ok) continue;
                long long newSum = curSum - ci - cj + cj + ci; // equal, so no benefit
                // but swapping may enable later reductions; accept with small prob
                if (rand01() < 0.1) {
                    swap(M.localColor[i], M.localColor[j]); swapDone = true; improved = true; break;
                }
            }
        }
        if (swapDone) continue;
        // neighborhood 3: try recolor to larger color to unblock elsewhere (randomized)
        if (rand01() < 0.05) {
            int i = randint(0, k - 1);
            int cur = M.localColor[i];
            int rnd = randint(1, 2);
            int cand = ((cur + rnd) < (k + 2)) ? (cur + rnd) : (k + 1);
            vector<char> used(k + 2, false);
            for (int nb : ladj[i]) used[M.localColor[nb]] = true;
            if (!used[cand]) { M.localColor[i] = cand; improved = true; }
        }
        if (!improved) break;
    }
}

// --------------------------- Inter-module coordination ----------------------
// Idea: modules have local colors; by shifting base offsets we change full coloring
// Use greedy improvement: try shifting a module offset up/down to reduce sum while
// preserving global properness.

bool tryShiftModuleOffset(const Graph& G, vector<Module>& modules, int mIndex) {
    Module& M = modules[mIndex];
    int n = G.n;
    // produce current full colors
    vector<int> cur = modulesToFullColor(modules, n);
    long long curSum = coloringSum(cur);
    // try shifts in range [-maxShift, +maxShift]
    int maxShift = 5; // small
    bool improved = false;
    for (int shift = -maxShift; shift <= maxShift; ++shift) {
        if (shift == 0) continue;
        M.baseOffset += shift;
        vector<int> cand = modulesToFullColor(modules, n);
        bool proper = isProper(G, cand);
        if (proper) {
            long long s = coloringSum(cand);
            if (s < curSum) {
                curSum = s; improved = true; break; // accept first improving shift
            }
        }
        M.baseOffset -= shift; // revert
    }
    return improved;
}

// Apply coordination phase: attempt shifts on modules in random order
void coordinationPhase(const Graph& G, vector<Module>& modules, int triesPerModule = 3) {
    int k = modules.size();
    vector<int> order(k); iota(order.begin(), order.end(), 0);
    shuffle(order.begin(), order.end(), rng);
    for (int idx : order) {
        for (int t = 0; t < triesPerModule; t++) {
            bool improved = tryShiftModuleOffset(G, modules, idx);
            if (improved) break;
        }
    }
}

// --------------------------- Structural mutation ----------------------------
// Merge two modules if merging yields potential benefit: similarity measured by
// color palette overlap and adjacency pattern. After merging, reinitialize local coloring.

bool mergeModulesIfBeneficial(const Graph& G, vector<Module>& modules) {
    int k = modules.size();
    if (k <= 1) return false;
    // pick two candidate modules randomly and attempt merge
    int a = randint(0, k - 1), b = randint(0, k - 1);
    if (a == b) return false;
    // compute palette overlap
    auto& A = modules[a], & B = modules[b];
    unordered_set<int> paletteA(A.localColor.begin(), A.localColor.end());
    unordered_set<int> paletteB(B.localColor.begin(), B.localColor.end());
    int common = 0;
    for (int x : paletteA) if (paletteB.count(x)) ++common;
    double overlap = (double)common / (1 + (int)max(paletteA.size(), paletteB.size()));
    if (overlap < 0.25) return false; // not similar enough
    // create merged module
    Module M;
    M.id = min(A.id, B.id);
    M.vertices = A.vertices; M.vertices.insert(M.vertices.end(), B.vertices.begin(), B.vertices.end());
    M.type = PRIME; // set prime
    // initialize local coloring greedily
    M.localColor = greedyColorModule(G, M.vertices);
    M.baseOffset = min(A.baseOffset, B.baseOffset);

    // compute full coloring before and after
    vector<Module> cand = modules;
    // replace A with M, erase B (ensure order)
    if (a < b) { cand[a] = M; cand.erase(cand.begin() + b); }
    else { cand[b] = M; cand.erase(cand.begin() + a); }
    vector<int> cur = modulesToFullColor(modules, G.n);
    vector<int> after = modulesToFullColor(cand, G.n);
    if (isProper(G, after) && coloringSum(after) <= coloringSum(cur)) {
        modules = move(cand);
        return true;
    }
    return false;
}

// ----------------------------- Main MSDG loop -------------------------------

vector<int> MSDG_search(const Graph& G, vector<Module> modules, const Params& P) {
    int n = G.n;
    // initialization: if modules have no localColor, compute greedy
    for (Module& M : modules) { if (M.localColor.size() != M.vertices.size()) M.localColor = greedyColorModule(G, M.vertices); }
    vector<int> bestColor = modulesToFullColor(modules, n);
    long long bestSum = isProper(G, bestColor) ? coloringSum(bestColor) : LLONG_MAX / 4;

    auto start = chrono::steady_clock::now();
    for (int iter = 0; iter < P.maxIters; ++iter) {
        // 1) Intra-module local improvement (parallelizable)
        for (Module& M : modules) { AVND_improve_module(G, M, P.avndRounds); }

        // 2) Coordination phase (inter-module offsets)
        coordinationPhase(G, modules, 4);

        // Evaluate and update best
        vector<int> cur = modulesToFullColor(modules, n);
        if (isProper(G, cur)) {
            long long s = coloringSum(cur);
            if (s < bestSum) { bestSum = s; bestColor = cur; }
        }

        // 3) Structural mutation occasionally
        if (iter % P.mutationFreq == 0) {
            if (mergeModulesIfBeneficial(G, modules)) {
                // re-evaluate immediately
                vector<int> cand = modulesToFullColor(modules, n);
                if (isProper(G, cand)) {
                    long long s = coloringSum(cand);
                    if (s < bestSum) { bestSum = s; bestColor = cand; }
                }
            }
        }

        // 4) Diversification: randomly perturb some module local colors
        if (rand01() < 0.02) {
            int idx = randint(0, (int)modules.size() - 1);
            Module& M = modules[idx];
            if (M.vertices.size() > 2) {
                int i = randint(0, (int)M.vertices.size() - 1);
                M.localColor[i] = max(1, M.localColor[i] + randint(1, 2));
            }
        }

        // stop on time
        auto now = chrono::steady_clock::now();
        double elapsed = chrono::duration<double>(now - start).count();
        if (elapsed > P.timeLimitSec) break;
    }

    // final repair: ensure proper coloring by greedy recoloring if necessary
    if (!isProper(G, bestColor)) {
        // perform global greedy recolor
        vector<int> col(n, 0);
        for (int u = 0; u < n; u++) {
            vector<char> used(n + 2, false);
            for (int v : G.adj[u]) if (col[v] > 0) used[col[v]] = true;
            int c = 1; while (used[c]) ++c; col[u] = c;
        }
        bestColor = col;
    }
    return bestColor;
}

// --------------------------- I/O helpers ------------------------------------

bool readGraph(const string& path, Graph& G) {
    ifstream in(path);
    if (!in) return false;
    int n, m; in >> n >> m; G = Graph(n);
    for (int i = 0; i < m; i++) { int u, v; in >> u >> v; if (u >= 0 && v >= 0 && u < n && v < n) G.addEdge(u, v); }
    return true;
}

bool readModules(const string& path, vector<Module>& modules) {
    ifstream in(path);
    if (!in) return false;
    int k; in >> k; modules.clear(); modules.reserve(k);
    for (int i = 0; i < k; i++) {
        int t, sz; in >> t >> sz; Module M; M.id = i; M.type = (ModuleType)t; M.vertices.resize(sz);
        for (int j = 0; j < sz; j++) in >> M.vertices[j];
        modules.push_back(M);
    }
    return true;
}

// ==== Load graph from file ====
Graph loadGraph(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Cannot open graph file: " << filename << endl;
        exit(1);
    }

    int n;
    file >> n;
    Graph G(n);

    int v, x;
    while (file >> v) {
        G.adj[v].clear();
        string line;
        getline(file, line);

        stringstream ss(line);
        while (ss >> x) {
            G.adj[v].push_back(x);
        }
    }

    return G;
}

// ==== Construct graph from Adjacency matrix ====
Graph constructFromA(unsigned int& n, unsigned int& m, vector<vector<int> >& A)
{
    Graph G(n);

    for (unsigned int i = 0; i < m; i++)
    {
        for (unsigned int j = 0; j < n; j++)
        {
            if (A[i][j] == 1)
            {
                for (unsigned int k = j + 1; k < n; k++)
                {
                    if (A[i][k] == 1)
                    {
                        G.adj[j].push_back(k);
                        G.adj[k].push_back(j);
                        break;
                    }
                }
                break;
            }
        }
    }
    for (unsigned int i = 0; i < n; i++)
    {
        sort(G.adj[i].begin(), G.adj[i].end());
    }
    return G;
}

// ==== Load modules from file ====
vector<Module> loadModules(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Cannot open module file: " << filename << endl;
        exit(1);
    }

    int M;  // number of modules
    file >> M;

    vector<Module> modules;
    modules.reserve(M);

    int t, k, v;
    for (int i = 0; i < M; i++) {
        file >> t >> k;
        Module mod;
        mod.id = i;
        mod.type = (ModuleType)t;

        for (int j = 0; j < k; j++) {
            file >> v;
            mod.vertices.push_back(v);
        }

        modules.push_back(mod);
    }

    return modules;
}

// ==== Load Modules from File version 2 ====
void getModules(vector<ModuleMOG>& listModules, const string& md, unsigned int& n)
{
    unsigned int nn = n;
    if (md.empty())
    {
        stack<ModuleMOG> modules;
        fstream tree("tree.txt");
        char c;
        while (tree >> c)
        {
            switch (c)
            {
            case '(':
            {
                if (modules.empty())
                {
                    modules.push(ModuleMOG());
                    modules.top().setBouchra(-1);
                }
                else
                {
                    int origin = modules.top().getNom();
                    modules.push(ModuleMOG());
                    modules.top().setBouchra(origin);
                }
                modules.top().setSommets(vector<int>());
                break;
            }
            case ')':
            {
                listModules.push_back(modules.top());
                int nom_ = modules.top().getNom();
                modules.pop();
                if (!modules.empty())
                {
                    vector<int> s(modules.top().getSommets());
                    s.push_back(nom_);
                    modules.top().setSommets(s);
                    //modules.top().getSommets().push_back(nom_);
                }
                break;
            }
            case 'P':
            {
                modules.top().setNom(nn);
                nn++;
                modules.top().setDegenerate(0);
                modules.top().setStrong(true);
                modules.top().setTrivial(false);
                break;
            }
            case 'U':
            {
                modules.top().setNom(nn);
                nn++;
                modules.top().setDegenerate(1);
                modules.top().setStrong(true);
                modules.top().setTrivial(false);
                break;
            }
            case 'J':
            {
                modules.top().setNom(nn);
                nn++;
                modules.top().setDegenerate(2);
                modules.top().setStrong(true);
                modules.top().setTrivial(false);
                break;
            }
            default:
            {
                //cout << "i = " << i << " => " << md[i] << endl;
                string str;
                while (tree >> c)
                {
                    if (c == '\'')
                    {
                        break;
                    }
                    //cout << "i = " << i << " => " << md[i] << endl;
                    str += string(1, c);
                }
                int nom_ = stoi(str);
                modules.top().setNom(nom_);
                modules.top().setDegenerate(0);
                modules.top().setStrong(true);
                modules.top().setTrivial(true);
                vector<int> s(modules.top().getSommets());
                s.push_back(nom_);
                modules.top().setSommets(s);
                //modules.top().getSommets().push_back(nom_);
                break;
            }
            }
        }
    }
    else
    {
        stack<ModuleMOG> modules;
        //cout << "total size = " << md.size() << endl;
        for (int i = 0; i < md.size(); i++)
        {
            //cout << "i = " << i << " => " << md[i] << endl;
            switch (md[i])
            {
            case '(':
            {
                if (modules.empty())
                {
                    modules.push(ModuleMOG());
                    modules.top().setBouchra(-1);
                }
                else
                {
                    int origin = modules.top().getNom();
                    modules.push(ModuleMOG());
                    modules.top().setBouchra(origin);
                }
                modules.top().setSommets(vector<int>());
                break;
            }
            case ')':
            {
                listModules.push_back(modules.top());
                int nom_ = modules.top().getNom();
                modules.pop();
                if (!modules.empty())
                {
                    vector<int> s(modules.top().getSommets());
                    s.push_back(nom_);
                    modules.top().setSommets(s);
                    //modules.top().getSommets().push_back(nom_);
                }
                break;
            }
            case 'P':
            {
                modules.top().setNom(nn);
                nn++;
                modules.top().setDegenerate(0);
                modules.top().setStrong(true);
                modules.top().setTrivial(false);
                break;
            }
            case 'U':
            {
                modules.top().setNom(nn);
                nn++;
                modules.top().setDegenerate(1);
                modules.top().setStrong(true);
                modules.top().setTrivial(false);
                break;
            }
            case 'J':
            {
                modules.top().setNom(nn);
                nn++;
                modules.top().setDegenerate(2);
                modules.top().setStrong(true);
                modules.top().setTrivial(false);
                break;
            }
            default:
            {
                i++;
                //cout << "i = " << i << " => " << md[i] << endl;
                string str;
                for (; md[i] != '\''; i++)
                {
                    //cout << "i = " << i << " => " << md[i] << endl;
                    str += md[i];
                }
                int nom_ = stoi(str);
                modules.top().setNom(nom_);
                modules.top().setDegenerate(0);
                modules.top().setStrong(true);
                modules.top().setTrivial(true);
                vector<int> s(modules.top().getSommets());
                s.push_back(nom_);
                modules.top().setSommets(s);
                //modules.top().getSommets().push_back(nom_);
                break;
            }
            }
        }
    }
}

// ==== Retrieve vertices recursively of modules ====
void retrieveVertices(vector<ModuleMOG>& modules, vector<int>& vertices, int vertex, int n)
{
    if (vertex < n)
    {
        vertices.push_back(vertex);
    }
    else
    {
        int pos = -1;
        for (int i = 0; i < modules.size(); i++)
        {
            if (modules[i].getNom() == vertex)
            {
                pos = i;
                break;
            }
        }
        if (pos == -1)
        {
            cerr << "Module not found!" << endl;
            exit(2);
        }
        for (int i = 0; i < modules[pos].getSommets().size(); i++)
        {
            retrieveVertices(modules, vertices, modules[pos].getSommets()[i], n);
        }
    }
}

// ==== Transform modules from github form to chatGPT form
vector<Module> transformModules(vector<ModuleMOG>& modules, unsigned int& n)
{
    if (modules.empty())
    {
        cerr << "Error: No modules" << endl;
        exit(1);
    }
    vector<Module> M;
    int size_ = modules.size() - n;
    M.reserve(size_);

    int j = 0;
    for (int i = 0; i < modules.size(); i++)
    {
        if (modules[i].getNom() >= n)
        {
            Module mod;
            mod.id = modules[i].getNom();

            mod.type = (modules[i].getDegenerate()==0)?PRIME:((modules[i].getDegenerate() == 1)?PARALLEL:SERIES);
            vector<int> vertices;
            for (int k = 0; k < modules[i].getSommets().size(); k++)
            {
                retrieveVertices(modules, vertices, modules[i].getSommets()[k], n);
            }
            M.push_back(mod);
        }
    }
    return M;
}

// ==== Load graph as DIMACS file ====
void lectureDimacs(const string& filename, unsigned int& n, unsigned int& m, unsigned int& delta, vector<vector<int> >& A)
{
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Cannot open graph file: " << filename << endl;
        exit(1);
    }

    ifstream data(filename);
    string info;
    data >> info;
    data >> info;
    data >> n;
    data >> m;
    vector<int> ligneA;
    for (unsigned int j = 0; j < n; j++)
    {
        ligneA.push_back(0);
    }
    for (unsigned int i = 0; i < m; i++)
    {
        A.push_back(ligneA);
    }
    int s1, s2;
    for (int i = 0; i < m; i++)
    {
        data >> info;
        data >> s1;
        data >> s2;
        A[i][s1] = 1;
        A[i][s2] = 1;
    }
    unsigned int max_;
    vector<unsigned int> somme(n, 0);
    for (unsigned int j = 0; j < n; j++)
    {
        for (unsigned int i = 0; i < A.size(); i++)
        {
            somme[j] += A[i][j];
        }
    }
    max_ = somme[0];
    for (unsigned int j = 1; j < n; j++)
    {
        if (max_ < somme[j])
        {
            max_ = somme[j];
        }
    }
    delta = max_ + 1;
}

// ------------------------------- Main --------------------------------------
int main(int argc, char** argv) {
    /*
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " graph_file modules_file [timeLimitSec]" << "\n";
        return 1;
    }
    rng.seed((unsigned)chrono::system_clock::now().time_since_epoch().count());
    Graph G; vector<Module> modules;
    if (!readGraph(argv[1], G)) { cerr << "Error reading graph file\n"; return 1; }
    if (!readModules(argv[2], modules)) { cerr << "Error reading modules file\n"; return 1; }
    Params P; if (argc >= 4) P.timeLimitSec = atof(argv[3]);
    */

    rng.seed((unsigned)chrono::system_clock::now().time_since_epoch().count());
    Params P;
    const string graphFile = "graph.txt";
    const string moduleFile = "modules.txt";
    vector<vector<int> > A;
    unsigned int n, m, delta;
    vector<ModuleMOG> M;
    vector<Module> modules;

    lectureDimacs(graphFile, n, m, delta, A);
    cout << "Dimacs" << endl;
    // Graph G = loadGraph(graphFile);
    Graph G = constructFromA(n, m, A);
    cout << "G" << endl;
    //vector<Module> modules = loadModules(moduleFile);
    getModules(M, moduleFile, n);
    modules = transformModules(M, n);
    cout << "Modules" << endl;

    // initialize module local colorings if empty
    for (Module& M : modules) { if (M.localColor.empty()) M.localColor = greedyColorModule(G, M.vertices); }
    cout << "local" << endl;

    auto best = MSDG_search(G, modules, P);
    cout << "search" << endl;

    // output result: sum and coloring
    long long s = coloringSum(best);
    cout << "Sum = " << s << "\n";
    cout << "Coloring (0-based vertices, color ints):\n";
    for (int i = 0; i < G.n; i++) cout << best[i] << (i + 1 == G.n ? "\n" : " ");
    return 0;
}
