#include <algorithm>
#include <fstream>
#include <set>
#include <unordered_set>
#include <utility>
#include <queue>

#include "ModularChromaticSum.h"

#define ITER_MAX 1
#define ITER_EBTS 10000
#define TABU_TENURE 12

using namespace std;

struct myclass {
	bool operator() (vector<int> i, vector<int> j) { return (i.size() > j.size()); }
} sortBySize;

struct myclass2 {
	int getEvaluation(const vector<vector<int> >& solution)
	{
		int eval = 0;
		for (int i = 0; i < solution.size(); i++)
		{
			eval += solution[i].size() * (i + 1);
		}
		return eval;
	}
	bool operator() (vector<vector<int>> i, vector<vector<int>> j) { return (getEvaluation(i) < getEvaluation(j)); }
} sortByEvaluation;

struct comparatorVector
{
	inline bool operator() (const vector<int>& struct1, const vector<int>& struct2)
	{
		return (struct1.size() > struct2.size());
	}
};

struct comparatorVector2
{
	inline bool operator() (const int& struct1, const int& struct2)
	{
		return (struct1 > struct2);
	}
};
class comparatorVector3
{
	bool reverse;
	int n;
public:
	comparatorVector3(const int& n_, const bool& revparam = false)
	{
		n = n_;
		reverse = revparam;
	}
	bool operator() (const int** S1, const int** S2)
	{
		int eval1 = 0;
		for (int j = 0; j < n; j++)
		{
			int nb = 0;
			for (int i = 0; i < n; i++)
			{
				nb = nb + S1[i][j];
			}
			eval1 += nb * (j + 1);
		}
		int eval2 = 0;
		for (int j = 0; j < n; j++)
		{
			int nb = 0;
			for (int i = 0; i < n; i++)
			{
				nb = nb + S2[i][j];
			}
			eval2 += nb * (j + 1);
		}
		return (eval1 > eval2);
	}
};

// Hash function
struct hashFunction
{
	size_t operator()(const vector<int>& myVector) const
	{
		hash<int> hasher;
		size_t answer = 0;

		for (int i : myVector)
		{
			answer ^= hasher(i) + 0x9e3779b9 + (answer << 6) + (answer >> 2);
		}
		return answer;
	}
};

int amelioratif = 0, inutil = 0, couteux = 0, interdit = 0, prometeuxFort1 = 0, prometeuxFort2 = 0, prometeuxFaible1 = 0, prometeuxFaible2 = 0, vraiAmelioration = 0, nsrn = 0, nbMove = 0, nbNS2 = 0, interditChoix = 0;
ofstream out("output16.txt");
ofstream cost("cost_16.csv");
typedef priority_queue<int**, vector<int**>, comparatorVector3> listSolutions;

void ecrireData2(vector<vector<int> >& A, unsigned int& m, unsigned int& n, unsigned int& delta, vector<int>& p)
{
	ofstream model;
	model.open("OCCP.dat", ofstream::trunc);
	model << "n= " << n << ";\nm= " << m << ";\ndelta= " << delta << ";\nA =[";
	for (unsigned int i = 0; i < m; i++)
	{
		model << "[ ";
		for (unsigned int j = 0; j < n; j++)
		{
			model << A[i][j] << " ";
		}
		if (i != (m - 1))
		{
			model << "]," << endl;
		}
		else
		{
			model << "]];" << endl;
		}
	}
	model << " p = [";
	for (unsigned int i = 0; i < p.size(); i++)
	{
		model << p[i] << ' ';
	}
	model << "];";
	model.close();
}

void recurseAdd(vector<int>& B, vector<Module>& t, int& module)
{
	int ind;
	for (int i = 0; i < t.size(); i++)
	{
		if (t[i].getNom() == module)
		{
			ind = i;
			break;
		}
	}
	if (t[ind].getSommets().size() == 1)
	{
		B.push_back(t[ind].getSommets()[0]);
	}
	else
	{
		for (int i = 0; i < t[ind].getSommets().size(); i++)
		{
			recurseAdd(B, t, t[ind].getSommets()[i]);
		}
	}
}

vector<vector<int> > modulesToList(vector<Module>& modules)
{
	vector<vector<int> > listModules;
	for (int i = 0; i < modules.size(); i++)
	{
		if (modules[i].getNiveau() == 1) {
			vector<int> inter;
			int depart = modules[i].getNom();
			recurseAdd(inter, modules, depart);
			listModules.push_back(inter);
		}
	}
	return listModules;
}

void afficher(map<int, int>& P)
{
	std::cout << '{' << endl;
	for (map<int, int>::iterator it = P.begin(); it != P.end(); it++)
	{
		std::cout << it->first << " : " << it->second << endl;
	}
	std::cout << '}' << endl;
}

map<int, vector<vector<Module>::iterator>, classcomp > trieModuleParNiveau(vector<Module>& listModule, int& nivMax)
{
	nivMax = 0;
	for (vector<Module>::iterator it = listModule.begin(); it != listModule.end(); it++)
	{
		if ((*it).getNiveau() > nivMax)
		{
			nivMax = (*it).getNiveau();
		}
	}
	map<int, vector<vector<Module>::iterator>, classcomp > modulesTries;
	for (int i = 0; i <= nivMax; i++)
	{
		vector<vector<Module>::iterator> inter;
		modulesTries[i] = inter;
	}
	for (vector<Module>::iterator it = listModule.begin(); it != listModule.end(); it++)
	{
		modulesTries[(*it).getNiveau()].push_back(it);
	}
	return modulesTries;
}

void getCout(vector<vector<int> >& solution, unsigned int& z)
{
	z = 0;
	for (int i = 0; i < solution.size(); i++)
	{
		z += solution[i].size() * (i + 1);
	}
}

bool containsValue(vector<int>& c, int val)
{
	for (vector<int>::iterator it = c.begin(); it != c.end(); ++it)
	{
		if (*it == val)
		{
			return true;
		}
	}
	return false;
}

map<int, vector<int> > gToListAdj2(vector<vector<int> >& A)
{
	map<int, vector<int> > out;
	for (int j = 0; j < A[0].size(); j++)
	{
		vector<int> inter;
		for (int i = 0; i < A.size(); i++)
		{
			if (A[i][j] == 1)
			{
				for (int k = 0; k < A[0].size(); k++)
				{
					if (j != k && A[i][k] == 1)
					{
						if (!containsValue(inter, k))
						{
							inter.push_back(k);
						}
					}
				}
			}
		}
		out[j] = inter;
	}
	/*cout << "Graphe G : " << endl;
	afficher(out);*/
	return out;
}

void maxVec(vector<int>& deg, unsigned int& max_, unsigned int& ind)
{
	max_ = deg[0]; ind = 0;
	for (int i = 1; i < deg.size(); i++)
	{
		if (deg[i] > max_)
		{
			max_ = deg[i];
			ind = i;
		}
	}
	max_++;
}

bool isNull(int** S, int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (S[i][j] != 0)
			{
				return false;
			}
		}
	}
	return true;
}

