#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <utility>
#include <queue>
#include <csignal>
#include <Windows.h>

#include <boost/timer/timer.hpp>

#define ITER_MAX 1
#define ITER_EBTS 200
#define TABU_TENURE 12

using namespace std;

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
			eval1 += nb*(j + 1);
		}
		int eval2 = 0;
		for (int j = 0; j < n; j++)
		{
			int nb = 0;
			for (int i = 0; i < n; i++)
			{
				nb = nb + S2[i][j];
			}
			eval2 += nb*(j + 1);
		}
		return (eval1 > eval2);
	}
};

// Hash function
struct hashFunction
{
	size_t operator()(const vector<int> &myVector) const
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

void toCplexData(unsigned int& n, unsigned int& m, unsigned int& delta, vector<vector<int> >& A, map<int, vector<int> >& G)
{
	ofstream file("cplex.dat");
	file << "n = " << n << ";" << endl;
	file << "m = " << m << ";" << endl;
	file << "delta = " << delta << ";" << endl;
	file << "A = [" << endl;
	for (int i = 0; i < m; i++)
	{
		file << "[";
		for (int j = 0; j < n; j++)
		{
			file << A[i][j] << " ";
		}
		if (i != (m - 1))
		{
			file << "]," << endl;
		}
		else
		{
			file << "]];\nB=[" << endl;
		}
	}
	for (int i = 0; i < n; i++)
	{
		file << "[";
		vector<int> inter(n, 0);
		for (int j = 0; j < G[i].size(); j++)
		{
			inter[G[i][j]] = 1;
		}
		for (int j = 0; j < n; j++)
		{
			file << inter[j] << " ";
		}
		if (i != (n - 1))
		{
			file << "]," << endl;
		}
		else
		{
			file << "]];" << endl;
		}
	}
	file.close();
}

void lectureDimacs(unsigned int& n, unsigned int& m, unsigned int& delta, vector<vector<int> > & A)
{
	ifstream data("dimacs.txt");
	string info;
	data >> info;
	data >> info;
	data >> n;
	data >> m;
	vector<int> ligneA;
	for (unsigned int j = 0; j<n; j++)
	{
		ligneA.push_back(0);
	}
	for (unsigned int i = 0; i<m; i++)
	{
		A.push_back(ligneA);
	}
	int s1, s2;
	for (int i = 0; i<m; i++)
	{
		data >> info;
		data >> s1;
		data >> s2;
		A[i][s1] = 1;
		A[i][s2] = 1;
	}
	unsigned int max_;
	vector<unsigned int> somme(n, 0);
	for (unsigned int j = 0; j<n; j++)
	{
		for (unsigned int i = 0; i<A.size(); i++)
		{
			somme[j] += A[i][j];
		}
	}
	max_ = somme[0];
	for (unsigned int j = 1; j<n; j++)
	{
		if (max_ < somme[j])
		{
			max_ = somme[j];
		}
	}
	delta = max_ + 1;
	//toCplexData(n, m, delta, A);
}

void lireData(vector<vector<int> >& A, unsigned int& m, unsigned int& n, unsigned int& delta)
{
	ifstream fichier("input.txt");
	unsigned int var, contr, mot; //Une variable pour stocker les lignes lues
	fichier >> var;
	fichier >> contr;
	vector<int> inter;
	cout << var << endl;
	cout << contr << endl;
	for (unsigned int i = 0; i<contr; i++)
	{
		for (unsigned int j = 0; j<var; j++)
		{
			fichier >> mot;
			inter.push_back(mot);
		}
		A.push_back(inter);
		inter.clear();
	}
	n = var; m = contr;
	unsigned int max_;
	vector<unsigned int> somme(n, 0);
	for (unsigned int j = 0; j<n; j++)
	{
		for (unsigned int i = 0; i<m; i++)
		{
			somme[j] += A[i][j];
		}
	}
	max_ = somme[0];
	for (unsigned int j = 1; j<n; j++)
	{
		if (max_ < somme[j])
		{
			max_ = somme[j];
		}
	}
	delta = max_ + 1;
}

void afficher(vector<vector<int> >& P)
{
	cout << '{';
	for (int i = 0; i<P.size(); i++)
	{
		cout << '{';
		for (int j = 0; j<P[i].size(); j++)
		{
			cout << P[i][j] << ' ';
		}
		cout << "} ";
	}
	cout << '}' << endl;
}

void afficherOut(vector<vector<int> >& P)
{
	out << '{';
	for (int i = 0; i<P.size(); i++)
	{
		out << '{';
		for (int j = 0; j<P[i].size(); j++)
		{
			out << P[i][j] << ' ';
		}
		out << "} ";
	}
	out << '}' << endl;
}

void afficherOut(map<int, vector<int> >& G)
{
	out << "G : " << endl;
	for (map<int, vector<int> >::iterator it=G.begin(); it!=G.end(); it++)
	{
		out << it->first << " : ";
		for (int j = 0; j<it->second.size(); j++)
		{
			out << it->second[j] << ", ";
		}
		out << endl;
	}
}

void afficherSolution(int **P, int n, int m)
{
	cout << '{';
	for (int i = 0; i<m; i++)
	{
		cout << '{';
		for (int j = 0; j<n; j++)
		{
			if (P[j][i] == 1)
			{
				cout << j << ' ';
			}
		}
		cout << "} ";
	}
	cout << '}' << endl;
}

void afficherSolution(map<int, vector<int> >& G, int **P, int n, int m)
{
	cout << '{';
	for (int i = 0; i<m; i++)
	{
		cout << '{';
		map<int, vector<int> >::iterator it = G.begin();
		for (int j = 0; j<n; j++)
		{
			if (P[j][i] == 1)
			{
				cout << it->first << ' ';
			}
			it++;
		}
		cout << "} ";
	}
	cout << '}' << endl;
}

void afficherSolutionOut(int **P, int n, int m)
{
	out << '{';
	for (int i = 0; i<m; i++)
	{
		out << '{';
		for (int j = 0; j<n; j++)
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

void afficherSolutionOut(map<int, vector<int> >& G, int **P, int n, int m)
{
	out << '{';
	for (int i = 0; i<m; i++)
	{
		out << '{';
		map<int, vector<int> >::iterator it = G.begin();
		for (int j = 0; j<n; j++)
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

void afficher(int **P, int n, int m)
{
	cout << '[';
	for (int i = 0; i<m; i++)
	{
		cout << '[';
		for (int j = 0; j<n; j++)
		{
			cout << P[i][j] << ' ';
		}
		cout << "] " << endl;
	}
	cout << ']' << endl;
}

void afficherOut(int **P, int n, int m)
{
	out << '[';
	for (int i = 0; i<m; i++)
	{
		out << '[';
		for (int j = 0; j<n+1; j++)
		{
			out << P[i][j] << ' ';
		}
		out << "] " << endl;
	}
	out << ']' << endl;
}

void afficher(map<int, vector<int> >& P)
{
	cout << '{' << endl;
	for (map<int, vector<int> >::iterator it = P.begin(); it != P.end(); it++)
	{
		cout << it->first << " : ";
		for (int j = 0; j<(it->second).size(); j++)
		{
			cout << (it->second)[j] << ' ';
		}
		cout << endl;
	}
	cout << '}' << endl;
}

void afficher(vector<int> P)
{
	cout << '{';
	for (int i = 0; i<P.size(); i++)
	{
		cout << P[i] << ' ';
	}
	cout << '}' << endl;
}

void afficher(unordered_set<vector<int>, hashFunction>& list)
{
	cout << '{';
	for (auto it = list.begin(); it != list.end(); it++)
	{
		afficher(*it);
		cout << ',';
	}
	cout << '}';
}

void afficherOut(vector<int> P)
{
	out << '{';
	for (auto e : P)
	{
		out << e << ' ';
	}
	out << '}';
}

void afficherOut(unordered_set<vector<int>, hashFunction>& list)
{
	out << '{';
	for (auto it = list.begin(); it != list.end(); it++)
	{
		afficherOut(*it);
		out << ',';
	}
	out << '}';
}

void afficherOut(vector<unordered_set<vector<int>, hashFunction> >& list)
{
	out << '[';
	for (vector<unordered_set<vector<int>, hashFunction> >::iterator it = list.begin(); it != list.end(); it++)
	{
		afficherOut(*it);
		out << '\n';
	}
	out << ']' << endl;
}

void afficherOut(vector<pair<int, int> >& P)
{
	out << "[";
	for (auto e : P)
	{
		out << "<" << e.first << ", " << e.second << ">, ";
	}
	out << "]" << endl;
}

void afficher(map<int, int>& P)
{
	cout << '{';
	for (map<int, int>::iterator it = P.begin(); it != P.end(); it++)
	{
		cout << it->first << " : " << it->second << endl;
	}
	cout << '}' << endl;
}

void afficher(map<pair<int,int>, int>& P)
{
	cout << '{';
	for (map<pair<int, int>, int>::iterator it = P.begin(); it != P.end(); it++)
	{
		cout << '(' << it->first.first << " , " << it->first.second << ") : " << it->second << endl;
	}
	cout << '}' << endl;
}

void afficher(map<int, vector<vector<int> > >& P)
{
	cout << '[';
	for (map<int, vector<vector<int> > >::iterator it = P.begin(); it != P.end(); it++)
	{
		cout << it->first << " : " << endl;
		for (int i = 0; i < it->second.size(); i++)
		{
			cout << '\t';
			for (int j = 0; j < it->second[i].size(); j++)
			{
				cout << it->second[i][j] << " ";
			}
			cout << endl;
		}
	}
	cout << ']' << endl;
}

int getEvaluation(vector<vector<int> >& solution)
{
	int eval = 0;
	for (int i = 0; i < solution.size(); i++)
	{
		eval += solution[i].size() * (i + 1);
	}
	return eval;
}

int getEvaluation(vector<int> & taille)
{
	int eval = 0;
	for (int i = 0; i < taille.size(); i++)
	{
		eval += taille[i] * (i + 1);
	}
	return eval;
}

int getEvaluation(int **S, int n)
{
	int eval = 0;
	for (int j = 0; j < n; j++)
	{
		int nb = 0;
		for (int i = 0; i < n; i++)
		{
			nb = nb + S[i][j];
		}
		eval += nb*(j + 1);
	}
	return eval;
}

map<int, vector<int> > gToListAdj(vector<vector<int> >& A)
{
	map<int, vector<int> > out;
	for (int j = 0; j<A[0].size(); j++)
	{
		vector<int> inter;
		for (int i = 0; i<A.size(); i++)
		{
			if (A[i][j] == 1)
			{
				for (int k = 0; k<A[0].size(); k++)
				{
					if (j != k && A[i][k] == 1 && find(inter.begin(), inter.end(), k) == inter.end())
					{
						inter.push_back(k);
						break;
					}
				}
			}
		}
		out[j] = inter;
	}
	return out;
}

map<int, vector<int> > gToGBar(map<int, vector<int> > G)
{
	map<int, vector<int> > GBar;
	vector<int> listSommets;
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
	{
		listSommets.push_back(it->first);
	}
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
	{
		vector<int> inter;
		for (int i = 0; i<listSommets.size(); i++)
		{
			if ((find((it->second).begin(), (it->second).end(), listSommets[i]) == (it->second).end()) && (it->first != listSommets[i]))
			{
				inter.push_back(listSommets[i]);
			}
		}
		GBar[it->first] = inter;
	}
	return GBar;
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

int getTaille(int **S, int n)
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

void ConstructedGreedyRandomizedSolution(map<int, vector<int> >& G, vector<int>& S, int **DM)
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

void ConstructedGreedyRandomizedInitialSolution(map<int, vector<int> >& G, vector<int>& S)
{
	vector<int> sommets;
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); ++it)
	{
		sommets.push_back(it->first);
	}
	int min, ind;
	for (int i = 0; i < (sommets.size() - 1); i++)
	{
		min = G[sommets[i]].size(); 
		ind = i;
		for (int j = i + 1; j < sommets.size(); j++)
		{
			if (min > G[sommets[j]].size())
			{
				min = G[sommets[j]].size();
				ind = j;
			}
		}
		int x = sommets[i];
		sommets[i] = sommets[ind];
		sommets[ind] = x;
	}
	while (!sommets.empty())
	{
		S.push_back(sommets[0]);
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
	sort(S.begin(), S.end(), comparatorVector());
}

void ConstructedGreedyRandomizedSolution(map<int, vector<int> >& G, vector<vector<int> >& S, int **DM)
{
	map<int, vector<int> > G_(G);
	while (!G_.empty())
	{
		vector<int> S_;
		ConstructedGreedyRandomizedSolution(G_, S_, DM);
		S.push_back(S_);
		G_ = GminusS(G_, S_);
	}
	sort(S.begin(), S.end(), comparatorVector());
}

void ConstructedGreedyRandomizedInitialSolution(map<int, vector<int> >& G, vector<vector<int> >& S)
{
	map<int, vector<int> > G_(G);
	while (!G_.empty())
	{
		vector<int> S_;
		ConstructedGreedyRandomizedInitialSolution(G_, S_);
		S.push_back(S_);
		G_ = GminusS(G_, S_);
	}
	sort(S.begin(), S.end(), comparatorVector());
}

void ConstructedGreedyRandomizedSolution(map<int, vector<int> >& G, int **S)
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

void ConstructedGreedyRandomizedInitialSolution(map<int, vector<int> >& G, int **S)
{
	vector<vector<int> > S_;
	ConstructedGreedyRandomizedInitialSolution(G, S_);
	for (int i = 0; i < S_.size(); i++)
	{
		for (int j = 0; j < S_[i].size(); j++)
		{
			S[S_[i][j]][i] = 1;
		}
	}
}

void ConstructedGreedyRandomizedSolution(map<int, vector<int> >& G, int **S, int **DM)
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

void constructSolutionOnline(map<int, vector<int> >& G, int** S)
{
	vector<int> vertices;
	vector<vector<int> > sol;
	for (int i = 0; i < G.size(); i++)
	{
		vertices.push_back(i);
		sol.push_back(vector<int>());
	}
	
	vector<int> taken;
	int taille = 0;
	vector<int> position;
	for (int i = 0; i < G.size(); i++)
	{
		position.push_back(-1);
	}
	while (!vertices.empty())
	{
		taken.push_back(vertices[vertices.size() - 1]);
		vertices.pop_back();
		if (taken.size() == 1)
		{
			sol[0].push_back(taken[0]);
			taille++;
		}
		else
		{
			int* K = new int[taille + 1];
			int* B = new int[taille + 1];
			for (int i = 0; i < taille + 1; i++)
			{
				K[i] = 0;
			}
			for (int j = 0; j < G[taken[taken.size() - 1]].size(); j++)
			{
				if (find(taken.begin(), taken.end(), G[taken[taken.size() - 1]][j]) != taken.end())
				{
					for (int i = 0; i < taille; i++)
					{
						if (find(sol[i].begin(), sol[i].end(), G[taken[taken.size() - 1]][j]) != sol[i].end())
						{
							K[i]++;
							break;
						}
					}
				}
			}
			
			for (int j = 0; j < sol.size(); j++)
			{
				for (int k = 0; k < sol[j].size(); k++)
				{
					position[sol[j][k]] = k;
				}
			}
			/*cout << "taille : " << endl;
			afficher(taille);
			cout << "position : " << endl;
			afficher(position);*/
			/*for (int j = 0; j < G.size(); j++)
			{
				for (int k = 0; k < taille + 1; k++)
				{
					if (K[j][k] == -1)
					{
						B[j][k] = 0;
					}
					else
					{
						vector<int> t(taille);
						bool tabu(false);
						t[position[j]]--;
						t[k]++;
						for (int h = 0; h < G[j].size(); h++)
						{
							if (K[G[j][h]][k] == -1)
							{
								bool trouver = false;
								for (int r = 0; r < tailleInitial; r++)
								{
									if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
						sort(t.begin(), t.end(), comparatorVector2());
						B[j][k] = getEvaluation(t) - getEvaluation(taille);
						if (B[j][k] > 0)
						{
							
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
									{
										NS0.insert(NS0.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}*/
			free(K);
			free(B);
		}
	}
	// Transform the solution to matrix
}

void beNull(int **S, int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			S[i][j] = 0;
		}
	}
}

bool inTabu(map<pair<int, int>, int>& tabu, int& j, int& k, map<int, vector<int> >& G, vector<vector<int> >& S)
{
	if (tabu.empty())
	{
		return false;
	}
	for (int i = 0; i < G.size(); i++)
	{
		if (find(S[k].begin(), S[k].end(), i) != S[k].end() && find(G[j].begin(), G[j].end(), i) == G[j].end())
		{
			pair<int, int> key;
			if (j < i)
			{
				key = make_pair(j, i);
			}
			else
			{
				key = make_pair(i, j);
			}
			for (map<pair<int, int>, int>::iterator it = tabu.begin(); it != tabu.end(); it++)
			{
				if (it->first == key)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool inTabu(int **tabu, int& j, int& k, map<int, vector<int> >& G, int **S)
{
	for (int i = 0; i < G.size(); i++)
	{
		if (S[i][k] == 1 && find(G[j].begin(), G[j].end(), i) == G[j].end())
		{
			if (tabu[j][i] > 0)
			{
				return true;
			}
		}
	}
	return false;
}

bool inTabu(map<pair<int, vector<int> >, int>& tabu, int& j, int& k, map<int, vector<int> >& G, vector<vector<int> >& S)
{
	if (tabu.empty())
	{
		return false;
	}
	vector<int> vec;
	pair<int, vector<int> > key;
	for (int i = 0; i < G.size(); i++)
	{
		if (find(S[k].begin(), S[k].end(), i) != S[k].end() && find(G[j].begin(), G[j].end(), i) == G[j].end())
		{
			vec.push_back(i);
		}
	}
	sort(vec.begin(), vec.end());
	key = make_pair(j, vec);
	for (map<pair<int, vector<int> >, int>::iterator it = tabu.begin(); it != tabu.end(); it++)
	{
		if (it->first == key)
		{
			return true;
		}
	}
	return false;
}

bool isNull(int **S, int n)
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

vector<int> getSTaille(int **S, int n)
{
	vector<int> t;
	for (int i = 0; i < n; i++)
	{
		int nb = 0;
		for (int j = 0; j < n; j++)
		{
			if (S[j][i] != 0)
			{
				nb++;
			}
		}
		if (nb != 0)
		{
			t.push_back(nb);
		}
		else
		{
			break;
		}
	}
	return t;
}

void triSolution(int **S, int n)
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
	/*cout << "\ttaille : " << endl;
	afficher(taille);*/
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
			int *s_ = new int[n];
			for (int j = 0; j < n; j++)
			{
				s_[j] = S[j][ind];
				S[j][ind] = S[j][i];
				S[j][i] = s_[j];
			}
		}
	}
}

void triSolution(map<int, vector<int> >& Gq, int **S, int n, vector<vector<int> >& serie, vector<vector<int> >& parallele)
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
			int *s_ = new int[n];
			for (int j = 0; j < Gq.size(); j++)
			{
				s_[j] = S[j][ind];
				S[j][ind] = S[j][i];
				S[j][i] = s_[j];
			}
		}
	}
}

void updateDM(int **DM, int** S, int n)
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

int exist(vector<int**>& list, vector<int>& solEval, int** S, int n)
{
	int eval = getEvaluation(S, n);
	for (int i = solEval.size() - 1; i > 0; i--)
	{
		if (eval == solEval[i])
		{
			bool equal = true;
			for (int j = 0; j < n; j++)
			{
				for (int k = 0; k < n; k++)
				{
					if (S[j][k] != list[i][j][k])
					{
						equal = false;
						break;
					}
				}
				if (!equal)
				{
					break;
				}
			}
			if (equal)
			{
				return -1;
			}
		}
		else if(eval < solEval[i])
		{
			return i;
		}
	}
	return 0;
}

unordered_set<vector<int>, hashFunction > solutionToVector(int** S, int n)
{
	vector<vector<int> > sol;
	for (int i = 0; i<n; i++)
	{
		vector<int> inter;
		for (int j = 0; j<n; j++)
		{
			if (S[j][i] == 1)
			{
				inter.push_back(j);
			}
		}
		if (inter.empty())
		{
			break;
		}
		sol.push_back(inter);
	}
	unordered_set<vector<int>, hashFunction > sol_;
	for (int i = 0; i < sol.size(); i++)
	{
		sol_.insert(sol[i]);
	}
	return sol_;
}

bool equal(unordered_set<vector<int>, hashFunction>& a, unordered_set<vector<int>, hashFunction>& b)
{
	if (a.size() != b.size())
	{
		return false;
	}
	for (auto it = a.begin(); it != a.end(); ++it)
	{
		bool trouver = false;
		for (auto it2 = b.begin(); it2 != b.end(); ++it2)
		{
			bool trouver2 = true;
			for (int i = 0; i < (*it).size(); i++)
			{
				if (find((*it2).begin(), (*it2).end(), (*it)[i]) == (*it2).end())
				{
					trouver2 = false;
					if (i > 0)
					{
						return false;
					}
					break;
				}
			}
			if (trouver2)
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
	return true;
}

bool exist(vector<vector<pair<int, int> > >& listVec, int index, pair<int, int>& element)
{
	if (index != -1 || listVec.empty() || listVec.size() <= index)
	{
		return false;
	}
	for (int i = 0; i < listVec[index].size(); i++)
	{
		if (listVec[index][i].first == element.first && listVec[index][i].second == element.second)
		{
			return true;
		}
	}
	return false;
}

int exist(vector<unordered_set<vector<int>, hashFunction> >& list, vector<int>& solEval, vector<pair<int,int> >& indexEval, int** S, int n)
{
	if (solEval.empty())
	{
		return 0;
	}
	unordered_set<vector<int>, hashFunction > S_(solutionToVector(S, n));
	int eval = getEvaluation(S, n);
	int first = indexEval[0].first, second = 0;
	for (int i = indexEval.size() - 1; i >= 0; i--)
	{
		if (indexEval[i].second > eval && i == (indexEval.size() - 1))
		{
			return indexEval[i].first + 1;
		}
		if (indexEval[i].second == eval)
		{
			first = indexEval[i].first;
			i--;
			while (i >= 0 && indexEval[i].second == eval)
			{
				i--;
			}
			if (i >= 0)
			{
				second = indexEval[i].first + 1;
			}
			break;
		}
		else if(indexEval[i].second > eval)
		{
			second = indexEval[i].first + 1;
			first = indexEval[i + 1].first - 1;
			break;
		}
	}
	/*out << "Solution a traiter : " << endl;
	afficherOut(S_);
	out << "index :" << endl;
	afficherOut(indexEval);
	out << "first = " << first << "\t|second = " << second << endl;*/
	if (first < second)
	{
		return second;
	}
	bool trouver = false;
	int ind = -2;
	for (int i = first; i >= second; i--)
	{
		if (eval == solEval[i])
		{
			trouver = true;
			ind = i;
			/*out << "A comparer : " << endl;
			afficherOut(S_);
			out << "------WITH-----" << endl;
			afficherOut(list[i]);
			out << "---------------" << endl;*/
			if (equal(S_, list[i]))
			{
				//out << "===> EQUAL" << endl;
				return -1;
			}
			else
			{
				//out << "===> NOT EQUAL" << endl;
			}
		}
		else if (eval < solEval[i])
		{
			return i + 1;
		}
	}
	if (trouver)
	{
		return ind;
	}
	return second;
}

int exist(vector<unordered_set<vector<int>, hashFunction> >& list, vector<int>& solEval, vector<pair<int, int> >& indexEval, int** S, int n, int& posExact)
{
	posExact = -1;
	if (solEval.empty())
	{
		return 0;
	}
	unordered_set<vector<int>, hashFunction > S_(solutionToVector(S, n));
	int eval = getEvaluation(S, n);
	int first = indexEval[0].first, second = 0;
	for (int i = indexEval.size() - 1; i >= 0; i--)
	{
		if (indexEval[i].second > eval && i == (indexEval.size() - 1))
		{
			return indexEval[i].first + 1;
		}
		if (indexEval[i].second == eval)
		{
			first = indexEval[i].first;
			i--;
			while (i >= 0 && indexEval[i].second == eval)
			{
				i--;
			}
			if (i >= 0)
			{
				second = indexEval[i].first + 1;
			}
			break;
		}
		else if (indexEval[i].second > eval)
		{
			second = indexEval[i].first + 1;
			first = indexEval[i + 1].first - 1;
			break;
		}
	}
	/*out << "Solution a traiter : " << endl;
	afficherOut(S_);
	out << "index :" << endl;
	afficherOut(indexEval);
	out << "first = " << first << "\t|second = " << second << endl;*/
	if (first < second)
	{
		return second;
	}
	bool trouver = false;
	int ind = -2;
	for (int i = first; i >= second; i--)
	{
		if (eval == solEval[i])
		{
			trouver = true;
			ind = i;
			/*out << "A comparer : " << endl;
			afficherOut(S_);
			out << "------WITH-----" << endl;
			afficherOut(list[i]);
			out << "---------------" << endl;*/
			if (equal(S_, list[i]))
			{
				//out << "===> EQUAL" << endl;
				posExact = i;
				return -1;
			}
			else
			{
				//out << "===> NOT EQUAL" << endl;
			}
		}
		else if (eval < solEval[i])
		{
			return i + 1;
		}
	}
	if (trouver)
	{
		return ind;
	}
	return second;
}

int** vectorToMatrix(unordered_set<vector<int>, hashFunction>& S, int n)
{
	int **S0 = new int*[n];
	for (int i = 0; i < n; i++)
	{
		S0[i] = new int[n];
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			S0[i][j] = 0;
		}
	}
	vector<vector<int> > SS;
	for (unordered_set<vector<int>, hashFunction>::iterator it = S.begin(); it != S.end(); it++)
	{
		SS.push_back(*it);
	}
	sort(SS.begin(), SS.end(), comparatorVector());
	for (int i = 0; i < SS.size(); i++)
	{
		for (int j = 0; j < SS[i].size(); j++)
		{
			S0[SS[i][j]][i] = 1;
		}
	}
	return S0;
}

void updateIndex(vector<int>& eval, vector<pair<int, int> >& index, bool ajout, int pos, int val = 0)
{
	if (ajout)
	{
		if (index.empty())
		{
			index.push_back(make_pair(0, val));
		}
		else
		{
			int range = ceil(sqrt(eval.size() + 1));
			if (pos == eval.size())
			{
				if (range == index.size())
				{
					index[index.size() - 1] = make_pair(eval.size(), val);
				}
				else
				{
					index.clear();
					index.push_back(make_pair(pos, val));
					pos -= range;
					while (pos >= 0)
					{
						int k = 0;
						while (k < range && (pos + 1) < eval.size())
						{
							if (eval[pos] == eval[pos + 1])
							{
								pos++;
								k++;
							}
							else
							{
								break;
							}
						}
						if (k == range)
						{
							pos -= range;
							while ((pos - 1) >= 0)
							{
								if (eval[pos] == eval[pos - 1])
								{
									pos--;
								}
								else
								{
									break;
								}
							}
							if (pos < 0)
							{
								break;
							}
							else
							{
								index.insert(index.begin(), make_pair(pos, eval[pos]));
								pos -= range;
							}
						}
						else
						{
							index.insert(index.begin(), make_pair(pos, eval[pos]));
							pos -= range;
						}						
					}
				}
			}
			else
			{
				if (range == index.size())
				{
					for (int k = index.size() - 1; k > -1; k--)
					{
						if (index[k].first >= pos)
						{
							index[k] = make_pair(index[k].first + 1, index[k].second);
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					index.clear();
					vector<int> eval_(eval);
					eval_.insert(eval_.begin() + pos, val);
					int ind = eval_.size() - 1;
					index.push_back(make_pair(ind, eval_[eval_.size() - 1]));
					ind -= range;
					while (ind >= 0)
					{
						int k = 0;
						while (k < range && (ind + 1) < eval.size())
						{
							if (eval_[ind] == eval_[ind + 1])
							{
								ind++;
								k++;
							}
							else
							{
								break;
							}
						}
						if (k == range)
						{
							ind -= range;
							while ((ind - 1) >= 0)
							{
								if (eval_[ind] == eval_[ind - 1])
								{
									ind--;
								}
								else
								{
									break;
								}
								
							}
							if (ind < 0)
							{
								break;
							}
							else
							{
								index.insert(index.begin(), make_pair(ind, eval_[ind]));
								ind -= range;
							}
						}
						else
						{
							index.insert(index.begin(), make_pair(ind, eval_[ind]));
							ind -= range;
						}
					}
				}
			}
		}
	}
	else
	{
		if (eval.size() == 1)
		{
			index.clear();
		}
		else
		{
			int range = ceil(sqrt(eval.size() - 1));
			index.clear();
			vector<int> eval_(eval);
			eval_.erase(eval_.begin() + pos);
			int ind = eval_.size() - 1;
			index.push_back(make_pair(ind, eval_[eval_.size() - 1]));
			ind -= range;
			while (ind >= 0)
			{
				int k = 0;
				while (k < range && (ind + 1) < eval_.size())
				{
					if (eval_[ind] == eval_[ind + 1])
					{
						ind++;
						k++;
					}
					else
					{
						break;
					}
				}
				if (k == range)
				{
					ind -= range;
					while ((ind - 1) >= 0)
					{
						if (eval_[ind] == eval_[ind - 1])
						{
							ind--;
						}
						else
						{
							break;
						}
					}
					if (ind < 0)
					{
						break;
					}
					else
					{
						index.insert(index.begin(), make_pair(ind, eval_[ind]));
						ind -= range;
					}
				}
				else
				{
					index.insert(index.begin(), make_pair(ind, eval_[ind]));
					ind -= range;
				}
			}
		}
	}
}

bool isSolution(map<int, vector<int> >& G, int **S)
{
	for (int j = 0; j < G.size(); j++)
	{
		for (int i = 0; i < (G.size() - 1); i++)
		{
			if (S[i][j] == 1)
			{
				for (int k = i + 1; k < G.size(); k++)
				{
					if (S[k][j] == 1)
					{
						if (find(G[i].begin(), G[i].end(), k) != G[i].end())
						{
							cout << "i = " << i << " et k = " << k << " dans le stable : " << j << endl;
							return false;
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < G.size(); i++)
	{
		bool trouver = false;
		for (int j = 0; j < G.size(); j++)
		{
			if (S[i][j] == 1)
			{
				trouver = true;
				break;
			}
		}
		if (!trouver)
		{
			cout << "Le sommet " << i << " n'existe pas" << endl;
			return false;
		}
	}
	return true;
}

bool isSolutionQ(map<int, vector<int> >& G, int **S)
{
	for (int j = 0; j < G.size(); j++)
	{
		map<int, vector<int> >::iterator it = G.begin();
		for (int i = 0; i < (G.size() - 1); i++)
		{
			if (S[i][j] == 1)
			{
				map<int, vector<int> >::iterator it2 = it;
				it2++;
				for (int k = i + 1; k < G.size(); k++)
				{
					if (S[k][j] == 1)
					{
						if (find(G[it->first].begin(), G[it->first].end(), it2->first) != G[it->first].end())
						{
							cout << "i = " << i << " et k = " << k << " dans le stable : " << j << endl;
							return false;
						}
					}
					it2++;
				}
			}
			it++;
		}
	}
	for (int i = 0; i < G.size(); i++)
	{
		bool trouver = false;
		for (int j = 0; j < G.size(); j++)
		{
			if (S[i][j] == 1)
			{
				trouver = true;
				break;
			}
		}
		if (!trouver)
		{
			cout << "Le sommet " << i << " n'existe pas" << endl;
			return false;
		}
	}
	return true;
}

void SqToS(map<int, vector<int> >& G, map<int, vector<int> >& Gq, int **Sq, int **S, vector<vector<int> >& serie, vector<vector<int> >& parallele)
{
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S[i][j] = 0;
		}
	}
	map<int, vector<int> >::iterator it = Gq.begin();
	for (int i = 0; i < Gq.size(); i++)
	{
		for (int j = 0; j < Gq.size(); j++)
		{
			if (Sq[i][j] == 1)
			{
				if (it->first < G.size())
				{
					S[it->first][j] = 1;
				}
				else if(it->first < (G.size() + serie.size()))
				{
					S[serie[it->first - G.size()][0]][j] = 1;
				}
				else
				{
					for (int k = 0; k < parallele[it->first - G.size() - serie.size()].size(); k++)
					{
						S[parallele[it->first - G.size() - serie.size()][k]][j] = 1;
					}
				}
				break;
			}
		}
		it++;
	}
	for (int h = 0; h < serie.size(); h++)
	{
		for (int k = 1; k < serie[h].size(); k++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				bool adj = false;
				for (int i = 0; i < G.size(); i++)
				{
					if (i != serie[h][k])
					{
						if (S[i][j] == 1)
						{
							for (int b = 0; b < G[i].size(); b++)
							{
								if (G[i][b] == serie[h][k])
								{
									adj = true;
									break;
								}
							}
							if (adj)
							{
								break;
							}
						}
					}
				}
				if (!adj)
				{
					S[serie[h][k]][j] = 1;
					break;
				}
			}
		}
	}
	triSolution(S, G.size());
}

// (k,1)-exchange + (min, max, aléatoire) diversification + tabu list bi-naire
void EBTS(map<int, vector<int> >& G, int** &S, int** &S0, int& Z, bool& diversification)
{
	if (isNull(S, G.size()))
	{
		cout << "here" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
	}
	else if (diversification)
	{
		cout << "here2" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0);
		diversification = false;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
	}
	else
	{
		cout << "here3" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = S[i][j];
			}
		}
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		/*cout << "iter = " << i << endl;
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{						
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
							//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							NS2.push_back(make_pair(j, k));
						}
						else if(B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
					/*else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						NS0.push_back(make_pair(j, k));
					}*/
				}
			}
		}
		if (indCN != -1)
		{
			choix = make_pair(indLN, indCN);
			amelioratif++;
		}
		else if(!NS1.empty())
		{
			choix = NS1[rand() % NS1.size()];
			inutil++;
		}
		else if(indCP != -1)
		{
			//choix = make_pair(indLP, indCP);
			choix = NS2[rand() % NS2.size()];
			couteux++;
		}
		else // Tous les mouvements sont interdits alors on choisit un aléatoirement sans vérifier la liste taboue
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
		}
		/*cout << "K = " << endl;
		afficher(K, (int) tailleInitial, (int) G.size());
		cout << "B = " << endl;
		afficher(B, (int) tailleInitial, (int) G.size());
		cout << "Tabu List : " << endl;
		afficher(tabuList, G.size(), G.size());
		cout << "Choix : " << choix.first << "\t| " << choix.second << endl;*/
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				/*if (j < choix.first)
				{
					tabuList[make_pair(j, choix.first)] = TABU_TENURE;
				}
				else
				{
					tabuList[make_pair(choix.first, j)] = TABU_TENURE;
				}*/
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					/*if (j < out[k])
					{
						tabuList[make_pair(j, out[k])] = TABU_TENURE;
					}
					else
					{
						tabuList[make_pair(out[k], j)] = TABU_TENURE;
					}*/
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//cout << "S0 apres tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}	
}

