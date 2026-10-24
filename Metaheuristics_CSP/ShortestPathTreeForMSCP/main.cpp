#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <limits>
#include <algorithm>
#include <random>    // <-- nécessaire pour random_device, mt19937, distribution
#include <chrono>    // <-- fallback seed si random_device est peu fiable
using namespace std;

using ll = long long;
const ll INF = LLONG_MAX / 4;

// -----------------------------------------------------------
// 🔹 Fonction pour ajuster les distances après Dijkstra
// -----------------------------------------------------------
void adjustDistances(vector<ll>& dist,
    const vector<vector<int>>& adj,
    const vector<vector<pair<int, ll>>>& G,
    const vector<int>& deg,
    int n, int S, int T)
{
    bool changed = true;
    int iterations = 0;

    auto canSet = [&](int x, ll newVal, const vector<ll>& dist) {
        if (newVal < 1) return false;
        for (int nb = 0; nb < n; ++nb)
            if (adj[x][nb] && dist[nb] == newVal)
                return false;
        return true;
        };

    while (changed && iterations < n * 2) {
        changed = false;
        iterations++;

        // --- Étape 1 : corriger les distances égales entre sommets adjacents ---
        for (int u = 0; u < n; ++u) {
            for (int v = u + 1; v < n; ++v) {
                if (!adj[u][v]) continue;
                if (dist[u] == dist[v]) {
                    ll d = dist[u];
                    ll bestU = dist[u], bestV = dist[v];
                    bool canReduceU = false, canReduceV = false;

                    // 🔹 Essayer de réduire u le plus possible
                    for (ll val = d - 1; val >= 1; --val) {
                        if (canSet(u, val, dist)) {
                            bestU = val;
                            canReduceU = true;
                            break;
                        }
                    }

                    // 🔹 Essayer de réduire v le plus possible
                    for (ll val = d - 1; val >= 1; --val) {
                        if (canSet(v, val, dist)) {
                            bestV = val;
                            canReduceV = true;
                            break;
                        }
                    }

                    // 🔹 Cas réduction possible : on prend celle qui offre la plus grande baisse
                    if (canReduceU || canReduceV) {
                        ll reductionU = dist[u] - bestU;
                        ll reductionV = dist[v] - bestV;
                        if (reductionU > reductionV) {
                            dist[u] = bestU;
                        }
                        else if (reductionV > 0) {
                            dist[v] = bestV;
                        }
                        changed = true;
                    }
                    // 🔹 Sinon, on augmente le moins possible
                    else {
                        ll newU = dist[u], newV = dist[v];
                        bool canIncreaseU = false, canIncreaseV = false;

                        for (ll val = d + 1; val <= n; ++val) {
                            if (canSet(u, val, dist)) {
                                newU = val;
                                canIncreaseU = true;
                                break;
                            }
                        }
                        for (ll val = d + 1; val <= n; ++val) {
                            if (canSet(v, val, dist)) {
                                newV = val;
                                canIncreaseV = true;
                                break;
                            }
                        }

                        // choisir la plus petite augmentation
                        if (canIncreaseU && canIncreaseV) {
                            if (newU - d <= newV - d) dist[u] = newU;
                            else dist[v] = newV;
                        }
                        else if (canIncreaseU) dist[u] = newU;
                        else if (canIncreaseV) dist[v] = newV;

                        if (canIncreaseU || canIncreaseV) changed = true;
                    }
                }
            }
        }

        // --- Étape 2 : réduire les distances trop grandes (deg < dist) ---
        for (int w = 0; w < n; ++w) {
            if (deg[w] < dist[w]) {
                for (ll val = dist[w] - 1; val >= 1; --val) {
                    if (canSet(w, val, dist)) {
                        dist[w] = val;
                        changed = true;
                        break;
                    }
                }
            }
        }

        // --- Étape 3 : casser les égalités restantes si besoin (petite hausse fine) ---
        for (int u = 0; u < n; ++u) {
            for (int v = u + 1; v < n; ++v) {
                if (adj[u][v] && dist[u] == dist[v]) {
                    for (ll val = dist[v] + 1; val <= n; ++val) {
                        if (canSet(v, val, dist)) {
                            dist[v] = val;
                            changed = true;
                            break;
                        }
                    }
                }
            }
        }
    }
}
// -----------------------------------------------------------