map<int, vector<int> > GminusS(map<int, vector<int> >& G, vector<int>& S)
{
	map<int, vector<int> > GS(G);
	for (int i = 0; i < S.size(); i++)
	{
		GS.erase(S[i]);
	}
	for (int i = 0; i < S.size(); i++)
	{
		for (map<int, vector<int> >::iterator it = GS.begin(); it != GS.end(); ++it)
		{
			for (vector<int>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			{
				if (S[i] == *it2)
				{
					GS[it->first].erase(it2);
					break;
				}
			}
		}
	}
	return GS;
}

void ConstructedGreedyRandomizedSolution(map<int, vector<int> >& G, vector<int>& S)
{
	vector<int> sommets;
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); ++it)
	{
		sommets.push_back(it->first);
	}
	while (!sommets.empty())
	{
		int e = rand() % sommets.size();
		S.push_back(sommets[e]);
		for (vector<int>::iterator it = G[S[S.size() - 1]].begin(); it != G[S[S.size() - 1]].end(); ++it)
		{
			for (vector<int>::iterator it2 = sommets.begin(); it2 != sommets.end(); ++it2)
			{
				if (*it == *it2)
				{
					sommets.erase(it2);
					break;
				}
			}
		}
		for (vector<int>::iterator it = sommets.begin(); it != sommets.end(); ++it)
		{
			if (*it == S[S.size() - 1])
			{
				sommets.erase(it);
				break;
			}
		}
	}
}

void ConstructedGreedyRandomizedSolution(map<int, vector<int> >& G, vector<vector<int> >& S)
{
	map<int, vector<int> > G_(G);
	while (!G_.empty())
	{
		vector<int> S_;
		ConstructedGreedyRandomizedSolution(G_, S_);
		S.push_back(S_);
		G_ = GminusS(G_, S_);
	}
	std::sort(S.begin(), S.end(), comparatorVector());
}

void ConstructedGreedyRandomizedSolution(map<int, vector<int> >& G, int** S)
{
	vector<vector<int> > S_;
	ConstructedGreedyRandomizedSolution(G, S_);
	map<int, int> link;
	int pos = 0;
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
	{
		link[it->first] = pos;
		pos++;
	}
	for (int i = 0; i < S_.size(); i++)
	{
		for (int j = 0; j < S_[i].size(); j++)
		{
			S[link[S_[i][j]]][i] = 1;
		}
	}
}