// (k,1)-exchange + (min, max, aléatoire) diversification + tabu list bi-naire + Affichage
void EBTSwithScreen(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification)
{
	if (isNull(S, G.size()))
	{
		out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		cost << Z << endl;
	}
	else if (diversification)
	{
		out << "Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0);
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		diversification = false;
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		out << "\tBest Actual = " << Z << endl;
	}
	else
	{
		out << "No Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = S[i][j];
			}
		}
		out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		out << "iter = " << i << endl;
		/*cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
							//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							NS2.push_back(make_pair(j, k));
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());*/
		if (indCN != -1)
		{
			choix = make_pair(indLN, indCN);
			amelioratif++;
			out << "Choix Amelioratif" << endl;
		}
		else if (!NS1.empty())
		{
			choix = NS1[rand() % NS1.size()];
			inutil++;
			out << "Choix Inutil" << endl;
		}
		else if (indCP != -1)
		{
			//choix = make_pair(indLP, indCP);
			choix = NS2[rand() % NS2.size()];
			couteux++;
			out << "Choix Couteux aleatoire" << endl;
		}
		else // Tous les mouvements sont interdits alors on choisit un aléatoirement sans vérifier la liste taboue
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
			out << "Choix Interdit" << endl;
		}
		out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		out << "Nouveau S0 apres tri : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}
}

// (k,1)-exchange + (min, max, aléatoire) diversification + tabu list k-naire
void EBTS2(map<int, vector<int> >& G, vector<vector<int> >& S, int& Z)
{
	vector<vector<int> > S0;
	if (S.empty())
	{
		ConstructedGreedyRandomizedSolution(G, S0);
		S = S0;
		Z = getEvaluation(S);
	}
	else
	{
		ConstructedGreedyRandomizedSolution(G, S0);
		if (Z > getEvaluation(S0))
		{
			S = S0;
			Z = getEvaluation(S);
		}
		else
		{
			S0 = S;
		}
		out << "best new solution : " << endl;
		afficherOut(S);
		out << "Z = " << Z << endl;
	}
	map<pair<int, vector<int> >, int> tabuList;

	for (int i = 0; i < ITER_EBTS; i++)
	{
		//cout << "iter = " << i << endl;
		//cout << "S0  : " << endl;
		//afficher(S0);
		//cout << "cout  = " << getEvaluation(S0) << endl;
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = S0.size();
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[S0.size() + 1];
			B[j] = new int[S0.size() + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < S0.size(); j++)
			{
				if (find(S0[j].begin(), S0[j].end(), it->first) != S0[j].end())
				{
					K[it->first][j] = -1;
					for (int k = 0; k < S0.size(); k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < S0[k].size(); h++)
							{
								if (find(G[it->first].begin(), G[it->first].end(), S0[k][h]) != G[it->first].end())
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][S0.size()] = 0;
			}
		}
		//calcul de B et choix de l'élément à déplacer
		vector<int> taille, position;
		vector<pair<int, int> > NS0, NS1, NS2;
		pair<int, int> choix;
		int minP = G.size() * S0.size(), indLP = -1, indCP = -1, minN = G.size() * S0.size(), indLN = -1, indCN = -1;
		for (int j = 0; j < S0.size(); j++)
		{
			taille.push_back(S0[j].size());
		}
		taille.push_back(0);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < S0.size(); k++)
			{
				if (find(S0[k].begin(), S0[k].end(), j) != S0[k].end())
				{
					position.push_back(k);
					break;
				}
			}
		}
		/*cout << "taille : " << endl;
		afficher(taille);
		cout << "position : " << endl;
		afficher(position);*/
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < S0.size(); k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < S0.size(); r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
								{
									t[r]++;
									t[k]--;
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								t[S0.size()]++;
								t[k]--;
							}
						}
					}
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
							}
							NS2.push_back(make_pair(j, k));
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
					/*else if (B[j][k] < 0)
					{
					if (minN > B[j][k])
					{
					indLN = j;
					indCN = k;
					minN = B[j][k];
					}
					NS0.push_back(make_pair(j, k));
					}*/
				}
			}
		}
		if (indCN != -1)
		{
			choix = make_pair(indLN, indCN);
			amelioratif++;
		}
		else if (!NS1.empty())
		{
			choix = NS1[rand() % NS1.size()];
			inutil++;
		}
		else if (indCP != -1)
		{
			choix = make_pair(indLP, indCP);
			couteux++;
		}
		else // Tous les mouvements sont interdits alors on choisit un aléatoirement sans vérifier la liste taboue
		{
			indCP = rand() % S0.size();
			indLP = rand() % G.size();
			while (K[indLP][indCP] == -1)
			{
				indCP = rand() % S0.size();
				indLP = rand() % G.size();
			}
			choix = make_pair(indLP, indCP);
			interdit++;
		}
		/*cout << "K = " << endl;
		afficher(K, (int) S0.size(), (int) G.size());
		cout << "B = " << endl;
		afficher(B, (int) S0.size(), (int) G.size());
		cout << "Tabu List : " << endl;
		afficher(tabuList);
		cout << "Choix : " << choix.first << "\t| " << choix.second << endl;*/
		// Construction de la nouvelle solution
		vector<int> out_, out2;
		S0[choix.second].push_back(choix.first);
		S0[position[choix.first]].erase(find(S0[position[choix.first]].begin(), S0[position[choix.first]].end(), choix.first));
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				out2.push_back(j);
			}
		}
		sort(out2.begin(), out2.end());
		tabuList[make_pair(choix.first, out2)] = TABU_TENURE;
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < S0.size(); k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[k].push_back(G[choix.first][j]);
						S0[choix.second].erase(find(S0[choix.second].begin(), S0[choix.second].end(), G[choix.first][j]));
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					if (tailleInitial == S0.size())
					{
						S0.push_back(vector<int>());
						S0[tailleInitial].push_back(G[choix.first][j]);
						S0[choix.second].erase(find(S0[choix.second].begin(), S0[choix.second].end(), G[choix.first][j]));
					}
					else
					{
						S0[tailleInitial].push_back(G[choix.first][j]);
						S0[choix.second].erase(find(S0[choix.second].begin(), S0[choix.second].end(), G[choix.first][j]));
					}
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		vector<int> adjOut;
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				adjOut.push_back(j);
			}
		}
		sort(adjOut.begin(), adjOut.end());
		for (int k = 0; k < out_.size(); k++)
		{
			tabuList[make_pair(out_[k], adjOut)] = TABU_TENURE;
		}
		sort(S0.begin(), S0.end(), comparatorVector());
		while (S0[S0.size() - 1].empty())
		{
			S0.erase(S0.end() - 1);
		}
		// Mise à jour de la solution
		if (Z > getEvaluation(S0))
		{
			S = S0;
			Z = getEvaluation(S);
			out << "best new solution : " << endl;
			afficherOut(S);
			out << "Z = " << Z << endl;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (map<pair<int, vector<int> >, int>::iterator it = tabuList.begin(); it != tabuList.end(); it++)
		{
			it->second--;
			if (it->second == 0)
			{
				if (it == tabuList.begin())
				{
					tabuList.erase(it);
					if (tabuList.empty())
					{
						break;
					}
					else
					{
						it = tabuList.begin();
						while (it->second == 0)
						{
							tabuList.erase(it);
							if (tabuList.empty())
							{
								break;
							}
							else
							{
								it = tabuList.begin();
							}
						}
						if (tabuList.empty())
						{
							break;
						}
					}
				}
				else
				{
					map<pair<int, vector<int> >, int>::iterator it2 = it;
					it2--;
					tabuList.erase(it);
					it = it2;
				}
			}
		}
	}

}

// (k,k')-exchange + (min, max, aléatoire) diversification + tabu list bi-naire
void EBTS3(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification)
{
	if (isNull(S, G.size()))
	{
		//cout << "here" << endl;
		ConstructedGreedyRandomizedInitialSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
	}
	else if (diversification)
	{
		//cout << "here2" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0);
		diversification = false;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
	}
	else
	{
		//cout << "here3" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = S[i][j];
			}
		}
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		/*cout << "iter = " << i << endl;
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille), out;
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							out.push_back(G[j][h]);
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					for (int h = 0; h < G.size(); h++)
					{
						if (j != h && K[h][position[j]] == -1)
						{
							bool canIn = true;
							for (int r = 0; r < G[h].size(); r++)
							{
								if (find(out.begin(), out.end(), G[h][r]) == out.end() && K[G[h][r]][k] == -1)
								{
									canIn = false;
									break;
								}
							}
							if (canIn)
							{
								t[position[j]]--;
								t[k]++;
							}

						}
					}
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							NS2.push_back(make_pair(j, k));
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}
		}
		if (indCN != -1)
		{
			choix = make_pair(indLN, indCN);
			amelioratif++;
		}
		else if (!NS1.empty())
		{
			choix = NS1[rand() % NS1.size()];
			inutil++;
		}
		else if (indCP != -1)
		{
			//choix = make_pair(indLP, indCP);
			choix = NS2[rand() % NS2.size()];
			couteux++;
		}
		else // Tous les mouvements sont interdits alors on choisit un aléatoirement sans vérifier la liste taboue
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
		}
		/*cout << "K = " << endl;
		afficher(K, (int) tailleInitial, (int) G.size());
		cout << "B = " << endl;
		afficher(B, (int) tailleInitial, (int) G.size());
		cout << "Tabu List : " << endl;
		afficher(tabuList, G.size(), G.size());
		cout << "Choix : " << choix.first << "\t| " << choix.second << endl;*/
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		vector<int> within;
		for (int h = 0; h < G.size(); h++)
		{
			if (choix.first != h && K[h][position[choix.first]] == -1)
			{
				bool canIn = true;
				for (int r = 0; r < G[h].size(); r++)
				{
					if (find(out_.begin(), out_.end(), G[h][r]) == out_.end() && K[G[h][r]][choix.second] == -1)
					{
						canIn = false;
						break;
					}
				}
				if (canIn)
				{
					S0[h][choix.second] = 1;
					S0[h][position[choix.first]] = 0;
					within.push_back(h);
				}

			}
		}
		for (int h = 0; h < within.size(); h++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				if (j != choix.first && find(within.begin(), within.end(), j) != within.end() && position[j] == position[within[h]])
				{
					tabuList[j][within[h]] = TABU_TENURE;
					tabuList[within[h]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//cout << "S0 apres tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}
}

// (k,k')-exchange + (min, max, aléatoire) diversification + tabu list k-aire
void EBTS4(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification)
{
	if (isNull(S, G.size()))
	{
		//cout << "here" << endl;
		ConstructedGreedyRandomizedInitialSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
	}
	else if (diversification)
	{
		//cout << "here2" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0);
		diversification = false;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
	}
	else
	{
		//cout << "here3" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = S[i][j];
			}
		}
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		/*cout << "iter = " << i << endl;
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille), out;
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							out.push_back(G[j][h]);
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					for (int h = 0; h < G.size(); h++)
					{
						if (j != h && K[h][position[j]] == -1)
						{
							bool canIn = true;
							for (int r = 0; r < G[h].size(); r++)
							{
								if (find(out.begin(), out.end(), G[h][r]) == out.end() && K[G[h][r]][k] == -1)
								{
									canIn = false;
									break;
								}
							}
							if (canIn)
							{
								t[position[j]]--;
								t[k]++;
							}

						}
					}
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							NS2.push_back(make_pair(j, k));
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}
		}
		if (indCN != -1)
		{
			choix = make_pair(indLN, indCN);
			amelioratif++;
		}
		else if (!NS1.empty())
		{
			choix = NS1[rand() % NS1.size()];
			inutil++;
		}
		else if (indCP != -1)
		{
			//choix = make_pair(indLP, indCP);
			choix = NS2[rand() % NS2.size()];
			couteux++;
		}
		else // Tous les mouvements sont interdits alors on choisit un aléatoirement sans vérifier la liste taboue
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
		}
		/*cout << "K = " << endl;
		afficher(K, (int) tailleInitial, (int) G.size());
		cout << "B = " << endl;
		afficher(B, (int) tailleInitial, (int) G.size());
		cout << "Tabu List : " << endl;
		afficher(tabuList, G.size(), G.size());
		cout << "Choix : " << choix.first << "\t| " << choix.second << endl;*/
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		vector<int> within;
		for (int h = 0; h < G.size(); h++)
		{
			if (choix.first != h && K[h][position[choix.first]] == -1)
			{
				bool canIn = true;
				for (int r = 0; r < G[h].size(); r++)
				{
					if (find(out_.begin(), out_.end(), G[h][r]) == out_.end() && K[G[h][r]][choix.second] == -1)
					{
						canIn = false;
						break;
					}
				}
				if (canIn)
				{
					S0[h][choix.second] = 1;
					S0[h][position[choix.first]] = 0;
					within.push_back(h);
				}

			}
		}
		for (int h = 0; h < within.size(); h++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				if (j != choix.first && find(within.begin(), within.end(), j) != within.end() && position[j] == position[within[h]])
				{
					tabuList[j][within[h]] = TABU_TENURE;
					tabuList[within[h]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//cout << "S0 apres tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}
}

// (k,1)-exchange + (min, max, aléatoire) diversification + tabu list bi-naire + DataMining
void EBTS5(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification, int **DM)
{
	if (isNull(S, G.size()))
	{
		//cout << "here" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		updateDM(DM, S, G.size());
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
	}
	else if (diversification)
	{
		//cout << "here2" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0);
		updateDM(DM, S, G.size());
		diversification = false;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
	}
	else
	{
		//cout << "here3" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = S[i][j];
			}
		}
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		/*cout << "iter = " << i << endl;
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							NS2.push_back(make_pair(j, k));
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
									{
										NS0.insert(NS0.begin()+ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
					/*else if (B[j][k] < 0)
					{
					if (minN > B[j][k])
					{
					indLN = j;
					indCN = k;
					minN = B[j][k];
					}
					NS0.push_back(make_pair(j, k));
					}*/
				}
			}
		}
		if (indCN != -1 && ((getEvaluation(S0, G.size()) + minN) < Z))
		{
			choix = make_pair(indLN, indCN);
			amelioratif++;
		}
		else if(indCN != -1 && ((getEvaluation(S0, G.size()) + minN) >= Z))
		{
			for (int j = 0; j < NS0.size(); j++)
			{
				
			}
		}
		if (indCN != -1)
		{
			choix = make_pair(indLN, indCN);
			amelioratif++;
		}
		else if (!NS1.empty())
		{
			choix = NS1[rand() % NS1.size()];
			inutil++;
		}
		else if (indCP != -1)
		{
			//choix = make_pair(indLP, indCP);
			choix = NS2[rand() % NS2.size()];
			couteux++;
		}
		else // Tous les mouvements sont interdits alors on choisit un aléatoirement sans vérifier la liste taboue
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
		}
		/*cout << "K = " << endl;
		afficher(K, (int) tailleInitial, (int) G.size());
		cout << "B = " << endl;
		afficher(B, (int) tailleInitial, (int) G.size());
		cout << "Tabu List : " << endl;
		afficher(tabuList, G.size(), G.size());
		cout << "Choix : " << choix.first << "\t| " << choix.second << endl;*/
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				/*if (j < choix.first)
				{
				tabuList[make_pair(j, choix.first)] = TABU_TENURE;
				}
				else
				{
				tabuList[make_pair(choix.first, j)] = TABU_TENURE;
				}*/
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					/*if (j < out[k])
					{
					tabuList[make_pair(j, out[k])] = TABU_TENURE;
					}
					else
					{
					tabuList[make_pair(out[k], j)] = TABU_TENURE;
					}*/
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//cout << "S0 apres tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
		/*for (map<pair<int, int>, int>::iterator it = tabuList.begin(); it != tabuList.end(); it++)
		{
		it->second--;
		if (it->second == 0)
		{
		if (it == tabuList.begin())
		{
		tabuList.erase(it);
		if (tabuList.empty())
		{
		break;
		}
		else
		{
		it = tabuList.begin();
		while (it->second == 0)
		{
		tabuList.erase(it);
		if (tabuList.empty())
		{
		break;
		}
		else
		{
		it = tabuList.begin();
		}
		}
		if (tabuList.empty())
		{
		break;
		}
		}
		}
		else
		{
		map<pair<int, int>, int>::iterator it2 = it;
		it2--;
		tabuList.erase(it);
		it = it2;
		}
		}
		}*/
	}

}

