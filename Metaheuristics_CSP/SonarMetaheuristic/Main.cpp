#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <utility>

#include "Module.h"

#define ITER_MAX_SBTS 100
#define TABU_TENURE_NS1_1 10
#define TABU_TENURE_NS1_2 
#define TABU_TENURE_NS2 7
#define ITER_MAX 5
#define ITER_EBTS 10
#define TABU_TENURE 12

using namespace std;

int num = 0;
int ameliorateS0 = 0;

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
		A[i][s1 - 1] = 1;
		A[i][s2 - 1] = 1;
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

void afficher(map<int, int>& P)
{
	cout << '{';
	for (map<int, int>::iterator it = P.begin(); it != P.end(); it++)
	{
		cout << it->first << " : " << it->second << endl;
	}
	cout << '}' << endl;
}

void afficher(vector<Module>& P)
{
	cout << '{';
	for (int i = 0; i<P.size(); i++)
	{
		cout << P[i] << ' ';
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
	sort(S.begin(), S.end(), comparatorVector());
}

void SBTS(map<int, vector<int> >& G, vector<int>& S, vector<int>& S0)
{
	// Initialisation
	if (S0.empty())
	{
		ConstructedGreedyRandomizedSolution(G, S);
	}
	else
	{
		ConstructedGreedyRandomizedSolution(G, S);
		if (S.size() < S0.size())
		{
			S = S0;
			ameliorateS0++;
		}
	}
	//cout << "Initial solution : " << endl;
	//afficher(S);

	vector<int> Slocal(S), VmS;
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); ++it)
	{
		if (find(Slocal.begin(), Slocal.end(), it->first) == Slocal.end())
		{
			VmS.push_back(it->first);
		}
	}
	//cout << "VmS = " << endl;
	//afficher(VmS);
	map<int, int> tabuList;

	// Début de la boucle principale
	for (int i = 0; i < ITER_MAX_SBTS; i++)
	{
		//cout << "\tj = " << i << endl;
		//cout << "Solution locale : " << endl;
		//afficher(Slocal);
		vector<int> KM, KD, KE, NS0, NS1, NS2, NS3Up;
		for (int j = 0; j < VmS.size(); j++)
		{
			int nb = 0;
			for (int k = 0; k < G[VmS[j]].size(); k++)
			{
				for (int h = 0; h < Slocal.size(); h++)
				{
					if (G[VmS[j]][k] == Slocal[h])
					{
						nb++;
						break;
					}
				}
			}
			KM.push_back(nb);
			if (nb == 0)
			{
				NS0.push_back(VmS[j]);
			}
			else if (nb == 1)
			{
				NS1.push_back(VmS[j]);
			}
			else if (nb == 2)
			{
				NS2.push_back(VmS[j]);
			}
			else
			{
				NS3Up.push_back(VmS[j]);
			}
			nb = 0;
			for (int k = 0; k < G[VmS[j]].size(); k++)
			{
				for (int h = 0; h < VmS.size(); h++)
				{
					if (G[VmS[j]][k] == VmS[h])
					{
						nb++;
						break;
					}
				}
			}
			KD.push_back(nb);
		}
		for (int j = 0; j < Slocal.size(); j++)
		{
			int nb = 0;
			for (int k = 0; k < G[Slocal[j]].size(); k++)
			{
				for (int h = 0; h < VmS.size(); h++)
				{
					if (G[Slocal[j]][k] == VmS[h] && KM[h] == 1)
					{
						nb++;
						break;
					}
				}
			}
			KE.push_back(nb);
		}
		/*cout << "KM = " << endl;
		afficher(KM);
		cout << "KD = " << endl;
		afficher(KD);
		cout << "KE = " << endl;
		afficher(KE);
		cout << "NS0 = " << endl;
		afficher(NS0);
		cout << "NS1 = " << endl;
		afficher(NS1);
		cout << "NS2 = " << endl;
		afficher(NS2);
		cout << "NS3 = " << endl;
		afficher(NS3Up);
		cout << "Tabu list : " << endl;
		afficher(tabuList);*/
		// Intensification
		vector<int> Sbefore(Slocal);
		if (!NS0.empty())
		{
			int v = rand() % NS0.size();
			Slocal.push_back(NS0[v]);
			tabuList.erase(NS0[v]);
			for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
			{
				if (*it == NS0[v])
				{
					VmS.erase(it);
					break;
				}
			}
			if (Slocal.size() > S.size())
			{
				S = Slocal;
			}
		}
		else if (!NS1.empty())
		{
			vector<int> KDcopie(KD);
			bool NS1supNS2(false);
			int tailleNS1 = NS1.size();
			if (NS1.size() > (NS2.size() + NS3Up.size()))
			{
				NS1supNS2 = true;
				for (int j = 0; j < Slocal.size(); j++)
				{
					if (KE[j] == 1)
					{
						for (int k = 0; k < G[Slocal[j]].size(); k++)
						{
							vector<int>::iterator kdit = KDcopie.begin();
							for (vector<int>::iterator it = NS1.begin(); it != NS1.end(); ++it)
							{
								if (*it == G[Slocal[j]][k])
								{
									NS1.erase(it);
									KDcopie.erase(kdit);
									break;
								}
								kdit++;
							}
						}
					}
				}
			}
			for (map<int, int>::iterator it2 = tabuList.begin(); it2 != tabuList.end(); ++it2)
			{
				vector<int>::iterator kdit = KDcopie.begin();
				for (vector<int>::iterator it = NS1.begin(); it != NS1.end(); ++it)
				{
					if (*it == it2->first)
					{
						NS1.erase(it);
						KDcopie.erase(kdit);
						break;
					}
					kdit++;
				}
			}
			if (!NS1.empty())
			{
				if (NS1.size() == 1)
				{
					Slocal.push_back(NS1[0]);
					tabuList.erase(Slocal[Slocal.size() - 1]);
					for (int j = 0; j < G[NS1[0]].size(); j++)
					{
						for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
						{
							if (*it == G[NS1[0]][j])
							{
								VmS.push_back(*it);
								if (NS1supNS2)
								{
									tabuList[*it] = TABU_TENURE_NS1_2 + tailleNS1;
								}
								else
								{
									tabuList[*it] = TABU_TENURE_NS1_1 + rand() % tailleNS1;
								}
								Slocal.erase(it);
								NS1.erase(NS1.begin());
								break;
							}
						}
						if (NS1.empty())
						{
							break;
						}
					}
					for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
					{
						if (*it == Slocal[Slocal.size() - 1])
						{
							VmS.erase(it);
							break;
						}
					}
				}
				else
				{
					vector<int> NS1_, NS1_2, KEcopie, KDcopie2;
					int maxKE = -1;
					for (int j = 0; j < NS1.size(); j++)
					{
						for (int k = 0; k < Slocal.size(); k++)
						{
							for (int h = 0; h < G[NS1[j]].size(); h++)
							{
								if (Slocal[k] == G[NS1[j]][h])
								{
									KEcopie.push_back(KE[k]);
									break;
								}
							}
							if (j == (KEcopie.size() - 1))
							{
								break;
							}
						}
					}
					maxKE = *max_element(KEcopie.begin(), KEcopie.end());
					for (int j = 0; j < NS1.size(); j++)
					{
						if (maxKE == KEcopie[j])
						{
							NS1_.push_back(NS1[j]);
							KDcopie2.push_back(KDcopie[j]);
						}
					}
					int maxKD = *max_element(KDcopie2.begin(), KDcopie2.end());
					for (int j = 0; j < NS1_.size(); j++)
					{
						if (maxKD == KDcopie2[j])
						{
							NS1_2.push_back(NS1_[j]);
						}
					}
					int v = rand() % NS1_2.size();
					Slocal.push_back(NS1[v]);
					tabuList.erase(Slocal[Slocal.size() - 1]);
					for (int j = 0; j < G[NS1[v]].size(); j++)
					{
						for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
						{
							if (*it == G[NS1[v]][j])
							{
								VmS.push_back(*it);
								if (NS1supNS2)
								{
									tabuList[*it] = TABU_TENURE_NS1_2 + tailleNS1;
								}
								else
								{
									tabuList[*it] = TABU_TENURE_NS1_1 + rand() % tailleNS1;
								}
								Slocal.erase(it);
								break;
							}
						}
					}
					for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
					{
						if (*it == Slocal[Slocal.size() - 1])
						{
							VmS.erase(it);
							break;
						}
					}
				}
			}
		}
		// Diversification
		if (Sbefore == Slocal)
		{
			if (NS1.size() > (NS2.size() + NS3Up.size()))
			{
				vector<int> KDcopie;
				for (map<int, int>::iterator it2 = tabuList.begin(); it2 != tabuList.end(); ++it2)
				{
					for (vector<int>::iterator it = NS3Up.begin(); it != NS3Up.end(); ++it)
					{
						if (*it == it2->first)
						{
							NS3Up.erase(it);
							break;
						}
					}
				}
				for (int j = 0; j < NS3Up.size(); j++)
				{
					for (int k = 0; k < VmS.size(); k++)
					{
						if (VmS[k] == NS3Up[j])
						{
							KDcopie.push_back(KD[k]);
							break;
						}
					}
				}
				if (!NS3Up.empty())
				{
					int maxKD = *max_element(KDcopie.begin(), KDcopie.end());
					vector<int> NS3Up_;
					for (int j = 0; j < NS3Up.size(); j++)
					{
						if (maxKD == KDcopie[j])
						{
							NS3Up_.push_back(NS3Up[j]);
						}
					}
					int v = rand() % NS3Up_.size();
					Slocal.push_back(NS3Up_[v]);
					tabuList.erase(Slocal[Slocal.size() - 1]);
					for (int j = 0; j < G[NS3Up_[v]].size(); j++)
					{
						for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
						{
							if (*it == G[NS3Up_[v]][j])
							{
								VmS.push_back(*it);
								tabuList[*it] = TABU_TENURE_NS2;
								vector<int>::iterator it2;
								bool first(false);
								if (it == Slocal.begin())
								{
									first = true;
								}
								else
								{
									it2 = it;
									it2--;
								}
								Slocal.erase(it);
								if (Slocal.empty())
								{
									break;
								}
								else if (first)
								{
									it = Slocal.begin();
								}
								else
								{
									it = it2;
								}
							}
						}
					}
					for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
					{
						if (*it == Slocal[Slocal.size() - 1])
						{
							VmS.erase(it);
							break;
						}
					}
				}
			}
			else // |NS1| <= |NS2| + |NS3Up|
			{
				if (!NS2.empty() && !NS3Up.empty())
				{
					if ((rand() % 2) == 1)
					{
						/*vector<int> KDcopie;
						for (int j = 0; j < NS3Up.size(); j++)
						{
						for (int k = 0; k < VmS.size(); k++)
						{
						if (VmS[k] == NS3Up[j])
						{
						KDcopie.push_back(KD[k]);
						break;
						}
						}
						}
						int maxKD = *max_element(KDcopie.begin(), KDcopie.end());
						vector<int> NS3Up_;
						for (int j = 0; j < NS3Up.size(); j++)
						{
						if (maxKD == KDcopie[j])
						{
						NS3Up_.push_back(NS3Up[j]);
						}
						}*/
						vector<int> NS3Up_(NS3Up);
						int v = rand() % NS3Up_.size();
						Slocal.push_back(NS3Up_[v]);
						tabuList.erase(NS3Up_[v]);
						for (int j = 0; j < G[NS3Up_[v]].size(); j++)
						{
							for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
							{
								if (*it == G[NS3Up_[v]][j])
								{
									VmS.push_back(*it);
									tabuList[*it] = TABU_TENURE_NS2;
									vector<int>::iterator it2;
									bool first(false);
									if (it == Slocal.begin())
									{
										first = true;
									}
									else
									{
										it2 = it;
										it2--;
									}
									Slocal.erase(it);
									if (Slocal.empty())
									{
										break;
									}
									else if (first)
									{
										it = Slocal.begin();
									}
									else
									{
										it = it2;
									}
								}
							}
						}
						for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
						{
							if (*it == Slocal[Slocal.size() - 1])
							{
								VmS.erase(it);
								break;
							}
						}
					}
					else // l'autre 50 %
					{
						vector<int> KDcopie;
						for (map<int, int>::iterator it2 = tabuList.begin(); it2 != tabuList.end(); ++it2)
						{
							for (vector<int>::iterator it = NS2.begin(); it != NS2.end(); ++it)
							{
								if (*it == it2->first)
								{
									NS2.erase(it);
									break;
								}
							}
						}
						if (!NS2.empty())
						{
							for (int j = 0; j < NS2.size(); j++)
							{
								for (int k = 0; k < VmS.size(); k++)
								{
									if (VmS[k] == NS2[j])
									{
										KDcopie.push_back(KD[k]);
										break;
									}
								}
							}
							int maxKD = *max_element(KDcopie.begin(), KDcopie.end());
							vector<int> NS2_;
							for (int j = 0; j < NS2.size(); j++)
							{
								if (maxKD == KDcopie[j])
								{
									NS2_.push_back(NS2[j]);
								}
							}
							if (!NS2_.empty())
							{
								int v = rand() % NS2_.size();
								Slocal.push_back(NS2_[v]);
								tabuList.erase(Slocal[Slocal.size() - 1]);
								for (int j = 0; j < G[NS2_[v]].size(); j++)
								{
									for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
									{
										if (*it == G[NS2_[v]][j])
										{
											VmS.push_back(*it);
											tabuList[*it] = TABU_TENURE_NS2;
											vector<int>::iterator it2;
											bool first(false);
											if (it == Slocal.begin())
											{
												first = true;
											}
											else
											{
												it2 = it;
												it2--;
											}
											Slocal.erase(it);
											if (Slocal.empty())
											{
												break;
											}
											else if (first)
											{
												it = Slocal.begin();
											}
											else
											{
												it = it2;
											}
										}
									}
								}
								for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
								{
									if (*it == Slocal[Slocal.size() - 1])
									{
										VmS.erase(it);
										break;
									}
								}
							}
						}
					}
				}
				else if (!NS3Up.empty())
				{
					/*vector<int> KDcopie;
					for (int j = 0; j < NS3Up.size(); j++)
					{
					for (int k = 0; k < VmS.size(); k++)
					{
					if (VmS[k] == NS3Up[j])
					{
					KDcopie.push_back(KD[k]);
					break;
					}
					}
					}
					int maxKD = *max_element(KDcopie.begin(), KDcopie.end());
					vector<int> NS3Up_;
					for (int j = 0; j < NS3Up.size(); j++)
					{
					if (maxKD == KDcopie[j])
					{
					NS3Up_.push_back(NS3Up[j]);
					}
					}*/
					vector<int> NS3Up_(NS3Up);
					int v = rand() % NS3Up_.size();
					Slocal.push_back(NS3Up_[v]);
					tabuList.erase(NS3Up_[v]);
					for (int j = 0; j < G[NS3Up_[v]].size(); j++)
					{
						for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
						{
							if (*it == G[NS3Up_[v]][j])
							{
								VmS.push_back(*it);
								tabuList[*it] = TABU_TENURE_NS2;
								vector<int>::iterator it2;
								bool first(false);
								if (it == Slocal.begin())
								{
									first = true;
								}
								else
								{
									it2 = it;
									it2--;
								}
								Slocal.erase(it);
								if (Slocal.empty())
								{
									break;
								}
								else if (first)
								{
									it = Slocal.begin();
								}
								else
								{
									it = it2;
								}
							}
						}
					}
					for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
					{
						if (*it == Slocal[Slocal.size() - 1])
						{
							VmS.erase(it);
							break;
						}
					}
				}
				else // NS3Up est vide
				{
					vector<int> KDcopie;
					for (map<int, int>::iterator it2 = tabuList.begin(); it2 != tabuList.end(); ++it2)
					{
						for (vector<int>::iterator it = NS2.begin(); it != NS2.end(); ++it)
						{
							if (*it == it2->first)
							{
								NS2.erase(it);
								break;
							}
						}
					}
					if (!NS2.empty())
					{
						for (int j = 0; j < NS2.size(); j++)
						{
							for (int k = 0; k < VmS.size(); k++)
							{
								if (VmS[k] == NS2[j])
								{
									KDcopie.push_back(KD[k]);
									break;
								}
							}
						}
						int maxKD = *max_element(KDcopie.begin(), KDcopie.end());
						vector<int> NS2_;
						for (int j = 0; j < NS2.size(); j++)
						{
							if (maxKD == KDcopie[j])
							{
								NS2_.push_back(NS2[j]);
							}
						}
						if (!NS2_.empty())
						{
							int v = rand() % NS2_.size();
							Slocal.push_back(NS2_[v]);
							tabuList.erase(Slocal[Slocal.size() - 1]);
							for (int j = 0; j < G[NS2_[v]].size(); j++)
							{
								for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
								{
									if (*it == G[NS2_[v]][j])
									{
										VmS.push_back(*it);
										tabuList[*it] = TABU_TENURE_NS2;
										vector<int>::iterator it2;
										bool first(false);
										if (it == Slocal.begin())
										{
											first = true;
										}
										else
										{
											it2 = it;
											it2--;
										}
										Slocal.erase(it);
										if (Slocal.empty())
										{
											break;
										}
										else if (first)
										{
											it = Slocal.begin();
										}
										else
										{
											it = it2;
										}
									}
								}
							}
							for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
							{
								if (*it == Slocal[Slocal.size() - 1])
								{
									VmS.erase(it);
									break;
								}
							}
						}
					}
				}
			}
		}
		map<int, int> tabu;
		for (map<int, int>::iterator it = tabuList.begin(); it != tabuList.end(); ++it)
		{
			if (it->second > 1)
			{
				tabu[it->first] = it->second - 1;
			}
		}
		tabuList = tabu;
	}
}