void ConstructedGreedyRandomizedSolution(map<int, vector<int> >& G, vector<int>& S, int** DM)
{
	vector<int> sommets;
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); ++it)
	{
		sommets.push_back(it->first);
	}
	int e, ee;
	while (!sommets.empty())
	{
		e = -1, ee = -1;
		for (int i = 0; i < (sommets.size() - 1); i++)
		{
			for (int j = i + 1; j < sommets.size(); j++)
			{
				if (DM[sommets[i]][sommets[j]] == 0)
				{
					e = i;
					ee = j;
					break;
				}
			}
			if (e != 1)
			{
				break;
			}
		}
		if (e != -1)
		{
			S.push_back(sommets[e]);
			S.push_back(sommets[ee]);
			for (vector<int>::iterator it = G[S[S.size() - 1]].begin(); it != G[S[S.size() - 1]].end(); ++it)
			{
				for (vector<int>::iterator it2 = sommets.begin(); it2 != sommets.end(); ++it2)
				{
					if (*it == *it2)
					{
						sommets.erase(it2);
						break;
					}
				}
			}
			for (vector<int>::iterator it = sommets.begin(); it != sommets.end(); ++it)
			{
				if (*it == S[S.size() - 1])
				{
					sommets.erase(it);
					break;
				}
			}
			for (vector<int>::iterator it = G[S[S.size() - 2]].begin(); it != G[S[S.size() - 2]].end(); ++it)
			{
				for (vector<int>::iterator it2 = sommets.begin(); it2 != sommets.end(); ++it2)
				{
					if (*it == *it2)
					{
						sommets.erase(it2);
						break;
					}
				}
			}
			for (vector<int>::iterator it = sommets.begin(); it != sommets.end(); ++it)
			{
				if (*it == S[S.size() - 2])
				{
					sommets.erase(it);
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	while (!sommets.empty())
	{
		e = rand() % sommets.size();
		S.push_back(sommets[e]);
		for (vector<int>::iterator it = G[S[S.size() - 1]].begin(); it != G[S[S.size() - 1]].end(); ++it)
		{
			for (vector<int>::iterator it2 = sommets.begin(); it2 != sommets.end(); ++it2)
			{
				if (*it == *it2)
				{
					sommets.erase(it2);
					break;
				}
			}
		}
		for (vector<int>::iterator it = sommets.begin(); it != sommets.end(); ++it)
		{
			if (*it == S[S.size() - 1])
			{
				sommets.erase(it);
				break;
			}
		}
	}
}

void ConstructedGreedyRandomizedSolution(map<int, vector<int> >& G, vector<vector<int> >& S, int** DM)
{
	map<int, vector<int> > G_(G);
	while (!G_.empty())
	{
		vector<int> S_;
		ConstructedGreedyRandomizedSolution(G_, S_, DM);
		S.push_back(S_);
		G_ = GminusS(G_, S_);
	}
	std::sort(S.begin(), S.end(), comparatorVector());
}

void ConstructedGreedyRandomizedSolution(map<int, vector<int> >& G, int** S, int** DM)
{
	vector<vector<int> > S_;
	ConstructedGreedyRandomizedSolution(G, S_, DM);
	for (int i = 0; i < S_.size(); i++)
	{
		for (int j = 0; j < S_[i].size(); j++)
		{
			S[S_[i][j]][i] = 1;
		}
	}
}

int getEvaluation(const vector<vector<int> >& solution)
{
	int eval = 0;
	for (int i = 0; i < solution.size(); i++)
	{
		eval += solution[i].size() * (i + 1);
	}
	return eval;
}

int getEvaluation(vector<int>& taille)
{
	int eval = 0;
	for (int i = 0; i < taille.size(); i++)
	{
		eval += taille[i] * (i + 1);
	}
	return eval;
}

int getEvaluation(int** S, int n)
{
	int eval = 0;
	for (int j = 0; j < n; j++)
	{
		int nb = 0;
		for (int i = 0; i < n; i++)
		{
			nb = nb + S[i][j];
		}
		eval += nb * (j + 1);
	}
	return eval;
}

void updateDM(int** DM, int** S, int n)
{
	for (int j = 0; j < n; j++)
	{
		for (int i = 0; i < (n - 1); i++)
		{
			if (S[i][j] == 1)
			{
				for (int k = i + 1; k < n; k++)
				{
					if (S[k][j] == 1)
					{
						DM[i][k]++;
						DM[k][i]++;
					}
				}
			}
		}
	}
}

int getTaille(int** S, int n)
{
	int nb = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (S[j][i] != 0)
			{
				nb++;
				break;
			}
		}
	}
	return nb;
}

void afficherSolutionOut(map<int, vector<int> >& G, int** P, int n, int m)
{
	out << '{';
	for (int i = 0; i < m; i++)
	{
		out << '{';
		map<int, vector<int> >::iterator it = G.begin();
		for (int j = 0; j < n; j++)
		{
			if (P[j][i] == 1)
			{
				out << it->first << ' ';
			}
			it++;
		}
		out << "} ";
	}
	out << '}' << endl;
}

void afficherSolutionOut(int** P, int n, int m)
{
	out << '{';
	for (int i = 0; i < m; i++)
	{
		out << '{';
		for (int j = 0; j < n; j++)
		{
			if (P[j][i] == 1)
			{
				out << j << ' ';
			}
		}
		out << "} ";
	}
	out << '}' << endl;
}

void afficherSolution(int** P, int n, int m, vector<int>& reverseLink)
{
	std::cout << '{';
	for (int i = 0; i < m; i++)
	{
		std::cout << '{';
		for (int j = 0; j < n; j++)
		{
			if (P[j][i] == 1)
			{
				std::cout << reverseLink[j] << ' ';
			}
		}
		std::cout << "} ";
	}
	std::cout << '}' << endl;
}

void afficherMatrice(int** P, int n, int m)
{
	std::cout << '{';
	for (int i = 0; i < n; i++)
	{
		std::cout << '{';
		for (int j = 0; j < m; j++)
		{
			std::cout << P[i][j] << ' ';
		}
		std::cout << "} ";
	}
	std::cout << '}' << endl;
}

void afficher(vector<vector<int> >& P)
{
	std::cout << '{';
	for (int i = 0; i < P.size(); i++)
	{
		std::cout << '{';
		for (int j = 0; j < P[i].size(); j++)
		{
			std::cout << P[i][j] << ' ';
		}
		std::cout << "} ";
	}
	std::cout << '}' << endl;
}

void afficher(vector<int>& P)
{
	std::cout << '{';
	for (int i = 0; i < P.size(); i++)
	{
		std::cout << P[i] << ' ';
	}
	std::cout << '}' << endl;
}

bool inTabu(int** tabu, int& j, int& n, int** S)
{
	int pos = -1;
	for (int ind = 0; ind < n; ind++)
	{
		if (S[j][ind] == 1)
		{
			pos = ind;
			break;
		}
	}
	for (int i = 0; i < n; i++)
	{
		if (S[i][pos] == 1 && i != j)
		{
			if (tabu[j][i] > 0)
			{
				return true;
			}
		}
	}
	return false;
}

bool inTabu(int** tabu, int& sommet, int& stable)
{
	if (tabu[sommet][stable] > 0)
	{
		return true;
	}
	return false;
}

bool inTabu(map<vector<int>, int>& tabu, vector<int>& solution)
{
	if (tabu.find(solution) != tabu.end())
	{
		return true;
	}
	return false;
}

void transform(vector<int>& reverseLink, vector<vector<int> >& solution, int** S, int n)
{
	for (int i = 0; i < n; i++)
	{
		vector<int> inter;
		for (int j = 0; j < n; j++)
		{
			if (S[j][i] == 1)
			{
				inter.push_back(reverseLink[j]);
			}
		}
		if (inter.empty())
		{
			continue;
		}
		solution.push_back(inter);
	}
}

void triSolution(int** S, int n)
{
	vector<int> taille;
	for (int i = 0; i < n; i++)
	{
		int nb = 0;
		for (int j = 0; j < n; j++)
		{
			nb += S[j][i];
		}
		taille.push_back(nb);
	}
	//cout << "\ttaille : " << endl;
	//afficher(taille);
	int max = -1, ind = -1;
	for (int i = 0; i < (n - 1); i++)
	{
		max = taille[i];
		ind = i;
		for (int j = i + 1; j < n; j++)
		{
			if (max < taille[j])
			{
				max = taille[j];
				ind = j;
			}
		}
		if (i != ind)
		{
			taille[ind] = taille[i];
			taille[i] = max;
			vector<int> s_(n);
			for (int j = 0; j < n; j++)
			{
				s_[j] = S[j][ind];
				S[j][ind] = S[j][i];
				S[j][i] = s_[j];
			}
		}
	}
}

void triSolution(map<int, vector<int> >& Gq, int** S, int n, vector<vector<int> >& serie, vector<vector<int> >& parallele)
{
	vector<int> taille;
	for (int i = 0; i < Gq.size(); i++)
	{
		int nb = 0;
		map<int, vector<int> >::iterator it = Gq.begin();
		for (int j = 0; j < Gq.size(); j++)
		{
			if (it->first < (n + serie.size()))
			{
				nb += S[j][i];
			}
			else
			{
				nb += parallele[it->first - n - serie.size()].size() * S[j][i];
			}
			it++;
		}
		taille.push_back(nb);
	}
	//out << "\ttaille : " << endl;
	//afficherOut(taille);
	out << endl;
	int max = -1, ind = -1;
	for (int i = 0; i < (Gq.size() - 1); i++)
	{
		max = taille[i];
		ind = i;
		for (int j = i + 1; j < Gq.size(); j++)
		{
			if (max < taille[j])
			{
				max = taille[j];
				ind = j;
			}
		}
		if (i != ind)
		{
			taille[ind] = taille[i];
			taille[i] = max;
			vector<int> s_(n);
			for (int j = 0; j < Gq.size(); j++)
			{
				s_[j] = S[j][ind];
				S[j][ind] = S[j][i];
				S[j][i] = s_[j];
			}
		}
	}
}

// Vérifie si a domine b
bool dominates(const vector<vector<int>>& a, const vector<vector<int>>& b)
{
	if (a.size() > b.size())
		return false;

	if (getEvaluation(a) > getEvaluation(b))
		return false;

	return true;
}

void insertIfValid(vector<vector<vector<int>>>& uncontrolled, const vector<vector<int>>& solution)
{
	int solSum = getEvaluation(solution);

	// 1) Vérifier si solution est dominée par un élément existant
	for (const auto& t : uncontrolled)
	{
		if (t.size() <= solution.size() && getEvaluation(t) <= solSum)
		{
			return; // solution rejetée
		}
	}

	// 2) Supprimer tous les t dominés par solution
	for (auto it = uncontrolled.begin(); it != uncontrolled.end(); )
	{
		if (dominates(solution, *it) && *it != solution)
			it = uncontrolled.erase(it);
		else
			++it;
	}

	// 3) Insérer solution
	uncontrolled.push_back(solution);
}

void keepOnlyNonDominated(vector<vector<vector<int>>>& M)
{
	for (auto it = M.begin(); it != M.end(); )
	{
		bool dominated = false;

		for (auto jt = M.begin(); jt != M.end(); ++jt)
		{
			if (it == jt) continue;

			if (dominates(*jt, *it) && !dominates(*it, *jt))
			{
				dominated = true;
				break;
			}
		}

		if (dominated)
			it = M.erase(it);
		else
			++it;
	}
}

// Deep-copy an int** matrix (rows x cols)
int** copyMatrix(int** src, int rows, int cols)
{
	if (src == nullptr) return nullptr;
	int** dst = new int* [rows];
	for (int i = 0; i < rows; ++i)
	{
		dst[i] = new int[cols];
		for (int j = 0; j < cols; ++j)
			dst[i][j] = src[i][j];
	}
	return dst;
}

void freeMatrix(int** mat, int rows)
{
	if (mat == nullptr) return;
	for (int i = 0; i < rows; ++i)
		delete[] mat[i];
	delete[] mat;
}

// (k,1)-exchange + min diversification + tabu list bi-naire + DataMining + DM Greedy
void EBTS8(map<int, vector<int> >& G, int** S, int** S0, int& Z, int** DM, map<int, int>& link, vector<int>& reverseLink, vector<vector<vector<int> > >& uncontrolled, vector<Module>& listModules, int n)
{
	//cout << "G : " << endl;
	//cout << '{' << endl;
	//for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
	//{
		//cout << it->first << " : ";
		//for (int j = 0; j < (it->second).size(); j++)
		//{
			//cout << (it->second)[j] << ' ';
		//}
		//cout << endl;
	//}
	//cout << '}' << endl;
	//map<pair<int, int>, int> tabuList;
	//int** tabuList = new int* [G.size()];
	//int** moveTabuList = new int* [G.size()];
	map<vector<int>, int> tabuListVec;
	/*for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
		moveTabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
			moveTabuList[i][j] = 0;
		}
	}*/
	int nb = (G.size() < 20) ? 100 : (G.size() < 50) ? 1000 : (G.size() < 100) ? 2000 : (G.size() < 501) ? 5000 : ITER_EBTS;
	//nb = 50;
	int** K = new int* [G.size()], ** B = new int* [G.size()];
	for (int j = 0; j < G.size(); j++)
	{
		K[j] = new int[G.size() + 1];
		B[j] = new int[G.size() + 1];
	}
	for (int iii = 0; iii < nb; iii++)
	{
		//out << "iter = " << i << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		/*cout << "taille S0 : " << endl;
		vector<int> ss(getSTaille(S0, G.size()));
		afficher(ss);
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		int nn = 0;
		for (int ii = 0; ii < ss.size(); ii++)
		{
		nn += ss[ii];
		}
		if (nn > 125 || nn < 125)
		{
		system("pause");
		}*/
		//int** K = new int* [G.size()], ** B = new int* [G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j_ = 0; j_ < G.size(); j_++)
		{
			for (int k_ = 0; k_ < tailleInitial + 1; k_++)
			{
				K[j_][k_] = 0;
				B[j_][k_] = 0;
			}
		}
		/*for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}*/
		//cout << "Taille de G : " << G.size() << endl;
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			//cout << "it->first = " << it->first << endl;
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[link[it->first]][j] == 1)
				{
					K[link[it->first]][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[link[G[it->first][h]]][k] == 1)
								{
									nb++;
								}
							}
							K[link[it->first]][k] = nb;
						}
					}
					break;
				}
			}
			K[link[it->first]][tailleInitial] = 0;
		}
		//cout << "K = " << endl;
		//afficherMatrice(K, (int)G.size(), (int)(tailleInitial + 1));
		//calcul de B et choix de l'élément à déplacer
		vector<int> taille, position;
		vector<pair<int, int> > NS0, NS1, NS2;
		pair<int, int> choix;
		int tailleActuel = getTaille(S0, G.size());
		int minP = G.size() * tailleInitial, indLP = -1, indCP = -1, minN = G.size() * tailleInitial, indLN = -1, indCN = -1;
		int maxP = G.size() * tailleInitial * (-1);
		for (int j = 0; j < tailleActuel; j++)
		{
			int nb = 0;
			for (int i = 0; i < G.size(); i++)
			{
				nb += S0[i][j];
			}
			taille.push_back(nb);
		}
		taille.push_back(0);
		for (int j = 0; j < G.size(); j++)
		{
			bool nt = false;
			for (int k = 0; k < tailleActuel; k++)
			{
				if (S0[j][k] == 1)
				{
					position.push_back(k);
					nt = true;
					break;
				}
			}
			if (!nt)
			{
				position.push_back(-1);
			}
		}
		/*cout << "taille : " << endl;
		afficher(taille);
		cout << "position : " << endl;
		afficher(position);*/
		for (int jjj = 0; jjj < G.size(); jjj++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[jjj][k] == -1)
				{
					B[jjj][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[jjj]]--;
					t[k]++;
					for (int h = 0; h < G[reverseLink[jjj]].size(); h++)
					{
						if (K[link[G[reverseLink[jjj]][h]]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[link[G[reverseLink[jjj]][h]]][r] == 0) || (r == position[jjj] && K[link[G[reverseLink[jjj]][h]]][r] == 1)))
								{
									t[r]++;
									t[k]--;
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								t[tailleInitial]++;
								t[k]--;
							}
						}
					}
					std::sort(t.begin(), t.end(), comparatorVector2());
					B[jjj][k] = getEvaluation(t) - getEvaluation(taille);
					int** S_ = copyMatrix(S0, n, n);
					S_[jjj][k] = 1;
					S_[jjj][position[k]] = 0;
					for (int h = 0; h < G[reverseLink[jjj]].size(); h++)
					{
						if (K[link[G[reverseLink[jjj]][h]]][k] == -1)
						{
							bool trouver = false;
							for (int f = 0; f < tailleInitial; f++)
							{
								if (K[link[G[reverseLink[jjj]][h]]][f] == 0)
								{
									S_[link[G[reverseLink[jjj]][h]]][f] = 1;
									S_[link[G[reverseLink[jjj]][h]]][k] = 0;
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								//cout << "inserted out with tailleInitial = " << tailleInitial << endl;
								S_[link[G[reverseLink[jjj]][h]]][tailleInitial] = 1;
								S_[link[G[reverseLink[jjj]][h]]][k] = 0;
							}
						}
					}
					triSolution(S_, n);
					vector<vector<int> > S0_;
					transform(reverseLink, S0_, S0, G.size());
					vector<int> tailleS0_;
					for (int ii = 0; ii < S0_.size(); ii++)
					{
						tailleS0_.push_back(S0_[ii].size());
					}
					// Libérer la mémoire
					freeMatrix(S_, n); // n == G.size() in that scope
					if (!inTabu(tabuListVec, tailleS0_)/*!inTabu(tabuList, j, n, S_) && !inTabu(moveTabuList, j, k)*/)
					{
						if (B[jjj][k] > 0)
						{
							if (minP > B[jjj][k])
								//if(maxP < B[j][k])
							{
								indLP = jjj;
								indCP = k;
								minP = B[jjj][k];
								//maxP = B[j][k];
							}
							if (NS2.empty())
							{
								NS2.push_back(make_pair(jjj, k));
							}
							else
							{
								for (int ind = 0; ind < NS2.size(); ind++)
								{
									if (B[NS2[ind].first][NS2[ind].second] > B[jjj][k])
									{
										NS2.insert(NS2.begin() + ind, make_pair(jjj, k));
										break;
									}
								}
							}
						}
						else if (B[jjj][k] < 0)
						{
							if (minN > B[jjj][k])
							{
								indLN = jjj;
								indCN = k;
								minN = B[jjj][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(make_pair(jjj, k));
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[jjj][k])
									{
										NS0.insert(NS0.begin() + ind, make_pair(jjj, k));
										break;
									}
								}
							}
						}
						else
						{
							indLP = jjj;
							indCP = k;
							NS1.push_back(make_pair(jjj, k));
						}
					}
				}
			}
		}

		//cout << "B = " << endl;
		//afficherMatrice(B, (int)G.size(), (int)(tailleInitial + 1));
		//cout << "Tabu List : " << endl;
		//afficherMatrice(tabuList, G.size(), G.size());
		//cout << "Moves Tabu List : " << endl;
		//afficherMatrice(moveTabuList, G.size(), G.size());
		//cout << "DM = " << endl;
		//afficherMatrice(DM, (int)G.size(), (int)G.size());
		if (!NS0.empty() && ((getEvaluation(S0, G.size()) + minN) < Z))
		{
			choix = make_pair(indLN, indCN);
			amelioratif++;
			//out << "Choix amelioratif (intensification Normale)" << endl;
		}
		else if (indCN != -1 && ((getEvaluation(S0, G.size()) + minN) >= Z))
		{
			bool trouver = false;
			for (int j = 0; j < NS0.size(); j++)
			{
				for (int k = 0; k < G.size(); k++)
				{
					if (reverseLink[k] != reverseLink[NS0[j].first])
					{
						if (find(G[reverseLink[NS0[j].first]].begin(), G[reverseLink[NS0[j].first]].end(), reverseLink[k]) == G[reverseLink[NS0[j].first]].end())
						{
							if (position[k] == NS0[j].second && DM[NS0[j].first][k] == 0)
							{
								choix = NS0[j];
								trouver = true;
								prometeuxFort1++;
								//out << "Prometeux Fort (new intensification)" << endl;
								break;
							}
						}
					}
				}
				if (trouver)
				{
					break;
				}
			}
			if (!trouver)
			{
				int ind = -1, maxDM = -1;
				for (int j = 0; j < NS0.size(); j++)
				{
					for (int k = 0; k < G.size(); k++)
					{
						if (k != NS0[j].first)
						{
							if (find(G[reverseLink[NS0[j].first]].begin(), G[reverseLink[NS0[j].first]].end(), reverseLink[k]) == G[reverseLink[NS0[j].first]].end())
							{
								if (position[k] == NS0[j].second && DM[NS0[j].first][k] > maxDM)
								{
									maxDM = DM[NS0[j].first][k];
									ind = j;
								}
							}
						}
					}
				}
				if (ind != -1)
				{
					choix = NS0[ind];
					prometeuxFort2++;
					//out << "Prometeux Fort 2 (old intensification)" << endl;
				}
				else
				{
					choix = NS0[0];
					interdit++;
					//out << "Prometeux Fort 2 (old intensification) : premier forcé" << endl;
				}
			}
		}
		else if (!NS1.empty())
		{
			bool trouver = false;
			for (int j = 0; j < NS1.size(); j++)
			{
				for (int k = 0; k < G.size(); k++)
				{
					if (k != NS1[j].first)
					{
						if (find(G[reverseLink[NS1[j].first]].begin(), G[reverseLink[NS1[j].first]].end(), reverseLink[k]) == G[reverseLink[NS1[j].first]].end())
						{
							if (position[k] == NS1[j].second && DM[NS1[j].first][k] == 0)
							{
								choix = NS1[j];
								trouver = true;
								prometeuxFaible1++;
								//out << "Prometeux Faible 1 (new diversification)" << endl;
								break;
							}
						}
					}
				}
				if (trouver)
				{
					break;
				}
			}
			if (!trouver)
			{
				if (!NS2.empty())
				{
					bool trouver = false;
					for (int j = 0; j < NS2.size(); j++)
					{
						for (int k = 0; k < G.size(); k++)
						{
							if (k != NS2[j].first)
							{
								if (find(G[reverseLink[NS2[j].first]].begin(), G[reverseLink[NS2[j].first]].end(), reverseLink[k]) == G[reverseLink[NS2[j].first]].end())
								{
									if (position[k] == NS2[j].second && DM[NS2[j].first][k] == 0)
									{
										choix = NS2[j];
										trouver = true;
										prometeuxFaible2++;
										//out << "Prometeux Faible 2 (new diversification)" << endl;
										break;
									}
								}
							}
						}
						if (trouver)
						{
							break;
						}
					}
					if (!trouver)
					{
						int ind = -1, maxDM = -1;
						for (int j = 0; j < NS2.size(); j++)
						{
							for (int k = 0; k < G.size(); k++)
							{
								if (k != NS2[j].first)
								{
									if (find(G[reverseLink[NS2[j].first]].begin(), G[reverseLink[NS2[j].first]].end(), reverseLink[k]) == G[reverseLink[NS2[j].first]].end())
									{
										if (position[k] == NS2[j].second && DM[NS2[j].first][k] > maxDM)
										{
											maxDM = DM[NS2[j].first][k];
											ind = j;
										}
									}
								}
							}
						}
						if (ind != -1)
						{
							choix = NS2[ind];
							couteux++;
							//out << "Couteux (old diversification)" << endl;
						}
						else
						{
							choix = NS2[0];
							interdit++;
							//out << "Couteux (old diversification) : premier forcé" << endl;
						}
					}
				}
				else // Cas inutil : NS0 et NS2 vides en plus, NS1 ne vérifie pas les conditions alors on prend un cas inutil
				{
					choix = NS1[0];
					trouver = true;
					inutil++;
					//out << "Inutil" << endl;
				}
			}
		}
		else if (!NS2.empty())
		{
			bool trouver = false;
			for (int j = 0; j < NS2.size(); j++)
			{
				for (int k = 0; k < G.size(); k++)
				{
					if (k != NS2[j].first)
					{
						if (find(G[reverseLink[NS2[j].first]].begin(), G[reverseLink[NS2[j].first]].end(), reverseLink[k]) == G[reverseLink[NS2[j].first]].end())
						{
							if (position[k] == NS2[j].second && DM[NS2[j].first][k] == 0)
							{
								choix = NS2[j];
								trouver = true;
								prometeuxFaible2++;
								//out << "Prometeux Faible 2 (new diversification)" << endl;
								break;
							}
						}
					}
				}
				if (trouver)
				{
					break;
				}
			}
			if (!trouver)
			{
				int ind = -1, maxDM = -1;
				for (int j = 0; j < NS2.size(); j++)
				{
					for (int k = 0; k < G.size(); k++)
					{
						if (k != NS2[j].first)
						{
							if (find(G[reverseLink[NS2[j].first]].begin(), G[reverseLink[NS2[j].first]].end(), reverseLink[k]) == G[reverseLink[NS2[j].first]].end())
							{
								if (position[k] == NS2[j].second && DM[NS2[j].first][k] > maxDM)
								{
									maxDM = DM[NS2[j].first][k];
									ind = j;
								}
							}
						}
					}
				}
				if (ind != -1)
				{
					choix = NS2[ind];
					couteux++;
					//out << "Couteux (old diversification)" << endl;
				}
				else
				{
					choix = NS2[0];
					interdit++;
					//out << "Couteux (old diversification) : premier forcé" << endl;
				}
			}
		}
		else
		{
			indCP = rand() % tailleInitial;
			indLP = rand() % G.size();
			while (K[indLP][indCP] == -1)
			{
				indCP = rand() % tailleInitial;
				indLP = rand() % G.size();
			}
			choix = make_pair(indLP, indCP);
			interdit++;
			//out << "Choix interdit" << endl;
		}

		cout << "Choix : " << reverseLink[choix.first] << "\t| " << choix.second << endl;
		cout << "B[choix] = " << B[choix.first][choix.second] << endl;
		// Construction de la nouvelle solution
		//vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		//moveTabuList[choix.first][choix.second] = TABU_TENURE;
		vector<int> out_module1, out_module2;
		int parent = 3;
		/*for (int i = 0; i<listModules.size(); i++)
		{
			if(listModules[i].getNom() == reverseLink[choix.first])
			{
				for (int j = 0; j < listModules.size(); j++)
				{
					if(listModules[j].getNom() == listModules[i].getBouchra() && listModules[j].getDegenerate() != 0)
					{
						parent = j;
						for(int k=0; k<listModules[j].getSommets().size(); k++)
						{
							if(listModules[j].getSommets()[k] < n)
							{
								for(int h=0; h<G.size(); h++)
								{
									if(link[listModules[j].getSommets()[k]] == h)
									{
										//out_module1.push_back(h);
										moveTabuList[k][choix.second] = TABU_TENURE;
										break;
									}
								}
							}
						}
						break;
					}
					else if(listModules[j].getDegenerate() == 0)
					{
						break;
					}
				}
				break;
			}
		}
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == choix.second && find(out_module2.begin(), out_module2.end(), j) == out_module2.end())
			{
				for (int i = 0; i < listModules.size(); i++)
				{
					if (listModules[i].getNom() == reverseLink[j])
					{
						for (int k = 0; k < listModules.size(); k++)
						{
							if (listModules[k].getNom() == listModules[i].getBouchra() && listModules[k].getDegenerate() != 0)
							{
								for (int h = 0; h < listModules[k].getSommets().size(); h++)
								{
									if (listModules[k].getSommets()[h] < n)
									{
										for (int l = 0; l < G.size(); l++)
										{
											if (link[listModules[k].getSommets()[h]] == l)
											{
												out_module2.push_back(h);
												//moveTabuList[h][choix.second] = TABU_TENURE;
												break;
											}
										}
									}
								}
								break;
							}
							else if (listModules[k].getDegenerate() == 0)
							{
								break;
							}
						}
						break;
					}
				}
			}
		}*/
		//cout << "position of choix.first : " << position[choix.first] << endl;
		/*for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first] && find(out_module1.begin(), out_module1.end(), j) == out_module1.end())
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
				for (vector<int>::iterator it = out_module1.begin(); it != out_module1.end(); it++)
				{
					tabuList[j][*it] = TABU_TENURE;
					tabuList[*it][j] = TABU_TENURE;
				}
			}
		}
		for (int j = 0; j < out_module1.size(); j++)
		{
			for(int k=0; k<out_module2.size(); k++)
			{
				tabuList[out_module1[j]][out_module2[k]] = TABU_TENURE;
				tabuList[out_module2[k]][out_module1[j]] = TABU_TENURE;
			}
		}*/
		for (int j = 0; j < G[reverseLink[choix.first]].size(); j++)
		{
			if (K[link[G[reverseLink[choix.first]][j]]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[link[G[reverseLink[choix.first]][j]]][k] == 0)
					{
						S0[link[G[reverseLink[choix.first]][j]]][k] = 1;
						S0[link[G[reverseLink[choix.first]][j]]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					//cout << "inserted out with tailleInitial = " << tailleInitial << endl;
					S0[link[G[reverseLink[choix.first]][j]]][tailleInitial] = 1;
					S0[link[G[reverseLink[choix.first]][j]]][choix.second] = 0;
				}
				//out_.push_back(link[G[reverseLink[choix.first]][j]]);
			}
		}
		vector<vector<int> > solution0;
		triSolution(S0, G.size());
		transform(reverseLink, solution0, S0, G.size());
		//std::sort(solution0.begin(), solution0.end(), sortBySize);
		//cout << "solution0 : " << endl;
		//afficher(solution0);
		insertIfValid(uncontrolled, solution0);

		vector<int> tailleS0(solution0.size(), 0);
		for (int ii = 0; ii < solution0.size(); ii++)
		{
			tailleS0[ii] = solution0[ii].size();
		}
		tabuListVec[tailleS0] = TABU_TENURE;
		//cout << "out : " << endl;
		//afficher(out);
		/*for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}*/
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		//triSolution(S0, G.size());
		cout << "S0 apres tri : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()), reverseLink);
		//cost << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		// Mise à jour de la solution
		if (Z > getEvaluation(S0, G.size()))
		{
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S[i][j] = S0[i][j];
				}
			}
			Z = getEvaluation(S, G.size());
			//out << "best new solution : " << endl;
			//afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			//out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		/*for (int j = 0; j < (int)G.size(); j++)
		{
			delete[] K[j];
			delete[] B[j];
		}
		delete[] K;
		delete[] B;*/
		/*for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
				if(moveTabuList[j][k] != 0)
				{
					moveTabuList[j][k]--;
				}
			}
		}*/
		for (int ii = 0; ii < tabuListVec.size(); ii++)
		{
			if (tabuListVec[tailleS0] != 0)
			{
				tabuListVec[tailleS0]--;
			}
			else
			{
				tabuListVec.erase(tailleS0);
			}
		}
	}
	// Libérer la mémoire
	for (int j = 0; j < (int)G.size(); j++)
	{
		delete[] K[j];
		delete[] B[j];
	}
	delete[] K;
	delete[] B;
}