// (k,1)-exchange + (min, max, aléatoire) diversification + tabu list bi-naire + DataMining + Affichage
void EBTS5withScreen(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification, int **DM)
{
	if (isNull(S, G.size()))
	{
		out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		cost << Z << endl;
		updateDM(DM, S, G.size());
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else if (diversification)
	{
		out << "Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0);
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());
		diversification = false;
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		out << "\tBest Actual = " << Z << endl;
	}
	else
	{
		out << "No Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = S[i][j];
			}
		}
		out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		out << "iter = " << i << endl;
		/*cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							NS2.push_back(make_pair(j, k));
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
									{
										NS0.insert(NS0.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
		if (!NS0.empty() && ((getEvaluation(S0, G.size()) + minN) < Z))
		{
			choix = make_pair(indLN, indCN);
			amelioratif++;
			out << "Choix amelioratif (intensification Normale)" << endl;
		}
		else if (indCN != -1 && ((getEvaluation(S0, G.size()) + minN) >= Z))
		{
			bool trouver = false;
			for (int j = 0; j < NS0.size(); j++)
			{
				for (int k = 0; k < G.size(); k++)
				{
					if (k != NS0[j].first)
					{
						if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
						{
							if (position[k] == NS0[j].second && DM[NS0[j].first][k] == 0)
							{
								choix = NS0[j];
								trouver = true;
								prometeuxFort1++;
								out << "Prometeux Fort (new intensification)" << endl;
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
							if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
					out << "Prometeux Fort 2 (old intensification)" << endl;
				}
				else
				{
					choix = NS0[0];
					prometeuxFort2++;
					out << "Prometeux Fort 2 (old intensification) : premier forcé" << endl;
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
						if (find(G[NS1[j].first].begin(), G[NS1[j].first].end(), k) == G[NS1[j].first].end())
						{
							if (position[k] == NS1[j].second && DM[NS1[j].first][k] == 0)
							{
								choix = NS1[j];
								trouver = true;
								prometeuxFaible1++;
								out << "Prometeux Faible 1 (new diversification)" << endl;
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
								if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
								{
									if (position[k] == NS2[j].second && DM[NS2[j].first][k] == 0)
									{
										choix = NS2[j];
										trouver = true;
										prometeuxFaible2++;
										out << "Prometeux Faible 2 (new diversification)" << endl;
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
									if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
							out << "Couteux (old diversification)" << endl;
						}
						else
						{
							choix = NS2[0];
							couteux++;
							out << "Couteux (old diversification) : premier forcé" << endl;
						}
					}
				}
				else // Cas inutil : NS0 et NS2 vides en plus, NS1 ne vérifie pas les conditions alors on prend un cas inutil
				{
					choix = NS1[0];
					trouver = true;
					inutil++;
					out << "Inutil" << endl;
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
						if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
						{
							if (position[k] == NS2[j].second && DM[NS2[j].first][k] == 0)
							{
								choix = NS2[j];
								trouver = true;
								prometeuxFaible2++;
								out << "Prometeux Faible 2 (new diversification)" << endl;
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
							if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
					out << "Couteux (old diversification)" << endl;
				}
				else
				{
					choix = NS2[0];
					couteux++;
					out << "Couteux (old diversification) : premier forcé" << endl;
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
			out << "Choix interdit" << endl;
		}
			
		out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		out << "S0 apres tri : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}
}

// (k,1)-exchange + (min, max, aléatoire) diversification + tabu list bi-naire + DataMining + No Big Diversification + DM Greedy
void EBTS6(map<int, vector<int> >& G, int** S, int** S0, int& Z, bool& diversification, int** DM)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		updateDM(DM, S, G.size());
		/*out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
	}
	else if (diversification)
	{
		//out << "Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0, DM);
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		diversification = false;
		//cost << getEvaluation(S0, G.size()) << endl;
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
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	else
	{
		//out << "No Diversification" << endl;
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	int** tabuList = new int* [G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		//out << "iter = " << i << endl;
		int** K = new int* [G.size()], ** B = new int* [G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							NS2.push_back(make_pair(j, k));
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
									{
										NS0.insert(NS0.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
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
					if (k != NS0[j].first)
					{
						if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
							if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
					prometeuxFort2++;
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
						if (find(G[NS1[j].first].begin(), G[NS1[j].first].end(), k) == G[NS1[j].first].end())
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
								if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
									if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
							couteux++;
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
						if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
							if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
					couteux++;
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

		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
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
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}
}

// (k,1)-exchange + (min, max, aléatoire) diversification + tabu list bi-naire + DataMining + No Big Diversification + DM Greedy + Affichage
void EBTS6withScreen(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification, int **DM)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		cost << Z << endl;
		updateDM(DM, S, G.size());
		/*out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
	}
	else if (diversification)
	{
		//out << "Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0, DM);
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		diversification = false;
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	else
	{
		//out << "No Diversification" << endl;
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		//out << "iter = " << i << endl;
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							NS2.push_back(make_pair(j, k));
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
									{
										NS0.insert(NS0.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
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
					if (k != NS0[j].first)
					{
						if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
							if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
					prometeuxFort2++;
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
						if (find(G[NS1[j].first].begin(), G[NS1[j].first].end(), k) == G[NS1[j].first].end())
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
								if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
									if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
							couteux++;
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
						if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
							if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
					couteux++;
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

		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}
}

// (k,1)-exchange + min diversification + tabu list bi-naire + DataMining + Affichage
void EBTS7withScreen(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification, int **DM)
{
	if (isNull(S, G.size()))
	{
		out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		cost << Z << endl;
		updateDM(DM, S, G.size());
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else if (diversification)
	{
		out << "Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0);
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());
		diversification = false;
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		out << "\tBest Actual = " << Z << endl;
	}
	else
	{
		out << "No Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = S[i][j];
			}
		}
		out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		out << "iter = " << i << endl;
		/*cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							if (NS2.empty())
							{
								NS2.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS2.size(); ind++)
								{
									if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
									{
										NS2.insert(NS2.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
									{
										NS0.insert(NS0.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
		if (!NS0.empty() && ((getEvaluation(S0, G.size()) + minN) < Z))
		{
			choix = make_pair(indLN, indCN);
			amelioratif++;
			out << "Choix amelioratif (intensification Normale)" << endl;
		}
		else if (indCN != -1 && ((getEvaluation(S0, G.size()) + minN) >= Z))
		{
			bool trouver = false;
			for (int j = 0; j < NS0.size(); j++)
			{
				for (int k = 0; k < G.size(); k++)
				{
					if (k != NS0[j].first)
					{
						if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
						{
							if (position[k] == NS0[j].second && DM[NS0[j].first][k] == 0)
							{
								choix = NS0[j];
								trouver = true;
								prometeuxFort1++;
								out << "Prometeux Fort (new intensification)" << endl;
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
							if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
					out << "Prometeux Fort 2 (old intensification)" << endl;
				}
				else
				{
					choix = NS0[0];
					prometeuxFort2++;
					out << "Prometeux Fort 2 (old intensification) : premier forcé" << endl;
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
						if (find(G[NS1[j].first].begin(), G[NS1[j].first].end(), k) == G[NS1[j].first].end())
						{
							if (position[k] == NS1[j].second && DM[NS1[j].first][k] == 0)
							{
								choix = NS1[j];
								trouver = true;
								prometeuxFaible1++;
								out << "Prometeux Faible 1 (new diversification)" << endl;
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
								if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
								{
									if (position[k] == NS2[j].second && DM[NS2[j].first][k] == 0)
									{
										choix = NS2[j];
										trouver = true;
										prometeuxFaible2++;
										out << "Prometeux Faible 2 (new diversification)" << endl;
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
									if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
							out << "Couteux (old diversification)" << endl;
						}
						else
						{
							choix = NS2[0];
							couteux++;
							out << "Couteux (old diversification) : premier forcé" << endl;
						}
					}
				}
				else // Cas inutil : NS0 et NS2 vides en plus, NS1 ne vérifie pas les conditions alors on prend un cas inutil
				{
					choix = NS1[0];
					trouver = true;
					inutil++;
					out << "Inutil" << endl;
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
						if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
						{
							if (position[k] == NS2[j].second && DM[NS2[j].first][k] == 0)
							{
								choix = NS2[j];
								trouver = true;
								prometeuxFaible2++;
								out << "Prometeux Faible 2 (new diversification)" << endl;
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
							if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
					out << "Couteux (old diversification)" << endl;
				}
				else
				{
					choix = NS2[0];
					couteux++;
					out << "Couteux (old diversification) : premier forcé" << endl;
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
			out << "Choix interdit" << endl;
		}

		out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		out << "S0 apres tri : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}

}

// (k,1)-exchange + min diversification + tabu list bi-naire + DataMining
void EBTS7(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification, int **DM)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else if (diversification)
	{
		//out << "Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0);
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		diversification = false;
		//cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	else
	{
		//out << "No Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = S[i][j];
			}
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		//out << "iter = " << i << endl;
		/*cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							if (NS2.empty())
							{
								NS2.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS2.size(); ind++)
								{
									if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
									{
										NS2.insert(NS2.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
									{
										NS0.insert(NS0.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
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
					if (k != NS0[j].first)
					{
						if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
							if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
						if (find(G[NS1[j].first].begin(), G[NS1[j].first].end(), k) == G[NS1[j].first].end())
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
								if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
									if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
						if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
							if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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

		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}

}

// (k,1)-exchange + min diversification + tabu list bi-naire + DataMining + DM Greedy + Affichage
void EBTS8withScreen(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification, int **DM)
{
	if (isNull(S, G.size()))
	{
		out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		cost << Z << endl;
		updateDM(DM, S, G.size());
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else if (diversification)
	{
		out << "Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0, DM);
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());
		diversification = false;
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		out << "\tBest Actual = " << Z << endl;
	}
	else
	{
		out << "No Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = S[i][j];
			}
		}
		out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		out << "iter = " << i << endl;
		/*cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							if (NS2.empty())
							{
								NS2.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS2.size(); ind++)
								{
									if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
									{
										NS2.insert(NS2.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
									{
										NS0.insert(NS0.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
		if (!NS0.empty() && ((getEvaluation(S0, G.size()) + minN) < Z))
		{
			choix = make_pair(indLN, indCN);
			amelioratif++;
			out << "Choix amelioratif (intensification Normale)" << endl;
		}
		else if (indCN != -1 && ((getEvaluation(S0, G.size()) + minN) >= Z))
		{
			bool trouver = false;
			for (int j = 0; j < NS0.size(); j++)
			{
				for (int k = 0; k < G.size(); k++)
				{
					if (k != NS0[j].first)
					{
						if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
						{
							if (position[k] == NS0[j].second && DM[NS0[j].first][k] == 0)
							{
								choix = NS0[j];
								trouver = true;
								prometeuxFort1++;
								out << "Prometeux Fort (new intensification)" << endl;
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
							if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
					out << "Prometeux Fort 2 (old intensification)" << endl;
				}
				else
				{
					choix = NS0[0];
					prometeuxFort2++;
					out << "Prometeux Fort 2 (old intensification) : premier forcé" << endl;
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
						if (find(G[NS1[j].first].begin(), G[NS1[j].first].end(), k) == G[NS1[j].first].end())
						{
							if (position[k] == NS1[j].second && DM[NS1[j].first][k] == 0)
							{
								choix = NS1[j];
								trouver = true;
								prometeuxFaible1++;
								out << "Prometeux Faible 1 (new diversification)" << endl;
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
								if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
								{
									if (position[k] == NS2[j].second && DM[NS2[j].first][k] == 0)
									{
										choix = NS2[j];
										trouver = true;
										prometeuxFaible2++;
										out << "Prometeux Faible 2 (new diversification)" << endl;
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
									if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
							out << "Couteux (old diversification)" << endl;
						}
						else
						{
							choix = NS2[0];
							couteux++;
							out << "Couteux (old diversification) : premier forcé" << endl;
						}
					}
				}
				else // Cas inutil : NS0 et NS2 vides en plus, NS1 ne vérifie pas les conditions alors on prend un cas inutil
				{
					choix = NS1[0];
					trouver = true;
					inutil++;
					out << "Inutil" << endl;
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
						if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
						{
							if (position[k] == NS2[j].second && DM[NS2[j].first][k] == 0)
							{
								choix = NS2[j];
								trouver = true;
								prometeuxFaible2++;
								out << "Prometeux Faible 2 (new diversification)" << endl;
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
							if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
					out << "Couteux (old diversification)" << endl;
				}
				else
				{
					choix = NS2[0];
					couteux++;
					out << "Couteux (old diversification) : premier forcé" << endl;
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
			out << "Choix interdit" << endl;
		}

		out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		out << "S0 apres tri : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}

}

// (k,1)-exchange + min diversification + tabu list bi-naire + DataMining + DM Greedy
void EBTS8(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification, int **DM)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else if (diversification)
	{
		//out << "Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0, DM);
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		diversification = false;
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	else
	{
		//out << "No Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = S[i][j];
			}
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		//out << "iter = " << i << endl;
		/*cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							if (NS2.empty())
							{
								NS2.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS2.size(); ind++)
								{
									if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
									{
										NS2.insert(NS2.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
									{
										NS0.insert(NS0.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
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
					if (k != NS0[j].first)
					{
						if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
							if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
						if (find(G[NS1[j].first].begin(), G[NS1[j].first].end(), k) == G[NS1[j].first].end())
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
								if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
									if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
						if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
							if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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

		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}
}

// (k,1)-exchange + min diversification + tabu list bi-naire + DataMining + DM Greedy + No Diversification (recherche continue)
void EBTS9(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification, int **DM)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else if (diversification)
	{
		//out << "Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0, DM);
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		diversification = false;
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	else
	{
		//out << "No Diversification" << endl;
		
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	int **tabuList = new int*[G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		//out << "iter = " << i << endl;
		/*cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							if (NS2.empty())
							{
								NS2.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS2.size(); ind++)
								{
									if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
									{
										NS2.insert(NS2.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(make_pair(j, k));
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
									{
										NS0.insert(NS0.begin() + ind, make_pair(j, k));
										break;
									}
								}
							}
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(make_pair(j, k));
						}
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
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
					if (k != NS0[j].first)
					{
						if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
							if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
						if (find(G[NS1[j].first].begin(), G[NS1[j].first].end(), k) == G[NS1[j].first].end())
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
								if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
									if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
						if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
							if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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

		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}
}

// (k,1)-exchange + min diversification + no tabu list bi-naire + Historical
void EBTS10(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification, int **DM, vector<int**>& treatedSolutions, vector<int>& treatedEval)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S0[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(G, S0);
			int posTreated = exist(treatedSolutions, treatedEval, S0, G.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		diversification = false;
		//cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<int**> hopefulSolutions;
	vector<int> solEval;
	//for (int i = 0; i < ITER_EBTS; i++)
	while(true)
	{
		//out << "iter = " << i << endl;
		/*cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						if (minP > B[j][k])
							//if(maxP < B[j][k])
						{
							indLP = j;
							indCP = k;
							minP = B[j][k];
							//maxP = B[j][k];
						}
						if (NS2.empty())
						{
							NS2.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS2.size(); ind++)
							{
								if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
								{
									NS2.insert(NS2.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
		if (!NS0.empty())
		{
			choix = NS0[0];
			amelioratif++;
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);
				
				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posHopeful = exist(hopefulSolutions, solEval, S0_, G.size());
				int posTreated = exist(treatedSolutions, treatedEval, S0_, G.size());
				if (posHopeful != -1 && posTreated != -1)
				{
					hopefulSolutions.insert(hopefulSolutions.begin() + posHopeful, S0_);
					solEval.insert(solEval.begin() + posHopeful, getEvaluation(S0_, G.size()));
				}
				posTreated = exist(treatedSolutions, treatedEval, S0, G.size());
				if (posTreated != -1)
				{
					treatedSolutions.insert(treatedSolutions.begin() + posTreated, S0);
					treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
				}
			}
		}
		else if(!hopefulSolutions.empty())
		{
			int posTreated = exist(treatedSolutions, treatedEval, S0, G.size());
			if (posTreated != -1)
			{
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, S0);
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			for (int j = 0; j < G.size(); j++)
			{
				for (int k = 0; k < G.size(); k++)
				{
					S0[j][k] = hopefulSolutions[hopefulSolutions.size() - 1][j][k];
				}
			}
			hopefulSolutions.pop_back();
			solEval.pop_back();
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			continue;
		}
		else
		{
			int posTreated = exist(treatedSolutions, treatedEval, S0, G.size());
			if (posTreated != -1)
			{
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, S0);
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			break;
		}
		
		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}

}

// (k,1)-exchange + min diversification + no tabu list bi-naire + Historical
void EBTS10withScreen(map<int, vector<int> >& G, int **S, int **S0, int& Z, bool& diversification, int **DM, vector<int**>& treatedSolutions, vector<int>& treatedEval)
{
	/*if (isNull(S, G.size()))
	{
		out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		updateDM(DM, S, G.size());
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S0[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(G, S0);
			int posTreated = exist(treatedSolutions, treatedEval, S0, G.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		diversification = false;
		//cost << getEvaluation(S0, G.size()) << endl;
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
			//ameliorateS0++;
		}
		out << "\tBest Actual = " << Z << endl;
	}*/
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<int**> hopefulSolutions;
	vector<int> solEval;
	// Fixe solution
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S0[i][j] = 0;
		}
	}
	S0[0][0] = 1;
	S0[2][0] = 1;
	S0[8][0] = 1;
	S0[9][0] = 1;
	S0[1][1] = 1;
	S0[4][1] = 1;
	S0[6][1] = 1;
	S0[3][2] = 1;
	S0[5][2] = 1;
	S0[7][3] = 1;
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S[i][j] = S0[i][j];
		}
	}
	Z = getEvaluation(S, G.size());
	for (int i = 0; i < ITER_EBTS; i++)
	{
		out << "iter = " << i << endl;
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
		vector<int> ss(getSTaille(S0, G.size()));
		afficher(ss);
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "cout = " << getEvaluation(S0, G.size()) << endl;
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		cout << "taille initiale = " << tailleInitial << endl;
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
			}
			K[it->first][tailleInitial] = 0;
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			//cout << "j = " << j << endl;
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				//cout << "k = " << k << endl;
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					/*cout << "t = ";
					afficher(t);
					cout << "taille = ";
					afficher(taille);*/
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						if (minP > B[j][k])
							//if(maxP < B[j][k])
						{
							indLP = j;
							indCP = k;
							minP = B[j][k];
							//maxP = B[j][k];
						}
						if (NS2.empty())
						{
							NS2.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS2.size(); ind++)
							{
								if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
								{
									NS2.insert(NS2.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
		}
		out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());

		if (!NS0.empty())
		{
			cout << "taille NS0 : " << NS0.size() << endl;
			out << "\ttaille NS0 : " << NS0.size() << endl;
			choix = NS0[0];
			amelioratif++;
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posHopeful = exist(hopefulSolutions, solEval, S0_, G.size());
				int posTreated = exist(treatedSolutions, treatedEval, S0_, G.size());
				if (posHopeful != -1 && posTreated != -1)
				{
					hopefulSolutions.insert(hopefulSolutions.begin() + posHopeful, S0_);
					solEval.insert(solEval.begin() + posHopeful, getEvaluation(S0_, G.size()));
				}
				posTreated = exist(treatedSolutions, treatedEval, S0, G.size());
				if (posTreated != -1)
				{
					treatedSolutions.insert(treatedSolutions.begin() + posTreated, S0);
					treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
				}
			}
		}
		else if (!hopefulSolutions.empty())
		{
			cout << "Hopeful choice" << endl;
			out << "\tHopeful choice" << endl;
			int posTreated = exist(treatedSolutions, treatedEval, S0, G.size());
			if (posTreated != -1)
			{
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, S0);
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			out << "Liste Hopeful actuel : " << endl;
			for (int k = 0; k < hopefulSolutions.size(); k++)
			{
				afficherSolutionOut(hopefulSolutions[k], G.size(), getTaille(hopefulSolutions[k], G.size()));
			}
			for (int j = 0; j < G.size(); j++)
			{
				for (int k = 0; k < G.size(); k++)
				{
					S0[j][k] = hopefulSolutions[hopefulSolutions.size() - 1][j][k];
				}
			}
			//S0 = hopefulSolutions[hopefulSolutions.size() - 1];
			hopefulSolutions.pop_back();
			solEval.pop_back();
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			continue;
		}
		else if(!NS1.empty())
		{
			choix = NS1[0];
		}
		else
		{
			cout << "No hopeful choice" << endl;
			out << "\tNo hopeful choice" << endl;
			int posTreated = exist(treatedSolutions, treatedEval, S0, G.size());
			if (posTreated != -1)
			{
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, S0);
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			break;
		}

		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;

		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		if (i < ITER_EBTS - 1)
		{
			cout << "Nomber of solutions in Hopeful : " << hopefulSolutions.size() << endl;
			out << "Nomber of solutions in Hopeful : " << hopefulSolutions.size() << endl;
		}
	}

}