void adjustDistancesRandomized(vector<ll>& dist,
    const vector<vector<int>>& adj,
    const vector<vector<pair<int, ll>>>& G,
    const vector<int>& deg,
    int n, int S, int T)
{
    bool changed = true;
    int iterations = 0;

    // ---- initialisation robuste du générateur aléatoire ----
    std::random_device rd;
    unsigned seed;
    // si random_device fournit de l'entropie, on l'utilise, sinon fallback chrono
    if (rd.entropy() > 0.0) seed = rd();
    else seed = static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> coin(0, 1);

    auto canSet = [&](int x, ll newVal, const vector<ll>& distSnapshot) {
        if (newVal < 1) return false;
        for (int nb = 0; nb < n; ++nb)
            if (adj[x][nb] && distSnapshot[nb] == newVal)
                return false;
        return true;
        };

    while (changed && iterations < n * 2) {
        changed = false;
        iterations++;

        // --- Étape 1 : corriger les distances égales entre sommets adjacents ---
        for (int u = 0; u < n; ++u) {
            for (int v = u + 1; v < n; ++v) {
                if (!adj[u][v]) continue;
                if (dist[u] == dist[v]) {
                    ll d = dist[u];
                    ll bestU = dist[u], bestV = dist[v];
                    bool canReduceU = false, canReduceV = false;

                    // chercher la meilleure réduction possible pour u (le plus bas possible >=1)
                    for (ll val = d - 1; val >= 1; --val) {
                        if (canSet(u, val, dist)) { bestU = val; canReduceU = true; break; }
                    }
                    // chercher la meilleure réduction possible pour v
                    for (ll val = d - 1; val >= 1; --val) {
                        if (canSet(v, val, dist)) { bestV = val; canReduceV = true; break; }
                    }

                    // --- Si les deux peuvent être réduits : choisir aléatoirement lequel réduire ---
                    if (canReduceU && canReduceV) {
                        if (coin(gen) == 0) dist[u] = bestU;
                        else dist[v] = bestV;
                        changed = true;
                    }
                    // --- Si un seul peut être réduit, on le réduit ---
                    else if (canReduceU) {
                        dist[u] = bestU;
                        changed = true;
                    }
                    else if (canReduceV) {
                        dist[v] = bestV;
                        changed = true;
                    }
                    // --- Sinon aucune réduction possible -> tenter augmentation minimale ---
                    else {
                        ll newU = dist[u], newV = dist[v];
                        bool canIncreaseU = false, canIncreaseV = false;

                        for (ll val = d + 1; val <= n; ++val) {
                            if (canSet(u, val, dist)) { newU = val; canIncreaseU = true; break; }
                        }
                        for (ll val = d + 1; val <= n; ++val) {
                            if (canSet(v, val, dist)) { newV = val; canIncreaseV = true; break; }
                        }

                        // Si les deux peuvent être augmentés -> choix aléatoire
                        if (canIncreaseU && canIncreaseV) {
                            if (coin(gen) == 0) dist[u] = newU;
                            else dist[v] = newV;
                            changed = true;
                        }
                        else if (canIncreaseU) {
                            dist[u] = newU; changed = true;
                        }
                        else if (canIncreaseV) {
                            dist[v] = newV; changed = true;
                        }
                    }
                }
            }
        }

        // --- Étape 2 : réduire les distances trop grandes (deg < dist) ---
        for (int w = 0; w < n; ++w) {
            if (deg[w] < dist[w]) {
                for (ll val = dist[w] - 1; val >= 1; --val) {
                    if (canSet(w, val, dist)) {
                        dist[w] = val;
                        changed = true;
                        break;
                    }
                }
            }
        }

        // --- Étape 3 : casser les égalités restantes si besoin (petite hausse fine) ---
        for (int u = 0; u < n; ++u) {
            for (int v = u + 1; v < n; ++v) {
                if (adj[u][v] && dist[u] == dist[v]) {
                    for (ll val = dist[v] + 1; val <= n; ++val) {
                        if (canSet(v, val, dist)) {
                            dist[v] = val;
                            changed = true;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void afficher(vector<vector<int> >& P)
{
    cout << '{';
    for (int i = 0; i < P.size(); i++)
    {
        cout << '{';
        for (int j = 0; j < P[i].size(); j++)
        {
            cout << P[i][j] << ' ';
        }
        cout << "} ";
    }
    cout << '}' << endl;
}

void afficher(vector<int> &P)
{
    cout << '{';
    for (int i = 0; i < P.size(); i++)
    {
        cout << P[i] << ' ';
    }
    cout << '}' << endl;
}

void afficher(vector<vector<pair<int, ll>>>& P)
{
    cout << '{';
    for (int i = 0; i < P.size(); i++)
    {
        cout << '{';
        for (int j = 0; j < P[i].size(); j++)
        {
            cout << '(' << P[i][j].first << ',' << P[i][j].second << ") ";
        }
        cout << "} ";
    }
	cout << '}' << endl;
}

void lectureDimacs(unsigned int& n, unsigned int& m, unsigned int& delta, vector<vector<int> >& A)
{
    ifstream data("dimacs.txt");
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
        A[i][s1 - 1] = 1;
        A[i][s2 - 1] = 1;
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
    //toCplexData(n, m, delta, A);
}

int main() {
    // --- Lecture du fichier ---
    unsigned int n, m, delta;
    /*ifstream fin("input.txt");
    if (!fin) {
        cerr << "Erreur : impossible d'ouvrir le fichier graphe.txt\n";
        return 1;
    }

    
    fin >> n >> m;

    vector<vector<int>> incidence(m, vector<int>(n, 0));
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            fin >> incidence[i][j];
    fin.close();*/

	vector<vector<int>> incidence;
    lectureDimacs(n, m, delta, incidence);

	cout << "=== Matrice d’incidence ===\n";
	//afficher(incidence);
	cout << endl;

    // --- Construction de la matrice d’adjacence ---
    vector<vector<int>> adj(n, vector<int>(n, 0));
    for (int e = 0; e < m; ++e) {
        vector<int> endpoints;
        for (int v = 0; v < n; ++v)
            if (incidence[e][v] == 1)
                endpoints.push_back(v);

        if (endpoints.size() == 2) {
            int u = endpoints[0];
            int v = endpoints[1];
            adj[u][v] = adj[v][u] = 1;
        }
    }

	cout << "=== Matrice d’adjacence ===\n";
	//afficher(adj);
	cout << endl;

    // --- Degrés ---
    vector<int> deg(n, 0);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (adj[i][j]) deg[i]++;

	cout << "=== Degrés des sommets ===\n";
	//afficher(deg);
	cout << endl;

    // --- Construction du graphe orienté ---
    int S = n;
    int T = n + 1;
    int N = n + 2;

    vector<vector<pair<int, ll>>> G(N);
    vector<int> indeg(n, 0), outdeg(n, 0);

    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) {
            if (!adj[u][v]) continue;
            if (deg[u] < deg[v]) {
                G[u].push_back({ v, deg[v] - deg[u] });
                outdeg[u]++; indeg[v]++;
            }
            else if (deg[v] < deg[u]) {
                G[v].push_back({ u, deg[u] - deg[v] });
                outdeg[v]++; indeg[u]++;
            }
            else {
                G[u].push_back({ v, 1 });
                G[v].push_back({ u, 1 });
                outdeg[u]++; indeg[v]++;
                outdeg[v]++; indeg[u]++;
            }
        }
    }

    // --- Ajout de la source et du puits ---
    for (int v = 0; v < n; ++v)
        if (indeg[v] == 0)
            G[S].push_back({ v, 1 });
    int minSucc = *min_element(outdeg.begin(), outdeg.end());
    for (int v = 0; v < n; ++v)
        if (outdeg[v] == minSucc)
            G[v].push_back({ T, 0 });
    
    // --- Vérification de la connexité à partir de la source ---
    // On vérifie quels sommets sont atteignables depuis la source S
    vector<int> reachable(N, 0);
    queue<int> q;
    q.push(S);
    reachable[S] = 1;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (auto& [v, w] : G[u]) {
            if (!reachable[v]) {
                reachable[v] = 1;
                q.push(v);
            }
        }
    }

    // Pour chaque sommet non atteignable (dans le graphe orienté),
    // on ajoute un arc source → sommet avec poids = 1
    for (int v = 0; v < n; ++v) {
        if (!reachable[v]) {
            G[S].push_back({ v, 1 });
            cout << "[Ajout] Arc manquant : source → v" << v << " (poids = 1)\n";
        }
    }

    cout << "=== Graphe orienté avec S et T ===\n";
	//afficher(G);
	cout << endl;

	// --- Affichage des degrés entrants et sortants ---
	cout << "=== Degrés entrants et sortants ===\n";
	cout << "Indeg : ";
	afficher(indeg);
	cout << "Outdeg: ";
	afficher(outdeg);
	cout << endl;

    // --- Dijkstra ---
    vector<ll> dist(N, INF);
    vector<int> parent(N, -1);
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<>> pq;
    dist[S] = 0;
    pq.push({ 0, S });

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
		cout << "Visite de v" << u << " avec distance " << d << "\n";
        if (d != dist[u]) continue;
        for (auto& [v, w] : G[u]) {
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({ dist[v], v });
				cout << "\tMise à jour de v" << v << " avec distance " << dist[v] << "\n";
            }
        }
    }

    cout << "=== Distances initiales ===\n";
    for (int i = 0; i < n; ++i)
        cout << "v" << i << " : " << dist[i] << "\n";

    // --- 🔧 Ajustement des distances ---
    adjustDistances(dist, adj, G, deg, n, S, T);
	//adjustDistancesRandomized(dist, adj, G, deg, n, S, T);

    cout << "\n=== Distances ajustées ===\n";
    for (int i = 0; i < n; ++i)
        cout << "v" << i << " : " << dist[i] << "\n";

	// --- Calcul du coût total ---
	ll totalCost = 0;

    for (int u = 0; u < n; ++u) {
        totalCost += dist[u];
    }

    // -----------------------------------------------------------
// Vérification : aucun couple de sommets adjacents ne doit avoir la même distance
// -----------------------------------------------------------
    bool ok = true;
    vector<pair<int, int>> conflicts;

    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) {
            if (adj[u][v] && dist[u] == dist[v]) {
                ok = false;
                conflicts.push_back({ u, v });
            }
        }
    }

    if (ok) {
        cout << "✅ Vérification réussie : aucun sommet adjacent n'a la même distance." << endl;
    }
    else {
        cout << "⚠️  Conflit détecté : certains sommets adjacents ont la même distance !" << endl;
        for (auto [u, v] : conflicts) {
            cout << "   Sommet " << u << " et Sommet " << v
                << " sont adjacents avec distance = " << dist[u] << endl;
        }
    }

    cout << "\nCoût total de la solution du MSCP : " << totalCost << "\n";

    return 0;
}