void cliquer(map<int, vector<int> >& Gq, vector<vector<int> >& Amodulaire, map<int, vector<vector<int> > >& listSolutionsModules, int& ind, vector<int>& adj, vector<vector<int> >& listX)
{
	for (int i = 0; i < adj.size(); i++)
	{
		vector<int> inter(listX.size(), 0);
		map<int, vector<vector<int> > >::iterator j = listSolutionsModules.begin();
		while (j->first != adj[i])
		{
			j++;
		}
		for (int k = 0; k < listSolutionsModules[j->first].size(); k++)
		{
			for (int l = 0; l < listX.size(); l++)
			{
				if (listX[l] == listSolutionsModules[j->first][k])
				{
					inter[l] = 1;
					break;
				}
			}
		}
		for (vector<int>::iterator it = Gq[adj[i]].begin(); it != Gq[adj[i]].end(); it++)
		{
			vector<int> inter2(inter); map<int, vector<vector<int> > >::iterator b = listSolutionsModules.begin();
			while (b->first != *it)
			{
				b++;
			}
			for (int k = 0; k < listSolutionsModules[b->first].size(); k++)
			{
				for (int l = 0; l < listX.size(); l++)
				{
					if (listX[l] == listSolutionsModules[b->first][k])
					{
						inter2[l] = 1;
						break;
					}
				}
			}
			if (find(Amodulaire.begin(), Amodulaire.end(), inter2) == Amodulaire.end())
			{
				Amodulaire.push_back(inter2);
			}
		}
	}
}