// (k,1)-exchange + min and = 0 diversification + no tabu list bi-naire + Historical + Set uses + index
void EBTS11(map<int, vector<int> >& G, int **S, int **S0, int& Z, int **DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int,int> >& indexTreated)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		//updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S0[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(G, S0);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<unordered_set<vector<int>, hashFunction> > hopefulSolutions;
	vector<int> solEval;
	vector<pair<int, int> > indexHopeful;
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;
	
	
	//for (int i = 0; i < ITER_EBTS; i++)
	while (true)
	{
		//out << "iter = " << i << endl;
		/*cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int Z0 = getEvaluation(S0, G.size());
		cout << "Z = " << Z << endl;
		cout << "Z0 = " << Z0 << endl;
		int posHF = exist(hopefulSolutions, solEval, indexHopeful, S0, G.size());
		if (posHF == -1)
		{
			if (!hopefulSolutions.empty())
			{
				updateIndex(solEval, indexHopeful, false, solEval.size() - 1);
				S0 = vectorToMatrix(hopefulSolutions[hopefulSolutions.size() - 1], G.size());
				hopefulSolutions.pop_back();
				solEval.pop_back();
				//updateDM(DM, S0, G.size());
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else if (!alternativeSolutions.empty())
			{
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				//updateDM(DM, S0, G.size());
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			int posAL = exist(alternativeSolutions, alterEval, indexAlternative, S0, G.size());
			if (posAL == -1)
			{
				if (!hopefulSolutions.empty())
				{
					updateIndex(solEval, indexHopeful, false, solEval.size() - 1);
					S0 = vectorToMatrix(hopefulSolutions[hopefulSolutions.size() - 1], G.size());
					hopefulSolutions.pop_back();
					solEval.pop_back();
					//updateDM(DM, S0, G.size());
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else if (!alternativeSolutions.empty())
				{
					updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
					S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
					alternativeSolutions.pop_back();
					alterEval.pop_back();
					//updateDM(DM, S0, G.size());
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else
				{
					break;
				}
			}
			else
			{
				int posTR = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
				if (posTR == -1)
				{
					if (!hopefulSolutions.empty())
					{
						updateIndex(solEval, indexHopeful, false, solEval.size() - 1);
						S0 = vectorToMatrix(hopefulSolutions[hopefulSolutions.size() - 1], G.size());
						hopefulSolutions.pop_back();
						solEval.pop_back();
						//updateDM(DM, S0, G.size());
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
							out << "best new solution : " << endl;
							afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
							out << "Z = " << Z << endl;
							vraiAmelioration++;
						}
						continue;
					}
					else if (!alternativeSolutions.empty())
					{
						updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
						S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
						alternativeSolutions.pop_back();
						alterEval.pop_back();
						//updateDM(DM, S0, G.size());
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
							out << "best new solution : " << endl;
							afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
							out << "Z = " << Z << endl;
							vraiAmelioration++;
						}
						continue;
					}
					else
					{
						break;
					}
				}
			}
		}
		
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
			}
			K[it->first][tailleInitial] = 0;
		}
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < G.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							//out << "One move applied" << endl;
							S0[i][h] = 1;
							S0[i][k] = 0;
							bool trouver = false;
							for (int j = 0; j < G.size(); j++)
							{
								if (S0[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								//cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < G.size(); j++)
									{
										S0[j][h - 1] = S0[j][h];
									}
								}
							}
							triSolution(S0, G.size());
							move = true;
							break;
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						/*if (minP > B[j][k])
							//if(maxP < B[j][k])
						{
							indLP = j;
							indCP = k;
							minP = B[j][k];
							//maxP = B[j][k];
						}
						if (NS2.empty())
						{
							NS2.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS2.size(); ind++)
							{
								if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
								{
									NS2.insert(NS2.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}*/
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
		if (!NS0.empty())
		{
			choix = NS0[0];
			amelioratif++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				
				int posHopeful = exist(hopefulSolutions, solEval, indexHopeful, S0_, G.size());
				if (posHopeful != -1)
				{
					int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
					if (posAlternative != -1)
					{
						posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
						if (posTreated != -1)
						{
							int *Z0_ = new int;
							*Z0_ = getEvaluation(S0_, G.size());
							updateIndex(solEval, indexHopeful, true, posHopeful, *Z0_);
							hopefulSolutions.insert(hopefulSolutions.begin() + posHopeful, solutionToVector(S0_, G.size()));
							solEval.insert(solEval.begin() + posHopeful, *Z0_);
							free(Z0_);
							Z0_ = nullptr;
						}
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posHopeful = exist(hopefulSolutions, solEval, indexHopeful, S0_, G.size());
				if (posHopeful != -1)
				{
					int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
					if (posAlternative != -1)
					{
						posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
						if (posTreated != -1)
						{
							int *Z0_ = new int;
							*Z0_ = getEvaluation(S0_, G.size());
							updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
							alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
							alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
							free(Z0_);
							Z0_ = nullptr;
						}
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!hopefulSolutions.empty())
		{
			int posTreated;
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posHopeful = exist(hopefulSolutions, solEval, indexHopeful, S0_, G.size());
				if (posHopeful != -1)
				{
					int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
					if (posAlternative != -1)
					{
						posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
						if (posTreated != -1)
						{
							int *Z0_ = new int;
							*Z0_ = getEvaluation(S0_, G.size());
							updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
							alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
							alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
							free(Z0_);
							Z0_ = nullptr;
						}
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			updateIndex(solEval, indexHopeful, false, solEval.size() - 1);
			S0 = vectorToMatrix(hopefulSolutions[hopefulSolutions.size() - 1], G.size());
			hopefulSolutions.pop_back();
			solEval.pop_back();			
			//updateDM(DM, S0, G.size());
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			
			continue;
		}
		else if(!NS1.empty())
		{
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posHopeful = exist(hopefulSolutions, solEval, indexHopeful, S0_, G.size());
				if (posHopeful != -1)
				{
					int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
					if (posAlternative != -1)
					{
						posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
						if (posTreated != -1)
						{
							int *Z0_ = new int;
							*Z0_ = getEvaluation(S0_, G.size());
							updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
							alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
							alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
							free(Z0_);
							Z0_ = nullptr;
						}
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			//updateDM(DM, S0, G.size());
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			continue;
		}
		else
		{
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			break;
		}

		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;

		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k,1)-exchange + min and = 0 diversification + no tabu list bi-naire + Historical + Set uses + index + Affichage
void EBTS11withScreen(map<int, vector<int> >& G, int **S, int **S0, int& Z, int **DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated)
{
	if (isNull(S, G.size()))
	{
		out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		cost << Z << endl;
		//updateDM(DM, S, G.size());
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S0[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(G, S0);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	// Fixe solution
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S0[i][j] = 0;
		}
	}
	S0[0][3] = 1;
	S0[1][2] = 1;
	S0[2][1] = 1;
	S0[3][0] = 1;
	S0[4][3] = 1;
	S0[5][2] = 1;
	S0[6][1] = 1;
	S0[7][2] = 1;
	S0[8][3] = 1;
	S0[9][1] = 1;
	S0[10][0] = 1;
	S0[11][0] = 1;
	S0[12][1] = 1;
	S0[13][1] = 1;
	S0[14][0] = 1;
	S0[15][0] = 1;
	S0[16][2] = 1;
	S0[17][0] = 1;
	S0[18][3] = 1;
	S0[19][2] = 1;
	S0[20][1] = 1;
	S0[21][2] = 1;
	S0[22][0] = 1;
	S0[23][1] = 1;
	S0[24][1] = 1;
	S0[25][2] = 1;
	S0[26][1] = 1;
	S0[27][0] = 1;
	S0[28][0] = 1;
	S0[29][0] = 1;
	S0[30][2] = 1;
	S0[31][0] = 1;
	S0[32][0] = 1;
	S0[33][0] = 1;
	S0[34][2] = 1;
	S0[35][0] = 1;
	S0[36][1] = 1;
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S[i][j] = S0[i][j];
		}
	}
	Z = getEvaluation(S, G.size());
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<unordered_set<vector<int>, hashFunction> > hopefulSolutions;
	vector<int> solEval;
	vector<pair<int, int> > indexHopeful;
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;
	
	//for (int i = 0; i < ITER_EBTS; i++)
	while (true)
	{
		//out << "iter = " << i << endl;
		out << "NEW ITERATION" << endl;
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
		vector<int> ss(getSTaille(S0, G.size()));
		afficher(ss);
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		out << "Hopeful Solutions : " << endl;
		afficherOut(hopefulSolutions);
		out << "Nombre des Hopeful : " << hopefulSolutions.size() << endl;
		out << "Evaluations Hopeful : " << endl;
		afficherOut(solEval);
		out << "index hopeful : " << endl;
		afficherOut(indexHopeful);
		out << "Alternative Solutions : " << endl;
		afficherOut(alternativeSolutions);
		out << "Nombre des Alternatives : " << alternativeSolutions.size() << endl;
		out << "Evaluations Alternative : " << endl;
		afficherOut(alterEval);
		out << "index alternative : " << endl;
		afficherOut(indexAlternative);
		out << "treated Solutions : " << endl;
		afficherOut(treatedSolutions);
		out << "Nombre des Treated : " << treatedSolutions.size() << endl;
		out << "Evaluations Treated : " << endl;
		afficherOut(treatedEval);
		out << "index treated : " << endl;
		afficherOut(indexTreated);
		int Z0 = getEvaluation(S0, G.size());

		int posHF = exist(hopefulSolutions, solEval, indexHopeful, S0, G.size());
		int posAL = exist(alternativeSolutions, alterEval, indexAlternative, S0, G.size());
		int posTR = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
		out << "hopeful = " << posHF << endl;
		out << "alternative = " << posAL << endl;
		out << "treated = " << posTR << endl;
		if (posHF == -1 || posAL == -1 || posTR == -1)
		{
			if (!hopefulSolutions.empty())
			{
				out << "HOPEFUL EXIST" << endl;
				updateIndex(solEval, indexHopeful, false, solEval.size() - 1);
				S0 = vectorToMatrix(hopefulSolutions[hopefulSolutions.size() - 1], G.size());
				hopefulSolutions.pop_back();
				solEval.pop_back();
				//updateDM(DM, S0, G.size());
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else if(!alternativeSolutions.empty())
			{
				out << "ALTERNATIVE EXIST" << endl;
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				out << "Evaluations Alternative : " << endl;
				afficherOut(alterEval);
				out << "index alternative : " << endl;
				afficherOut(indexAlternative);
				//updateDM(DM, S0, G.size());
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				break;
			}
		}
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
			}
			K[it->first][tailleInitial] = 0;
		}
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < G.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							out << "One move applied" << endl;
							S0[i][h] = 1;
							S0[i][k] = 0;
							bool trouver = false;
							for (int j = 0; j < G.size(); j++)
							{
								if (S0[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < G.size(); j++)
									{
										S0[j][h - 1] = S0[j][h];
									}
								}
							}
							triSolution(S0, G.size());
							move = true;
							break;
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						if (minP > B[j][k])
							//if(maxP < B[j][k])
						{
							indLP = j;
							indCP = k;
							minP = B[j][k];
							//maxP = B[j][k];
						}
						if (NS2.empty())
						{
							NS2.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS2.size(); ind++)
							{
								if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
								{
									NS2.insert(NS2.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
		}
		out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());

		if (!NS0.empty())
		{
			out << "NS0 not empty" << endl;
			choix = NS0[0];
			amelioratif++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posHopeful = exist(hopefulSolutions, solEval, indexHopeful, S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
				out << "hopeful = " << posHopeful << endl;
				out << "alternative = " << posAlternative << endl;
				out << "treated = " << posTreated << endl;
				if (posHopeful != -1 && posAlternative != -1 && posTreated != -1)
				{
					int *Z0_ = new int;
					*Z0_ = getEvaluation(S0_, G.size());
					updateIndex(solEval, indexHopeful, true, posHopeful, *Z0_);
					hopefulSolutions.insert(hopefulSolutions.begin() + posHopeful, solutionToVector(S0_, G.size()));
					solEval.insert(solEval.begin() + posHopeful, *Z0_);
					free(Z0_);
					Z0_ = nullptr;
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posHopeful = exist(hopefulSolutions, solEval, indexHopeful, S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
				out << "hopeful = " << posHopeful << endl;
				out << "alternative = " << posAlternative << endl;
				out << "treated = " << posTreated << endl;
				if (posHopeful != -1 && posAlternative != -1 && posTreated != -1)
				{
					int* Z0_ = new int;
					*Z0_ = getEvaluation(S0_, G.size());
					out << "in with : " << *Z0_ << endl;
					updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
					alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
					alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
					free(Z0_);
					Z0_ = nullptr;
					out << "Evaluations Alternative : " << endl;
					afficherOut(alterEval);
					out << "index alternative : " << endl;
					afficherOut(indexAlternative);
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!hopefulSolutions.empty())
		{
			out << "HOPEFUL EXIST" << endl;
			int posTreated;
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posHopeful = exist(hopefulSolutions, solEval, indexHopeful, S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
				out << "hopeful = " << posHopeful << endl;
				out << "alternative = " << posAlternative << endl;
				out << "treated = " << posTreated << endl;
				if (posHopeful != -1 && posAlternative != -1 && posTreated != -1)
				{
					int* Z0_ = new int;
					*Z0_ = getEvaluation(S0_, G.size());
					updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
					alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
					alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
					free(Z0_);
					Z0_ = nullptr;
					out << "Evaluations Alternative : " << endl;
					afficherOut(alterEval);
					out << "index alternative : " << endl;
					afficherOut(indexAlternative);
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			updateIndex(solEval, indexHopeful, false, solEval.size() - 1);
			S0 = vectorToMatrix(hopefulSolutions[hopefulSolutions.size() - 1], G.size());
			hopefulSolutions.pop_back();
			solEval.pop_back();
			out << "Evaluations Hopeful ...: " << endl;
			afficherOut(solEval);
			out << "index hopeful ...: " << endl;
			afficherOut(indexHopeful);
			//updateDM(DM, S0, G.size());
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			
			continue;
		}
		else if (!NS1.empty())
		{
			out << "NS1 not empty" << endl;
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			out << "TREATED HERE = " << posTreated << endl;
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			out << "Evaluations Treated HERE : " << endl;
			afficherOut(treatedEval);
			out << "index treated HERE : " << endl;
			afficherOut(indexTreated);
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posHopeful = exist(hopefulSolutions, solEval, indexHopeful, S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
				out << "hopeful = " << posHopeful << endl;
				out << "alternative = " << posAlternative << endl;
				out << "treated = " << posTreated << endl;
				if (posHopeful != -1 && posAlternative != -1 && posTreated != -1)
				{
					int* Z0_ = new int;
					*Z0_ = getEvaluation(S0_, G.size());
					out << "in with : " << *Z0_ << endl;
					updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
					alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
					alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
					free(Z0_);
					Z0_ = nullptr;
					out << "Evaluations Alternative : " << endl;
					afficherOut(alterEval);
					out << "index alternative : " << endl;
					afficherOut(indexAlternative);
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			out << "ALTERNATIVE EXIST" << endl;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			out << "Evaluations Alternative : " << endl;
			afficherOut(alterEval);
			out << "index alternative : " << endl;
			afficherOut(indexAlternative);
			//updateDM(DM, S0, G.size());
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			continue;
		}
		else
		{
			out << "ALL EMPTY" << endl;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			break;
		}

		out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;

		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k,1)-exchange + min and = 0 diversification + no tabu list bi-naire + Historical + One Set uses alternative + index
void EBTS12(map<int, vector<int> >& G, int** &S, int** &S0, int& Z, int** &DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		//updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S0[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(G, S0);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;
	afficherSolutionOut(S0, G.size(), G.size());
	// Fixe solution
	/*for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S0[i][j] = 0;
		}
	}
	S0[0][0] = 1;
	S0[1][2] = 1;
	S0[2][1] = 1;
	S0[3][0] = 1;
	S0[4][0] = 1;
	S0[5][3] = 1;
	S0[6][1] = 1;
	S0[7][0] = 1;
	S0[8][2] = 1;
	S0[9][1] = 1;
	S0[10][2] = 1;
	S0[11][2] = 1;
	S0[12][1] = 1;
	S0[13][0] = 1;
	S0[14][3] = 1;
	S0[15][2] = 1;
	S0[16][2] = 1;
	S0[17][0] = 1;
	S0[18][1] = 1;
	S0[19][1] = 1;
	S0[20][0] = 1;
	S0[21][2] = 1;
	S0[22][0] = 1;
	S0[23][1] = 1;
	S0[24][2] = 1;
	S0[25][2] = 1;
	S0[26][0] = 1;
	S0[27][1] = 1;
	S0[28][0] = 1;
	S0[29][1] = 1;
	S0[30][2] = 1;
	S0[31][0] = 1;
	S0[32][1] = 1;
	S0[33][1] = 1;
	S0[34][0] = 1;
	S0[35][3] = 1;
	S0[36][2] = 1;
	S0[37][0] = 1;
	S0[38][1] = 1;
	S0[39][2] = 1;
	S0[40][0] = 1;
	S0[41][3] = 1;
	S0[42][2] = 1;
	S0[43][1] = 1;
	S0[44][0] = 1;
	S0[45][3] = 1;
	S0[46][0] = 1;
	S0[47][2] = 1;
	S0[48][1] = 1;
	S0[49][0] = 1;
	S0[50][1] = 1;
	S0[51][1] = 1;
	S0[52][1] = 1;
	S0[53][0] = 1;
	S0[54][3] = 1;
	S0[55][1] = 1;
	S0[56][2] = 1;
	S0[57][0] = 1;
	S0[58][0] = 1;
	S0[59][1] = 1;
	S0[60][2] = 1;
	S0[61][0] = 1;
	S0[62][1] = 1;
	S0[63][2] = 1;
	S0[64][3] = 1;
	S0[65][1] = 1;
	S0[66][2] = 1;
	S0[67][1] = 1;
	S0[68][2] = 1;
	S0[69][0] = 1;
	S0[70][1] = 1;
	S0[71][2] = 1;
	S0[72][0] = 1;
	S0[73][2] = 1;
	S0[74][1] = 1;
	S0[75][0] = 1;
	S0[76][1] = 1;
	S0[77][2] = 1;
	S0[78][0] = 1;
	S0[79][3] = 1;
	S0[80][2] = 1;
	S0[81][0] = 1;
	S0[82][0] = 1;
	S0[83][1] = 1;
	S0[84][3] = 1;
	S0[85][0] = 1;
	S0[86][1] = 1;
	S0[87][2] = 1;

	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S[i][j] = S0[i][j];
		}
	}
	Z = getEvaluation(S, G.size());*/
	//for (int i = 0; i < ITER_EBTS; i++)
	while (true)
	{
		/*//out << "iter = " << i << endl;
		out << "NEW ITERATION" << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cout << "taille S0 : " << endl;
		//vector<int> ss(getSTaille(S0, G.size()));
		//afficher(ss);
		//cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		out << "Alternative Solutions : " << endl;
		afficherOut(alternativeSolutions);
		out << "Nombre des Alternatives : " << alternativeSolutions.size() << endl;
		out << "Evaluations Alternative : " << endl;
		afficherOut(alterEval);
		out << "index alternative : " << endl;
		afficherOut(indexAlternative);
		out << "treated Solutions : " << endl;
		afficherOut(treatedSolutions);
		out << "Nombre des Treated : " << treatedSolutions.size() << endl;
		out << "Evaluations Treated : " << endl;
		afficherOut(treatedEval);
		out << "index treated : " << endl;
		afficherOut(indexTreated);
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
		vector<int> ss(getSTaille(S0, G.size()));
		afficher(ss);
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		*/
		int Z0 = getEvaluation(S0, G.size());
		cout << "Z = " << Z << endl;
		cout << "Z0 = " << Z0 << endl;
		if (!isSolution(G, S0))
		{
			out << "BREAK ===> NOT SOLUTION" << endl;
			cout << "BREAK ===> NOT SOLUTION" << endl;
			afficherOut(S0, G.size(), G.size());
			afficherSolutionOut(S0, G.size(), G.size());
			system("pause");
			break;
		}
		int posAL = exist(alternativeSolutions, alterEval, indexAlternative, S0, G.size());
		if (posAL == -1)
		{
			if (!alternativeSolutions.empty())
			{
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				//updateDM(DM, S0, G.size());
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			int posTR = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTR == -1)
			{
				if (!alternativeSolutions.empty())
				{
					updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
					S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
					alternativeSolutions.pop_back();
					alterEval.pop_back();
					//updateDM(DM, S0, G.size());
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else
				{
					break;
				}
			}
		}

		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
			}
			K[it->first][tailleInitial] = 0;
		}
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < G.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							//out << "One move applied" << endl;
							cout << "sommet : " << i << " de " << k << " vers " << h << endl;
							//cout << "Ancienne solution : " << endl;
							//afficherSolution(S0, G.size(), G.size());
							S0[i][h] = 1;
							S0[i][k] = 0;
							//cout << "Nouvelle solution : " << endl;
							//afficherSolution(S0, G.size(), G.size());
							bool trouver = false;
							for (int j = 0; j < G.size(); j++)
							{
								if (S0[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < G.size(); j++)
									{
										S0[j][h - 1] = S0[j][h];
									}
								}
							}
							triSolution(S0, G.size());
							move = true;
							break;
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						/*if (minP > B[j][k])
						//if(maxP < B[j][k])
						{
						indLP = j;
						indCP = k;
						minP = B[j][k];
						//maxP = B[j][k];
						}
						if (NS2.empty())
						{
						NS2.push_back(make_pair(j, k));
						}
						else
						{
						for (int ind = 0; ind < NS2.size(); ind++)
						{
						if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
						{
						NS2.insert(NS2.begin() + ind, make_pair(j, k));
						break;
						}
						}
						}*/
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());*/
		if (!NS0.empty())
		{
			choix = NS0[0];
			amelioratif++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			int posTreated;
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				/*cout << "S0_ here :" << endl;
				afficher(solutionToVector(S0_, G.size()));
				cout << endl;*/
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						/*out << "\nNS1 => " << NS1[ind].first << " : " << NS1[ind].second << endl;
						out << "S0_ : " << endl;
						afficherOut(alternativeSolutions[posAlternative]);*/
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
				/*if (NS1[ind].first == 54 && NS1[ind].second == 2)
				{
					char c;
					cout << "PAUSE" << endl;
					out << "\n";
					cin >> c;
				}*/
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			//updateDM(DM, S0, G.size());
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			
			continue;
		}
		else if (!NS1.empty())
		{
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else
		{
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			break;
		}

		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;

		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k,1)-exchange + min and = 0 diversification + no tabu list bi-naire + Historical + One Set uses alternative + index + Affichage
void EBTS12withScreen(map<int, vector<int> >& G, int **S, int **S0, int& Z, int **DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated)
{
	if (isNull(S, G.size()))
	{
		out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		cost << Z << endl;
		//updateDM(DM, S, G.size());
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S0[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(G, S0);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	// Fixe solution
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S0[i][j] = 0;
		}
	}
	S0[0][1] = 1;
	S0[1][0] = 1;
	S0[2][1] = 1;
	S0[3][3] = 1;
	S0[4][0] = 1;
	S0[5][1] = 1;
	S0[6][2] = 1;
	S0[7][2] = 1;
	S0[8][0] = 1;
	S0[9][0] = 1;

	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S[i][j] = S0[i][j];
		}
	}
	Z = getEvaluation(S, G.size());
	//map<pair<int, int>, int> tabuList;
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;

	//for (int i = 0; i < ITER_EBTS; i++)
	while (true)
	{
		//out << "iter = " << i << endl;
		out << "NEW ITERATION" << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cout << "taille S0 : " << endl;
		//vector<int> ss(getSTaille(S0, G.size()));
		//afficher(ss);
		//cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		out << "Alternative Solutions : " << endl;
		afficherOut(alternativeSolutions);
		out << "Nombre des Alternatives : " << alternativeSolutions.size() << endl;
		out << "Evaluations Alternative : " << endl;
		afficherOut(alterEval);
		out << "index alternative : " << endl;
		afficherOut(indexAlternative);
		out << "treated Solutions : " << endl;
		afficherOut(treatedSolutions);
		out << "Nombre des Treated : " << treatedSolutions.size() << endl;
		out << "Evaluations Treated : " << endl;
		afficherOut(treatedEval);
		out << "index treated : " << endl;
		afficherOut(indexTreated);
		int Z0 = getEvaluation(S0, G.size());

		cout << "Z = " << Z << endl;
		cout << "Z0 = " << Z0 << endl;
		if (!isSolution(G, S0))
		{
			out << "BREAK ===> NOT SOLUTION" << endl;
			afficherOut(S0, G.size(), G.size());
			break;
		}
		int posAL = exist(alternativeSolutions, alterEval, indexAlternative, S0, G.size());
		//cout << "posAL = " << posAL << endl;
		if (posAL == -1)
		{
			if (!alternativeSolutions.empty())
			{
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				//updateDM(DM, S0, G.size());
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			int posTR = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			//cout << "posTR = " << posTR << endl;
			if (posTR == -1)
			{
				if (!alternativeSolutions.empty())
				{
					updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
					S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
					alternativeSolutions.pop_back();
					alterEval.pop_back();
					//updateDM(DM, S0, G.size());
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else
				{
					break;
				}
			}
		}
		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
			}
			K[it->first][tailleInitial] = 0;
		}
		//Appliquer One-move s'il existe
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < G.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							out << "One move applied" << endl;
							S0[i][h] = 1;
							S0[i][k] = 0;
							bool trouver = false;
							for (int j = 0; j < G.size(); j++)
							{
								if (S0[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < G.size(); j++)
									{
										S0[j][h - 1] = S0[j][h];
									}
								}
							}
							triSolution(S0, G.size());
							move = true;
							break;
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						/*if (minP > B[j][k])
							//if(maxP < B[j][k])
						{
							indLP = j;
							indCP = k;
							minP = B[j][k];
							//maxP = B[j][k];
						}
						if (NS2.empty())
						{
							NS2.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS2.size(); ind++)
							{
								if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
								{
									NS2.insert(NS2.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}*/
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
		}
		out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());

		if (!NS0.empty())
		{
			out << "NS0 not empty" << endl;
			choix = NS0[0];
			amelioratif++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				//out << "posAlternative = " << posAlternative << endl;
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					//out << "posTreated = " << posTreated << endl;
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				//out << "posAlternative = " << posAlternative << endl;
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					//out << "posTreated = " << posTreated << endl;
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			out << "ALTERNATIVE EXIST" << endl;
			int posTreated;
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				//out << "posAlternative = " << posAlternative << endl;
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					//out << "posTreated = " << posTreated << endl;
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			/*out << "Evaluations Alternative : " << endl;
			afficherOut(alterEval);
			out << "index alternative : " << endl;
			afficherOut(indexAlternative);*/
			//updateDM(DM, S0, G.size());
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			continue;
		}
		else if (!NS1.empty())
		{
			out << "NS1 not empty" << endl;
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			//out << "TREATED HERE = " << posTreated << endl;
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			/*out << "Evaluations Treated HERE : " << endl;
			afficherOut(treatedEval);
			out << "index treated HERE : " << endl;
			afficherOut(indexTreated);*/
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				//out << "posAlternative = " << posAlternative << endl;
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					//out << "posTreated = " << posTreated << endl;
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else
		{
			out << "ALL EMPTY" << endl;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			break;
		}

		out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;

		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k,1)-exchange + min and = 0 diversification + no tabu list bi-naire + Historical + One Set uses alternative + index + only best
void EBTS13(map<int, vector<int> >& G, int **S, int **S0, int& Z, int **DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		/*for (int i = 2000; i < 3000; i++)
		{
			int **S_ = new int*[G.size()];
			for (int i = 0; i < G.size(); i++)
			{
				S_[i] = new int[G.size()];
			}
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S_[i][j] = 0;
				}
			}
			srand(i);
			ConstructedGreedyRandomizedSolution(G, S_);
			Z = getEvaluation(S_, G.size());
			out << "i = " << i << " => " << Z << endl;
			free(S_);
		}
		system("Pause");*/
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		//updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S0[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(G, S0);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;
	
	Z = getEvaluation(S, G.size());
	//for (int i = 0; i < ITER_EBTS; i++)
	while (true)
	{
		/*//out << "iter = " << i << endl;
		out << "NEW ITERATION" << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cout << "taille S0 : " << endl;
		//vector<int> ss(getSTaille(S0, G.size()));
		//afficher(ss);
		//cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		out << "Alternative Solutions : " << endl;
		afficherOut(alternativeSolutions);
		out << "Nombre des Alternatives : " << alternativeSolutions.size() << endl;
		out << "Evaluations Alternative : " << endl;
		afficherOut(alterEval);
		out << "index alternative : " << endl;
		afficherOut(indexAlternative);
		out << "treated Solutions : " << endl;
		afficherOut(treatedSolutions);
		out << "Nombre des Treated : " << treatedSolutions.size() << endl;
		out << "Evaluations Treated : " << endl;
		afficherOut(treatedEval);
		out << "index treated : " << endl;
		afficherOut(indexTreated);
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
		vector<int> ss(getSTaille(S0, G.size()));
		afficher(ss);
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		*/
		int Z0 = getEvaluation(S0, G.size());
		cout << "Z = " << Z << endl;
		cout << "Z0 = " << Z0 << endl;
		cout << "Alter = " << alterEval.size() << endl;
		cout << "Treated = " << treatedEval.size() << endl;
		cout << "Nb Move = " << nbMove << endl;
		if (!isSolution(G, S0))
		{
			out << "BREAK ===> NOT SOLUTION" << endl;
			cout << "BREAK ===> NOT SOLUTION" << endl;
			afficherOut(S0, G.size(), G.size());
			break;
		}
		int posAL = exist(alternativeSolutions, alterEval, indexAlternative, S0, G.size());
		if (posAL == -1)
		{
			if (!alternativeSolutions.empty())
			{
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				//updateDM(DM, S0, G.size());
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			int posTR = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTR == -1)
			{
				if (!alternativeSolutions.empty())
				{
					updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
					S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
					alternativeSolutions.pop_back();
					alterEval.pop_back();
					//updateDM(DM, S0, G.size());
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else
				{
					break;
				}
			}
		}

		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
			}
			K[it->first][tailleInitial] = 0;
		}
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < G.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							out << "One move applied" << endl;
							S0[i][h] = 1;
							S0[i][k] = 0;
							bool trouver = false;
							for (int j = 0; j < G.size(); j++)
							{
								if (S0[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < G.size(); j++)
									{
										S0[j][h - 1] = S0[j][h];
									}
								}
							}
							triSolution(S0, G.size());
							move = true;
							break;
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
		//calcul de B et choix de l'élément à déplacer
		vector<int> taille, position;
		vector<pair<int, int> > NS0, NS1, NS2;
		pair<int, int> choix;
		int tailleActuel = getTaille(S0, G.size());
		int minP = G.size() * tailleInitial, indLP = -1, indCP = -1, minN = G.size() * tailleInitial, indLN = -1, indCN = -1, cpN = 0;
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						/*if (minP > B[j][k])
						//if(maxP < B[j][k])
						{
						indLP = j;
						indCP = k;
						minP = B[j][k];
						//maxP = B[j][k];
						}
						if (NS2.empty())
						{
						NS2.push_back(make_pair(j, k));
						}
						else
						{
						for (int ind = 0; ind < NS2.size(); ind++)
						{
						if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
						{
						NS2.insert(NS2.begin() + ind, make_pair(j, k));
						break;
						}
						}
						}*/
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());*/
		if (!NS0.empty())
		{
			choix = NS0[0];
			amelioratif++;
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				if (B[NS0[ind].first][NS0[ind].second] == B[NS0[0].first][NS0[0].second])
				{
					cpN++;
				}
				else
				{
					break;
				}
			}
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			alternativeSolutions.clear();
			alterEval.clear();
			indexAlternative.clear();
						
			for (int ind = 1; ind < cpN; ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			int posTreated;
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				/*cout << "S0_ here :" << endl;
				afficher(solutionToVector(S0_, G.size()));
				cout << endl;*/
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						/*out << "\nNS1 => " << NS1[ind].first << " : " << NS1[ind].second << endl;
						out << "S0_ : " << endl;
						afficherOut(alternativeSolutions[posAlternative]);*/
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
				/*if (NS1[ind].first == 54 && NS1[ind].second == 2)
				{
				char c;
				cout << "PAUSE" << endl;
				out << "\n";
				cin >> c;
				}*/
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			//updateDM(DM, S0, G.size());
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			continue;
		}
		else if (!NS1.empty())
		{
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else
		{
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			break;
		}

		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;

		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k,1)-exchange + min and = 0 diversification + no tabu list bi-naire + Historical + One Set uses alternative + index + only best + Affichage
void EBTS13withScreen(map<int, vector<int> >& G, int **S, int **S0, int& Z, int **DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		//updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S0[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(G, S0);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;

	// Fixe solution
	/*for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S0[i][j] = 0;
		}
	}
	S0[0][1] = 1;
	S0[1][0] = 1;
	S0[2][1] = 1;
	S0[3][3] = 1;
	S0[4][0] = 1;
	S0[5][1] = 1;
	S0[6][2] = 1;
	S0[7][2] = 1;
	S0[8][0] = 1;
	S0[9][0] = 1;
	
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S[i][j] = S0[i][j];
		}
	}
	Z = getEvaluation(S, G.size());*/
	//for (int i = 0; i < ITER_EBTS; i++)
	while (true)
	{
		//out << "iter = " << i << endl;
		out << "NEW ITERATION" << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cout << "taille S0 : " << endl;
		//vector<int> ss(getSTaille(S0, G.size()));
		//afficher(ss);
		//cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		out << "Alternative Solutions : " << endl;
		afficherOut(alternativeSolutions);
		out << "Nombre des Alternatives : " << alternativeSolutions.size() << endl;
		out << "Evaluations Alternative : " << endl;
		afficherOut(alterEval);
		out << "index alternative : " << endl;
		afficherOut(indexAlternative);
		out << "treated Solutions : " << endl;
		afficherOut(treatedSolutions);
		out << "Nombre des Treated : " << treatedSolutions.size() << endl;
		out << "Evaluations Treated : " << endl;
		afficherOut(treatedEval);
		out << "index treated : " << endl;
		afficherOut(indexTreated);
		
		int Z0 = getEvaluation(S0, G.size());
		cout << "Z = " << Z << endl;
		cout << "Z0 = " << Z0 << endl;
		cout << "Alter = " << alterEval.size() << endl;
		cout << "Treated = " << treatedEval.size() << endl;
		if (!isSolution(G, S0))
		{
			out << "BREAK ===> NOT SOLUTION" << endl;
			cout << "BREAK ===> NOT SOLUTION" << endl;
			afficherOut(S0, G.size(), G.size());
			break;
		}
		int posAL = exist(alternativeSolutions, alterEval, indexAlternative, S0, G.size());
		if (posAL == -1)
		{
			if (!alternativeSolutions.empty())
			{
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				//updateDM(DM, S0, G.size());
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			int posTR = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTR == -1)
			{
				if (!alternativeSolutions.empty())
				{
					updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
					S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
					alternativeSolutions.pop_back();
					alterEval.pop_back();
					//updateDM(DM, S0, G.size());
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else
				{
					break;
				}
			}
		}

		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
			}
			K[it->first][tailleInitial] = 0;
		}
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < G.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							out << "One move applied" << endl;
							S0[i][h] = 1;
							S0[i][k] = 0;
							bool trouver = false;
							for (int j = 0; j < G.size(); j++)
							{
								if (S0[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < G.size(); j++)
									{
										S0[j][h - 1] = S0[j][h];
									}
								}
							}
							triSolution(S0, G.size());
							move = true;
							break;
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
		//calcul de B et choix de l'élément à déplacer
		vector<int> taille, position;
		vector<pair<int, int> > NS0, NS1, NS2;
		pair<int, int> choix;
		int tailleActuel = getTaille(S0, G.size());
		int minP = G.size() * tailleInitial, indLP = -1, indCP = -1, minN = G.size() * tailleInitial, indLN = -1, indCN = -1, cpN = 0;
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						/*if (minP > B[j][k])
						//if(maxP < B[j][k])
						{
						indLP = j;
						indCP = k;
						minP = B[j][k];
						//maxP = B[j][k];
						}
						if (NS2.empty())
						{
						NS2.push_back(make_pair(j, k));
						}
						else
						{
						for (int ind = 0; ind < NS2.size(); ind++)
						{
						if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
						{
						NS2.insert(NS2.begin() + ind, make_pair(j, k));
						break;
						}
						}
						}*/
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
		}
		out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		if (!NS0.empty())
		{
			out << "NS0 not Empty" << endl;
			choix = NS0[0];
			amelioratif++;
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				if (B[NS0[ind].first][NS0[ind].second] == B[NS0[0].first][NS0[0].second])
				{
					cpN++;
				}
				else
				{
					break;
				}
			}
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			alternativeSolutions.clear();
			alterEval.clear();
			indexAlternative.clear();

			for (int ind = 1; ind < cpN; ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			out << "Alternative Solutions not Empty" << endl;
			int posTreated;
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				/*cout << "S0_ here :" << endl;
				afficher(solutionToVector(S0_, G.size()));
				cout << endl;*/
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						/*out << "\nNS1 => " << NS1[ind].first << " : " << NS1[ind].second << endl;
						out << "S0_ : " << endl;
						afficherOut(alternativeSolutions[posAlternative]);*/
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
				/*if (NS1[ind].first == 54 && NS1[ind].second == 2)
				{
				char c;
				cout << "PAUSE" << endl;
				out << "\n";
				cin >> c;
				}*/
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			//updateDM(DM, S0, G.size());
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			continue;
		}
		else if (!NS1.empty())
		{
			out << "NS1 not Empty" << endl;
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else
		{
			out << "All Empty" << endl;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			break;
		}

		out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;

		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k,1)-exchange + min and = 0 diversification + no tabu list bi-naire + Historical + One Set uses alternative + index + only best + Diversification if fail
void EBTS14(map<int, vector<int> >& G, int **S, int **S0, int& Z, int **DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		//updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S0[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(G, S0);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;
	bool diver = false;

	Z = getEvaluation(S, G.size());
	//for (int i = 0; i < ITER_EBTS; i++)
	while (true)
	{
		/*//out << "iter = " << i << endl;
		out << "NEW ITERATION" << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cout << "taille S0 : " << endl;
		//vector<int> ss(getSTaille(S0, G.size()));
		//afficher(ss);
		//cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		out << "Alternative Solutions : " << endl;
		afficherOut(alternativeSolutions);
		out << "Nombre des Alternatives : " << alternativeSolutions.size() << endl;
		out << "Evaluations Alternative : " << endl;
		afficherOut(alterEval);
		out << "index alternative : " << endl;
		afficherOut(indexAlternative);
		out << "treated Solutions : " << endl;
		afficherOut(treatedSolutions);
		out << "Nombre des Treated : " << treatedSolutions.size() << endl;
		out << "Evaluations Treated : " << endl;
		afficherOut(treatedEval);
		out << "index treated : " << endl;
		afficherOut(indexTreated);
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
		vector<int> ss(getSTaille(S0, G.size()));
		afficher(ss);
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		*/
		int Z0 = getEvaluation(S0, G.size());
		cout << "Z = " << Z << endl;
		cout << "Z0 = " << Z0 << endl;
		cout << "Alter = " << alterEval.size() << endl;
		cout << "Treated = " << treatedEval.size() << endl;
		cout << "Nb Move = " << nbMove << endl;
		cout << "Nb NS2 = " << nbNS2 << endl;
		if (!isSolution(G, S0))
		{
			out << "BREAK ===> NOT SOLUTION" << endl;
			cout << "BREAK ===> NOT SOLUTION" << endl;
			afficherOut(S0, G.size(), G.size());
			break;
		}
		int posAL = exist(alternativeSolutions, alterEval, indexAlternative, S0, G.size());
		if (posAL == -1)
		{
			if (!alternativeSolutions.empty())
			{
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				//updateDM(DM, S0, G.size());
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				cout << "OUT1" << endl;
				diver = true;
				//break;
			}
		}
		else
		{
			int posTR = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTR == -1)
			{
				if (!alternativeSolutions.empty())
				{
					updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
					S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
					alternativeSolutions.pop_back();
					alterEval.pop_back();
					//updateDM(DM, S0, G.size());
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else
				{
					cout << "OUT2" << endl;
					diver = true;
					//break;
				}
			}
		}

		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
			}
			K[it->first][tailleInitial] = 0;
		}
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < G.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							//out << "One move applied" << endl;
							int **S0_ = new int*[G.size()];
							for (int i = 0; i < G.size(); i++)
							{
								S0_[i] = new int[G.size()];
							}
							for (int i = 0; i < G.size(); i++)
							{
								for (int j = 0; j < G.size(); j++)
								{
									S0_[i][j] = S0[i][j];
								}
							}
							S0_[i][h] = 1;
							S0_[i][k] = 0;
							bool trouver = false;
							for (int j = 0; j < G.size(); j++)
							{
								if (S0_[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								//cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < G.size(); j++)
									{
										S0_[j][h - 1] = S0_[j][h];
									}
								}
							}
							triSolution(S0_, G.size());
							int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
							if (posTreated != -1)
							{
								posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
								if (posTreated != -1)
								{
									updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
									treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
									treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
								}
								for (int i = 0; i < G.size(); i++)
								{
									for (int j = 0; j < G.size(); j++)
									{
										S0[i][j] = S0_[i][j];
									}
								}
								move = true;
								// Libérer la mémoire
								for (int j = 0; j < G.size(); j++)
								{
									free(S0_[j]);
								}
								free(S0_);
								break;
							}
							// Libérer la mémoire
							for (int j = 0; j < G.size(); j++)
							{
								free(S0_[j]);
							}
							free(S0_);
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
		//calcul de B et choix de l'élément à déplacer
		vector<int> taille, position;
		vector<pair<int, int> > NS0, NS1, NS2;
		pair<int, int> choix;
		int tailleActuel = getTaille(S0, G.size());
		int minP = G.size() * tailleInitial, indLP = -1, indCP = -1, minN = G.size() * tailleInitial, indLN = -1, indCN = -1, cpN = 0;
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						if (minP > B[j][k])
						//if(maxP < B[j][k])
						{
							indLP = j;
							indCP = k;
							minP = B[j][k];
							//maxP = B[j][k];
						}
						if (NS2.empty())
						{
							NS2.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS2.size(); ind++)
							{
								if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
								{
									NS2.insert(NS2.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());*/
		if (diver)
		{
			cout << "Diver" << endl;
			for (int ind = 0; ind < NS2.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				choix = NS2[ind];
				S0_[choix.first][choix.second] = 1;
				S0_[choix.first][position[choix.first]] = 0;
				for (int j = 0; j < G[choix.first].size(); j++)
				{
					if (K[G[choix.first][j]][choix.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
							{
								S0_[G[choix.first][j]][k] = 1;
								S0_[G[choix.first][j]][choix.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix.first][j]][tailleInitial] = 1;
							S0_[G[choix.first][j]][choix.second] = 0;
						}
						out_.push_back(G[choix.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						// Libérer la mémoire
						for (int j = 0; j < G.size(); j++)
						{
							free(S0_[j]);
						}
						free(S0_);
						break;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			nbNS2++;
			diver = false;
		}
		else if (!NS0.empty())
		{
			choix = NS0[0];
			amelioratif++;
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				if (B[NS0[ind].first][NS0[ind].second] == B[NS0[0].first][NS0[0].second])
				{
					cpN++;
				}
				else
				{
					break;
				}
			}
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			alternativeSolutions.clear();
			alterEval.clear();
			indexAlternative.clear();

			for (int ind = 1; ind < cpN; ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			int posTreated;
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				/*cout << "S0_ here :" << endl;
				afficher(solutionToVector(S0_, G.size()));
				cout << endl;*/
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						/*out << "\nNS1 => " << NS1[ind].first << " : " << NS1[ind].second << endl;
						out << "S0_ : " << endl;
						afficherOut(alternativeSolutions[posAlternative]);*/
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
				/*if (NS1[ind].first == 54 && NS1[ind].second == 2)
				{
				char c;
				cout << "PAUSE" << endl;
				out << "\n";
				cin >> c;
				}*/
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			//updateDM(DM, S0, G.size());
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			continue;
		}
		else if (!NS1.empty())
		{
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else
		{
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			for (int ind = 0; ind < NS2.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				choix = NS2[ind];
				S0_[choix.first][choix.second] = 1;
				S0_[choix.first][position[choix.first]] = 0;
				for (int j = 0; j < G[choix.first].size(); j++)
				{
					if (K[G[choix.first][j]][choix.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
							{
								S0_[G[choix.first][j]][k] = 1;
								S0_[G[choix.first][j]][choix.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix.first][j]][tailleInitial] = 1;
							S0_[G[choix.first][j]][choix.second] = 0;
						}
						out_.push_back(G[choix.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						// Libérer la mémoire
						for (int j = 0; j < G.size(); j++)
						{
							free(S0_[j]);
						}
						free(S0_);
						break;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			nbNS2++;
			diver = false;
		}

		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;

		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k,1)-exchange + min and = 0 diversification + no tabu list bi-naire + Historical + One Set uses alternative + index + only best + Diversification if fail + Affichage
void EBTS14withScreen(map<int, vector<int> >& G, int **S, int **S0, int& Z, int **DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		//updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < G.size(); i++)
			{
				for (int j = 0; j < G.size(); j++)
				{
					S0[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(G, S0);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;
	bool diver = false;

	Z = getEvaluation(S, G.size());
	for (int i = 0; i < ITER_EBTS; i++)
	//while (true)
	{
		//out << "iter = " << i << endl;
		out << "NEW ITERATION" << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cout << "taille S0 : " << endl;
		//vector<int> ss(getSTaille(S0, G.size()));
		//afficher(ss);
		//cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		out << "Alternative Solutions : " << endl;
		afficherOut(alternativeSolutions);
		out << "Nombre des Alternatives : " << alternativeSolutions.size() << endl;
		out << "Evaluations Alternative : " << endl;
		afficherOut(alterEval);
		out << "index alternative : " << endl;
		afficherOut(indexAlternative);
		out << "treated Solutions : " << endl;
		afficherOut(treatedSolutions);
		out << "Nombre des Treated : " << treatedSolutions.size() << endl;
		out << "Evaluations Treated : " << endl;
		afficherOut(treatedEval);
		out << "index treated : " << endl;
		afficherOut(indexTreated);
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
		vector<int> ss(getSTaille(S0, G.size()));
		afficher(ss);
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		
		int Z0 = getEvaluation(S0, G.size());
		cout << "Z = " << Z << endl;
		cout << "Z0 = " << Z0 << endl;
		cout << "Alter = " << alterEval.size() << endl;
		cout << "Treated = " << treatedEval.size() << endl;
		cout << "Nb Move = " << nbMove << endl;
		cout << "Nb NS2 = " << nbNS2 << endl;
		if (!isSolution(G, S0))
		{
			out << "BREAK ===> NOT SOLUTION" << endl;
			cout << "BREAK ===> NOT SOLUTION" << endl;
			afficherOut(S0, G.size(), G.size());
			break;
		}
		int posAL = exist(alternativeSolutions, alterEval, indexAlternative, S0, G.size());
		if (posAL == -1)
		{
			if (!alternativeSolutions.empty())
			{
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				//updateDM(DM, S0, G.size());
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				cout << "OUT1" << endl;
				out << "OUT1" << endl;
				diver = true;
				//break;
			}
		}
		else
		{
			int posTR = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTR == -1)
			{
				if (!alternativeSolutions.empty())
				{
					updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
					S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
					alternativeSolutions.pop_back();
					alterEval.pop_back();
					//updateDM(DM, S0, G.size());
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else
				{
					cout << "OUT2" << endl;
					out << "OUT2" << endl;
					diver = true;
					//break;
				}
			}
		}

		int **K = new int*[G.size()], **B = new int*[G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
			}
			K[it->first][tailleInitial] = 0;
		}
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < G.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							out << "One move applied" << endl;
							out << "sommet " << i << " : du stable : " << k << " vers " << h << endl;
							int **S0_ = new int*[G.size()];
							for (int i = 0; i < G.size(); i++)
							{
								S0_[i] = new int[G.size()];
							}
							for (int i = 0; i < G.size(); i++)
							{
								for (int j = 0; j < G.size(); j++)
								{
									S0_[i][j] = S0[i][j];
								}
							}
							S0_[i][h] = 1;
							S0_[i][k] = 0;
							bool trouver = false;
							for (int j = 0; j < G.size(); j++)
							{
								if (S0_[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								//cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < G.size(); j++)
									{
										S0_[j][h - 1] = S0_[j][h];
									}
								}
							}
							triSolution(S0_, G.size());
							int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
							if (posTreated != -1)
							{
								posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
								if (posTreated != -1)
								{
									updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
									treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
									treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
								}
								out << "PASS" << endl;
								for (int i = 0; i < G.size(); i++)
								{
									for (int j = 0; j < G.size(); j++)
									{
										S0[i][j] = S0_[i][j];
									}
								}
								move = true;
								// Libérer la mémoire
								for (int j = 0; j < G.size(); j++)
								{
									free(S0_[j]);
								}
								free(S0_);
								break;
							}
							else
							{
								out << "NOT PASS" << endl;
							}
							// Libérer la mémoire
							for (int j = 0; j < G.size(); j++)
							{
								free(S0_[j]);
							}
							free(S0_);
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
		//calcul de B et choix de l'élément à déplacer
		vector<int> taille, position;
		vector<pair<int, int> > NS0, NS1, NS2;
		pair<int, int> choix;
		int tailleActuel = getTaille(S0, G.size());
		int minP = G.size() * tailleInitial, indLP = -1, indCP = -1, minN = G.size() * tailleInitial, indLN = -1, indCN = -1, cpN = 0;
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						if (minP > B[j][k])
							//if(maxP < B[j][k])
						{
							indLP = j;
							indCP = k;
							minP = B[j][k];
							//maxP = B[j][k];
						}
						if (NS2.empty())
						{
							NS2.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS2.size(); ind++)
							{
								if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
								{
									NS2.insert(NS2.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());*/
		if (diver)
		{
			out << "Diver" << endl;
			for (int ind = 0; ind < NS2.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				choix = NS2[ind];
				S0_[choix.first][choix.second] = 1;
				S0_[choix.first][position[choix.first]] = 0;
				for (int j = 0; j < G[choix.first].size(); j++)
				{
					if (K[G[choix.first][j]][choix.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
							{
								S0_[G[choix.first][j]][k] = 1;
								S0_[G[choix.first][j]][choix.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix.first][j]][tailleInitial] = 1;
							S0_[G[choix.first][j]][choix.second] = 0;
						}
						out_.push_back(G[choix.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						// Libérer la mémoire
						for (int j = 0; j < G.size(); j++)
						{
							free(S0_[j]);
						}
						free(S0_);
						break;
					}
				}
			}
			nbNS2++;
			diver = false;
		}
		else if (!NS0.empty())
		{
			out << "NS0" << endl;
			choix = NS0[0];
			amelioratif++;
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				if (B[NS0[ind].first][NS0[ind].second] == B[NS0[0].first][NS0[0].second])
				{
					cpN++;
				}
				else
				{
					break;
				}
			}
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			alternativeSolutions.clear();
			alterEval.clear();
			indexAlternative.clear();

			for (int ind = 1; ind < cpN; ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			out << "ALTER" << endl;
			int posTreated;
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());
				/*cout << "S0_ here :" << endl;
				afficher(solutionToVector(S0_, G.size()));
				cout << endl;*/
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						/*out << "\nNS1 => " << NS1[ind].first << " : " << NS1[ind].second << endl;
						out << "S0_ : " << endl;
						afficherOut(alternativeSolutions[posAlternative]);*/
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
				/*if (NS1[ind].first == 54 && NS1[ind].second == 2)
				{
				char c;
				cout << "PAUSE" << endl;
				out << "\n";
				cin >> c;
				}*/
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			S0 = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], G.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			//updateDM(DM, S0, G.size());
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}
			continue;
		}
		else if (!NS1.empty())
		{
			out << "NS1" << endl;
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(S0, G.size()));
			}
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				for (int j = 0; j < G[choix2.first].size(); j++)
				{
					if (K[G[choix2.first][j]][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix2.first][j]][k] == 0 || (k == position[choix2.first] && K[G[choix2.first][j]][k] == 1))
							{
								S0_[G[choix2.first][j]][k] = 1;
								S0_[G[choix2.first][j]][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix2.first][j]][tailleInitial] = 1;
							S0_[G[choix2.first][j]][choix2.second] = 0;
						}
						out_.push_back(G[choix2.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, G.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, G.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < G.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else
		{
			out << "NS2" << endl;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			}
			for (int ind = 0; ind < NS2.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[G.size()];
				for (int i = 0; i < G.size(); i++)
				{
					S0_[i] = new int[G.size()];
				}
				for (int i = 0; i < G.size(); i++)
				{
					for (int j = 0; j < G.size(); j++)
					{
						S0_[i][j] = S0[i][j];
					}
				}
				choix = NS2[ind];
				S0_[choix.first][choix.second] = 1;
				S0_[choix.first][position[choix.first]] = 0;
				for (int j = 0; j < G[choix.first].size(); j++)
				{
					if (K[G[choix.first][j]][choix.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
							{
								S0_[G[choix.first][j]][k] = 1;
								S0_[G[choix.first][j]][choix.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[G[choix.first][j]][tailleInitial] = 1;
							S0_[G[choix.first][j]][choix.second] = 0;
						}
						out_.push_back(G[choix.first][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(S0_, G.size());

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, G.size());
				if (posAlternative != -1)
				{
					int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, G.size());
					if (posTreated != -1)
					{
						// Libérer la mémoire
						for (int j = 0; j < G.size(); j++)
						{
							free(S0_[j]);
						}
						free(S0_);
						break;
					}
				}
			}
			nbNS2++;
			diver = false;
		}

		out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;

		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0 || (k == position[choix.first] && K[G[choix.first][j]][k] == 1))
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k,1)-exchange + min and = 0 diversification + no tabu list bi-naire + Historical + One Set uses alternative + index + B-modular(bleu)
void EBTS15(map<int, vector<int> >& G, map<int, vector<int> >& Gq, int **S, int **S0, int **Sq, int& Z, int **DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated, vector<vector<int> >& serie, vector<vector<int> >& parallele)
{

	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(Gq, Sq);
		SqToS(G, Gq, Sq, S0, serie, parallele);
		triSolution(S0, G.size());
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		//updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < Gq.size(); i++)
			{
				for (int j = 0; j < Gq.size(); j++)
				{
					Sq[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(Gq, Sq);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
		SqToS(G, Gq, Sq, S0, serie, parallele);
		triSolution(S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;
	//afficherSolutionOut(S0, G.size(), G.size());

	//for (int i = 0; i < ITER_EBTS; i++)
	while (true)
	{
		/*//out << "iter = " << i << endl;
		out << "NEW ITERATION" << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cout << "taille S0 : " << endl;
		//vector<int> ss(getSTaille(S0, G.size()));
		//afficher(ss);
		//cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		out << "Alternative Solutions : " << endl;
		afficherOut(alternativeSolutions);
		out << "Nombre des Alternatives : " << alternativeSolutions.size() << endl;
		out << "Evaluations Alternative : " << endl;
		afficherOut(alterEval);
		out << "index alternative : " << endl;
		afficherOut(indexAlternative);
		out << "treated Solutions : " << endl;
		afficherOut(treatedSolutions);
		out << "Nombre des Treated : " << treatedSolutions.size() << endl;
		out << "Evaluations Treated : " << endl;
		afficherOut(treatedEval);
		out << "index treated : " << endl;
		afficherOut(indexTreated);
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		cout << "Sq  : " << endl;
		afficherSolution(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		out << "Sq : " << endl;
		afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		*/
		int Z0 = getEvaluation(S0, G.size());
		int Zq = getEvaluation(Sq, Gq.size());

		cout << "Z = " << Z << endl;
		cout << "Z0 = " << Z0 << endl;
		cout << "Zq = " << Zq << endl;
		//out << "Z = " << Z << endl;
		//out << "Z0 = " << Z0 << endl;
		//out << "Zq = " << Zq << endl;
		if (!isSolution(G, S0))
		{
			out << "BREAK ===> NOT SOLUTION" << endl;
			cout << "BREAK ===> NOT SOLUTION" << endl;
			afficherOut(S0, G.size(), G.size());
			afficherSolutionOut(S0, G.size(), G.size());
			system("pause");
			break;
		}
		if (!isSolutionQ(Gq, Sq))
		{
			out << "BREAK ===> Sq NOT SOLUTION" << endl;
			cout << "BREAK ===> Sq NOT SOLUTION" << endl;
			afficherOut(Sq, Gq.size(), Gq.size());
			afficherSolutionOut(Sq, Gq.size(), Gq.size());
			system("pause");
			break;
		}
		int posAL = exist(alternativeSolutions, alterEval, indexAlternative, Sq, Gq.size());
		if (posAL == -1)
		{
			if (!alternativeSolutions.empty())
			{
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				//updateDM(DM, S0, G.size());
				// Mise à jour de la solution
				SqToS(G, Gq, Sq, S0, serie, parallele);
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			int posTR = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTR == -1)
			{
				if (!alternativeSolutions.empty())
				{
					updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
					Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
					alternativeSolutions.pop_back();
					alterEval.pop_back();
					//updateDM(DM, S0, G.size());
					// Mise à jour de la solution
					SqToS(G, Gq, Sq, S0, serie, parallele);
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else
				{
					break;
				}
			}
		}

		int **K = new int*[Gq.size()], **B = new int*[Gq.size()];
		int tailleInitial = getTaille(Sq, Gq.size());
		for (int j = 0; j < Gq.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		int it2 = -1;
		for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); it++)
		{
			it2++;
			for (int j = 0; j < tailleInitial; j++)
			{
				if (Sq[it2][j] == 1)
				{
					K[it2][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < Gq[it->first].size(); h++)
							{
								int it3 = -1;
								for (map<int, vector<int> >::iterator p = Gq.begin(); p != Gq.end(); p++)
								{
									it3++;
									if (Gq[it->first][h] == p->first)
									{
										break;
									}
								}
								if (Sq[it3][k] == 1)
								{
									nb++;
								}
							}
							K[it2][k] = nb;
						}
					}
					break;
				}
			}
			K[it2][tailleInitial] = 0;
		}
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < Gq.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							//out << "One move applied" << endl;
							//out << "sommet " << i << " : du stable : " << k << " vers " << h << endl;
							int** S0_ = new int*[Gq.size()];
							for (int i = 0; i < Gq.size(); i++)
							{
								S0_[i] = new int[Gq.size()];
							}
							for (int i = 0; i < Gq.size(); i++)
							{
								for (int j = 0; j < Gq.size(); j++)
								{
									S0_[i][j] = Sq[i][j];
								}
							}
							S0_[i][h] = 1;
							S0_[i][k] = 0;
							bool trouver = false;
							for (int j = 0; j < Gq.size(); j++)
							{
								if (S0_[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								//cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < Gq.size(); j++)
									{
										S0_[j][h - 1] = S0_[j][h];
									}
								}
							}
							triSolution(Gq, S0_, G.size(), serie, parallele);
							int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
							if (posTreated != -1)
							{
								posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
								if (posTreated != -1)
								{
									updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
									treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
									treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
								}
								//out << "PASS" << endl;
								for (int i = 0; i < Gq.size(); i++)
								{
									for (int j = 0; j < Gq.size(); j++)
									{
										Sq[i][j] = S0_[i][j];
									}
								}
								move = true;
								// Libérer la mémoire
								for (int j = 0; j < Gq.size(); j++)
								{
									free(S0_[j]);
								}
								free(S0_);
								break;
							}
							else
							{
								//out << "NOT PASS" << endl;
							}
							// Libérer la mémoire
							for (int j = 0; j < Gq.size(); j++)
							{
								free(S0_[j]);
							}
							free(S0_);
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
		//calcul de B et choix de l'élément à déplacer
		vector<int> taille, position;
		vector<pair<int, int> > NS0, NS1, NS2;
		pair<int, int> choix;
		int tailleActuel = getTaille(Sq, Gq.size());
		int minP = Gq.size() * tailleInitial, indLP = -1, indCP = -1, minN = Gq.size() * tailleInitial, indLN = -1, indCN = -1;
		int maxP = Gq.size() * tailleInitial * (-1);
		for (int j = 0; j < tailleActuel; j++)
		{
			int nb = 0;
			map<int, vector<int> >::iterator q = Gq.begin();
			for (int i = 0; i < Gq.size(); i++)
			{
				if (q->first < (G.size() + serie.size()))
				{
					nb += Sq[i][j];
				}
				else
				{
					nb += parallele[q->first - G.size() - serie.size()].size()*Sq[i][j];
				}
				q++;
			}
			taille.push_back(nb);
		}
		taille.push_back(0);
		for (int j = 0; j < Gq.size(); j++)
		{
			bool nt = false;
			for (int k = 0; k < tailleActuel; k++)
			{
				if (Sq[j][k] == 1)
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
		map<int, vector<int> >::iterator q = Gq.begin();
		for (int j = 0; j < Gq.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					if (q->first < (G.size() + serie.size()))
					{
						t[position[j]]--;
						t[k]++;
					}
					else
					{
						t[position[j]] -= parallele[q->first - G.size() - serie.size()].size();
						t[k] += parallele[q->first - G.size() - serie.size()].size();
					}
					for (int h = 0; h < q->second.size(); h++)
					{
						int pos = 0;
						for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); it++)
						{
							if (q->second[h] == it->first)
							{
								break;
							}
							else
							{
								pos++;
							}
						}
						if (K[pos][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[pos][r] == 0) || (r == position[j] && K[pos][r] == 1)))
								{
									if (q->second[h] < (G.size() + serie.size()))
									{
										t[r]++;
										t[k]--;
									}
									else
									{
										t[r] += parallele[q->second[h] - G.size() - serie.size()].size();
										t[k] -= parallele[q->second[h] - G.size() - serie.size()].size();
									}
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								if (q->second[h] < (G.size() + serie.size()))
								{
									t[tailleInitial]++;
									t[k]--;
								}
								else
								{
									t[tailleInitial] += parallele[q->second[h] - G.size() - serie.size()].size();
									t[k] -= parallele[q->second[h] - G.size() - serie.size()].size();
								}
							}
						}
					}
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						/*if (minP > B[j][k])
						//if(maxP < B[j][k])
						{
						indLP = j;
						indCP = k;
						minP = B[j][k];
						//maxP = B[j][k];
						}
						if (NS2.empty())
						{
						NS2.push_back(make_pair(j, k));
						}
						else
						{
						for (int ind = 0; ind < NS2.size(); ind++)
						{
						if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
						{
						NS2.insert(NS2.begin() + ind, make_pair(j, k));
						break;
						}
						}
						}*/
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
			q++;
		}
		//out << "K = " << endl;
		//afficherOut(K, (int)tailleInitial, (int)Gq.size());
		//out << "B = " << endl;
		//afficherOut(B, (int)tailleInitial, (int)Gq.size());
		if (!NS0.empty())
		{
			//out << "NS0" << endl;
			choix = NS0[0];
			amelioratif++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Zq);
			}
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				//cout << "Gq avant : " << endl;
				//afficher(Gq);
				//out << "Sq  avant tri : " << endl;
				//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));

				//out << "S0_  avant tri : " << endl;
				//afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				//out << "S0_  apres tri : " << endl;
				//afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));

				//out << "Sq apres tri : " << endl;
				//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				//out << "S0_  avant tri : " << endl;
				//afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				//out << "S0_  apres tri : " << endl;
				//afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			//out << "ALTER" << endl;
			int posTreated;
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				//out << "S0_  avant tri : " << endl;
				//afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				//out << "S0_  apres tri : " << endl;
				//afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				/*cout << "S0_ here :" << endl;
				afficher(solutionToVector(S0_, G.size()));
				cout << endl;*/
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						/*out << "\nNS1 => " << NS1[ind].first << " : " << NS1[ind].second << endl;
						out << "S0_ : " << endl;
						afficherOut(alternativeSolutions[posAlternative]);*/
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
				/*if (NS1[ind].first == 54 && NS1[ind].second == 2)
				{
				char c;
				cout << "PAUSE" << endl;
				out << "\n";
				cin >> c;
				}*/
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			//updateDM(DM, S0, G.size());
			// Mise à jour de la solution
			SqToS(G, Gq, Sq, S0, serie, parallele);
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}

			continue;
		}
		else if (!NS1.empty())
		{
			//out << "NS1" << endl;
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
			}
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				//out << "S0_  avant tri : " << endl;
				//afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				//out << "S0_  apres tri : " << endl;
				//afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else
		{
			//out << "No case" << endl;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Zq);
			}
			break;
		}

		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		Sq[choix.first][choix.second] = 1;
		Sq[choix.first][position[choix.first]] = 0;

		int posChoix = 0;
		for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix.first; it_++)
		{
			if (posChoix == choix.first)
			{
				posChoix = it_->first;
			}
			else
			{
				posChoix++;
			}
		}
		for (int j = 0; j < Gq[posChoix].size(); j++)
		{
			int pos = 0;
			for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
			{
				if (it_->first == Gq[posChoix][j])
				{
					break;
				}
				else
				{
					pos++;
				}
			}
			if (K[pos][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[pos][k] == 0 || (k == position[choix.first] && K[pos][k] == 1))
					{
						Sq[pos][k] = 1;
						Sq[pos][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					Sq[pos][tailleInitial] = 1;
					Sq[pos][choix.second] = 0;
				}
				out_.push_back(pos);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		//out << "Sq  avant tri: " << endl;
		//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		//out << "Sq avant tri : " << endl;
		//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		triSolution(Gq, Sq, G.size(), serie, parallele);
		//out << "Sq apres tri : " << endl;
		//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		SqToS(G, Gq, Sq, S0, serie, parallele);

		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "Sq apres tri : " << endl;
		//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < Gq.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k, 1) - exchange + min and = 0 diversification + no tabu list bi - naire + Historical + One Set uses alternative + index + B - modular(bleu) + Affichage
void EBTS15withScreen(map<int, vector<int> >& G, map<int, vector<int> >& Gq, int **S, int **S0, int **Sq, int& Z, int **DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated, vector<vector<int> >& serie, vector<vector<int> >& parallele)
{
	
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(Gq, Sq);
		SqToS(G, Gq, Sq, S0, serie, parallele);
		triSolution(S0, G.size());
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		//updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < Gq.size(); i++)
			{
				for (int j = 0; j < Gq.size(); j++)
				{
					Sq[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(Gq, Sq);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
		SqToS(G, Gq, Sq, S0, serie, parallele);
		triSolution(S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;
	//afficherSolutionOut(S0, G.size(), G.size());

	//for (int i = 0; i < ITER_EBTS; i++)
	while (true)
	{
		//out << "iter = " << i << endl;
		out << "NEW ITERATION" << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cout << "taille S0 : " << endl;
		//vector<int> ss(getSTaille(S0, G.size()));
		//afficher(ss);
		//cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		out << "Alternative Solutions : " << endl;
		afficherOut(alternativeSolutions);
		out << "Nombre des Alternatives : " << alternativeSolutions.size() << endl;
		out << "Evaluations Alternative : " << endl;
		afficherOut(alterEval);
		out << "index alternative : " << endl;
		afficherOut(indexAlternative);
		out << "treated Solutions : " << endl;
		afficherOut(treatedSolutions);
		out << "Nombre des Treated : " << treatedSolutions.size() << endl;
		out << "Evaluations Treated : " << endl;
		afficherOut(treatedEval);
		out << "index treated : " << endl;
		afficherOut(indexTreated);
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		cout << "Sq  : " << endl;
		afficherSolution(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		out << "Sq : " << endl;
		afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));

		int Z0 = getEvaluation(S0, G.size());
		int Zq = getEvaluation(Sq, Gq.size());
		
		cout << "Z = " << Z << endl;
		cout << "Z0 = " << Z0 << endl;
		cout << "Zq = " << Zq << endl;
		out << "Z = " << Z << endl;
		out << "Z0 = " << Z0 << endl;
		out << "Zq = " << Zq << endl;
		if (!isSolution(G, S0))
		{
			out << "BREAK ===> NOT SOLUTION" << endl;
			cout << "BREAK ===> NOT SOLUTION" << endl;
			afficherOut(S0, G.size(), G.size());
			afficherSolutionOut(S0, G.size(), G.size());
			system("pause");
			break;
		}
		if (!isSolutionQ(Gq, Sq))
		{
			out << "BREAK ===> Sq NOT SOLUTION" << endl;
			cout << "BREAK ===> Sq NOT SOLUTION" << endl;
			afficherOut(Sq, Gq.size(), Gq.size());
			afficherSolutionOut(Sq, Gq.size(), Gq.size());
			system("pause");
			break;
		}
		int posAL = exist(alternativeSolutions, alterEval, indexAlternative, Sq, Gq.size());
		if (posAL == -1)
		{
			if (!alternativeSolutions.empty())
			{
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				//updateDM(DM, S0, G.size());
				// Mise à jour de la solution
				SqToS(G, Gq, Sq, S0, serie, parallele);
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			int posTR = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTR == -1)
			{
				if (!alternativeSolutions.empty())
				{
					updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
					Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
					alternativeSolutions.pop_back();
					alterEval.pop_back();
					//updateDM(DM, S0, G.size());
					// Mise à jour de la solution
					SqToS(G, Gq, Sq, S0, serie, parallele);
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else
				{
					break;
				}
			}
		}

		int **K = new int*[Gq.size()], **B = new int*[Gq.size()];
		int tailleInitial = getTaille(Sq, Gq.size());
		for (int j = 0; j < Gq.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		int it2 = -1;
		for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); it++)
		{
			it2++;
			for (int j = 0; j < tailleInitial; j++)
			{
				if (Sq[it2][j] == 1)
				{
					K[it2][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < Gq[it->first].size(); h++)
							{
								int it3 = -1;
								for (map<int, vector<int> >::iterator p = Gq.begin(); p != Gq.end(); p++)
								{
									it3++;
									if (Gq[it->first][h] == p->first)
									{
										break;
									}
								}
								if (Sq[it3][k] == 1)
								{
									nb++;
								}
							}
							K[it2][k] = nb;
						}
					}
					break;
				}
			}
			K[it2][tailleInitial] = 0;
		}
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < Gq.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							//out << "One move applied" << endl;
							//out << "sommet " << i << " : du stable : " << k << " vers " << h << endl;
							int** S0_ = new int*[Gq.size()];
							for (int i = 0; i < Gq.size(); i++)
							{
								S0_[i] = new int[Gq.size()];
							}
							for (int i = 0; i < Gq.size(); i++)
							{
								for (int j = 0; j < Gq.size(); j++)
								{
									S0_[i][j] = Sq[i][j];
								}
							}
							S0_[i][h] = 1;
							S0_[i][k] = 0;
							bool trouver = false;
							for (int j = 0; j < Gq.size(); j++)
							{
								if (S0_[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								//cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < Gq.size(); j++)
									{
										S0_[j][h - 1] = S0_[j][h];
									}
								}
							}
							triSolution(Gq, S0_, G.size(), serie, parallele);
							int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
							if (posTreated != -1)
							{
								posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
								if (posTreated != -1)
								{
									updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
									treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
									treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
								}
								//out << "PASS" << endl;
								for (int i = 0; i < Gq.size(); i++)
								{
									for (int j = 0; j < Gq.size(); j++)
									{
										Sq[i][j] = S0_[i][j];
									}
								}
								move = true;
								// Libérer la mémoire
								for (int j = 0; j < Gq.size(); j++)
								{
									free(S0_[j]);
								}
								free(S0_);
								break;
							}
							else
							{
								//out << "NOT PASS" << endl;
							}
							// Libérer la mémoire
							for (int j = 0; j < Gq.size(); j++)
							{
								free(S0_[j]);
							}
							free(S0_);
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
		//calcul de B et choix de l'élément à déplacer
		vector<int> taille, position;
		vector<pair<int, int> > NS0, NS1, NS2;
		pair<int, int> choix;
		int tailleActuel = getTaille(Sq, Gq.size());
		int minP = Gq.size() * tailleInitial, indLP = -1, indCP = -1, minN = Gq.size() * tailleInitial, indLN = -1, indCN = -1;
		int maxP = Gq.size() * tailleInitial * (-1);
		for (int j = 0; j < tailleActuel; j++)
		{
			int nb = 0;
			map<int, vector<int> >::iterator q = Gq.begin();
			for (int i = 0; i < Gq.size(); i++)
			{
				if (q->first < (G.size() + serie.size()))
				{
					nb += Sq[i][j];
				}
				else
				{
					nb += parallele[q->first - G.size() - serie.size()].size()*Sq[i][j];
				}
				q++;
			}
			taille.push_back(nb);
		}
		taille.push_back(0);
		for (int j = 0; j < Gq.size(); j++)
		{
			bool nt = false;
			for (int k = 0; k < tailleActuel; k++)
			{
				if (Sq[j][k] == 1)
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
		map<int, vector<int> >::iterator q = Gq.begin();
		for (int j = 0; j < Gq.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					if (q->first < (G.size() + serie.size()))
					{
						t[position[j]]--;
						t[k]++;
					}
					else
					{
						t[position[j]] -= parallele[q->first - G.size() - serie.size()].size();
						t[k] += parallele[q->first - G.size() - serie.size()].size();
					}
					for (int h = 0; h < q->second.size(); h++)
					{
						int pos = 0;
						for (map<int, vector<int> >::iterator it=Gq.begin(); it!=Gq.end(); it++)
						{
							if (q->second[h] == it->first)
							{
								break;
							}
							else
							{
								pos++;
							}
						}
						if (K[pos][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[pos][r] == 0) || (r == position[j] && K[pos][r] == 1)))
								{
									if (q->second[h] < (G.size() + serie.size()))
									{
										t[r]++;
										t[k]--;
									}
									else
									{
										t[r] += parallele[q->second[h] - G.size() - serie.size()].size();
										t[k] -= parallele[q->second[h] - G.size() - serie.size()].size();
									}
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								if (q->second[h] < (G.size() + serie.size()))
								{
									t[tailleInitial]++;
									t[k]--;
								}
								else
								{
									t[tailleInitial] += parallele[q->second[h] - G.size() - serie.size()].size();
									t[k] -= parallele[q->second[h] - G.size() - serie.size()].size();
								}
							}
						}
					}
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					if (B[j][k] > 0)
					{
						/*if (minP > B[j][k])
						//if(maxP < B[j][k])
						{
						indLP = j;
						indCP = k;
						minP = B[j][k];
						//maxP = B[j][k];
						}
						if (NS2.empty())
						{
						NS2.push_back(make_pair(j, k));
						}
						else
						{
						for (int ind = 0; ind < NS2.size(); ind++)
						{
						if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
						{
						NS2.insert(NS2.begin() + ind, make_pair(j, k));
						break;
						}
						}
						}*/
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
			q++;
		}
		out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)Gq.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)Gq.size());
		if (!NS0.empty())
		{
			out << "NS0" << endl;
			choix = NS0[0];
			amelioratif++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Zq);
			}
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				//cout << "Gq avant : " << endl;
				//afficher(Gq);
				//out << "Sq  avant tri : " << endl;
				//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
				
				out << "S0_  avant tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				out << "S0_  apres tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));

				//out << "Sq apres tri : " << endl;
				//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
				
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				
				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				out << "S0_  avant tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				out << "S0_  apres tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			out << "ALTER" << endl;
			int posTreated;
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				
				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				out << "S0_  avant tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				out << "S0_  apres tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				/*cout << "S0_ here :" << endl;
				afficher(solutionToVector(S0_, G.size()));
				cout << endl;*/
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						/*out << "\nNS1 => " << NS1[ind].first << " : " << NS1[ind].second << endl;
						out << "S0_ : " << endl;
						afficherOut(alternativeSolutions[posAlternative]);*/
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
				/*if (NS1[ind].first == 54 && NS1[ind].second == 2)
				{
				char c;
				cout << "PAUSE" << endl;
				out << "\n";
				cin >> c;
				}*/
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			//updateDM(DM, S0, G.size());
			// Mise à jour de la solution
			SqToS(G, Gq, Sq, S0, serie, parallele);
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}

			continue;
		}
		else if (!NS1.empty())
		{
			out << "NS1" << endl;
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
			}
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;
				
				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				out << "S0_  avant tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				out << "S0_  apres tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else
		{
			out << "No case" << endl;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Zq);
			}
			break;
		}

		out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		Sq[choix.first][choix.second] = 1;
		Sq[choix.first][position[choix.first]] = 0;
		
		int posChoix = 0;
		for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix.first; it_++)
		{
			if (posChoix == choix.first)
			{
				posChoix = it_->first;
			}
			else
			{
				posChoix++;
			}
		}
		for (int j = 0; j < Gq[posChoix].size(); j++)
		{
			int pos = 0;
			for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
			{
				if (it_->first == Gq[posChoix][j])
				{
					break;
				}
				else
				{
					pos++;
				}
			}
			if (K[pos][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[pos][k] == 0 || (k == position[choix.first] && K[pos][k] == 1))
					{
						Sq[pos][k] = 1;
						Sq[pos][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					Sq[pos][tailleInitial] = 1;
					Sq[pos][choix.second] = 0;
				}
				out_.push_back(pos);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		//out << "Sq  avant tri: " << endl;
		//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		out << "Sq avant tri : " << endl;
		afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		triSolution(Gq, Sq, G.size(), serie, parallele);
		out << "Sq apres tri : " << endl;
		afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		SqToS(G, Gq, Sq, S0, serie, parallele);

		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "Sq apres tri : " << endl;
		//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < Gq.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k, 1) - exchange + min and = 0 diversification + no tabu list bi - naire + Historical + One Set uses alternative + index + B - modular(rouge)
void EBTS16(map<int, vector<int> >& G, map<int, vector<int> >& Gq, int **S, int **S0, int **Sq, int& Z, int **DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated, vector<vector<int> >& serie, vector<vector<int> >& parallele)
{

	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(Gq, Sq);
		SqToS(G, Gq, Sq, S0, serie, parallele);
		triSolution(S0, G.size());
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		//updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < Gq.size(); i++)
			{
				for (int j = 0; j < Gq.size(); j++)
				{
					Sq[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(Gq, Sq);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
		SqToS(G, Gq, Sq, S0, serie, parallele);
		triSolution(S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;
	//afficherSolutionOut(S0, G.size(), G.size());

	//for (int i = 0; i < ITER_EBTS; i++)
	while (true)
	{
		//out << "iter = " << i << endl;
		/*out << "NEW ITERATION" << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cout << "taille S0 : " << endl;
		//vector<int> ss(getSTaille(S0, G.size()));
		//afficher(ss);
		//cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		out << "Alternative Solutions : " << endl;
		afficherOut(alternativeSolutions);
		out << "Nombre des Alternatives : " << alternativeSolutions.size() << endl;
		out << "Evaluations Alternative : " << endl;
		afficherOut(alterEval);
		out << "index alternative : " << endl;
		afficherOut(indexAlternative);
		out << "treated Solutions : " << endl;
		afficherOut(treatedSolutions);
		out << "Nombre des Treated : " << treatedSolutions.size() << endl;
		out << "Evaluations Treated : " << endl;
		afficherOut(treatedEval);
		out << "index treated : " << endl;
		afficherOut(indexTreated);
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		cout << "Sq  : " << endl;
		afficherSolution(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		out << "Sq : " << endl;
		afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		*/
		int Z0 = getEvaluation(S0, G.size());
		int Zq = getEvaluation(Sq, Gq.size());

		cout << "Z = " << Z << endl;
		cout << "Z0 = " << Z0 << endl;
		cout << "Zq = " << Zq << endl;
		//out << "Z = " << Z << endl;
		//out << "Z0 = " << Z0 << endl;
		//out << "Zq = " << Zq << endl;
		if (!isSolution(G, S0))
		{
			out << "BREAK ===> NOT SOLUTION" << endl;
			cout << "BREAK ===> NOT SOLUTION" << endl;
			afficherOut(S0, G.size(), G.size());
			afficherSolutionOut(S0, G.size(), G.size());
			system("pause");
			break;
		}
		if (!isSolutionQ(Gq, Sq))
		{
			out << "BREAK ===> Sq NOT SOLUTION" << endl;
			cout << "BREAK ===> Sq NOT SOLUTION" << endl;
			afficherOut(Sq, Gq.size(), Gq.size());
			afficherSolutionOut(Sq, Gq.size(), Gq.size());
			system("pause");
			break;
		}
		int posAL = exist(alternativeSolutions, alterEval, indexAlternative, Sq, Gq.size());
		if (posAL == -1)
		{
			if (!alternativeSolutions.empty())
			{
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				//updateDM(DM, S0, G.size());
				// Mise à jour de la solution
				SqToS(G, Gq, Sq, S0, serie, parallele);
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			int posTR = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTR == -1)
			{
				if (!alternativeSolutions.empty())
				{
					updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
					Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
					alternativeSolutions.pop_back();
					alterEval.pop_back();
					//updateDM(DM, S0, G.size());
					// Mise à jour de la solution
					SqToS(G, Gq, Sq, S0, serie, parallele);
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else
				{
					break;
				}
			}
		}

		int **K = new int*[Gq.size()], **B = new int*[Gq.size()];
		int tailleInitial = getTaille(Sq, Gq.size());
		for (int j = 0; j < Gq.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		int it2 = -1;
		for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); it++)
		{
			it2++;
			for (int j = 0; j < tailleInitial; j++)
			{
				if (Sq[it2][j] == 1)
				{
					K[it2][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < Gq[it->first].size(); h++)
							{
								int it3 = -1;
								for (map<int, vector<int> >::iterator p = Gq.begin(); p != Gq.end(); p++)
								{
									it3++;
									if (Gq[it->first][h] == p->first)
									{
										break;
									}
								}
								if (Sq[it3][k] == 1)
								{
									nb++;
								}
							}
							K[it2][k] = nb;
						}
					}
					break;
				}
			}
			K[it2][tailleInitial] = 0;
		}
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < Gq.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							//out << "One move applied" << endl;
							//out << "sommet " << i << " : du stable : " << k << " vers " << h << endl;
							int** S0_ = new int*[Gq.size()];
							for (int i = 0; i < Gq.size(); i++)
							{
								S0_[i] = new int[Gq.size()];
							}
							for (int i = 0; i < Gq.size(); i++)
							{
								for (int j = 0; j < Gq.size(); j++)
								{
									S0_[i][j] = Sq[i][j];
								}
							}
							S0_[i][h] = 1;
							S0_[i][k] = 0;
							bool trouver = false;
							for (int j = 0; j < Gq.size(); j++)
							{
								if (S0_[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								//cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < Gq.size(); j++)
									{
										S0_[j][h - 1] = S0_[j][h];
									}
								}
							}
							triSolution(Gq, S0_, G.size(), serie, parallele);
							int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
							if (posTreated != -1)
							{
								posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
								if (posTreated != -1)
								{
									updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
									treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
									treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
								}
								//out << "PASS" << endl;
								for (int i = 0; i < Gq.size(); i++)
								{
									for (int j = 0; j < Gq.size(); j++)
									{
										Sq[i][j] = S0_[i][j];
									}
								}
								move = true;
								// Libérer la mémoire
								for (int j = 0; j < Gq.size(); j++)
								{
									free(S0_[j]);
								}
								free(S0_);
								break;
							}
							else
							{
								//out << "NOT PASS" << endl;
							}
							// Libérer la mémoire
							for (int j = 0; j < Gq.size(); j++)
							{
								free(S0_[j]);
							}
							free(S0_);
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
		//calcul de B et choix de l'élément à déplacer
		vector<int> taille, position;
		vector<pair<int, int> > NS0, NS1, NS2;
		pair<int, int> choix;
		int tailleActuel = getTaille(Sq, Gq.size());
		int minP = Gq.size() * tailleInitial, indLP = -1, indCP = -1, minN = Gq.size() * tailleInitial, indLN = -1, indCN = -1;
		int maxP = Gq.size() * tailleInitial * (-1);
		int tailleAct = getTaille(S0, G.size());
		for (int j = 0; j < tailleAct; j++)
		{
			int nb = 0;
			for (int i = 0; i < G.size(); i++)
			{
				nb += S0[i][j];
			}
			taille.push_back(nb);
		}
		taille.push_back(0);
		for (int j = 0; j < Gq.size(); j++)
		{
			bool nt = false;
			for (int k = 0; k < tailleActuel; k++)
			{
				if (Sq[j][k] == 1)
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
		map<int, vector<int> >::iterator q = Gq.begin();
		for (int j = 0; j < Gq.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> out_;
					int **S0_ = new int*[Gq.size()];
					for (int i = 0; i < Gq.size(); i++)
					{
						S0_[i] = new int[Gq.size()];
					}
					for (int i = 0; i < Gq.size(); i++)
					{
						for (int j = 0; j < Gq.size(); j++)
						{
							S0_[i][j] = Sq[i][j];
						}
					}
					int **S1 = new int*[G.size()];
					for (int i = 0; i < G.size(); i++)
					{
						S1[i] = new int[G.size()];
					}
					for (int i = 0; i < G.size(); i++)
					{
						for (int j = 0; j < G.size(); j++)
						{
							S1[i][j] = 0;
						}
					}
					pair<int, int> ch = make_pair(j, k);
					S0_[ch.first][ch.second] = 1;
					S0_[ch.first][position[ch.first]] = 0;

					int posChoix = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= ch.first; it_++)
					{
						if (posChoix == ch.first)
						{
							posChoix = it_->first;
						}
						else
						{
							posChoix++;
						}
					}
					for (int j = 0; j < Gq[posChoix].size(); j++)
					{
						int pos = 0;
						for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
						{
							if (it_->first == Gq[posChoix][j])
							{
								break;
							}
							else
							{
								pos++;
							}
						}
						if (K[pos][ch.second] == -1)
						{
							bool trouver = false;
							for (int k = 0; k < tailleInitial; k++)
							{
								if (K[pos][k] == 0 || (k == position[ch.first] && K[pos][k] == 1))
								{
									S0_[pos][k] = 1;
									S0_[pos][ch.second] = 0;
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								S0_[pos][tailleInitial] = 1;
								S0_[pos][ch.second] = 0;
							}
							out_.push_back(pos);
						}
					}
					//cout << "out : " << endl;
					//afficher(out);
					//cout << "S0 avant tri : " << endl;
					//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
					//out << "Sq  avant tri: " << endl;
					//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
					triSolution(Gq, S0_, G.size(), serie, parallele);
					SqToS(G, Gq, S0_, S1, serie, parallele);
					B[j][k] = getEvaluation(S1, G.size()) - getEvaluation(taille);
					// Libérer la mémoire
					for (int j = 0; j < Gq.size(); j++)
					{
						free(S0_[j]);
					}
					free(S0_);
					// Libérer la mémoire
					for (int j = 0; j < G.size(); j++)
					{
						free(S1[j]);
					}
					free(S1);
					if (B[j][k] > 0)
					{
						/*if (minP > B[j][k])
						//if(maxP < B[j][k])
						{
						indLP = j;
						indCP = k;
						minP = B[j][k];
						//maxP = B[j][k];
						}
						if (NS2.empty())
						{
						NS2.push_back(make_pair(j, k));
						}
						else
						{
						for (int ind = 0; ind < NS2.size(); ind++)
						{
						if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
						{
						NS2.insert(NS2.begin() + ind, make_pair(j, k));
						break;
						}
						}
						}*/
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
			q++;
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)Gq.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)Gq.size());*/
		if (!NS0.empty())
		{
			choix = NS0[0];
			amelioratif++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Zq);
			}
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				//cout << "Gq avant : " << endl;
				//afficher(Gq);
				//out << "Sq  avant tri : " << endl;
				//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));

				triSolution(Gq, S0_, G.size(), serie, parallele);
				
				//out << "Sq apres tri : " << endl;
				//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			int posTreated;
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				/*cout << "S0_ here :" << endl;
				afficher(solutionToVector(S0_, G.size()));
				cout << endl;*/
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						/*out << "\nNS1 => " << NS1[ind].first << " : " << NS1[ind].second << endl;
						out << "S0_ : " << endl;
						afficherOut(alternativeSolutions[posAlternative]);*/
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
				/*if (NS1[ind].first == 54 && NS1[ind].second == 2)
				{
				char c;
				cout << "PAUSE" << endl;
				out << "\n";
				cin >> c;
				}*/
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			//updateDM(DM, S0, G.size());
			// Mise à jour de la solution
			SqToS(G, Gq, Sq, S0, serie, parallele);
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}

			continue;
		}
		else if (!NS1.empty())
		{
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
			}
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else
		{
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Zq);
			}
			break;
		}

		// Construction de la nouvelle solution
		vector<int> out_;
		Sq[choix.first][choix.second] = 1;
		Sq[choix.first][position[choix.first]] = 0;

		int posChoix = 0;
		for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix.first; it_++)
		{
			if (posChoix == choix.first)
			{
				posChoix = it_->first;
			}
			else
			{
				posChoix++;
			}
		}
		for (int j = 0; j < Gq[posChoix].size(); j++)
		{
			int pos = 0;
			for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
			{
				if (it_->first == Gq[posChoix][j])
				{
					break;
				}
				else
				{
					pos++;
				}
			}
			if (K[pos][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[pos][k] == 0 || (k == position[choix.first] && K[pos][k] == 1))
					{
						Sq[pos][k] = 1;
						Sq[pos][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					Sq[pos][tailleInitial] = 1;
					Sq[pos][choix.second] = 0;
				}
				out_.push_back(pos);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		//out << "Sq  avant tri: " << endl;
		//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		triSolution(Gq, Sq, G.size(), serie, parallele);
		SqToS(G, Gq, Sq, S0, serie, parallele);

		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "Sq apres tri : " << endl;
		//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < Gq.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k, 1) - exchange + min and = 0 diversification + no tabu list bi - naire + Historical + One Set uses alternative + index + B - modular(rouge) + Affichage
void EBTS16withScreen(map<int, vector<int> >& G, map<int, vector<int> >& Gq, int **S, int **S0, int **Sq, int& Z, int **DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated, vector<vector<int> >& serie, vector<vector<int> >& parallele)
{

	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(Gq, Sq);
		SqToS(G, Gq, Sq, S0, serie, parallele);
		triSolution(S0, G.size());
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		//updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else
	{
		while (true)
		{
			//out << "Diversification" << endl;
			for (int i = 0; i < Gq.size(); i++)
			{
				for (int j = 0; j < Gq.size(); j++)
				{
					Sq[i][j] = 0;
				}
			}
			ConstructedGreedyRandomizedSolution(Gq, Sq);
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				break;
			}
		}
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		//cost << getEvaluation(S0, G.size()) << endl;
		SqToS(G, Gq, Sq, S0, serie, parallele);
		triSolution(S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	//listSolutions hopefulSolutions(comparatorVector3(G.size(), true));
	vector<unordered_set<vector<int>, hashFunction> > alternativeSolutions;
	vector<int> alterEval;
	vector<pair<int, int> > indexAlternative;
	//afficherSolutionOut(S0, G.size(), G.size());

	// Fix solution
	/*for (int i = 0; i < Gq.size(); i++)
	{
		for (int j = 0; j < Gq.size(); j++)
		{
			Sq[i][j] = 0;
		}
	}
	Sq[0][2] = 1;
	Sq[1][1] = 1;
	Sq[2][0] = 1;
	Sq[3][0] = 1;
	Sq[4][2] = 1;
	Sq[5][1] = 1;
	Sq[6][0] = 1;
	Sq[7][1] = 1;
	SqToS(G, Gq, Sq, S0, serie, parallele);
	triSolution(S0, G.size());
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			S[i][j] = S0[i][j];
		}
	}
	Z = getEvaluation(S, G.size());*/
	//for (int i = 0; i < ITER_EBTS; i++)
	while (true)
	{
		//out << "iter = " << i << endl;
		out << "NEW ITERATION" << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		out << "\tS0  : " << endl;
		afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//cout << "taille S0 : " << endl;
		//vector<int> ss(getSTaille(S0, G.size()));
		//afficher(ss);
		//cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		out << "Alternative Solutions : " << endl;
		afficherOut(alternativeSolutions);
		out << "Nombre des Alternatives : " << alternativeSolutions.size() << endl;
		out << "Evaluations Alternative : " << endl;
		afficherOut(alterEval);
		out << "index alternative : " << endl;
		afficherOut(indexAlternative);
		out << "treated Solutions : " << endl;
		afficherOut(treatedSolutions);
		out << "Nombre des Treated : " << treatedSolutions.size() << endl;
		out << "Evaluations Treated : " << endl;
		afficherOut(treatedEval);
		out << "index treated : " << endl;
		afficherOut(indexTreated);
		cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "cout  = " << getEvaluation(S0, G.size()) << endl;
		cout << "Sq  : " << endl;
		afficherSolution(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		out << "Sq : " << endl;
		afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));

		int Z0 = getEvaluation(S0, G.size());
		int Zq = getEvaluation(Sq, Gq.size());

		cout << "Z = " << Z << endl;
		cout << "Z0 = " << Z0 << endl;
		cout << "Zq = " << Zq << endl;
		out << "Z = " << Z << endl;
		out << "Z0 = " << Z0 << endl;
		out << "Zq = " << Zq << endl;
		if (!isSolution(G, S0))
		{
			out << "BREAK ===> NOT SOLUTION" << endl;
			cout << "BREAK ===> NOT SOLUTION" << endl;
			afficherOut(S0, G.size(), G.size());
			afficherSolutionOut(S0, G.size(), G.size());
			system("pause");
			break;
		}
		if (!isSolutionQ(Gq, Sq))
		{
			out << "BREAK ===> Sq NOT SOLUTION" << endl;
			cout << "BREAK ===> Sq NOT SOLUTION" << endl;
			afficherOut(Sq, Gq.size(), Gq.size());
			afficherSolutionOut(Sq, Gq.size(), Gq.size());
			system("pause");
			break;
		}
		int posAL = exist(alternativeSolutions, alterEval, indexAlternative, Sq, Gq.size());
		if (posAL == -1)
		{
			if (!alternativeSolutions.empty())
			{
				updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
				Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
				alternativeSolutions.pop_back();
				alterEval.pop_back();
				//updateDM(DM, S0, G.size());
				// Mise à jour de la solution
				SqToS(G, Gq, Sq, S0, serie, parallele);
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
					out << "best new solution : " << endl;
					afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
					out << "Z = " << Z << endl;
					vraiAmelioration++;
				}
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			int posTR = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTR == -1)
			{
				if (!alternativeSolutions.empty())
				{
					updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
					Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
					alternativeSolutions.pop_back();
					alterEval.pop_back();
					//updateDM(DM, S0, G.size());
					// Mise à jour de la solution
					SqToS(G, Gq, Sq, S0, serie, parallele);
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
						out << "best new solution : " << endl;
						afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
						out << "Z = " << Z << endl;
						vraiAmelioration++;
					}
					continue;
				}
				else
				{
					break;
				}
			}
		}

		int **K = new int*[Gq.size()], **B = new int*[Gq.size()];
		int tailleInitial = getTaille(Sq, Gq.size());
		for (int j = 0; j < Gq.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		int it2 = -1;
		for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); it++)
		{
			it2++;
			for (int j = 0; j < tailleInitial; j++)
			{
				if (Sq[it2][j] == 1)
				{
					K[it2][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < Gq[it->first].size(); h++)
							{
								int it3 = -1;
								for (map<int, vector<int> >::iterator p = Gq.begin(); p != Gq.end(); p++)
								{
									it3++;
									if (Gq[it->first][h] == p->first)
									{
										break;
									}
								}
								if (Sq[it3][k] == 1)
								{
									nb++;
								}
							}
							K[it2][k] = nb;
						}
					}
					break;
				}
			}
			K[it2][tailleInitial] = 0;
		}
		bool move = false;
		for (int k = tailleInitial - 1; k > 0; k--)
		{
			for (int i = 0; i < Gq.size(); i++)
			{
				if (K[i][k] == -1)
				{
					for (int h = 0; h < k; h++)
					{
						if (K[i][h] == 0)
						{
							//out << "One move applied" << endl;
							//out << "sommet " << i << " : du stable : " << k << " vers " << h << endl;
							int** S0_ = new int*[Gq.size()];
							for (int i = 0; i < Gq.size(); i++)
							{
								S0_[i] = new int[Gq.size()];
							}
							for (int i = 0; i < Gq.size(); i++)
							{
								for (int j = 0; j < Gq.size(); j++)
								{
									S0_[i][j] = Sq[i][j];
								}
							}
							S0_[i][h] = 1;
							S0_[i][k] = 0;
							bool trouver = false;
							for (int j = 0; j < Gq.size(); j++)
							{
								if (S0_[j][k] == 1)
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								//cout << "fill void" << endl;
								for (h = k + 1; h <= tailleInitial; h++)
								{
									for (int j = 0; j < Gq.size(); j++)
									{
										S0_[j][h - 1] = S0_[j][h];
									}
								}
							}
							triSolution(Gq, S0_, G.size(), serie, parallele);
							int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
							if (posTreated != -1)
							{
								posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
								if (posTreated != -1)
								{
									updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
									treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
									treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
								}
								//out << "PASS" << endl;
								for (int i = 0; i < Gq.size(); i++)
								{
									for (int j = 0; j < Gq.size(); j++)
									{
										Sq[i][j] = S0_[i][j];
									}
								}
								move = true;
								// Libérer la mémoire
								for (int j = 0; j < Gq.size(); j++)
								{
									free(S0_[j]);
								}
								free(S0_);
								break;
							}
							else
							{
								//out << "NOT PASS" << endl;
							}
							// Libérer la mémoire
							for (int j = 0; j < Gq.size(); j++)
							{
								free(S0_[j]);
							}
							free(S0_);
						}
					}
					if (move)
					{
						break;
					}
				}
			}
			if (move)
			{
				break;
			}
		}
		if (move)
		{
			nbMove++;
			continue;
		}
		//calcul de B et choix de l'élément à déplacer
		vector<int> taille, position;
		vector<pair<int, int> > NS0, NS1, NS2;
		pair<int, int> choix;
		int tailleActuel = getTaille(Sq, Gq.size());
		int minP = Gq.size() * tailleInitial, indLP = -1, indCP = -1, minN = Gq.size() * tailleInitial, indLN = -1, indCN = -1;
		int maxP = Gq.size() * tailleInitial * (-1);
		int tailleAct = getTaille(S0, G.size());
		for (int j = 0; j < tailleAct; j++)
		{
			int nb = 0;
			for (int i = 0; i < G.size(); i++)
			{
				nb += S0[i][j];
			}
			taille.push_back(nb);
		}
		taille.push_back(0);
		for (int j = 0; j < Gq.size(); j++)
		{
			bool nt = false;
			for (int k = 0; k < tailleActuel; k++)
			{
				if (Sq[j][k] == 1)
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
		map<int, vector<int> >::iterator q = Gq.begin();
		for (int j = 0; j < Gq.size(); j++)
		{
			for (int k = 0; k < tailleInitial + 1; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> out_;
					int **S0_ = new int*[Gq.size()];
					for (int i = 0; i < Gq.size(); i++)
					{
						S0_[i] = new int[Gq.size()];
					}
					for (int i = 0; i < Gq.size(); i++)
					{
						for (int j = 0; j < Gq.size(); j++)
						{
							S0_[i][j] = Sq[i][j];
						}
					}
					int **S1 = new int*[G.size()];
					for (int i = 0; i < G.size(); i++)
					{
						S1[i] = new int[G.size()];
					}
					for (int i = 0; i < G.size(); i++)
					{
						for (int j = 0; j < G.size(); j++)
						{
							S1[i][j] = 0;
						}
					}
					out << "Mouvement tester : " << j << " To S" << k << endl;
					pair<int, int> ch = make_pair(j, k);
					out << "position actuelle : " << position[ch.first] << endl;
					S0_[ch.first][ch.second] = 1;
					S0_[ch.first][position[ch.first]] = 0;

					int posChoix = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= ch.first; it_++)
					{
						if (posChoix == ch.first)
						{
							posChoix = it_->first;
						}
						else
						{
							posChoix++;
						}
					}
					out << "PosChoix = " << posChoix << endl;
					for (int j = 0; j < Gq[posChoix].size(); j++)
					{
						int pos = 0;
						for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
						{
							if (it_->first == Gq[posChoix][j])
							{
								break;
							}
							else
							{
								pos++;
							}
						}
						if (K[pos][ch.second] == -1)
						{
							out << "Adjacent : " << Gq[posChoix][j] << " a la position : " << pos << endl;
							bool trouver = false;
							for (int k = 0; k < tailleInitial; k++)
							{
								if (K[pos][k] == 0 || (k == position[ch.first] && K[pos][k] == 1))
								{
									S0_[pos][k] = 1;
									S0_[pos][ch.second] = 0;
									trouver = true;
									out << "Changement au stable : " << k << endl;
									break;
								}
							}
							if (!trouver)
							{
								S0_[pos][tailleInitial] = 1;
								S0_[pos][ch.second] = 0;
								out << "Changement au stable : " << tailleInitial << endl;
							}
							out_.push_back(pos);
						}
					}
					//cout << "out : " << endl;
					//afficher(out);
					//cout << "S0 avant tri : " << endl;
					//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
					//out << "Sq  avant tri: " << endl;
					//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
					out << "S0_ avant tri pour B : " << endl;
					afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
					triSolution(Gq, S0_, G.size(), serie, parallele);
					out << "S0_ apres tri pour B : " << endl;
					afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
					SqToS(G, Gq, S0_, S1, serie, parallele);
					out << "S0_ correspondante dans G : " << endl;
					afficherSolutionOut(S1, G.size(), getTaille(S1, G.size()));
					B[j][k] = getEvaluation(S1, G.size()) - getEvaluation(taille);
					// Libérer la mémoire
					for (int j = 0; j < Gq.size(); j++)
					{
						free(S0_[j]);
					}
					free(S0_);
					// Libérer la mémoire
					for (int j = 0; j < G.size(); j++)
					{
						free(S1[j]);
					}
					free(S1);
					if (B[j][k] > 0)
					{
						/*if (minP > B[j][k])
						//if(maxP < B[j][k])
						{
						indLP = j;
						indCP = k;
						minP = B[j][k];
						//maxP = B[j][k];
						}
						if (NS2.empty())
						{
						NS2.push_back(make_pair(j, k));
						}
						else
						{
						for (int ind = 0; ind < NS2.size(); ind++)
						{
						if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
						{
						NS2.insert(NS2.begin() + ind, make_pair(j, k));
						break;
						}
						}
						}*/
					}
					else if (B[j][k] < 0)
					{
						if (minN > B[j][k])
						{
							indLN = j;
							indCN = k;
							minN = B[j][k];
						}
						if (NS0.empty())
						{
							NS0.push_back(make_pair(j, k));
						}
						else
						{
							for (int ind = 0; ind < NS0.size(); ind++)
							{
								if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
								{
									NS0.insert(NS0.begin() + ind, make_pair(j, k));
									break;
								}
							}
						}
					}
					else
					{
						indLP = j;
						indCP = k;
						NS1.push_back(make_pair(j, k));
					}
				}
			}
			q++;
		}
		out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)Gq.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)Gq.size());
		if (!NS0.empty())
		{
			out << "NS0" << endl;
			choix = NS0[0];
			amelioratif++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Zq);
			}
			for (int ind = 1; ind < NS0.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS0[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				//cout << "Gq avant : " << endl;
				//afficher(Gq);
				//out << "Sq  avant tri : " << endl;
				//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));

				out << "S0_  avant tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				out << "S0_  apres tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));

				//out << "Sq apres tri : " << endl;
				//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				out << "S0_  avant tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				out << "S0_  apres tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else if (!alternativeSolutions.empty())
		{
			out << "ALTER" << endl;
			int posTreated;
			for (int ind = 0; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				out << "S0_  avant tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				out << "S0_  apres tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				/*cout << "S0_ here :" << endl;
				afficher(solutionToVector(S0_, G.size()));
				cout << endl;*/
				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						/*out << "\nNS1 => " << NS1[ind].first << " : " << NS1[ind].second << endl;
						out << "S0_ : " << endl;
						afficherOut(alternativeSolutions[posAlternative]);*/
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
				/*if (NS1[ind].first == 54 && NS1[ind].second == 2)
				{
				char c;
				cout << "PAUSE" << endl;
				out << "\n";
				cin >> c;
				}*/
			}
			posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
			}
			updateIndex(alterEval, indexAlternative, false, alterEval.size() - 1);
			Sq = vectorToMatrix(alternativeSolutions[alternativeSolutions.size() - 1], Gq.size());
			alternativeSolutions.pop_back();
			alterEval.pop_back();
			//updateDM(DM, S0, G.size());
			// Mise à jour de la solution
			SqToS(G, Gq, Sq, S0, serie, parallele);
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
				out << "best new solution : " << endl;
				afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
				out << "Z = " << Z << endl;
				vraiAmelioration++;
			}

			continue;
		}
		else if (!NS1.empty())
		{
			out << "NS1" << endl;
			choix = NS1[0];
			inutil++;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, getEvaluation(Sq, Gq.size()));
			}
			for (int ind = 1; ind < NS1.size(); ind++)
			{
				// Construction de la nouvelle solution
				vector<int> out_;
				int **S0_ = new int*[Gq.size()];
				for (int i = 0; i < Gq.size(); i++)
				{
					S0_[i] = new int[Gq.size()];
				}
				for (int i = 0; i < Gq.size(); i++)
				{
					for (int j = 0; j < Gq.size(); j++)
					{
						S0_[i][j] = Sq[i][j];
					}
				}
				pair<int, int> choix2 = NS1[ind];
				S0_[choix2.first][choix2.second] = 1;
				S0_[choix2.first][position[choix2.first]] = 0;

				int posChoix = 0;
				for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix2.first; it_++)
				{
					if (posChoix == choix2.first)
					{
						posChoix = it_->first;
					}
					else
					{
						posChoix++;
					}
				}
				for (int j = 0; j < Gq[posChoix].size(); j++)
				{
					int pos = 0;
					for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
					{
						if (it_->first == Gq[posChoix][j])
						{
							break;
						}
						else
						{
							pos++;
						}
					}
					if (K[pos][choix2.second] == -1)
					{
						bool trouver = false;
						for (int k = 0; k < tailleInitial; k++)
						{
							if (K[pos][k] == 0 || (k == position[choix2.first] && K[pos][k] == 1))
							{
								S0_[pos][k] = 1;
								S0_[pos][choix2.second] = 0;
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							S0_[pos][tailleInitial] = 1;
							S0_[pos][choix2.second] = 0;
						}
						out_.push_back(Gq[posChoix][j]);
					}
				}
				//cout << "out : " << endl;
				//afficher(out);

				//cout << "S0 avant tri : " << endl;
				//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
				out << "S0_  avant tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));
				triSolution(Gq, S0_, G.size(), serie, parallele);
				out << "S0_  apres tri : " << endl;
				afficherSolutionOut(Gq, S0_, Gq.size(), getTaille(S0_, Gq.size()));

				int posAlternative = exist(alternativeSolutions, alterEval, indexAlternative, S0_, Gq.size());
				if (posAlternative != -1)
				{
					posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0_, Gq.size());
					if (posTreated != -1)
					{
						int *Z0_ = new int;
						*Z0_ = getEvaluation(S0_, Gq.size());
						updateIndex(alterEval, indexAlternative, true, posAlternative, *Z0_);
						alternativeSolutions.insert(alternativeSolutions.begin() + posAlternative, solutionToVector(S0_, Gq.size()));
						alterEval.insert(alterEval.begin() + posAlternative, *Z0_);
						free(Z0_);
						Z0_ = nullptr;
					}
				}
				// Libérer la mémoire
				for (int j = 0; j < Gq.size(); j++)
				{
					free(S0_[j]);
				}
				free(S0_);
			}
		}
		else
		{
			out << "No case" << endl;
			int posTreated = exist(treatedSolutions, treatedEval, indexTreated, Sq, Gq.size());
			if (posTreated != -1)
			{
				updateIndex(treatedEval, indexTreated, true, posTreated, Zq);
				treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(Sq, Gq.size()));
				treatedEval.insert(treatedEval.begin() + posTreated, Zq);
			}
			break;
		}

		out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		Sq[choix.first][choix.second] = 1;
		Sq[choix.first][position[choix.first]] = 0;

		int posChoix = 0;
		for (map<int, vector<int> >::iterator it_ = Gq.begin(); posChoix <= choix.first; it_++)
		{
			if (posChoix == choix.first)
			{
				posChoix = it_->first;
			}
			else
			{
				posChoix++;
			}
		}
		for (int j = 0; j < Gq[posChoix].size(); j++)
		{
			int pos = 0;
			for (map<int, vector<int> >::iterator it_ = Gq.begin(); it_ != Gq.end(); it_++)
			{
				if (it_->first == Gq[posChoix][j])
				{
					break;
				}
				else
				{
					pos++;
				}
			}
			if (K[pos][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[pos][k] == 0 || (k == position[choix.first] && K[pos][k] == 1))
					{
						Sq[pos][k] = 1;
						Sq[pos][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					Sq[pos][tailleInitial] = 1;
					Sq[pos][choix.second] = 0;
				}
				out_.push_back(pos);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		//out << "Sq  avant tri: " << endl;
		//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		out << "Sq avant tri : " << endl;
		afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		triSolution(Gq, Sq, G.size(), serie, parallele);
		out << "Sq apres tri : " << endl;
		afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		SqToS(G, Gq, Sq, S0, serie, parallele);

		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "Sq apres tri : " << endl;
		//afficherSolutionOut(Gq, Sq, Gq.size(), getTaille(Sq, Gq.size()));
		//cost << getEvaluation(S0, G.size()) << endl;
		//updateDM(DM, S0, G.size());
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
			out << "best new solution : " << endl;
			afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
			out << "Z = " << Z << endl;
			vraiAmelioration++;
		}
		// Libérer la mémoire
		for (int j = 0; j < Gq.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
	}
}

// (k,1)-exchange + min diversification + tabu list bi-naire + DataMining + DM Greedy
void EBTS17(map<int, vector<int> >& G, int** S, int** S0, int& Z, bool& diversification, int** DM, vector<unordered_set<vector<int>, hashFunction> >& treatedSolutions, vector<int>& treatedEval, vector<pair<int, int> >& indexTreated, vector<vector<pair<int, int> > >& choiceTreated)
{
	if (isNull(S, G.size()))
	{
		//out << "Solution Initiale" << endl;
		//ConstructedGreedyRandomizedInitialSolution(G, S0);
		ConstructedGreedyRandomizedSolution(G, S0);
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S[i][j] = S0[i][j];
			}
		}
		Z = getEvaluation(S, G.size());
		//cost << Z << endl;
		updateDM(DM, S, G.size());
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
	}
	else if (diversification)
	{
		//out << "Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = 0;
			}
		}
		ConstructedGreedyRandomizedSolution(G, S0, DM);
		//out << "\tS0  : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
		//out << "\tcout  = " << getEvaluation(S0, G.size()) << endl;
		updateDM(DM, S0, G.size());
		//out << "DM = " << endl;
		//afficherOut(DM, (int)G.size(), (int)G.size());
		diversification = false;
		//cost << getEvaluation(S0, G.size()) << endl;
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
			//ameliorateS0++;
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	else
	{
		//out << "No Diversification" << endl;
		for (int i = 0; i < G.size(); i++)
		{
			for (int j = 0; j < G.size(); j++)
			{
				S0[i][j] = S[i][j];
			}
		}
		//out << "\tBest Actual = " << Z << endl;
	}
	//map<pair<int, int>, int> tabuList;
	int** tabuList = new int* [G.size()];
	for (int i = 0; i < G.size(); i++)
	{
		tabuList[i] = new int[G.size()];
	}
	for (int i = 0; i < G.size(); i++)
	{
		for (int j = 0; j < G.size(); j++)
		{
			tabuList[i][j] = 0;
		}
	}
	for (int i = 0; i < ITER_EBTS; i++)
	{
		//out << "iter = " << i << endl;
		/*cout << "S0  : " << endl;
		afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "taille S0 : " << endl;
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
		int** K = new int* [G.size()], ** B = new int* [G.size()];
		int tailleInitial = getTaille(S0, G.size());
		for (int j = 0; j < G.size(); j++)
		{
			K[j] = new int[tailleInitial + 1];
			B[j] = new int[tailleInitial + 1];
		}
		// calcul de K
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			for (int j = 0; j < tailleInitial; j++)
			{
				if (S0[it->first][j] == 1)
				{
					K[it->first][j] = -1;
					for (int k = 0; k < tailleInitial; k++)
					{
						int nb = 0;
						if (j != k)
						{
							for (int h = 0; h < G[it->first].size(); h++)
							{
								if (S0[G[it->first][h]][k] == 1)
								{
									nb++;
								}
							}
							K[it->first][k] = nb;
						}
					}
					break;
				}
				K[it->first][tailleInitial] = 0;
			}
		}
		//calcul de B et choix de l'élément à déplacer
		vector<int> taille, position;
		vector<pair<int, int> > NS0, NS1, NS2;
		pair<int, int> choix;
		int tailleActuel = getTaille(S0, G.size());
		int minP = G.size() * tailleInitial, indLP = -1, indCP = -1, minN = G.size() * tailleInitial, indLN = -1, indCN = -1;
		int maxP = G.size() * tailleInitial * (-1);
		int posExact;
		int posTreated = exist(treatedSolutions, treatedEval, indexTreated, S0, G.size(), posExact);
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
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < tailleInitial; k++)
			{
				if (K[j][k] == -1)
				{
					B[j][k] = 0;
				}
				else
				{
					vector<int> t(taille);
					bool tabu(false);
					t[position[j]]--;
					t[k]++;
					for (int h = 0; h < G[j].size(); h++)
					{
						if (K[G[j][h]][k] == -1)
						{
							bool trouver = false;
							for (int r = 0; r < tailleInitial; r++)
							{
								if (k != r && ((K[G[j][h]][r] == 0) || (r == position[j] && K[G[j][h]][r] == 1)))
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
					sort(t.begin(), t.end(), comparatorVector2());
					B[j][k] = getEvaluation(t) - getEvaluation(taille);
					pair<int, int> choice(make_pair(j, k));
					if (exist(choiceTreated, posExact, choice))
					{
						interditChoix++;
						continue;
					}
					if (!inTabu(tabuList, j, k, G, S0))
					{
						if (B[j][k] > 0)
						{
							if (minP > B[j][k])
								//if(maxP < B[j][k])
							{
								indLP = j;
								indCP = k;
								minP = B[j][k];
								//maxP = B[j][k];
							}
							if (NS2.empty())
							{
								NS2.push_back(choice);
							}
							else
							{
								for (int ind = 0; ind < NS2.size(); ind++)
								{
									if (B[NS2[ind].first][NS2[ind].second] > B[j][k])
									{
										NS2.insert(NS2.begin() + ind, choice);
										break;
									}
								}
							}
						}
						else if (B[j][k] < 0)
						{
							if (minN > B[j][k])
							{
								indLN = j;
								indCN = k;
								minN = B[j][k];
							}
							if (NS0.empty())
							{
								NS0.push_back(choice);
							}
							else
							{
								for (int ind = 0; ind < NS0.size(); ind++)
								{
									if (B[NS0[ind].first][NS0[ind].second] > B[j][k])
									{
										NS0.insert(NS0.begin() + ind, choice);
										break;
									}
								}
							}
						}
						else
						{
							indLP = j;
							indCP = k;
							NS1.push_back(choice);
						}
					}
				}
			}
		}
		/*out << "K = " << endl;
		afficherOut(K, (int)tailleInitial, (int)G.size());
		out << "B = " << endl;
		afficherOut(B, (int)tailleInitial, (int)G.size());
		out << "Tabu List : " << endl;
		afficherOut(tabuList, G.size(), G.size());
		out << "DM = " << endl;
		afficherOut(DM, (int)G.size(), (int)G.size());*/
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
					if (k != NS0[j].first)
					{
						if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
							if (find(G[NS0[j].first].begin(), G[NS0[j].first].end(), k) == G[NS0[j].first].end())
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
						if (find(G[NS1[j].first].begin(), G[NS1[j].first].end(), k) == G[NS1[j].first].end())
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
								if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
									if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
						if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
							if (find(G[NS2[j].first].begin(), G[NS2[j].first].end(), k) == G[NS2[j].first].end())
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
		//Mise à jour des ensembles treatedSolutions et evalTreated et indexTreated
		int Z0 = getEvaluation(S0, G.size());
		if (posTreated != -1)
		{
			updateIndex(treatedEval, indexTreated, true, posTreated, Z0);
			treatedSolutions.insert(treatedSolutions.begin() + posTreated, solutionToVector(S0, G.size()));
			treatedEval.insert(treatedEval.begin() + posTreated, Z0);
			vector<pair<int, int> > choiceVector;
			choiceVector.push_back(choix);
			choiceTreated.insert(choiceTreated.begin() + posTreated, choiceVector);
		}
		else
		{
			choiceTreated[posExact].push_back(choix);
		}
		//out << "Choix : " << choix.first << "\t| " << choix.second << endl;
		// Construction de la nouvelle solution
		vector<int> out_;
		S0[choix.first][choix.second] = 1;
		S0[choix.first][position[choix.first]] = 0;
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				tabuList[j][choix.first] = TABU_TENURE;
				tabuList[choix.first][j] = TABU_TENURE;
			}
		}
		for (int j = 0; j < G[choix.first].size(); j++)
		{
			if (K[G[choix.first][j]][choix.second] == -1)
			{
				bool trouver = false;
				for (int k = 0; k < tailleInitial; k++)
				{
					if (K[G[choix.first][j]][k] == 0)
					{
						S0[G[choix.first][j]][k] = 1;
						S0[G[choix.first][j]][choix.second] = 0;
						trouver = true;
						break;
					}
				}
				if (!trouver)
				{
					S0[G[choix.first][j]][tailleInitial] = 1;
					S0[G[choix.first][j]][choix.second] = 0;
				}
				out_.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out_.begin(), out_.end(), j) == out_.end())
			{
				for (int k = 0; k < out_.size(); k++)
				{
					tabuList[j][out_[k]] = TABU_TENURE;
					tabuList[out_[k]][j] = TABU_TENURE;
				}
			}
		}
		//cout << "S0 avant tri : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		triSolution(S0, G.size());
		//out << "S0 apres tri : " << endl;
		//afficherSolutionOut(S0, G.size(), getTaille(S0, G.size()));
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
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (int j = 0; j < G.size(); j++)
		{
			for (int k = 0; k < G.size(); k++)
			{
				if (tabuList[j][k] != 0)
				{
					tabuList[j][k]--;
				}
			}
		}
	}
}

bool equal(vector<int>& s1, vector<int>& s2)
{
	if (s1.size() != s2.size())
	{
		return false;
	}
	for (int i = 0; i < s1.size(); i++)
	{
		bool trouver = false;
		for (int j = 0; j < s2.size(); j++)
		{
			if (s1[i] == s2[j])
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
	return true;
}

vector<vector<int> > getModule(map<int, vector<int> >& G)
{
	vector<vector<int> > p;
	vector<int> list;
	for (map<int, vector<int> >::iterator it=G.begin(); it!=G.end(); it++)
	{
		list.push_back(it->first);
	}
	while (!list.empty())
	{
		vector<int> module;
		module.push_back(list[0]);
		for (int i = 1; i < list.size(); i++)
		{
			if (equal(G[list[0]], G[list[i]]))
			{
				module.push_back(list[i]);
			}
		}
		if (module.size() > 1)
		{
			p.push_back(module);
		}
		for (int i = module.size() - 1; i > -1; i--)
		{
			for (int j = list.size() - 1; j > -1; j--)
			{
				if (module[i] == list[j])
				{
					list.erase(list.begin() + j);
					break;
				}
			}
		}
	}
	return p;
}

map<int, vector<int> > getGq(map<int, vector<int> >& G, vector<vector<int> >& serie, vector<vector<int> >& parallele)
{
	map<int, vector<int> > Gq(G);
	int n = G.size();
	for (int i = 0; i < serie.size(); i++)
	{
		Gq[n] = vector<int>();
		n++;
	}
	for (int i = 0; i < parallele.size(); i++)
	{
		Gq[n] = vector<int>();
		n++;
	}
	int k = G.size() - 1;
	for (int i = 0; i < serie.size(); i++)
	{
		k++;
		for (int j = 0; j < serie[i].size(); j++)
		{
			//cout << "Serie : " << i << ", " << j << " = " << serie[i][j] << endl;
			for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); )
			{
				//cout << "Sommet de Gq : " << it->first << endl;
				if (it->first == serie[i][j])
				{
					for (int h = 0; h < it->second.size(); h++)
					{
						//cout << "adjacent : " << h << " = " << it->second[h] << endl;
						for (vector<int>::iterator it2 = Gq[it->second[h]].begin(); it2 != Gq[it->second[h]].end(); it2++)
						{
							//cout << "sommet actuel : " << *it2 << endl;
							if (*it2 == it->first)
							{
								//cout << "in" << endl;
								Gq[it->second[h]].erase(it2);
								if (find(Gq[it->second[h]].begin(), Gq[it->second[h]].end(), k) == Gq[it->second[h]].end() && it->second[h] != k)
								{
									Gq[it->second[h]].push_back(k);
								}
								if (find(Gq[k].begin(), Gq[k].end(), it->second[h]) == Gq[k].end() && it->second[h] != k)
								{
									Gq[k].push_back(it->second[h]);
								}
								break;
							}
						}
					}
					Gq.erase(it);
					//cout << "\tGq : " << endl;
					//afficher(Gq);
					break;
				}
				else
				{
					it++;
				}
			}
		}
	}
	for (int i = 0; i < parallele.size(); i++)
	{
		k++;
		for (int j = 0; j < parallele[i].size(); j++)
		{
			//cout << "Parallele : " << i << ", " << j << " = " << parallele[i][j] << endl;
			for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); )
			{
				//cout << "Sommet de Gq : " << it->first << endl;
				if (it->first == parallele[i][j])
				{
					for (int h = 0; h < it->second.size(); h++)
					{
						//cout << "adjacent : " << h << " = " << it->second[h] << endl;
						for (vector<int>::iterator it2 = Gq[it->second[h]].begin(); it2 != Gq[it->second[h]].end(); it2++)
						{
							//cout << "sommet actuel : " << *it2 << endl;
							if (*it2 == it->first)
							{
								//cout << "in" << endl;
								Gq[it->second[h]].erase(it2);
								if (find(Gq[it->second[h]].begin(), Gq[it->second[h]].end(), k) == Gq[it->second[h]].end() && it->second[h] != k)
								{
									Gq[it->second[h]].push_back(k);
								}
								if (find(Gq[k].begin(), Gq[k].end(), it->second[h]) == Gq[k].end() && it->second[h] != k)
								{
									Gq[k].push_back(it->second[h]);
								}
								break;
							}
						}
					}
					Gq.erase(it);
					break;
				}
				else
				{
					it++;
				}
			}
		}
	}
	return Gq;
}

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{

	switch (CEvent)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		out << "S best : " << amelioratif << endl;
	}
	return TRUE;
}

int main()
{
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE)
	{
		out << "The handler is not going to work." << endl;
	}
	map<int, vector<int> > G, GB, Gq;
	unsigned int n, m, delta;
	int Z;
	vector<vector<int> > A;

	srand(time(0));

	//lireData(A, m, n, delta);
	lectureDimacs(n, m, delta, A);

	G = gToListAdj(A);
	GB = gToGBar(G);
	cout << "G : " << endl;
	
	toCplexData(n, m, delta, A, G);
	afficherOut(G);
	afficher(G);
	cout << "GB : " << endl;
	//afficher(GB);
	//vector<vector<int> > parallele = getModule(G);
	//vector<vector<int> > serie = getModule(GB);
	cout << "Parallele : " << endl;
	//afficher(parallele);
	cout << "Serie : " << endl;
	//afficher(serie);
	//Gq = getGq(G, serie, parallele);
	cout << "Gq : " << endl;
	//afficher(Gq);
	system("Pause");
	/*clock_t beg, ed, tm;
	vector<vector<int> > S;
	beg = clock();
	EBTS(G, S, Z);
	ed = clock();
	tm = ed - beg;
	cout << "Execution time : " << tm / 1000.0 << " sec" << endl;

	cout << "S  : " << endl;
	afficher(S);
	cout << "cout  = " << Z << endl;
	*/
	int lastZ = 30001;
	bool diversification = false;
	vector<vector<int> > solBest;
	vector<int> cardSol;
	clock_t beg, ed, tm;
	beg = clock();
	int **S = new int*[n];
	for (int i = 0; i < n; i++)
	{
		S[i] = new int[n];
	}
	int **DM = new int*[n];
	for (int i = 0; i < n; i++)
	{
		DM[i] = new int[n];
	}
	int **S0 = new int*[n];
	for (int i = 0; i < n; i++)
	{
		S0[i] = new int[n];
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			S[i][j] = 0;
			S0[i][j] = 0;
			if (find(G[i].begin(), G[i].end(), j) != G[i].end() || i==j)
			{
				DM[i][j] = -1;
			}
			else
			{
				DM[i][j] = 0;
			}
		}
	}
	afficher(G);
	/*int **Sq = new int*[Gq.size()];
	for (int i = 0; i < Gq.size(); i++)
	{
		Sq[i] = new int[Gq.size()];
	}
	for (int i = 0; i < Gq.size(); i++)
	{
		for (int j = 0; j < Gq.size(); j++)
		{
			Sq[i][j] = 0;
		}
	}*/
	out << "ITER_MAX = " << ITER_MAX << endl;
	out << "ITER_EBTS = " << ITER_EBTS << endl;
	out << "TABU_TENURE = " << TABU_TENURE << endl;
	//listSolutions treatedSolutions(comparatorVector3(G.size(), true));
	vector<int**> treatedSolutions;
	vector<int> treatedEval;
	vector<unordered_set<vector<int>, hashFunction> > treatedSolutions_;
	vector<pair<int, int> > indexTreated;
	// this is wallclock AND cpu time
	boost::timer::cpu_timer timer;
	for (int i = 0; i < ITER_MAX; i++)
	{
		cout << "i Global = " << i << endl;
		out << "i Global = " << i << endl;
		//EBTS(G, S, S0, Z, diversification);
		//EBTS2(G, SS, Z);
		//EBTS3(G, S, S0, Z, diversification);
		//EBTS4(G, S, S0, Z, diversification);
		//EBTSwithScreen(G, S, S0, Z, diversification);
		//EBTS5(G, S, S0, Z, diversification, DM);
		//EBTS5withScreen(G, S, S0, Z, diversification, DM);
		//EBTS6withScreen(G, S, S0, Z, diversification, DM);
		//EBTS7withScreen(G, S, S0, Z, diversification, DM);
		//EBTS7(G, S, S0, Z, diversification, DM);
		//EBTS8withScreen(G, S, S0, Z, diversification, DM);
		EBTS8(G, S, S0, Z, diversification, DM);
		//EBTS9(G, S, S0, Z, diversification, DM);
		//EBTS10(G, S, S0, Z, diversification, DM, treatedSolutions, treatedEval);
		//EBTS10withScreen(G, S, S0, Z, diversification, DM, treatedSolutions, treatedEval);
		//EBTS11(G, S, S0, Z, DM, treatedSolutions_, treatedEval, indexTreated);
		//EBTS11withScreen(G, S, S0, Z, DM, treatedSolutions_, treatedEval, indexTreated);
		//EBTS12(G, S, S0, Z, DM, treatedSolutions_, treatedEval, indexTreated);
		//EBTS12withScreen(G, S, S0, Z, DM, treatedSolutions_, treatedEval, indexTreated);
		//EBTS13(G, S, S0, Z, DM, treatedSolutions_, treatedEval, indexTreated);
		//EBTS13withScreen(G, S, S0, Z, DM, treatedSolutions_, treatedEval, indexTreated);
		//EBTS14(G, S, S0, Z, DM, treatedSolutions_, treatedEval, indexTreated);
		//EBTS14withScreen(G, S, S0, Z, DM, treatedSolutions_, treatedEval, indexTreated);
		//EBTS15(G, Gq, S, S0, Sq, Z, DM, treatedSolutions_, treatedEval, indexTreated, serie, parallele);
		//EBTS15withScreen(G, Gq, S, S0, Sq, Z, DM, treatedSolutions_, treatedEval, indexTreated, serie, parallele);
		//EBTS16(G, Gq, S, S0, Sq, Z, DM, treatedSolutions_, treatedEval, indexTreated, serie, parallele);
		//EBTS16withScreen(G, Gq, S, S0, Sq, Z, DM, treatedSolutions_, treatedEval, indexTreated, serie, parallele);

		int Z0 = getEvaluation(S0, n);
		if (Z0 == lastZ)
		{
			diversification = true;
		}
		lastZ = Z0;
		//cout << "S  : " << endl;
		//afficherSolution(S, G.size(), getTaille(S, G.size()));
		cout << "\tcout Z = " << Z << endl;
		//out << "\tcout Z = " << Z << endl;
		//cout << "S0  : " << endl;
		//afficherSolution(S0, G.size(), getTaille(S0, G.size()));
		cout << "\tcout Z0 = " << Z0 << endl;
		//out << "\tcout Z0 = " << Z0 << endl;
		/*cout << "S  : " << endl;
		afficher(SS);
		cout << "cout  = " << Z << endl;*/
		
	}
	ed = clock();
	tm = ed - beg;
	boost::timer::cpu_times elapsed = timer.elapsed();
	cout << " CPU TIME user: " << elapsed.user / 1e9 << "seconds\nCPU TIME system: " << elapsed.system / 1e9 << " seconds\n"
		<< "WALLCLOCK TIME: " << elapsed.wall / 1e9 << " seconds"
		<< std::endl;

	cout << "all CPU time: " << (elapsed.user + elapsed.system) / 1e9 << "seconds." << endl;
	cardSol.clear();
	cardSol = getSTaille(S, n);

	//cout << "Ameliorate S0 = " << ameliorateS0 << endl;
	cout << "Amelioratif = " << amelioratif << endl;
	cout << "Inutil = " << inutil << endl;
	cout << "couteux = " << couteux << endl;
	cout << "interdit = " << interdit << endl;

	cout << "S best : " << endl;
	afficherSolution(S, G.size(), getTaille(S, G.size()));
	cout << "cout best = " << Z << endl;

	cout << "Cardinalities : " << endl;
	afficher(cardSol);

	cout << "Execution time : " << tm / 1000.0 << " sec" << endl;

	cout << "Mean Execution time : " << tm / 1000.0 / ITER_MAX << " sec" << endl;

	out << "Amelioratif = " << amelioratif << endl;
	out << "Inutil = " << inutil << endl;
	out << "couteux = " << couteux << endl;
	out << "Interdit = " << interdit << endl;
	out << "Prometeux Fort 1 = " << prometeuxFort1 << endl;
	out << "Prometeux Fort 2 = " << prometeuxFort2 << endl;
	out << "Prometeux Faible 1 = " << prometeuxFaible1 << endl;
	out << "Prometeux Faible 2 = " << prometeuxFaible2 << endl;
	out << "Interdit Choix = " << interditChoix << endl;

	out << "S best : " << endl;
	afficherSolutionOut(S, G.size(), getTaille(S, G.size()));
	out << "cout best = " << Z << endl;

	out << "Cardinalities : " << endl;
	afficherOut(cardSol);

	out << "Execution time : " << tm / 1000.0 << " sec" << endl;

	out << "Mean Execution time : " << tm / 1000.0 / ITER_MAX << " sec" << endl;
	out << "Number of detected Solutions : " << treatedEval.size() << endl;
	cout << "Number of detected Solutions : " << treatedEval.size() << endl;
	out << "Nombre de move : " << nbMove << endl;

	out.close();
	cost.close();
	cout << '\a';
	system("pause");
	return 0;
}