inline int getElement(vector<int>& NS, vector<int>& NS_)
{
	vector<int> S(NS);
	for (int i = 0; i < NS_.size(); i++)
	{
		for (vector<int>::iterator it = S.begin(); it != S.end(); ++it)
		{
			if (*it == NS_[i])
			{
				S.erase(it);
				break;
			}
		}
	}
	int v = rand() % S.size();
	for (int i = 0; i < NS.size(); i++)
	{
		if (S[v] == NS[i])
		{
			return i;
		}
	}
}

inline int max_element(map<int, int>& K)
{
	if (!K.empty())
	{
		int max = K[K.begin()->first];
		for (map<int, int>::iterator it = K.begin(); it != K.end(); ++it)
		{
			if (max < K[it->first])
			{
				max = K[it->first];
			}
		}
		return max;
	}
	return -1;
}

void SBTS2(map<int, vector<int> >& G, vector<int>& S, vector<int>& S0)
{
	// Initialisation
	if (S0.empty())
	{
		ConstructedGreedyRandomizedSolution(G, S);
	}
	else
	{
		ConstructedGreedyRandomizedSolution(G, S);
		if (S.size() < S0.size())
		{
			S = S0;
			ameliorateS0++;
		}
	}
	//cout << "Initial solution : " << endl;
	//afficher(S);

	vector<int> Slocal(S), VmS;
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); ++it)
	{
		if (find(Slocal.begin(), Slocal.end(), it->first) == Slocal.end())
		{
			VmS.push_back(it->first);
		}
	}
	//cout << "VmS = " << endl;
	//afficher(VmS);
	map<int, int> tabuList;

	map<int, int> KM, KD, KE;
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); ++it)
	{
		KM[it->first] = -1;
		KD[it->first] = -1;
		KE[it->first] = -1;
	}
	vector<int> NS0, NS1, NS2, NS3Up;
	for (int j = 0; j < VmS.size(); j++)
	{
		int nb = 0;
		for (int k = 0; k < G[VmS[j]].size(); k++)
		{
			for (int h = 0; h < Slocal.size(); h++)
			{
				if (G[VmS[j]][k] == Slocal[h])
				{
					nb++;
					break;
				}
			}
		}
		KM[VmS[j]] = nb;//KM.push_back(nb);
		if (nb == 0)
		{
			NS0.push_back(VmS[j]);
		}
		else if (nb == 1)
		{
			NS1.push_back(VmS[j]);
		}
		else if (nb == 2)
		{
			NS2.push_back(VmS[j]);
		}
		else
		{
			NS3Up.push_back(VmS[j]);
		}
		nb = 0;
		for (int k = 0; k < G[VmS[j]].size(); k++)
		{
			for (int h = 0; h < VmS.size(); h++)
			{
				if (G[VmS[j]][k] == VmS[h])
				{
					nb++;
					break;
				}
			}
		}
		KD[VmS[j]] = nb; // KD.push_back(nb);
	}
	for (int j = 0; j < Slocal.size(); j++)
	{
		int nb = 0;
		for (int k = 0; k < G[Slocal[j]].size(); k++)
		{
			for (int h = 0; h < VmS.size(); h++)
			{
				if (G[Slocal[j]][k] == VmS[h] && KM[VmS[h]] == 1)
				{
					nb++;
					break;
				}
			}
		}
		KE[Slocal[j]] = nb; //KE.push_back(nb);
	}
	// Début de la boucle principale
	for (int i = 0; i < ITER_MAX_SBTS; i++)
	{
		/*cout << "\tj = " << i << endl;
		cout << "Solution locale : " << endl;
		afficher(Slocal);

		cout << "VmS = " << endl;
		afficher(VmS);

		cout << "KM = " << endl;
		afficher(KM);
		cout << "KD = " << endl;
		afficher(KD);
		cout << "KE = " << endl;
		afficher(KE);
		cout << "NS0 = " << endl;
		afficher(NS0);
		cout << "NS1 = " << endl;
		afficher(NS1);
		cout << "NS2 = " << endl;
		afficher(NS2);
		cout << "NS3 = " << endl;
		afficher(NS3Up);
		cout << "Tabu list : " << endl;
		afficher(tabuList);*/
		// Intensification
		vector<int> Sbefore(Slocal);
		if (!NS0.empty())
		{
			//cout << "NS0" << endl;
			int v = rand() % NS0.size();
			Slocal.push_back(NS0[v]);
			tabuList.erase(NS0[v]);
			NS0.erase(NS0.begin() + v);
			// updating
			KE[Slocal[Slocal.size() - 1]] = 0;
			KM[Slocal[Slocal.size() - 1]] = -1; KD[Slocal[Slocal.size() - 1]] = -1;
			for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
			{
				for (int k = 0; k < VmS.size(); k++)
				{
					if (G[Slocal[Slocal.size() - 1]][j] == VmS[k])
					{
						KM[VmS[k]]++;
						KD[VmS[k]]--;
						if (KM[VmS[k]] == 1)
						{
							KE[Slocal[Slocal.size() - 1]]++;
						}
						else if (KM[VmS[k]] == 2)
						{
							for (int h = 0; h < G[VmS[k]].size(); h++)
							{
								if (G[VmS[k]][h] != Slocal[Slocal.size() - 1] && (find(Slocal.begin(), Slocal.end(), G[VmS[k]][h]) != Slocal.end()))
								{
									KE[G[VmS[k]][h]]--;
								}
							}
						}
						break;
					}
				}
			}
			for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
			{
				if (*it == Slocal[Slocal.size() - 1])
				{
					VmS.erase(it);
					break;
				}
			}
			if (Slocal.size() > S.size())
			{
				S = Slocal;
			}
		}
		else if (!NS1.empty())
		{
			map<int, int> KDcopie(KD);
			vector<int> NS1_bar;
			bool NS1supNS2(false);
			int tailleNS1 = NS1.size();
			for (int j = 0; j < NS2.size(); j++)
			{
				KDcopie[NS2[j]] = -1;
			}
			for (int j = 0; j < NS3Up.size(); j++)
			{
				KDcopie[NS3Up[j]] = -1;
			}
			if (NS1.size() >(NS2.size() + NS3Up.size()))
			{
				NS1supNS2 = true;
				for (int j = 0; j < Slocal.size(); j++)
				{
					if (KE[Slocal[j]] == 1)
					{
						for (int k = 0; k < G[Slocal[j]].size(); k++)
						{
							for (vector<int>::iterator it = NS1.begin(); it != NS1.end(); ++it)
							{
								if (*it == G[Slocal[j]][k])
								{
									if (find(NS1_bar.begin(), NS1_bar.end(), *it) == NS1_bar.end())
									{
										NS1_bar.push_back(*it);
										KDcopie[G[Slocal[j]][k]] = -1;
										break;
									}
								}
							}
						}
					}
				}
			}
			for (map<int, int>::iterator it2 = tabuList.begin(); it2 != tabuList.end(); ++it2)
			{
				for (vector<int>::iterator it = NS1.begin(); it != NS1.end(); ++it)
				{
					if (*it == it2->first)
					{
						if (find(NS1_bar.begin(), NS1_bar.end(), *it) == NS1_bar.end())
						{
							NS1_bar.push_back(*it);
							KDcopie[it2->first] = -1;
							break;
						}
					}
				}
			}
			if (NS1.size() > NS1_bar.size()) // NS1 contient des éléments
			{
				if ((NS1.size() - NS1_bar.size()) == 1)
				{
					//cout << "NS1 _ unique" << endl;
					int v = getElement(NS1, NS1_bar);
					Slocal.push_back(NS1[v]);
					tabuList.erase(Slocal[Slocal.size() - 1]);
					NS1.erase(NS1.begin() + v);
					// updating
					KE[Slocal[Slocal.size() - 1]] = 0;
					KM[Slocal[Slocal.size() - 1]] = -1; KD[Slocal[Slocal.size() - 1]] = -1;
					for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
					{
						for (int k = 0; k < VmS.size(); k++)
						{
							if (G[Slocal[Slocal.size() - 1]][j] == VmS[k])
							{
								KM[VmS[k]]++;
								KD[VmS[k]]--;
								//KE[Slocal[Slocal.size() - 1]]++;
								if (KM[VmS[k]] == 1)
								{
									KE[Slocal[Slocal.size() - 1]]++;
									NS1.push_back(VmS[k]);
									NS0.erase(find(NS0.begin(), NS0.end(), VmS[k]));
								}
								else if (KM[VmS[k]] == 2)
								{
									KE[Slocal[Slocal.size() - 1]]--;
									NS2.push_back(VmS[k]);
									NS1.erase(find(NS1.begin(), NS1.end(), VmS[k]));
								}
								else if (KM[VmS[k]] == 3)
								{
									NS3Up.push_back(VmS[k]);
									NS2.erase(find(NS2.begin(), NS2.end(), VmS[k]));
								}
								break;
							}
						}
					}
					for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
					{
						if (*it == Slocal[Slocal.size() - 1])
						{
							VmS.erase(it);
							break;
						}
					}
					for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
					{
						for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
						{
							if (*it == G[Slocal[Slocal.size() - 1]][j])
							{
								VmS.push_back(*it);
								KM[*it] = 1;
								KE[Slocal[Slocal.size() - 1]]++;
								KD[*it] = 0;
								NS1.push_back(*it);
								if (NS1supNS2)
								{
									tabuList[*it] = TABU_TENURE_NS1_2 + tailleNS1;
								}
								else
								{
									tabuList[*it] = TABU_TENURE_NS1_1 + rand() % tailleNS1;
								}
								for (int k = 0; k < G[*it].size(); k++)
								{
									for (int h = 0; h < VmS.size(); h++)
									{
										if (G[*it][k] == VmS[h])
										{
											KM[VmS[h]]--;
											KD[VmS[h]]++;
											KD[*it]++;
											if (KM[VmS[h]] == 1)
											{
												for (int f = 0; f < G[VmS[h]].size(); f++)
												{
													for (int l = 0; l < Slocal.size(); l++)
													{
														if (G[VmS[h]][f] == Slocal[l])
														{
															KE[Slocal[l]]++;
															break;
														}
													}
												}
												NS1.push_back(VmS[h]);
												NS2.erase(find(NS2.begin(), NS2.end(), VmS[h]));
											}
											else if (KM[VmS[h]] == 0)
											{
												NS0.push_back(VmS[h]);
												NS1.erase(find(NS1.begin(), NS1.end(), VmS[h]));
											}
											else if (KM[VmS[h]] == 2)
											{
												NS2.push_back(VmS[h]);
												NS3Up.erase(find(NS3Up.begin(), NS3Up.end(), VmS[h]));
											}
											break;
										}
									}
								}
								KE[*it] = -1;
								Slocal.erase(it);
								//NS1_bar.clear();
								break;
							}
						}
						/*if (NS1_bar.empty())
						{
						break;
						}*/
					}
				}
				else // NS1 contient plusieurs éléments
				{
					map<int, int> KEcopie;
					for (map<int, int>::iterator it = KE.begin(); it != KE.end(); ++it)
					{
						KEcopie[it->first] = -1;
					}
					int maxKE = -1;
					for (int j = 0; j < NS1.size(); j++)
					{
						bool exist = false;
						for (int k = 0; k < NS1_bar.size(); k++)
						{
							if (NS1_bar[k] == NS1[j])
							{
								exist = true;
								break;
							}
						}
						if (exist)
						{
							continue;
						}
						for (int k = 0; k < Slocal.size(); k++)
						{
							for (int h = 0; h < G[NS1[j]].size(); h++)
							{
								if (Slocal[k] == G[NS1[j]][h])
								{
									KEcopie[Slocal[k]] = KE[Slocal[k]];
									break;
								}
							}
							/*if (j == (KEcopie.size() - 1))
							{
							break;
							}*/
						}
					}
					maxKE = max_element(KEcopie);
					for (int j = 0; j < Slocal.size(); j++)
					{
						if (maxKE != KEcopie[Slocal[j]])
						{
							for (int k = 0; k < G[Slocal[j]].size(); k++)
							{
								for (int h = 0; h < NS1.size(); h++)
								{
									if (NS1[h] == G[Slocal[j]][k])
									{
										if (find(NS1_bar.begin(), NS1_bar.end(), NS1[h]) == NS1_bar.end())
										{
											NS1_bar.push_back(NS1[h]);
											KDcopie[NS1[h]] = -1;
											break;
										}
									}
								}
							}
						}
					}
					int maxKD = max_element(KDcopie);
					for (int j = 0; j < NS1.size(); j++)
					{
						if (maxKD != KDcopie[NS1[j]])
						{
							if (find(NS1_bar.begin(), NS1_bar.end(), NS1[j]) == NS1_bar.end())
							{
								NS1_bar.push_back(NS1[j]);
							}
						}
					}
					//cout << "NS1 _ plusieurs" << endl;
					int v = getElement(NS1, NS1_bar);
					Slocal.push_back(NS1[v]);
					tabuList.erase(Slocal[Slocal.size() - 1]);
					NS1.erase(NS1.begin() + v);
					// updating
					KE[Slocal[Slocal.size() - 1]] = 0;
					KM[Slocal[Slocal.size() - 1]] = -1; KD[Slocal[Slocal.size() - 1]] = -1;
					for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
					{
						for (int k = 0; k < VmS.size(); k++)
						{
							if (G[Slocal[Slocal.size() - 1]][j] == VmS[k])
							{
								KM[VmS[k]]++;
								KD[VmS[k]]--;
								//KE[Slocal[Slocal.size() - 1]]++;
								if (KM[VmS[k]] == 1)
								{
									KE[Slocal[Slocal.size() - 1]]++;
									NS1.push_back(VmS[k]);
									NS0.erase(find(NS0.begin(), NS0.end(), VmS[k]));
								}
								else if (KM[VmS[k]] == 2)
								{
									KE[Slocal[Slocal.size() - 1]]--;
									NS2.push_back(VmS[k]);
									NS1.erase(find(NS1.begin(), NS1.end(), VmS[k]));
								}
								else if (KM[VmS[k]] == 3)
								{
									NS3Up.push_back(VmS[k]);
									NS2.erase(find(NS2.begin(), NS2.end(), VmS[k]));
								}
								break;
							}
						}
					}
					for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
					{
						if (*it == Slocal[Slocal.size() - 1])
						{
							VmS.erase(it);
							break;
						}
					}
					for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
					{
						for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
						{
							if (*it == G[Slocal[Slocal.size() - 1]][j])
							{
								VmS.push_back(*it);
								KM[*it] = 1;
								KE[Slocal[Slocal.size() - 1]]++;
								KD[*it] = 0;
								NS1.push_back(*it);
								if (NS1supNS2)
								{
									tabuList[*it] = TABU_TENURE_NS1_2 + tailleNS1;
								}
								else
								{
									tabuList[*it] = TABU_TENURE_NS1_1 + rand() % tailleNS1;
								}
								for (int k = 0; k < G[*it].size(); k++)
								{
									for (int h = 0; h < VmS.size(); h++)
									{
										if (G[*it][k] == VmS[h])
										{
											KM[VmS[h]]--;
											KD[VmS[h]]++;
											KD[*it]++;
											if (KM[VmS[h]] == 1)
											{
												for (int f = 0; f < G[VmS[h]].size(); f++)
												{
													for (int l = 0; l < Slocal.size(); l++)
													{
														if (G[VmS[h]][f] == Slocal[l])
														{
															KE[Slocal[l]]++;
															break;
														}
													}
												}
												NS1.push_back(VmS[h]);
												NS2.erase(find(NS2.begin(), NS2.end(), VmS[h]));
											}
											else if (KM[VmS[h]] == 0)
											{
												NS0.push_back(VmS[h]);
												NS1.erase(find(NS1.begin(), NS1.end(), VmS[h]));
											}
											else if (KM[VmS[h]] == 2)
											{
												NS2.push_back(VmS[h]);
												NS3Up.erase(find(NS3Up.begin(), NS3Up.end(), VmS[h]));
											}
											break;
										}
									}
								}
								KE[*it] = -1;
								Slocal.erase(it);
								break;
							}
						}
					}
				}
			}
		}
		// Diversification
		if (Sbefore == Slocal)
		{
			vector<int> NS2_bar, NS3_bar;
			if (NS1.size() > (NS2.size() + NS3Up.size()))
			{
				map<int, int> KDcopie(KD);
				for (map<int, int>::iterator it2 = tabuList.begin(); it2 != tabuList.end(); ++it2)
				{
					for (vector<int>::iterator it = NS3Up.begin(); it != NS3Up.end(); ++it)
					{
						if (*it == it2->first)
						{
							if (find(NS3_bar.begin(), NS3_bar.end(), *it) == NS3_bar.end())
							{
								NS3_bar.push_back(*it);
								KDcopie[*it] = -1;
								break;
							}
						}
					}
				}
				for (int k = 0; k < VmS.size(); k++)
				{
					bool trouver = false;
					for (int j = 0; j < NS3Up.size(); j++)
					{
						if (VmS[k] == NS3Up[j])
						{
							trouver = true;
							break;
						}
					}
					if (!trouver)
					{
						KDcopie[VmS[k]] = -1;
					}
				}
				if (NS3Up.size() > NS3_bar.size())
				{
					for (int j = 0; j < NS2.size(); j++)
					{
						KDcopie[NS2[j]] = -1;
					}
					for (int j = 0; j < NS1.size(); j++)
					{
						KDcopie[NS1[j]] = -1;
					}
					int maxKD = max_element(KDcopie);
					for (int j = 0; j < NS3Up.size(); j++)
					{
						if (maxKD != KDcopie[NS3Up[j]])
						{
							if (find(NS3_bar.begin(), NS3_bar.end(), NS3Up[j]) == NS3_bar.end())
							{
								NS3_bar.push_back(NS3Up[j]);
							}
						}
					}
					//cout << "NS3 _ NS1 > NS>2" << endl;
					int v = getElement(NS3Up, NS3_bar);
					Slocal.push_back(NS3Up[v]);
					tabuList.erase(Slocal[Slocal.size() - 1]);
					NS3Up.erase(NS3Up.begin() + v);
					// updating
					KE[Slocal[Slocal.size() - 1]] = 0;
					KM[Slocal[Slocal.size() - 1]] = -1; KD[Slocal[Slocal.size() - 1]] = -1;
					for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
					{
						for (int k = 0; k < VmS.size(); k++)
						{
							if (G[Slocal[Slocal.size() - 1]][j] == VmS[k])
							{
								KM[VmS[k]]++;
								KD[VmS[k]]--;
								//KE[Slocal[Slocal.size() - 1]]++;
								if (KM[VmS[k]] == 1)
								{
									KE[Slocal[Slocal.size() - 1]]++;
									NS1.push_back(VmS[k]);
									NS0.erase(find(NS0.begin(), NS0.end(), VmS[k]));
								}
								else if (KM[VmS[k]] == 2)
								{
									KE[Slocal[Slocal.size() - 1]]--;
									NS2.push_back(VmS[k]);
									NS1.erase(find(NS1.begin(), NS1.end(), VmS[k]));
								}
								else if (KM[VmS[k]] == 3)
								{
									NS3Up.push_back(VmS[k]);
									NS2.erase(find(NS2.begin(), NS2.end(), VmS[k]));
								}
								break;
							}
						}
					}
					for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
					{
						if (*it == Slocal[Slocal.size() - 1])
						{
							VmS.erase(it);
							break;
						}
					}
					for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
					{
						for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
						{
							if (*it == G[Slocal[Slocal.size() - 1]][j])
							{
								VmS.push_back(*it);
								KM[*it] = 1;
								KE[Slocal[Slocal.size() - 1]]++;
								KD[*it] = 0;
								NS1.push_back(*it);
								tabuList[*it] = TABU_TENURE_NS2;
								//cout << "\tout : " << *it << endl;
								for (int k = 0; k < G[*it].size(); k++)
								{
									for (int h = 0; h < VmS.size(); h++)
									{
										//cout << "\ttest : " << G[*it][k] << " avec " << VmS[h] << endl;
										if (G[*it][k] == VmS[h])
										{
											//cout << "\t\tadj : " << VmS[h] << endl;
											KM[VmS[h]]--;
											KD[VmS[h]]++;
											KD[*it]++;
											//cout << "\t\t\tKout : " << KD[*it] << endl;
											//cout << "\t\t\tKadj : " << KD[VmS[h]] << endl;
											if (KM[VmS[h]] == 1)
											{
												for (int f = 0; f < G[VmS[h]].size(); f++)
												{
													for (int l = 0; l < Slocal.size(); l++)
													{
														if (G[VmS[h]][f] == Slocal[l])
														{
															KE[Slocal[l]]++;
															break;
														}
													}
												}
												NS1.push_back(VmS[h]);
												NS2.erase(find(NS2.begin(), NS2.end(), VmS[h]));
											}
											else if (KM[VmS[h]] == 0)
											{
												NS0.push_back(VmS[h]);
												NS1.erase(find(NS1.begin(), NS1.end(), VmS[h]));
											}
											else if (KM[VmS[h]] == 2)
											{
												NS2.push_back(VmS[h]);
												NS3Up.erase(find(NS3Up.begin(), NS3Up.end(), VmS[h]));
											}
											break;
										}
									}
								}
								vector<int>::iterator it2;
								bool first(false);
								if (it == Slocal.begin())
								{
									first = true;
								}
								else
								{
									it2 = it;
									it2--;
								}
								KE[*it] = -1;
								Slocal.erase(it);
								if (Slocal.empty())
								{
									break;
								}
								else if (first)
								{
									it = Slocal.begin();
								}
								else
								{
									it = it2;
								}
							}
						}
					}

				}
			}
			else // |NS1| <= |NS2| + |NS3Up|
			{
				if (!NS2.empty() && !NS3Up.empty())
				{
					if ((rand() % 2) == 1)
					{
						map<int, int> KDcopie(KD);
						vector<int> NS3_bar;
						for (int k = 0; k < VmS.size(); k++)
						{
							bool trouver = false;
							for (int j = 0; j < NS3Up.size(); j++)
							{
								if (VmS[k] == NS3Up[j])
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								KDcopie[VmS[k]] = -1;
							}
						}
						for (int j = 0; j < NS2.size(); j++)
						{
							KDcopie[NS2[j]] = -1;
						}
						for (int j = 0; j < NS1.size(); j++)
						{
							KDcopie[NS1[j]] = -1;
						}
						int maxKD = max_element(KDcopie);
						for (int j = 0; j < NS3Up.size(); j++)
						{
							if (maxKD != KDcopie[NS3Up[j]])
							{
								if (find(NS3_bar.begin(), NS3_bar.end(), NS3Up[j]) == NS3_bar.end())
								{
									NS3_bar.push_back(NS3Up[j]);
								}
							}
						}
						int v = getElement(NS3Up, NS3_bar);
						Slocal.push_back(NS3Up[v]);
						tabuList.erase(Slocal[Slocal.size() - 1]);
						NS3Up.erase(NS3Up.begin() + v);
						// updating
						KE[Slocal[Slocal.size() - 1]] = 0;
						KM[Slocal[Slocal.size() - 1]] = -1; KD[Slocal[Slocal.size() - 1]] = -1;
						for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
						{
							for (int k = 0; k < VmS.size(); k++)
							{
								if (G[Slocal[Slocal.size() - 1]][j] == VmS[k])
								{
									KM[VmS[k]]++;
									KD[VmS[k]]--;
									//KE[Slocal[Slocal.size() - 1]]++;
									if (KM[VmS[k]] == 1)
									{
										KE[Slocal[Slocal.size() - 1]]++;
										NS1.push_back(VmS[k]);
										NS0.erase(find(NS0.begin(), NS0.end(), VmS[k]));
									}
									else if (KM[VmS[k]] == 2)
									{
										KE[Slocal[Slocal.size() - 1]]--;
										NS2.push_back(VmS[k]);
										NS1.erase(find(NS1.begin(), NS1.end(), VmS[k]));
									}
									else if (KM[VmS[k]] == 3)
									{
										NS3Up.push_back(VmS[k]);
										NS2.erase(find(NS2.begin(), NS2.end(), VmS[k]));
									}
									break;
								}
							}
						}
						for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
						{
							if (*it == Slocal[Slocal.size() - 1])
							{
								VmS.erase(it);
								break;
							}
						}
						for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
						{
							for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
							{
								if (*it == G[Slocal[Slocal.size() - 1]][j])
								{
									VmS.push_back(*it);
									KM[*it] = 1;
									KE[Slocal[Slocal.size() - 1]]++;
									KD[*it] = 0;
									NS1.push_back(*it);
									tabuList[*it] = TABU_TENURE_NS2;
									//cout << "\tout : " << *it << endl;
									for (int k = 0; k < G[*it].size(); k++)
									{
										for (int h = 0; h < VmS.size(); h++)
										{
											if (G[*it][k] == VmS[h])
											{
												//cout << "\t\tadj : " << VmS[k] << endl;
												KM[VmS[h]]--;
												KD[VmS[h]]++;
												KD[*it]++;
												//cout << "\t\t\tKout = " << KD[*it] << endl;
												//cout << "\t\t\tKadj = " << KD[VmS[h]] << endl;
												if (KM[VmS[h]] == 1)
												{
													for (int f = 0; f < G[VmS[h]].size(); f++)
													{
														for (int l = 0; l < Slocal.size(); l++)
														{
															if (G[VmS[h]][f] == Slocal[l])
															{
																KE[Slocal[l]]++;
																break;
															}
														}
													}
													NS1.push_back(VmS[h]);
													NS2.erase(find(NS2.begin(), NS2.end(), VmS[h]));
												}
												else if (KM[VmS[h]] == 0)
												{
													NS0.push_back(VmS[h]);
													NS1.erase(find(NS1.begin(), NS1.end(), VmS[h]));
												}
												else if (KM[VmS[h]] == 2)
												{
													NS2.push_back(VmS[h]);
													NS3Up.erase(find(NS3Up.begin(), NS3Up.end(), VmS[h]));
												}
												break;
											}
										}
									}
									vector<int>::iterator it2;
									bool first(false);
									if (it == Slocal.begin())
									{
										first = true;
									}
									else
									{
										it2 = it;
										it2--;
									}
									KE[*it] = -1;
									Slocal.erase(it);
									if (Slocal.empty())
									{
										break;
									}
									else if (first)
									{
										it = Slocal.begin();
									}
									else
									{
										it = it2;
									}
								}
							}
						}
					}
					else // l'autre 50 %
					{
						map<int, int> KDcopie(KD);
						vector<int> NS2_bar;
						for (map<int, int>::iterator it2 = tabuList.begin(); it2 != tabuList.end(); ++it2)
						{
							for (vector<int>::iterator it = NS2.begin(); it != NS2.end(); ++it)
							{
								if (*it == it2->first)
								{
									if (find(NS2_bar.begin(), NS2_bar.end(), *it) == NS2_bar.end())
									{
										NS2_bar.push_back(*it);
										KDcopie[*it] = -1;
										break;
									}
								}
							}
						}
						if (NS2.size() > NS2_bar.size())
						{
							for (int k = 0; k < VmS.size(); k++)
							{
								bool trouver = false;
								for (int j = 0; j < NS2.size(); j++)
								{
									if (VmS[k] == NS2[j])
									{
										trouver = true;
										break;
									}
								}
								if (!trouver)
								{
									KDcopie[VmS[k]] = -1;
								}
							}
							for (int j = 0; j < NS1.size(); j++)
							{
								KDcopie[NS1[j]] = -1;
							}
							for (int j = 0; j < NS3Up.size(); j++)
							{
								KDcopie[NS3Up[j]] = -1;
							}
							int maxKD = max_element(KDcopie);
							for (int j = 0; j < NS2.size(); j++)
							{
								if (maxKD != KDcopie[NS2[j]])
								{
									if (find(NS2_bar.begin(), NS2_bar.end(), NS2[j]) == NS2_bar.end())
									{
										NS2_bar.push_back(NS2[j]);
									}
								}
							}
							if (NS2.size() > NS2_bar.size())
							{
								int v = getElement(NS2, NS2_bar);
								Slocal.push_back(NS2[v]);
								tabuList.erase(Slocal[Slocal.size() - 1]);
								NS2.erase(NS2.begin() + v);
								// updating
								KE[Slocal[Slocal.size() - 1]] = 0;
								KM[Slocal[Slocal.size() - 1]] = -1; KD[Slocal[Slocal.size() - 1]] = -1;
								for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
								{
									for (int k = 0; k < VmS.size(); k++)
									{
										if (G[Slocal[Slocal.size() - 1]][j] == VmS[k])
										{
											KM[VmS[k]]++;
											KD[VmS[k]]--;
											//KE[Slocal[Slocal.size() - 1]]++;
											if (KM[VmS[k]] == 1)
											{
												KE[Slocal[Slocal.size() - 1]]++;
												NS1.push_back(VmS[k]);
												NS0.erase(find(NS0.begin(), NS0.end(), VmS[k]));
											}
											else if (KM[VmS[k]] == 2)
											{
												KE[Slocal[Slocal.size() - 1]]--;
												NS2.push_back(VmS[k]);
												NS1.erase(find(NS1.begin(), NS1.end(), VmS[k]));
											}
											else if (KM[VmS[k]] == 3)
											{
												NS3Up.push_back(VmS[k]);
												NS2.erase(find(NS2.begin(), NS2.end(), VmS[k]));
											}
											break;
										}
									}
								}
								for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
								{
									if (*it == Slocal[Slocal.size() - 1])
									{
										VmS.erase(it);
										break;
									}
								}
								for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
								{
									for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
									{
										if (*it == G[Slocal[Slocal.size() - 1]][j])
										{
											VmS.push_back(*it);
											KM[*it] = 1;
											KE[Slocal[Slocal.size() - 1]]++;
											KD[*it] = 0;
											NS1.push_back(*it);
											tabuList[*it] = TABU_TENURE_NS2;
											for (int k = 0; k < G[*it].size(); k++)
											{
												for (int h = 0; h < VmS.size(); h++)
												{
													if (G[*it][k] == VmS[h])
													{
														KM[VmS[h]]--;
														KD[VmS[h]]++;
														KD[*it]++;
														if (KM[VmS[h]] == 1)
														{
															for (int f = 0; f < G[VmS[h]].size(); f++)
															{
																for (int l = 0; l < Slocal.size(); l++)
																{
																	if (G[VmS[h]][f] == Slocal[l])
																	{
																		KE[Slocal[l]]++;
																		break;
																	}
																}
															}
															NS1.push_back(VmS[h]);
															NS2.erase(find(NS2.begin(), NS2.end(), VmS[h]));
														}
														else if (KM[VmS[h]] == 0)
														{
															NS0.push_back(VmS[h]);
															NS1.erase(find(NS1.begin(), NS1.end(), VmS[h]));
														}
														else if (KM[VmS[h]] == 2)
														{
															NS2.push_back(VmS[h]);
															NS3Up.erase(find(NS3Up.begin(), NS3Up.end(), VmS[h]));
														}
														break;
													}
												}
											}
											vector<int>::iterator it2;
											bool first(false);
											if (it == Slocal.begin())
											{
												first = true;
											}
											else
											{
												it2 = it;
												it2--;
											}
											KE[*it] = -1;
											Slocal.erase(it);
											if (Slocal.empty())
											{
												break;
											}
											else if (first)
											{
												it = Slocal.begin();
											}
											else
											{
												it = it2;
											}
										}
									}
								}
							}
						}
					}
				}
				else if (!NS3Up.empty()) // NS2 est vide
				{
					//cout << "NS2 est vide" << endl;
					map<int, int> KDcopie(KD);
					for (int k = 0; k < VmS.size(); k++)
					{
						bool trouver = false;
						for (int j = 0; j < NS3Up.size(); j++)
						{
							if (VmS[k] == NS3Up[j])
							{
								trouver = true;
								break;
							}
						}
						if (!trouver)
						{
							KDcopie[VmS[k]] = -1;
						}
					}
					for (int j = 0; j < NS2.size(); j++)
					{
						KDcopie[NS2[j]] = -1;
					}
					for (int j = 0; j < NS1.size(); j++)
					{
						KDcopie[NS1[j]] = -1;
					}
					int maxKD = max_element(KDcopie);
					for (int j = 0; j < NS3Up.size(); j++)
					{
						if (maxKD != KDcopie[NS3Up[j]])
						{
							if (find(NS3_bar.begin(), NS3_bar.end(), NS3Up[j]) == NS3_bar.end())
							{
								NS3_bar.push_back(NS3Up[j]);
							}
						}
					}
					int v = getElement(NS3Up, NS3_bar);
					Slocal.push_back(NS3Up[v]);
					tabuList.erase(Slocal[Slocal.size() - 1]);
					NS3Up.erase(NS3Up.begin() + v);
					// updating
					KE[Slocal[Slocal.size() - 1]] = 0;
					KM[Slocal[Slocal.size() - 1]] = -1; KD[Slocal[Slocal.size() - 1]] = -1;
					for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
					{
						for (int k = 0; k < VmS.size(); k++)
						{
							if (G[Slocal[Slocal.size() - 1]][j] == VmS[k])
							{
								KM[VmS[k]]++;
								KD[VmS[k]]--;
								//KE[Slocal[Slocal.size() - 1]]++;
								if (KM[VmS[k]] == 1)
								{
									KE[Slocal[Slocal.size() - 1]]++;
									NS1.push_back(VmS[k]);
									NS0.erase(find(NS0.begin(), NS0.end(), VmS[k]));
								}
								else if (KM[VmS[k]] == 2)
								{
									KE[Slocal[Slocal.size() - 1]]--;
									NS2.push_back(VmS[k]);
									NS1.erase(find(NS1.begin(), NS1.end(), VmS[k]));
								}
								else if (KM[VmS[k]] == 3)
								{
									NS3Up.push_back(VmS[k]);
									NS2.erase(find(NS2.begin(), NS2.end(), VmS[k]));
								}
								break;
							}
						}
					}
					for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
					{
						if (*it == Slocal[Slocal.size() - 1])
						{
							VmS.erase(it);
							break;
						}
					}
					for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
					{
						//cout << "in j = " << j << endl;
						for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
						{
							if (*it == G[Slocal[Slocal.size() - 1]][j])
							{
								VmS.push_back(*it);
								KM[*it] = 1;
								KE[Slocal[Slocal.size() - 1]]++;
								KD[*it] = 0;
								NS1.push_back(*it);
								tabuList[*it] = TABU_TENURE_NS2;
								//cout << "\tout : " << *it << endl;
								for (int k = 0; k < G[*it].size(); k++)
								{
									for (int h = 0; h < VmS.size(); h++)
									{
										if (G[*it][k] == VmS[h])
										{
											//cout << "\t\tadj : " << VmS[h] << endl;
											KM[VmS[h]]--;
											KD[VmS[h]]++;
											KD[*it]++;
											//cout << "\t\t\tKout = " << KD[*it] << endl;
											//cout << "\t\t\tKadj = " << KD[VmS[h]] << endl;
											if (KM[VmS[h]] == 1)
											{
												for (int f = 0; f < G[VmS[h]].size(); f++)
												{
													for (int l = 0; l < Slocal.size(); l++)
													{
														if (G[VmS[h]][f] == Slocal[l])
														{
															KE[Slocal[l]]++;
															break;
														}
													}
												}
												NS1.push_back(VmS[h]);
												NS2.erase(find(NS2.begin(), NS2.end(), VmS[h]));
											}
											else if (KM[VmS[h]] == 0)
											{
												NS0.push_back(VmS[h]);
												NS1.erase(find(NS1.begin(), NS1.end(), VmS[h]));
											}
											else if (KM[VmS[h]] == 2)
											{
												NS2.push_back(VmS[h]);
												NS3Up.erase(find(NS3Up.begin(), NS3Up.end(), VmS[h]));
											}
											break;
										}
									}
								}
								vector<int>::iterator it2;
								bool first(false);
								if (it == Slocal.begin())
								{
									first = true;
								}
								else
								{
									it2 = it;
									it2--;
								}
								KE[*it] = -1;
								Slocal.erase(it);
								if (Slocal.empty())
								{
									break;
								}
								else if (first)
								{
									it = Slocal.begin();
								}
								else
								{
									it = it2;
								}
							}
						}
					}
				}
				else // NS3Up est vide
				{
					map<int, int> KDcopie(KD);
					vector<int> NS2_bar;
					for (map<int, int>::iterator it2 = tabuList.begin(); it2 != tabuList.end(); ++it2)
					{
						for (vector<int>::iterator it = NS2.begin(); it != NS2.end(); ++it)
						{
							if (*it == it2->first)
							{
								if (find(NS2_bar.begin(), NS2_bar.end(), *it) == NS2_bar.end())
								{
									NS2_bar.push_back(*it);
									break;
								}
							}
						}
					}
					if (NS2.size() > NS2_bar.size())
					{
						for (int k = 0; k < VmS.size(); k++)
						{
							bool trouver = false;
							for (int j = 0; j < NS2.size(); j++)
							{
								if (VmS[k] == NS2[j])
								{
									trouver = true;
									break;
								}
							}
							if (!trouver)
							{
								KDcopie[VmS[k]] = -1;
							}
						}
						for (int j = 0; j < NS1.size(); j++)
						{
							KDcopie[NS1[j]] = -1;
						}
						for (int j = 0; j < NS3Up.size(); j++)
						{
							KDcopie[NS3Up[j]] = -1;
						}
						int maxKD = max_element(KDcopie);
						for (int j = 0; j < NS2.size(); j++)
						{
							if (maxKD != KDcopie[NS2[j]])
							{
								if (find(NS2_bar.begin(), NS2_bar.end(), NS2[j]) == NS2_bar.end())
								{
									NS2_bar.push_back(NS2[j]);
								}
							}
						}
						if (NS2.size() > NS2_bar.size())
						{
							int v = getElement(NS2, NS2_bar);
							Slocal.push_back(NS2[v]);
							tabuList.erase(Slocal[Slocal.size() - 1]);
							NS2.erase(NS2.begin() + v);
							// updating
							KE[Slocal[Slocal.size() - 1]] = 0;
							KM[Slocal[Slocal.size() - 1]] = -1; KD[Slocal[Slocal.size() - 1]] = -1;
							for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
							{
								for (int k = 0; k < VmS.size(); k++)
								{
									if (G[Slocal[Slocal.size() - 1]][j] == VmS[k])
									{
										KM[VmS[k]]++;
										KD[VmS[k]]--;
										//KE[Slocal[Slocal.size() - 1]]++;
										if (KM[VmS[k]] == 1)
										{
											KE[Slocal[Slocal.size() - 1]]++;
											NS1.push_back(VmS[k]);
											NS0.erase(find(NS0.begin(), NS0.end(), VmS[k]));
										}
										else if (KM[VmS[k]] == 2)
										{
											KE[Slocal[Slocal.size() - 1]]--;
											NS2.push_back(VmS[k]);
											NS1.erase(find(NS1.begin(), NS1.end(), VmS[k]));
										}
										else if (KM[VmS[k]] == 3)
										{
											NS3Up.push_back(VmS[k]);
											NS2.erase(find(NS2.begin(), NS2.end(), VmS[k]));
										}
										break;
									}
								}
							}
							for (vector<int>::iterator it = VmS.begin(); it != VmS.end(); ++it)
							{
								if (*it == Slocal[Slocal.size() - 1])
								{
									VmS.erase(it);
									break;
								}
							}
							for (int j = 0; j < G[Slocal[Slocal.size() - 1]].size(); j++)
							{
								for (vector<int>::iterator it = Slocal.begin(); it != Slocal.end(); ++it)
								{
									if (*it == G[Slocal[Slocal.size() - 1]][j])
									{
										VmS.push_back(*it);
										KM[*it] = 1;
										KE[Slocal[Slocal.size() - 1]]++;
										KD[*it] = 0;
										NS1.push_back(*it);
										tabuList[*it] = TABU_TENURE_NS2;
										for (int k = 0; k < G[*it].size(); k++)
										{
											for (int h = 0; h < VmS.size(); h++)
											{
												if (G[*it][k] == VmS[h])
												{
													KM[VmS[h]]--;
													KD[VmS[h]]++;
													KD[*it]++;
													if (KM[VmS[h]] == 1)
													{
														for (int f = 0; f < G[VmS[h]].size(); f++)
														{
															for (int l = 0; l < Slocal.size(); l++)
															{
																if (G[VmS[h]][f] == Slocal[l])
																{
																	KE[Slocal[l]]++;
																	break;
																}
															}
														}
														NS1.push_back(VmS[h]);
														NS2.erase(find(NS2.begin(), NS2.end(), VmS[h]));
													}
													else if (KM[VmS[h]] == 0)
													{
														NS0.push_back(VmS[h]);
														NS1.erase(find(NS1.begin(), NS1.end(), VmS[h]));
													}
													else if (KM[VmS[h]] == 2)
													{
														NS2.push_back(VmS[h]);
														NS3Up.erase(find(NS3Up.begin(), NS3Up.end(), VmS[h]));
													}
													break;
												}
											}
										}
										vector<int>::iterator it2;
										bool first(false);
										if (it == Slocal.begin())
										{
											first = true;
										}
										else
										{
											it2 = it;
											it2--;
										}
										KE[*it] = -1;
										Slocal.erase(it);
										if (Slocal.empty())
										{
											break;
										}
										else if (first)
										{
											it = Slocal.begin();
										}
										else
										{
											it = it2;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		map<int, int> tabu;
		for (map<int, int>::iterator it = tabuList.begin(); it != tabuList.end(); ++it)
		{
			if (it->second > 1)
			{
				tabu[it->first] = it->second - 1;
			}
		}
		tabuList = tabu;
	}
}

bool vgStable(map<int, vector<int> > graphqlq)
{
	unsigned int nb = 0;
	for (map<int, vector<int> >::iterator it = graphqlq.begin(); it != graphqlq.end(); ++it)
	{
		if (it->second.empty())
		{
			nb++;
		}
	}
	if (nb == graphqlq.size())
	{
		return true;
	}
	else
	{
		return false;
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

vector<map<int, vector<int> > > dCConnexe(map<int, vector<int> > g_)
{
	vector<map<int, vector<int> > > ecc;
	map<int, vector<int> > g(g_);
	map<int, int> marquage;
	int pred, k = 0;
	vector<int> cc;
	map<int, vector<int> > grapheCC;
	if (vgStable(g_))
	{
		for (map<int, vector<int> >::iterator it = g_.begin(); it != g_.end(); it++)
		{
			map<int, vector<int> > grapheCC2;
			grapheCC2[it->first] = it->second;
			ecc.push_back(grapheCC2);
		}
		return ecc;
	}
	for (map<int, vector<int> >::iterator it2 = g_.begin(); it2 != g_.end(); ++it2)
	{
		if (it2->second.empty()) {
			grapheCC[it2->first] = it2->second;
			map<int, vector<int> > grapheCC2;
			grapheCC2[it2->first] = it2->second;
			ecc.push_back(grapheCC2);
		}
	}
	if (!grapheCC.empty())
	{
		for (map<int, vector<int> >::iterator iter2 = grapheCC.begin(); iter2 != grapheCC.end(); ++iter2)
		{
			for (map<int, vector<int> >::iterator iter = g_.begin(); iter != g_.end(); ++iter)//supprimer les clés des cc dans g
			{
				if (iter->first == iter2->first)
				{
					g_.erase(iter);
					break;
				}
			}
		}
		grapheCC.clear();
	}
	map<int, vector<int> >::iterator it;
	if (!g_.empty())
	{
		it = g_.begin();
		vector<int>::iterator it3;
		marquage[it->first] = -1;
		for (map<int, vector<int> >::iterator iter = g_.begin(); iter != g_.end(); ++iter)
		{
			for (vector<int>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
			{
				if (*iter2 == it->first)
				{
					iter->second.erase(iter2);
					break;
				}
			}
		}
		while (!vgStable(g_))
		{
			if (!it->second.empty()) // marquer tous les sommets
			{
				marquage[(it->second)[0]] = it->first;// marqué le sécsesseur par son prédécéseuur
				pred = (it->second)[0];// présevé ce sucsséceu
				if (!containsValue(cc, it->first))
				{
					cc.push_back(it->first); //la composante prend ce prédécesseur
				}
				for (map<int, vector<int> >::iterator it2 = g_.begin(); it2 != g_.end(); ++it2)
				{
					if (it2->first == pred) // on cherche sur les clés à ce suc pour touvé ses suc aussi!
					{
						it = it2;// pointé sur la clé qui égal a ce suc
					}
					if (!it2->second.empty())
					{
						for (it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
						{
							if (*it3 == pred) //il faut qu'on march et on revient pas à ce suc=pred =(it->second)[0]
							{
								it2->second.erase(it3); // donc on supprime leur apparense dans ses voisins
								break;
							}
						}

					}
				}
			}
			else //le it_>second.est vide
			{
				pred = marquage[it->first]; // pred =le predécésseur de it empty
				if (!containsValue(cc, it->first))
				{
					cc.push_back(it->first); //la composante prend ce prédécesseur
				}
				if (pred != -1) //la clé qui a un suc
				{
					for (map<int, vector<int> >::iterator it2 = g_.begin(); it2 != g_.end(); ++it2)// parcourir le g
					{
						if (it2->first == pred)// cherché à ce pred dans  les clés de g pour remonté
						{
							it = it2; //it =la clé qui égale à pred
							break;
						}
					}
				}
				else// je revient au point de départ
				{
					for (vector<int>::iterator iter3 = cc.begin(); iter3 != cc.end(); ++iter3)
					{
						for (map<int, vector<int> >::iterator it2 = g.begin(); it2 != g.end(); ++it2)
						{
							if (it2->first == *iter3)
							{
								grapheCC[*iter3] = it2->second;
								break;
							}
						}
					}
					ecc.push_back(grapheCC);
					map<int, vector<int> > g2;
					for (map<int, vector<int> >::iterator iter9 = g_.begin(); iter9 != g_.end(); ++iter9)//supprimer les clés des cc dans g
					{
						if (!iter9->second.empty())
						{
							for (map<int, vector<int> >::iterator iter = g.begin(); iter != g.end(); ++iter)
							{
								if (iter9->first == iter->first)
								{
									g2[iter->first] = iter->second;
									break;
								}
							}
						}
					}
					g_ = g2;

					g2.clear();
					if (!g_.empty())
					{
						it = g_.begin();//sauté au 2ém cc
						marquage[it->first] = -1;
					}
					for (map<int, vector<int> >::iterator iter = g_.begin(); iter != g_.end(); ++iter)
					{
						for (vector<int>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
						{
							if (*iter2 == it->first)
							{
								iter->second.erase(iter2);

								break;
							}
						}
					}
					k++;
					cc.clear();
					grapheCC.clear();
				}
			}
		}
		for (map<int, vector<int> >::iterator iter = g_.begin(); iter != g_.end(); ++iter)
		{
			cc.push_back(iter->first);
		}
		for (vector<int>::iterator iter3 = cc.begin(); iter3 != cc.end(); ++iter3)
		{
			for (map<int, vector<int> >::iterator it2 = g.begin(); it2 != g.end(); ++it2)
			{
				if (it2->first == *iter3)
				{
					grapheCC[*iter3] = it2->second;
					break;
				}
			}
		}
		ecc.push_back(grapheCC);
	}
	return ecc;
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
					if (j != k && A[i][k] == 1)
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

bool inclu(map<int, vector<int> >& G, vector<int>& S)
{
	for (int i = 0; i < S.size(); i++)
	{
		bool trouver = false;
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); ++it)
		{
			if (S[i] == it->first)
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
	for (int i = 0; i < (S.size() - 1); i++)
	{
		for (int j = i + 1; j < S.size(); j++)
		{
			if (find(G[S[i]].begin(), G[S[i]].end(), S[j]) != G[S[i]].end())
			{
				return false;
			}
		}
	}
	return true;
}

void decomposition(map<int, vector<int> >& G, Module* tree, Module* actuelParent, vector<vector<int> >& S0)
{
	if (vgStable(G)) // Si G est un stable
	{
		tree->setNum(num);
		tree->setG(G);
		tree->setType(1);
		tree->setParent(actuelParent);
		num++;
		if (actuelParent != nullptr)
		{
			actuelParent->addEnfant(tree);
		}
		vector<map<int, vector<int> > > ECC(dCConnexe(G));
		for (int i = 0; i < ECC.size(); i++)
		{
			Module *m;
			m = new Module(num, ECC[i], 1, tree);
			num++;
			tree->addEnfant(m);
		}
		return;
	}
	else if (vgStable(gToGBar(G))) // Si G est une clique
	{
		tree->setNum(num);
		tree->setG(G);
		tree->setType(2);
		tree->setParent(actuelParent);
		num++;
		if (actuelParent != nullptr)
		{
			actuelParent->addEnfant(tree);
		}
		vector<map<int, vector<int> > > ECC(dCConnexe(gToGBar(G)));
		for (int i = 0; i < ECC.size(); i++)
		{
			Module *m;
			m = new Module(num, ECC[i], 1, tree);
			num++;
			tree->addEnfant(m);
		}
		return;
	}
	vector<map<int, vector<int> > > ECC(dCConnexe(G));
	vector<map<int, vector<int> > > ECCBar(dCConnexe(gToGBar(G)));
	if (ECC.size() > 1)
	{
		tree->setNum(num);
		tree->setG(G);
		tree->setType(1);
		tree->setParent(actuelParent);
		num++;
		if (actuelParent != nullptr)
		{
			actuelParent->addEnfant(tree);
		}
		for (int i = 0; i < ECC.size(); i++)
		{
			Module *m = new Module();
			decomposition(ECC[i], m, tree, S0);
		}
	}
	else if (ECCBar.size() > 1)
	{
		tree->setNum(num);
		tree->setG(G);
		tree->setType(2);
		tree->setParent(actuelParent);
		num++;
		if (actuelParent != nullptr)
		{
			actuelParent->addEnfant(tree);
		}
		for (int i = 0; i < ECCBar.size(); i++)
		{
			Module *m = new Module();
			decomposition(gToGBar(ECCBar[i]), m, tree, S0);
		}
	}
	else
	{
		tree->setNum(num);
		tree->setG(G);
		tree->setType(0);
		tree->setParent(actuelParent);
		num++;
		if (actuelParent != nullptr)
		{
			actuelParent->addEnfant(tree);
		}
		// On trouve le stable ici pour diviser la partie "prime"
		// ...
		vector<int> S;
		bool match = false;
		for (int i = 0; i < S0.size(); i++)
		{
			if (inclu(G, S0[i]))
			{
				SBTS(G, S, S0[i]);
				match = true;
				break;
			}
		}
		if (!match)
		{
			SBTS(G, S, S);
		}
		//ConstructedGreedyRandomizedSolution(G, S);
		map<int, vector<int> > GmS = GminusS(G, S), GS;
		for (int i = 0; i < S.size(); i++)
		{
			GS[S[i]] = vector<int>();
		}
		Module *m = new Module();
		decomposition(GS, m, tree, S0);
		Module *m2 = new Module();
		decomposition(GmS, m2, tree, S0);
	}
}

void constructStables(Module* tree)
{
	if (tree->getEnfants().size() == tree->getG().size())
	{
		if (tree->getType() == 1)
		{
			vector<int> stable;
			for (int i = 0; i < tree->getEnfants().size(); i++)
			{
				stable.push_back(tree->getEnfants()[i]->getG().begin()->first);
			}
			tree->addStable(stable);
		}
		else if (tree->getType() == 2)
		{
			for (int i = 0; i < tree->getEnfants().size(); i++)
			{
				vector<int> stable;
				stable.push_back(tree->getEnfants()[i]->getG().begin()->first);
				tree->addStable(stable);
			}
		}
	}
	else
	{
		for (int i = 0; i < tree->getEnfants().size(); i++)
		{
			constructStables(tree->getEnfants()[i]);
		}
		if (tree->getType() == 1)
		{
			for (int i = 0; i < tree->getEnfants().size(); i++)
			{
				vector<vector<int> > copieEnfants(tree->getEnfants()[i]->getListStables());
				sort(copieEnfants.begin(), copieEnfants.end(), comparatorVector());
				for (int j = 0; j < copieEnfants.size(); j++)
				{
					if (j < tree->getListStables().size())
					{
						for (int k = 0; k < copieEnfants[j].size(); k++)
						{
							tree->addToStable(j, copieEnfants[j][k]);
						}
					}
					else
					{
						tree->addStable(copieEnfants[j]);
					}
				}
			}
		}
		else // Série ou prime considérés comme série
		{
			for (int i = 0; i < tree->getEnfants().size(); i++)
			{
				for (int j = 0; j < tree->getEnfants()[i]->getListStables().size(); j++)
				{
					tree->addStable(tree->getEnfants()[i]->getListStables()[j]);
				}
			}
			vector<vector<int> > st(tree->getListStables());
			sort(st.begin(), st.end(), comparatorVector());
			tree->setListStables(st);
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

void EBTS(map<int, vector<int> >& G, vector<vector<int> >& S, int& Z)
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
			ameliorateS0++;
		}
		else
		{
			S0 = S;
		}
	}
	map<pair<int, int>, int> tabuList;

	for (int i = 0; i < ITER_EBTS; i++)
	{
		/*cout << "iter = " << i << endl;
		cout << "S0  : " << endl;
		afficher(S0);
		cout << "cout  = " << getEvaluation(S0) << endl;*/
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
								if (k != r && K[G[j][h]][r] == 0)
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
		}
		else if (!NS0.empty())
		{
			choix = NS0[rand() % NS0.size()];
		}
		else if (indCP != -1)
		{
			choix = make_pair(indLP, indCP);
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
		}
		/*cout << "K = " << endl;
		afficher(K, (int) S0.size(), (int) G.size());
		cout << "B = " << endl;
		afficher(B, (int) S0.size(), (int) G.size());
		cout << "Tabu List : " << endl;
		afficher(tabuList);
		cout << "Choix : " << choix.first << "\t| " << choix.second << endl;*/
		// Construction de la nouvelle solution
		vector<int> out;
		S0[choix.second].push_back(choix.first);
		S0[position[choix.first]].erase(find(S0[position[choix.first]].begin(), S0[position[choix.first]].end(), choix.first));
		for (int j = 0; j < G.size(); j++)
		{
			if (j != choix.first && position[j] == position[choix.first])
			{
				if (j < choix.first)
				{
					tabuList[make_pair(j, choix.first)] = TABU_TENURE;
				}
				else
				{
					tabuList[make_pair(choix.first, j)] = TABU_TENURE;
				}
			}
		}
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
				out.push_back(G[choix.first][j]);
			}
		}
		//cout << "out : " << endl;
		//afficher(out);
		for (int j = 0; j < G.size(); j++)
		{
			if (K[j][choix.second] == -1 && find(out.begin(), out.end(), j) == out.end())
			{
				for (int k = 0; k < out.size(); k++)
				{
					if (j < out[k])
					{
						tabuList[make_pair(j, out[k])] = TABU_TENURE;
					}
					else
					{
						tabuList[make_pair(out[k], j)] = TABU_TENURE;
					}
				}
			}
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
		}
		// Libérer la mémoire
		for (int j = 0; j < G.size(); j++)
		{
			free(K[j]);
			free(B[j]);
		}
		free(K);
		free(B);
		for (map<pair<int, int>, int>::iterator it = tabuList.begin(); it != tabuList.end(); it++)
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
		}
	}
}

int main()
{
	map<int, vector<int> > G;
	unsigned int n, m, delta;
	vector<vector<int> > A;

	srand(time(0));

	//lireData(A, m, n, delta);
	lectureDimacs(n, m, delta, A);

	G = gToListAdj(A);

	/*clock_t beg, ed, tm;
	vector<int> S;
	beg = clock();
	SBTS(gToGBar(G), S, S);
	ed = clock();
	tm = ed - beg;
	cout << "Execution time : " << tm / 1000.0 << " sec" << endl;

	cout << "S  : " << endl;
	afficher(S);
	cout << "cout  = " << S.size() << endl;*/

	int ZBest = 30000;
	vector<vector<int> > solBest;
	vector<int> cardSol;
	clock_t beg, ed, tm;
	beg = clock();
	for (int i = 0; i < ITER_MAX; i++)
	{
		cout << "i = " << i << endl;
		Module *decompositionTree = new Module();
		Module *actuelParent = 0;
		decomposition(G, decompositionTree, actuelParent, solBest);

		constructStables(decompositionTree);

		cout << endl << "Liste des tables : " << endl;
		afficher(decompositionTree->getListStables());

		int eval = getEvaluation(decompositionTree->getListStables());
		cout << endl << "Evaluation = " << eval << endl;

		cout << "Ameliorate S0 = " << ameliorateS0 << endl;
		ameliorateS0 = 0;
		if (eval < ZBest)
		{
			ZBest = eval;
			solBest = decompositionTree->getListStables();
			cardSol.clear();
			for (int j = 0; j < solBest.size(); j++)
			{
				cardSol.push_back(solBest[j].size());
			}
		}
		delete decompositionTree;
		actuelParent = 0;
		decompositionTree = 0;
	}

	for (int i = 0; i < ITER_MAX; i++)
	{
		cout << "i = " << i << endl;

		EBTS(G, solBest, ZBest);

		cout << "S  : " << endl;
		afficher(solBest);
		cout << "cout  = " << ZBest << endl;

	}

	cardSol.clear();
	for (int j = 0; j < solBest.size(); j++)
	{
		cardSol.push_back(solBest[j].size());
	}

	ed = clock();
	tm = ed - beg;
	cout << endl << "Best Solution : " << endl;
	afficher(solBest);

	cout << endl << "Best Evaluation = " << ZBest << endl;

	cout << "Cardinalities : " << endl;
	afficher(cardSol);

	cout << "Execution time : " << tm / 1000.0 << " sec" << endl;



	system("pause");
	return 0;
}