void graphe_de_solution(map<int, vector<int> >& Gq, map<int, vector<vector<int> > >& solutionModulaire, vector<vector<int> >& Amodulaire, vector<vector<int> >& listX)
{
	//cout << "Ensemble des solutions : " << endl;
	//afficher(solutionModulaire);
	for (map<int, vector<vector<int> > >::iterator it = solutionModulaire.begin(); it != solutionModulaire.end(); it++)
	{
		for (int j = 0; j < solutionModulaire[it->first].size(); j++)
		{
			listX.push_back(solutionModulaire[it->first][j]);
		}
	}
	int ind(0);
	for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); it++)
	{
		cliquer(Gq, Amodulaire, solutionModulaire, ind, it->second, listX);
		ind++;
	}

	//cout << "\nAmodulaire : " << endl;
	//afficher(Amodulaire);
	/*if (!Amodulaire.empty())
	{
	simplification(Amodulaire);
	cout << "\nAmodulaire simplifier : " << endl;
	//afficher(Amodulaire);
	}*/
	//cout << "\nlistX : " << endl;
	//afficher(listX);
	//cout << "TAPE TO CONTINUE" << endl;
	//char c;
	//cin >> c;
}

bool contains(vector<int>& container, vector<int>& containing)
{
	for (int i = 0; i < containing.size(); i++)
	{
		bool trouver(false);
		for (int j = 0; j < container.size(); j++)
		{
			if (containing[i] == container[j])
			{
				trouver = true;
				break;
			}
		}
		if (!trouver)
		{
			return false;
		}
	}
	if (container.size() == containing.size())
	{
		return false;
	}
	return true;
}

// ---------------- SORT ----------------
void sortPartition(vector<vector<int>>& p)
{
	for (auto& b : p)
		sort(b.begin(), b.end());
}

// ---------------- FLEXIBLE MERGE ----------------
vector<vector<int>> mergeFlexible(const vector<vector<int>>& A, const vector<vector<int>>& B)
{
	vector<vector<int>> R;

	int m = min(A.size(), B.size());

	for (int i = 0; i < m; i++)
	{
		vector<int> bloc = A[i];
		bloc.insert(bloc.end(), B[i].begin(), B[i].end());
		R.push_back(bloc);
	}

	for (int i = m; i < A.size(); i++)
		R.push_back(A[i]);

	for (int i = m; i < B.size(); i++)
		R.push_back(B[i]);

	return R;
}

// ---------------- COMBINATION ----------------
vector<vector<vector<int>>> combineAll(map<int, vector<vector<vector<int>>>>& M)
{
	vector<vector<vector<int>>> sols;
	bool first = true;

	for (auto& pair : M)
	{
		if (first)
		{
			sols = pair.second;
			first = false;
			continue;
		}

		vector<vector<vector<int>>> newSols;

		for (auto& s : sols)
		{
			for (auto& p : pair.second)
			{
				vector<vector<int>> r =
					mergeFlexible(s, p);
				sortPartition(r);
				newSols.push_back(r);
			}
		}

		sols.swap(newSols);
	}

	return sols;
}

// ---------- Construction de stables[parent] ----------
vector<vector<vector<int>>> buildStableFromFusion(const vector<vector<int>>& baseStables, const vector<vector<vector<int>>>& mergedSolutions)
{
	vector<vector<vector<int>>> result;

	for (const auto& fusion : mergedSolutions)
	{
		// Sommets impliqués dans cette fusion
		set<int> involved;
		for (const auto& block : fusion)
			for (int v : block)
				involved.insert(v);

		vector<vector<int>> sol;

		// Conserver les blocs NON concernés
		for (const auto& block : baseStables)
		{
			bool touched = false;
			for (int v : block)
			{
				if (involved.count(v))
				{
					touched = true;
					break;
				}
			}
			if (!touched)
				sol.push_back(block);
		}

		// Ajouter les blocs fusionnés
		for (auto block : fusion)
		{
			sort(block.begin(), block.end());
			sol.push_back(block);
		}

		sortPartition(sol);
		result.push_back(sol);
	}

	return result;
}

// ---------- Produit cartésien des partitions ----------
vector<vector<vector<int>>> cartesianPartitions(const map<int, vector<vector<vector<int> > > >& stables)
{
	vector<vector<vector<int>>> result;
	result.push_back({});

	for (const auto& [key, partitions] : stables)
	{
		vector<vector<vector<int>>> next;

		for (const auto& partial : result)
		{
			for (const auto& sol : partitions)
			{
				vector<vector<int>> merged = partial;

				for (auto block : sol)
				{
					sort(block.begin(), block.end());
					merged.push_back(block);
				}

				next.push_back(merged);
			}
		}

		result.swap(next);
	}

	return result;
}

vector<vector<int> > resolutionBottomUp(vector<vector<int> >& A, map<int, vector<int> >& G, vector<Module>& listModules, vector<vector<int> >& listModule_)
{
	int nivMax;
	map<int, vector<vector<vector<int> > > > stables;
	map<int, vector<vector<vector<int> > > > linkBottomUp;
	//stables[-1] = init;
	//cout << "Stables 0 : " << endl;
	//afficher(stables);
	// Trier les modules par niveau bottom-up
	map<int, vector<vector<Module>::iterator>, classcomp> indicesModulesTrieParNiveau(trieModuleParNiveau(listModules, nivMax));
	// On commence l'attribution des couleurs aux sommets selon les règles fixées
	for (int niv = nivMax - 1; niv > -1; niv--)
	{
		//cout << "niv = " << niv << endl;
		for (int mod = 0; mod < indicesModulesTrieParNiveau[niv].size(); mod++)
		{
			//cout << "\tj = " << j << endl;
			//cout << *(indicesModulesTrieParNiveau[i][j]) << endl;
			Module* M = &(*indicesModulesTrieParNiveau[niv][mod]);
			int module = M->getNom();
			auto sommets = M->getSommets();
			if (M->getTrivial())
			{
				continue;
			}

			if (M->getDegenerate() == 1)
			{
				//cout << "Parallele" << endl;
				if (linkBottomUp.empty())
				{
					vector<vector<int> > init;
					for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
					{
						vector<int> inter(1, it->first);
						init.push_back(inter);
					}
					//cout << "First Link" << endl;
					for (int k = 0; k < sommets.size(); k++)
					{
						vector<int> f(1, sommets[k]);
						for (vector<vector<int> >::iterator it = init.begin(); it != init.end(); it++)
						{
							if (*it == f)
							{
								init.erase(it);
								break;
							}
						}
					}
					linkBottomUp[module].push_back(vector<vector<int> >());
					linkBottomUp[module][0].push_back(sommets);
					init.push_back(sommets);
					stables[module].push_back(init);
					//cout << "Link 0 :" << endl;
					//afficher(linkBottomUp);
					//cout << "Stables 1 : " << endl;
					//afficher(stables);
				}
				else
				{
					map<int, vector<vector<vector<int> > > > MM;
					for (int k = 0; k < sommets.size(); k++)
					{
						if (sommets[k] >= G.size())
						{
							MM[sommets[k]] = linkBottomUp[sommets[k]];
						}
					}
					if (MM.empty())
					{
						vector<vector<int> > init;
						for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
						{
							vector<int> inter(1, it->first);
							init.push_back(inter);
						}
						for (int k = 0; k < sommets.size(); k++)
						{
							vector<int> f(1, sommets[k]);
							for (vector<vector<int> >::iterator it = init.begin(); it != init.end(); it++)
							{
								if (*it == f)
								{
									init.erase(it);
									break;
								}
							}
						}
						linkBottomUp[module].push_back(vector<vector<int> >());
						linkBottomUp[module][0].push_back(sommets);
						init.push_back(sommets);
						stables[module].push_back(init);
					}
					else
					{
						auto results = combineAll(MM);
						linkBottomUp[module] = results;
						stables[module] = buildStableFromFusion(stables[sommets[0]][0], results);
					}
					//cout << "Link 1 :" << endl;
					//afficher(linkBottomUp);
					//cout << "Stables 3 : " << endl;
					//afficher(stables);
				}
			}
			else if (M->getDegenerate() == 2 && M->getCographe())
			{
				if (linkBottomUp.empty())
				{
					vector<vector<int> > init;
					for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
					{
						vector<int> inter(1, it->first);
						init.push_back(inter);
					}
					//cout << "First Link" << endl;
					vector<vector<int> > inter;
					for (int k = 0; k < sommets.size(); k++)
					{
						vector<int> f(1, sommets[k]);
						inter.push_back(f);
					}
					linkBottomUp[module].push_back(inter);
					init.push_back(sommets);
					stables[module].push_back(init);
					//cout << "Link 0 :" << endl;
					//afficher(linkBottomUp);
					//cout << "Stables 1 : " << endl;
					//afficher(stables);
				}
				else
				{
					map<int, vector<vector<vector<int> > > > MM;
					for (int k = 0; k < sommets.size(); k++)
					{
						if (sommets[k] >= G.size())
						{
							MM[sommets[k]] = linkBottomUp[sommets[k]];
						}
					}
					if (MM.empty())
					{
						vector<vector<int> > init;
						for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
						{
							vector<int> inter(1, it->first);
							init.push_back(inter);
						}
						//cout << "First Link" << endl;
						vector<vector<int> > inter;
						for (int k = 0; k < sommets.size(); k++)
						{
							vector<int> f(1, sommets[k]);
							inter.push_back(f);
						}
						linkBottomUp[module].push_back(inter);
						init.push_back(sommets);
						stables[module].push_back(init);
					}
					else
					{
						auto results = cartesianPartitions(MM);
						linkBottomUp[module] = results;
						stables[module] = buildStableFromFusion(stables[sommets[0]][0], results);
					}
				}
				//cout << "Link 2 :" << endl;
				//afficher(linkBottomUp);
			}
			else
			{
				//cout << "Prime" << endl;
				vector<int> enfants;
				vector<int> tailles;
				set<vector<vector<int>>> allSolutions;

				for (int s : sommets)
				{
					if (s >= G.size())
					{
						enfants.push_back(s);
						tailles.push_back(linkBottomUp[s].size());
					}
				}

				int nbComb = 1;
				for (int t : tailles) nbComb *= t;

				for (int comb = 0; comb < nbComb; comb++)
				{
					int tmp = comb;
					map<int, int> choix;
					for (int k = 0; k < enfants.size(); k++)
					{
						choix[enfants[k]] = tmp % tailles[k];
						tmp /= tailles[k];
					}

					// -------- Construction solutionModulaire --------
					map<int, vector<vector<int>>> solutionModulaire;
					map<int, vector<int>> Gq;

					for (int s : sommets)
					{
						if (s >= G.size())
							solutionModulaire[s] = linkBottomUp[s][choix[s]];
						else
							solutionModulaire[s] = { {s} };
						Gq[s] = {};
					}

					// -------- Construction Gq --------
					for (int a = 0; a < sommets.size(); a++)
					{
						int Sa = sommets[a] < G.size()
							? sommets[a]
							: solutionModulaire[sommets[a]][0][0];

						for (int b = a + 1; b < sommets.size(); b++)
						{
							int Sb = sommets[b] < G.size()
								? sommets[b]
								: solutionModulaire[sommets[b]][0][0];

							if (find(G[Sa].begin(), G[Sa].end(), Sb) != G[Sa].end())
							{
								Gq[sommets[a]].push_back(sommets[b]);
								Gq[sommets[b]].push_back(sommets[a]);
							}
						}
					}

					// -------- Résolution EBTS --------
					vector<vector<int>> solution;
					vector<vector<int>> listX;
					vector<vector<int>> Amodulaire;

					graphe_de_solution(Gq, solutionModulaire, Amodulaire, listX);

					map<int, vector<int>> GM;
					for (auto& b : listX)
						for (int v : b)
							GM[v] = {};

					for (int i = 0; i < Amodulaire.size(); i++)
						for (int j = 0; j < Amodulaire[i].size(); j++)
							if (Amodulaire[i][j] == 1)
								for (int k = j + 1; k < Amodulaire[i].size(); k++)
									if (Amodulaire[i][k] == 1)
										for (int a : listX[k])
											for (int b : listX[j])
											{
												GM[a].push_back(b);
												GM[b].push_back(a);
											}

					// --- EBTS (inchangé) ---
					// (reprend exactement ton code ici)
					int n = GM.size();

					int** DM = new int* [n];
					for (int i = 0; i < n; i++)
					{
						DM[i] = new int[n];
					}
					int** S = new int* [n];
					for (int i = 0; i < n; i++)
					{
						S[i] = new int[n];
					}
					int** S0 = new int* [n];
					for (int i = 0; i < n; i++)
					{
						S0[i] = new int[n];
					}
					map<int, int> link;
					vector<int> reverseLink(GM.size(), 0);
					int p = 0;
					for (map<int, vector<int> >::iterator it = GM.begin(); it != GM.end(); it++)
					{
						reverseLink[p] = it->first;
						link[it->first] = p;
						p++;
					}
					for (int i = 0; i < n; i++)
					{
						for (int j = 0; j < n; j++)
						{
							S[i][j] = 0;
							S0[i][j] = 0;
							if (find(GM[reverseLink[i]].begin(), GM[reverseLink[i]].end(), reverseLink[j]) != GM[reverseLink[i]].end() || i == j)
							{
								DM[i][j] = -1;
							}
							else
							{
								DM[i][j] = 0;
							}
						}
					}

					vector<vector<int> > vertices(listX);
					vector<int> posInSolution;
					std::sort(vertices.begin(), vertices.end(), sortBySize);
					solution.push_back(vertices[0]);
					vector<int> pos(vertices.size(), 0);
					for (int i = 0; i < listX.size(); i++)
					{
						if (vertices[0] == listX[i])
						{
							pos[0] = i;
							break;
						}
					}
					posInSolution.push_back(0);
					for (int i = 1; i < vertices.size(); i++)
					{
						vector<bool> possibilities(solution.size() + 1, true);
						for (int j = 0; j < listX.size(); j++)
						{
							if (vertices[i] == listX[j])
							{
								pos[i] = j;
								break;
							}
						}
						for (int j = 0; j < i; j++)
						{
							for (int k = 0; k < Amodulaire.size(); k++)
							{
								if (Amodulaire[k][pos[i]] == 1 && Amodulaire[k][pos[j]] == 1)
								{
									possibilities[posInSolution[j]] = false;
									break;
								}
							}
						}
						for (int j = 0; j < possibilities.size(); j++)
						{
							if (possibilities[j])
							{
								if (j >= solution.size())
								{
									solution.push_back(vector<int>());
								}
								for (int k = 0; k < vertices[i].size(); k++)
								{
									solution[j].push_back(vertices[i][k]);
								}
								posInSolution.push_back(j);
								break;
							}
						}
					}
					for (int i = 0; i < solution.size(); i++)
					{
						for (int j = 0; j < solution[i].size(); j++)
						{
							S[link[solution[i][j]]][i] = 1;
							S0[link[solution[i][j]]][i] = 1;
						}
					}
					int Z = getEvaluation(S, n);
					//cout << "GM : " << endl;
					//cout << '{' << endl;
					//for (map<int, vector<int> >::iterator it = GM.begin(); it != GM.end(); it++)
					//{
						//cout << it->first << " : ";
						//for (int j = 0; j < (it->second).size(); j++)
						//{
							//cout << (it->second)[j] << ' ';
						//}
						//cout << endl;
					//}
					//cout << '}' << endl;
					cout << "solution in EBTS : " << endl;
					afficher(solution);
					vector<vector<vector<int> > > uncontrolledSolution;
					uncontrolledSolution.push_back(vector<vector<int> >(solution));
					EBTS8(GM, S, S0, Z, DM, link, reverseLink, uncontrolledSolution, listModules, n);
					//graphe_de_solution(Gq, solutionModulaire, Amodulaire, listX);
					//resolution_finale(listModule_, Gq, solutionModulaire, Amodulaire, solution, listX);
					solution.clear();
					//cout << "link : " << endl;
					//afficher(link);
					//cout << "S  : " << endl;
					//afficherSolution(S, GM.size(), getTaille(S, GM.size()));
					transform(reverseLink, solution, S, n);

					//cout << "solution after EBTS : " << endl;
					//afficher(solution);

					//linkBottomUp[module].push_back(solution);

					//cout << "////////////////////////////////////////////////Uncontrolled solutions : " << endl;
					for (int h = 0; h < uncontrolledSolution.size(); h++)
					{
						allSolutions.insert(uncontrolledSolution[h]);
						//cout << "Uncontroleed sol " << h  << " = " << endl;  afficher(uncontrolledSolution[h]);
					}
					// after transform(reverseLink, solution, S, n);
					freeMatrix(DM, n);
					freeMatrix(S, n);
					freeMatrix(S0, n);
				}
				vector<vector<vector<int> > > allSolutionsVector(allSolutions.begin(), allSolutions.end());

				keepOnlyNonDominated(allSolutionsVector);

				linkBottomUp[module] = allSolutionsVector;

				bool rempli = false;
				for (int h = 0; h < sommets.size(); h++)
				{
					if (sommets[h] >= G.size())
					{
						stables[module] = buildStableFromFusion(stables[sommets[h]][0], allSolutionsVector);
						rempli = true;
						break;
					}
				}
				if (!rempli)
				{
					vector<vector<int> > init;
					for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
					{
						vector<int> inter(1, it->first);
						init.push_back(inter);
					}
					stables[module] = buildStableFromFusion(init, allSolutionsVector);
				}
				//char c;
				//cin >> c;
				//cout << "Link 3 :" << endl;
				//afficher(linkBottomUp);
				//cout << "Stables 4 : " << endl;
				//afficher(stables);
			}
		}
		char c;
		//cin >> c;
	}
	std::sort(stables[indicesModulesTrieParNiveau[0][0]->getNom()].begin(), stables[indicesModulesTrieParNiveau[0][0]->getNom()].end(), sortByEvaluation);
	//system("pause");
	return stables[indicesModulesTrieParNiveau[0][0]->getNom()][0];
}