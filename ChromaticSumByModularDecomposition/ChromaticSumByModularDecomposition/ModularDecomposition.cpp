#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cmath>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <string>
#include <set>

#include "Module.h"
#include "ModularDecomposition.h"

using namespace std;

int nomModules;
unsigned int n, m, delta;

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
	//unsigned int somme[n];
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

void lectureDimacs0(unsigned int& n, unsigned int& m, unsigned int& delta, vector<vector<int> > & A)
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
	//unsigned int somme[n];
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
	/*
	cout << var << '\n' << contr << endl;

	cout << "A=\n" <<endl;

	for(unsigned int i=0; i<contr; i++)
	{
	for(unsigned int j=0; j<var; j++)
	{
	cout << A[i][j] << '\t';
	}
	cout << '\n';
	}
	*/
	unsigned int max_;
	//unsigned int somme[n];
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

void minVec(vector<int>& deg, int& min_, unsigned int& ind)
{
	min_ = deg[0]; ind = 0;
	for (int i = 1; i<deg.size(); i++)
	{
		if (deg[i] < min_)
		{
			min_ = deg[i];
			ind = i;
		}
	}
}

void maxVec(vector<int>& deg, unsigned int& max_, unsigned int& ind)
{
	max_ = deg[0]; ind = 0;
	for (int i = 1; i<deg.size(); i++)
	{
		if (deg[i] > max_)
		{
			max_ = deg[i];
			ind = i;
		}
	}
	max_++;
}

bool vgStable(map<int, vector<int> > graphqlq)
{
	//cout << "vgStable" << endl;
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
		//cout << "stable trouve\n Fin stable" << endl;
		return true;
	}
	else
	{
		//cout << "Pas stable trouve\n Fin stable" << endl;
		return false;
	}
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
	//cout << "détect composante connexe" << endl;
	vector<map<int, vector<int> > > ecc;
	map<int, vector<int> > g(g_);
	map<int, int> marquage;
	int pred, k = 0;
	vector<int> cc;
	map<int, vector<int> > grapheCC;
	/*cout << "g: " << endl;
	for(map<int,vector<int> >::iterator iter=g_.begin(); iter!=g_.end();++iter)
	{
	cout << "cle: " << iter->first << " => ";
	for(unsigned int m=0; m<iter->second.size(); m++)
	{
	cout << iter->second[m] << "  ";
	}
	cout << '\n';
	}*/
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
			//cout << "ici 0: " << it2->first << endl;
		}
	}
	if (!grapheCC.empty())
	{
		//ecc.push_back(grapheCC);
		/*
		for(map<int, vector<int> >::iterator iter = --(g_.end()); iter != g_.begin(); --iter)//supprimer les clés des cc dans g
		{
		for(map<int, vector<int> >::iterator iter2 = grapheCC.begin(); iter2 != grapheCC.end(); ++iter2)
		{
		if(iter->first == iter2->first)
		{
		g_.erase(iter);
		break;
		}
		}
		}
		for(map<int, vector<int> >::iterator iter2 = grapheCC.begin(); iter2 != grapheCC.end(); ++iter2)
		{
		if(g_.begin()->first == iter2->first)
		{
		g_.erase(g_.begin());
		break;
		}
		}*/
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
	/*cout << "g_ after first big update: " << endl;
	for(map<int,vector<int> >::iterator iter=g_.begin(); iter!=g_.end();++iter)
	{
	cout << "cle: " << iter->first << " => ";
	for(unsigned int m=0; m<iter->second.size(); m++)
	{
	cout << iter->second[m] << "  ";
	}
	cout << '\n';
	}*/
	map<int, vector<int> >::iterator it;
	if (!g_.empty())
	{
		it = g_.begin();
		//cout << "pointe sur: " << it->first << endl;
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
			//cout << "it->first pointé: " << it->first << endl;
			if (!it->second.empty()) // marquer tous les sommets
			{
				//cout << "ici1" << endl;
				marquage[(it->second)[0]] = it->first;// marqué le sécsesseur par son prédécéseuur
													  //cout << "ici2" << "   pred = " << pred << endl;
				pred = (it->second)[0];// présevé ce sucsséceur
									   //cout << "ici3" << "   pred = " << pred << endl;
				if (!containsValue(cc, it->first))
				{
					cc.push_back(it->first); //la composante prend ce prédécesseur
											 //cout << "IN CC : " << it->first << endl;
				}
				//cout << "ici4" << endl;
				for (map<int, vector<int> >::iterator it2 = g_.begin(); it2 != g_.end(); ++it2)
				{
					//cout << "ici5" << endl;
					if (it2->first == pred) // on cherche sur les clés à ce suc pour touvé ses suc aussi!
					{
						it = it2;// pointé sur la clé qui égal a ce suc
								 //cout << "pointe sur: " << it->first << endl;
					}
					//cout << "ici6" << endl;
					if (!it2->second.empty())
					{
						for (it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
						{
							if (*it3 == pred) //il faut qu'on march et on revient pas à ce suc=pred =(it->second)[0]
							{
								it2->second.erase(it3); // don on suprime leur apparense dans ses voisins
								break;
							}
						}

					}
					//8cout << "ici7" << endl;
				}
			}
			else //le it_>second.est vide
			{
				//cout << "ici8" << endl;
				pred = marquage[it->first]; // pred =le predécésseur de it empty
				if (!containsValue(cc, it->first))
				{
					cc.push_back(it->first); //la composante prend ce prédécesseur
											 //cout << "IN CC : " << it->first << endl;
				}
				//cout << "ici9" << endl;
				if (pred != -1) //la clé qui a un suc
				{
					for (map<int, vector<int> >::iterator it2 = g_.begin(); it2 != g_.end(); ++it2)// parcourir le g
					{
						//cout << "ici10" << endl;
						if (it2->first == pred)// cherché à ce pred dans  les clés de g pour remonté
						{
							it = it2; //it =la clé qui égale à pred
									  //cout << "pointe sur: " << it->first << endl;
							break;
						}
					}
				}
				else// je revenue au poit de départ
				{
					//cout << "ici11" << endl;
					for (vector<int>::iterator iter3 = cc.begin(); iter3 != cc.end(); ++iter3)
					{
						for (map<int, vector<int> >::iterator it2 = g.begin(); it2 != g.end(); ++it2)
						{
							//cout << "ici12" << endl;
							if (it2->first == *iter3)
							{
								grapheCC[*iter3] = it2->second;
								/*for(unsigned int j=0; j<it2->second.size(); j++)
								{
								cout << "ici12_2: " << it2->second[j] << endl;
								}
								cout << "ici12_1: " << *iter3 << endl;*/
								break;
							}
						}
					}
					//cout << "ici13" << endl;
					ecc.push_back(grapheCC);
					/*cout << "\n\ng_ before big update: " << endl;
					for(map<int,vector<int> >::iterator iter=g_.begin(); iter!=g_.end();++iter)
					{
					cout << "\n\ncle: " << iter->first << " => ";
					for(unsigned int m=0; m<iter->second.size(); m++)
					{
					cout << iter->second[m] << "  ";
					}
					cout << '\n';
					}*/
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
					}/*
					 for(map<int, vector<int> >::iterator iter2 = grapheCC.begin(); iter2 != grapheCC.end(); ++iter2)
					 {
					 if(g_.begin()->first == iter2->first)
					 {
					 g_.erase(g_.begin());
					 break;
					 }
					 }*/
					g_ = g2;

					g2.clear();
					/*cout << "g_ after big update: " << endl;
					for(map<int,vector<int> >::iterator iter=g_.begin(); iter!=g_.end();++iter)
					{
					cout << "cle: " << iter->first << " => ";
					for(unsigned int m=0; m<iter->second.size(); m++)
					{
					cout << iter->second[m] << "  ";
					}
					cout << '\n';
					}*/
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
				//cout << "\n\nici14" << endl;
			}
			//cout << "\n\nici15" << endl;
		}
		for (map<int, vector<int> >::iterator iter = g_.begin(); iter != g_.end(); ++iter)
		{
			cc.push_back(iter->first);
		}
		//cc.push_back(it->first);
		for (vector<int>::iterator iter3 = cc.begin(); iter3 != cc.end(); ++iter3)
		{
			for (map<int, vector<int> >::iterator it2 = g.begin(); it2 != g.end(); ++it2)
			{
				//cout << "ici16" << endl;
				if (it2->first == *iter3)
				{
					grapheCC[*iter3] = it2->second;
					//cout << "\n\nici16_1" << endl;
					break;
				}
			}
		}
		//cout << "\n\nici17" << endl;
		ecc.push_back(grapheCC);
	}
	/*
	for(vector<map<int, vector<int> > >::iterator iter = ecc.begin(); iter != ecc.end(); ++iter)
	{
	cout << "\n\nGraph\n";
	for(map<int, vector<int> >::iterator iter2 = (*iter).begin(); iter2 != (*iter).end(); ++iter2)
	{
	cout << "cle: " << iter2->first << "   => ";
	for(vector<int>::iterator iter3 = iter2->second.begin(); iter3 != iter2->second.end(); ++iter3)
	{
	cout << *iter3 << " / ";
	}
	cout << '\n';
	}
	}
	cout << "\n\nfin détect composante connexe" << endl;*/
	return ecc;
}

map<int, vector<int> > grapheInduit(map<int, vector<int> >& graphe, vector<int>& list)
{
	map<int, vector<int> > g;
	for (map<int, vector<int> >::iterator it = graphe.begin(); it != graphe.end(); it++)
	{
		for (int i = 0; i < list.size(); i++)
		{
			if (it->first == list[i])
			{
				g[it->first] = graphe[it->first];
				for (int k = g[it->first].size() - 1; k > -1; k--)
				{
					bool found = false;
					for (int j = 0; j < list.size(); j++)
					{
						if (g[it->first][k] == list[j])
						{
							found = true;
							break;
						}
					}
					if (!found)
					{
						/*cout << "effacer: " << k << endl;
						for (int l = 0; l < g[it->first].size(); l++)
						{
						cout << g[it->first][l] << " ";
						}
						cout << endl;*/
						g[it->first].erase(g[it->first].begin() + k);
					}
				}
				break;
			}
		}
	}
	return g;
}

bool comparerAdj(vector<int>& adj, vector<int>& pj)
{
	//cout << "begin chauvauge" << endl;
	//cout << "Intersection : "  ;
	vector<int> inter;
	for (int i = 0; i<adj.size(); i++)
	{
		//cout << "adj = " << adj[i] << endl;
		for (int j = 0; j<pj.size(); j++)
		{
			//cout << "pj = " << pj[j] << endl;
			if (adj[i] == pj[j])
			{
				inter.push_back(pj[j]);
				//cout << pj[j] << " ";
				break;
			}
		}
	}
	//cout << '\n';

	if (inter.size() == adj.size() && inter.size() == pj.size())
	{
		//cout << "égaux" << endl;
		return true;
	}
	//cout << "différents" << endl;
	return false;
}

bool comparerAdj2(const int a1, vector<int> adj, const int a2, vector<int> pj)
{
	//cout << "begin chauvauge" << endl;
	//cout << "Intersection : "  ;
	for (int i = 0; i<adj.size(); i++)
	{
		//cout << "pj = " << pj[j] << endl;
		if (adj[i] == a2)
		{
			adj.erase(adj.begin() + i);
			//cout << pj[j] << " ";
			break;
		}
	}
	for (int i = 0; i<pj.size(); i++)
	{
		//cout << "pj = " << pj[j] << endl;
		if (pj[i] == a1)
		{
			pj.erase(pj.begin() + i);
			//cout << pj[j] << " ";
			break;
		}
	}
	return comparerAdj(adj, pj);
}

bool contient(vector<int> sommets, vector<int>& trace, map<int, vector<int> >& moduleGarder)
{
	if (!moduleGarder.empty())
	{
	CONCAT:
		for (int i = 0; i < sommets.size(); i++)
		{
			for (map<int, vector<int> >::iterator it = moduleGarder.begin(); it != moduleGarder.end(); it++)
			{
				if (sommets[i] == it->first)
				{
					sommets.erase(sommets.begin() + i);
					for (int j = 0; j < it->second.size(); j++)
					{
						sommets.push_back(it->second[j]);
						goto CONCAT;
					}
				}
			}
		}
	}
	for (int i = 0; i < sommets.size(); i++)
	{
		bool trouver = false;
		for (int j = 0; j < trace.size(); j++)
		{
			if (sommets[i] == trace[j])
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

bool contientAuMoinsUnElmt(vector<int> sommets, vector<int>& trace, map<int, vector<int> >& moduleGarder)
{
	if (!moduleGarder.empty())
	{
	CONCAT:
		for (int i = 0; i < sommets.size(); i++)
		{
			for (map<int, vector<int> >::iterator it = moduleGarder.begin(); it != moduleGarder.end(); it++)
			{
				if (sommets[i] == it->first)
				{
					sommets.erase(sommets.begin() + i);
					for (int j = 0; j < it->second.size(); j++)
					{
						sommets.push_back(it->second[j]);
						goto CONCAT;
					}
				}
			}
		}
	}
	for (int i = 0; i < sommets.size(); i++)
	{
		for (int j = 0; j < trace.size(); j++)
		{
			if (sommets[i] == trace[j])
			{
				return true;
			}
		}
	}
	return false;
}

bool resteQuandMemePlusQueDeux(vector<int> sommets, vector<vector<int> >& trace, map<int, vector<int> >& moduleGarder, vector<vector<int> >& out)
{
	if (!moduleGarder.empty())
	{
	CONCAT:
		for (int i = 0; i < sommets.size(); i++)
		{
			for (map<int, vector<int> >::iterator it = moduleGarder.begin(); it != moduleGarder.end(); it++)
			{
				if (sommets[i] == it->first)
				{
					sommets.erase(sommets.begin() + i);
					for (int j = 0; j < it->second.size(); j++)
					{
						sommets.push_back(it->second[j]);
						//goto CONCAT;
					}
					goto CONCAT;
				}
			}
		}
	}
	//cout << "Sommet Plain : " << endl;
	//afficher(sommets);
	for (int i = 0; i < trace.size(); i++)
	{
		//cout << "i = " << i << endl;
		vector<int> inter;
		for (int j = 0; j < trace[i].size(); j++)
		{
			//cout << "j : " << j << endl;
			//cout << "trc = " << trace[i][j] << endl;
			for (int k = 0; k < sommets.size(); k++)
			{
				//cout << "Sommet[" << k << "] = " << sommets[k] << endl;
				if (sommets[k] == trace[i][j])
				{
					inter.push_back(sommets[k]);
					break;
				}
			}
		}
		if (!inter.empty())
		{
			out.push_back(inter);
		}
	}
	if (out.empty())
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool casseur(map<int, vector<int> >& g, vector<int>& v1, vector<int>& v2)
{
	for (int i = 0; i < v2.size(); i++)
	{
		bool exist = find(g[v1[0]].begin(), g[v1[0]].end(), v2[i]) == g[v1[0]].end();
		for (int j = 1; j < v1.size(); j++)
		{
			if (exist != (find(g[v1[j]].begin(), g[v1[j]].end(), v2[i]) == g[v1[j]].end()))
			{
				return true;
			}
		}
	}
	return false;
}

bool contient(vector<int>& out, vector<int>& in)
{
	for (int i = 0; i < out.size(); i++)
	{
		bool trouver = false;
		for (int j = 0; j < in.size(); j++)
		{
			if (out[i] == in[j])
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

void suppressionRestriction(vector<Module>& t, vector<vector<int> >& trace, vector<int>& deleted, vector<int>& supprimerNom)
{
	//cout << "Supression" << endl;
SUPPRESS:
	for (vector<Module>::iterator it = t.begin(); it != t.end(); it++)
	{
		//cout << "module: " << (*it).getNom() << endl;
		if ((*it).getBouchra() == -1)
		{
			//cout << "H1" << endl;
			vector<int> inter; int depart = (*it).getNom();
			recurseAdd(inter, t, depart);
			//cout << "inter" << endl;
			//afficher(inter);
			bool in = false;
			for (int i = 0; i < trace.size(); i++)
			{
				//cout << "actuel trace : " << endl;
				//afficher(trace[i]);
				if (contient(inter, trace[i]))
				{
					in = true;
					break;
				}
			}
			if (!in)
			{
				//cout << "H2" << endl;
				for (int i = 0; i < (*it).getSommets().size(); i++)
				{
					for (int j = 0; j < t.size(); j++)
					{
						if ((*it).getSommets()[i] == t[j].getNom())
						{
							t[j].setBouchra(-1);
							deleted.push_back(t[j].getNom());
							break;
						}
					}
				}
				supprimerNom.push_back((*it).getNom());
				t.erase(it);
				//cout << "t now : " << endl;
				//afficher(t);
				goto SUPPRESS;
			}
		}
	}
}

void grouper(vector<Module>& t, int& i, int& j, int& k, int& l, map<int, vector<int> >& g, vector<int>& inter, vector<int>& deleted)
{
	//cout << "Grouper" << endl;
	if (t[j].getDegenerate() == 1 && t[l].getDegenerate() == 1)
	{
		//cout << "G1" << endl;
		if (find(g[inter[0]].begin(), g[inter[0]].end(), inter[inter.size() - 1]) == g[inter[0]].end())
		{
			//cout << "G2" << endl;
			vector<int> s(t[j].getSommets());
			s.push_back(t[l].getNom());
			t[j].setSommets(s);
			t[l].setBouchra(t[j].getNom());
			deleted.erase(deleted.begin() + k);
		}
		else
		{
			//cout << "G3" << endl;
			nomModules++;
			t[j].setBouchra(nomModules);
			t[l].setBouchra(nomModules);
			vector<int> s;
			s.push_back(t[j].getNom());
			s.push_back(t[l].getNom());
			Module nvModule(nomModules, s, -1, 2, -1, true, false);
			t.push_back(nvModule);
			deleted[i] = nomModules;
			deleted.erase(deleted.begin() + k);
		}
	}
	else if (t[j].getDegenerate() == 2 && t[l].getDegenerate() == 2)
	{
		//cout << "G4" << endl;
		if (find(g[inter[0]].begin(), g[inter[0]].end(), inter[inter.size() - 1]) != g[inter[0]].end())
		{
			//cout << "G5" << endl;
			vector<int> s(t[j].getSommets());
			s.push_back(t[l].getNom());
			t[j].setSommets(s);
			t[l].setBouchra(t[j].getNom());
			deleted.erase(deleted.begin() + k);
		}
		else
		{
			//cout << "G6" << endl;
			nomModules++;
			t[j].setBouchra(nomModules);
			t[l].setBouchra(nomModules);
			vector<int> s;
			s.push_back(t[j].getNom());
			s.push_back(t[l].getNom());
			Module nvModule(nomModules, s, -1, 1, -1, true, false);
			t.push_back(nvModule);
			deleted[i] = nomModules;
			deleted.erase(deleted.begin() + k);
		}
	}
	else if (t[j].getDegenerate() == 1 && t[l].getDegenerate() == 2)
	{
		//cout << "G7" << endl;
		if (find(g[inter[0]].begin(), g[inter[0]].end(), inter[inter.size() - 1]) == g[inter[0]].end())
		{
			//cout << "G8" << endl;
			vector<int> s(t[j].getSommets());
			s.push_back(t[l].getNom());
			t[j].setSommets(s);
			t[l].setBouchra(t[j].getNom());
			deleted.erase(deleted.begin() + k);
		}
		else
		{
			//cout << "G9" << endl;
			vector<int> s(t[l].getSommets());
			s.push_back(t[j].getNom());
			t[l].setSommets(s);
			t[j].setBouchra(t[l].getNom());
			deleted.erase(deleted.begin() + i);
		}
	}
	else if (t[j].getDegenerate() == 2 && t[l].getDegenerate() == 1)
	{
		//cout << "G10" << endl;
		if (find(g[inter[0]].begin(), g[inter[0]].end(), inter[inter.size() - 1]) != g[inter[0]].end())
		{
			//cout << "G11" << endl;
			vector<int> s(t[j].getSommets());
			s.push_back(t[l].getNom());
			t[j].setSommets(s);
			t[l].setBouchra(t[j].getNom());
			deleted.erase(deleted.begin() + k);
		}
		else
		{
			//cout << "G12" << endl;
			vector<int> s(t[l].getSommets());
			s.push_back(t[j].getNom());
			t[l].setSommets(s);
			t[j].setBouchra(t[l].getNom());
			deleted.erase(deleted.begin() + i);
		}
	}
	else if (t[j].getDegenerate() == 1 && t[l].getDegenerate() == 0)
	{
		//cout << "G13" << endl;
		if (find(g[inter[0]].begin(), g[inter[0]].end(), inter[inter.size() - 1]) == g[inter[0]].end())
		{
			//cout << "G14" << endl;
			vector<int> s(t[j].getSommets());
			s.push_back(t[l].getNom());
			t[j].setSommets(s);
			t[l].setBouchra(t[j].getNom());
			deleted.erase(deleted.begin() + k);
		}
		else
		{
			//cout << "G15" << endl;
			nomModules++;
			t[j].setBouchra(nomModules);
			t[l].setBouchra(nomModules);
			vector<int> s;
			s.push_back(t[j].getNom());
			s.push_back(t[l].getNom());
			Module nvModule(nomModules, s, -1, 2, -1, true, false);
			t.push_back(nvModule);
			deleted[i] = nomModules;
			deleted.erase(deleted.begin() + k);
		}
	}
	else if (t[j].getDegenerate() == 0 && t[l].getDegenerate() == 1)
	{
		//cout << "G16" << endl;
		if (find(g[inter[0]].begin(), g[inter[0]].end(), inter[inter.size() - 1]) == g[inter[0]].end())
		{
			//cout << "G17" << endl;
			vector<int> s(t[l].getSommets());
			s.push_back(t[j].getNom());
			t[l].setSommets(s);
			t[j].setBouchra(t[j].getNom());
			deleted.erase(deleted.begin() + i);
		}
		else
		{
			//cout << "G18" << endl;
			nomModules++;
			t[j].setBouchra(nomModules);
			t[l].setBouchra(nomModules);
			vector<int> s;
			s.push_back(t[j].getNom());
			s.push_back(t[l].getNom());
			Module nvModule(nomModules, s, -1, 2, -1, true, false);
			t.push_back(nvModule);
			deleted[i] = nomModules;
			deleted.erase(deleted.begin() + k);
		}
	}
	else if (t[j].getDegenerate() == 2 && t[l].getDegenerate() == 0)
	{
		//cout << "G19" << endl;
		if (find(g[inter[0]].begin(), g[inter[0]].end(), inter[inter.size() - 1]) != g[inter[0]].end())
		{
			//cout << "G20" << endl;
			vector<int> s(t[j].getSommets());
			s.push_back(t[l].getNom());
			t[j].setSommets(s);
			t[l].setBouchra(t[j].getNom());
			deleted.erase(deleted.begin() + k);
		}
		else
		{
			//cout << "G21" << endl;
			nomModules++;
			t[j].setBouchra(nomModules);
			t[l].setBouchra(nomModules);
			vector<int> s;
			s.push_back(t[j].getNom());
			s.push_back(t[l].getNom());
			Module nvModule(nomModules, s, -1, 1, -1, true, false);
			t.push_back(nvModule);
			deleted[i] = nomModules;
			deleted.erase(deleted.begin() + k);
		}
	}
	else if (t[j].getDegenerate() == 0 && t[l].getDegenerate() == 2)
	{
		//cout << "G22" << endl;
		if (find(g[inter[0]].begin(), g[inter[0]].end(), inter[inter.size() - 1]) != g[inter[0]].end())
		{
			//cout << "G23" << endl;
			vector<int> s(t[l].getSommets());
			s.push_back(t[j].getNom());
			t[l].setSommets(s);
			t[j].setBouchra(t[j].getNom());
			deleted.erase(deleted.begin() + i);
		}
		else
		{
			//cout << "G24" << endl;
			nomModules++;
			t[j].setBouchra(nomModules);
			t[l].setBouchra(nomModules);
			vector<int> s;
			s.push_back(t[j].getNom());
			s.push_back(t[l].getNom());
			Module nvModule(nomModules, s, -1, 1, -1, true, false);
			t.push_back(nvModule);
			deleted[i] = nomModules;
			deleted.erase(deleted.begin() + k);
		}
	}
	else if (t[j].getDegenerate() == 0 && t[l].getDegenerate() == 0)
	{
		//cout << "G25" << endl;
		if (find(g[inter[0]].begin(), g[inter[0]].end(), inter[inter.size() - 1]) == g[inter[0]].end())
		{
			//cout << "G26" << endl;
			nomModules++;
			t[j].setBouchra(nomModules);
			t[l].setBouchra(nomModules);
			vector<int> s;
			s.push_back(t[j].getNom());
			s.push_back(t[l].getNom());
			Module nvModule(nomModules, s, -1, 1, -1, true, false);
			t.push_back(nvModule);
			deleted[i] = nomModules;
			deleted.erase(deleted.begin() + k);
		}
		else
		{
			//cout << "G27" << endl;
			nomModules++;
			t[j].setBouchra(nomModules);
			t[l].setBouchra(nomModules);
			vector<int> s;
			s.push_back(t[j].getNom());
			s.push_back(t[l].getNom());
			Module nvModule(nomModules, s, -1, 2, -1, true, false);
			t.push_back(nvModule);
			deleted[i] = nomModules;
			deleted.erase(deleted.begin() + k);
		}
	}
}

void groupementRestriction(vector<Module>& t, vector<vector<int> >& trace, vector<int>& deleted, map<int, vector<int> >& g)
{
	//cout << "Groupement" << endl;
GROUPEMENT:
	for (int i = 0; i < deleted.size(); i++)
	{
		for (int j = 0; j < t.size(); j++)
		{
			if (deleted[i] == t[j].getNom())
			{
				//cout << "Module : " << t[j].getNom() << endl;
				vector<int> inter; int depart = t[j].getNom();
				recurseAdd(inter, t, depart);
				//cout << "inter in :" << endl;
				//afficher(inter);
				for (int k = 0; k < deleted.size(); k++)
				{
					if (i != k)
					{
						for (int l = 0; l < t.size(); l++)
						{
							if (deleted[k] == t[l].getNom())
							{
								//cout << "second module : " << t[l].getNom() << endl;
								int depart2 = t[l].getNom();
								vector<int> inter2(inter);
								recurseAdd(inter2, t, depart2);
								//cout << "inter up : " << endl;
								//afficher(inter2);
								for (int s = 0; s < trace.size(); s++)
								{
									if (contient(inter2, trace[s]))
									{
										grouper(t, i, j, k, l, g, inter2, deleted);
										goto GROUPEMENT;
									}
								}
								break;
							}
						}
					}
				}
				break;
			}
		}
	}
}

void recursion(map<int, vector<int> >& g, vector<Module>& t, int& nivMax)
{
	cout << "\tRecursion" << endl;
	// On applique la récursion ici
	arborescence(g, t);
	/*int max = -1;
	for (int i = 0; i < t.size(); i++)
	{
	if (t[i].getNom() > max)
	{
	max = t[i].getNom();
	}
	}
	nomModules = max;
	nomProchainsModules = max;*/
	//cout << "List des modules ici : " << endl;
	//afficher(t);
}

void restriction(int& v0, vector<int>& nv0, vector<int>& nbv0, map<int, vector<int> >& g, vector<Module>& t1, vector<Module>& t2, int& nivMax1, int& nivMax2, vector<int>& supprimerNom)
{
	cout << "\tRestriction" << endl;
	map<int, vector<int> > mappingAdjNv0, mappingAdjNbv0;
	// Les adjacents de nv0 dans nbv0
	for (int i = 0; i < nv0.size(); i++)
	{
		vector<int> inter;
		for (int j = 0; j < g[nv0[i]].size(); j++)
		{
			for (int k = 0; k < nbv0.size(); k++)
			{
				if (g[nv0[i]][j] == nbv0[k])
				{
					inter.push_back(nbv0[k]);
					break;
				}
			}
		}
		mappingAdjNv0[nv0[i]] = inter;
	}
	//cout << "Adj N(v0) : " << endl;
	//afficher(mappingAdjNv0);
	// Les adjacents de nbv0 dans nv0
	for (int i = 0; i < nbv0.size(); i++)
	{
		vector<int> inter;
		for (int j = 0; j < g[nbv0[i]].size(); j++)
		{
			for (int k = 0; k < nv0.size(); k++)
			{
				if (g[nbv0[i]][j] == nv0[k])
				{
					inter.push_back(nv0[k]);
					break;
				}
			}
		}
		mappingAdjNbv0[nbv0[i]] = inter;
	}
	//cout << "Adj Nb(v0) : " << endl;
	//afficher(mappingAdjNbv0);
	// Detection des ressemblances des feuilles de t1 et t2
	// trace1 et trace2 contiennent la traçabilité des modules qu'on va contenir
	// Si un ensemble de sommets doivent se rassembler alors on les met ensemble, sinon on met chacun tout seul
	vector<vector<int> > trace1; vector<vector<int> > trace2;
	// Ici on marque les sommets qui appartiennent déjà à un ensemble, ce n'est pas la peine de les retraiter
	vector<int> dejaTraiter;
	// On commence ici à chercher les modules de G existant dans nv0
	for (map<int, vector<int> >::iterator it = mappingAdjNv0.begin(); it != mappingAdjNv0.end(); it++)
	{
		if (dejaTraiter.empty())
		{
			vector<int> inter;
			inter.push_back(it->first);

			for (map<int, vector<int> >::iterator it2 = it; it2 != mappingAdjNv0.end(); it2++)
			{
				if (it != it2 && comparerAdj(mappingAdjNv0[it->first], mappingAdjNv0[it2->first]))
				{
					inter.push_back(it2->first);
					dejaTraiter.push_back(it2->first);
				}
			}
			trace1.push_back(inter);
		}
		else if (find(dejaTraiter.begin(), dejaTraiter.end(), it->first) == dejaTraiter.end())
		{
			vector<int> inter;
			inter.push_back(it->first);

			for (map<int, vector<int> >::iterator it2 = it; it2 != mappingAdjNv0.end(); it2++)
			{
				if (it != it2 && comparerAdj(mappingAdjNv0[it->first], mappingAdjNv0[it2->first]))
				{
					inter.push_back(it2->first);
					dejaTraiter.push_back(it2->first);
				}
			}
			trace1.push_back(inter);
		}
	}
	//cout << "Trace1 ptimos: " << endl;
	//afficher(trace1);
	stack<vector<vector<int> >::iterator> toErase;
	vector<vector<int> > toAdd;
	bool backTrace(false);
BACK:
	for (vector<vector<int> >::iterator it = trace1.begin(); it != trace1.end(); it++)
	{
		if ((*it).size() != 1)
		{
			for (vector<vector<int> >::iterator it2 = trace1.begin(); it2 != trace1.end(); it2++)
			{
				if (it != it2)
				{
					if (casseur(g, *it, *it2))
					{
						vector<int> inter(*it);
						vector<vector<int> > dejaTraiter2;
						map<vector<int>, vector<int> > decomposition;
						toErase.push(it);
						for (int l = 0; l < inter.size(); l++)
						{
							vector<int> key;
							for (vector<vector<int> >::iterator it3 = trace1.begin(); it3 != trace1.end(); it3++)
							{
								if (it != it3)
								{
									for (vector<int>::iterator it4 = (*it3).begin(); it4 != (*it3).end(); it4++)
									{
										if (find(g[inter[l]].begin(), g[inter[l]].end(), *it4) != g[inter[l]].end())
										{
											key.push_back(*it4);
										}
									}
								}
							}
							sort(key.begin(), key.end());
							if (dejaTraiter2.empty() || find(dejaTraiter2.begin(), dejaTraiter2.end(), key) == dejaTraiter2.end())
							{
								vector<int> in;
								in.push_back(inter[l]);
								decomposition[key] = in;
								dejaTraiter2.push_back(key);
							}
							else if (find(dejaTraiter2.begin(), dejaTraiter2.end(), key) != dejaTraiter2.end())
							{
								vector<int> in(decomposition[key]);
								in.push_back(inter[l]);
								decomposition[key] = in;
							}
						}
						for (map<vector<int>, vector<int> >::iterator keys = decomposition.begin(); keys != decomposition.end(); keys++)
						{
							toAdd.push_back(keys->second);
						}
						break;
					}
				}
			}
		}
	}
	while (!toErase.empty())
	{
		trace1.erase(toErase.top());
		toErase.pop();
	}
	if (!toAdd.empty())
	{
		backTrace = true;
	}
	while (!toAdd.empty())
	{
		trace1.push_back(toAdd[0]);
		toAdd.erase(toAdd.begin());
	}
	if (backTrace)
	{
		backTrace = false;
		goto BACK;
	}
	//cout << "Trace1 : " << endl;
	//afficher(trace1);
	// On commence ici à chercher les modules de G existant dans nbv0
	for (map<int, vector<int> >::iterator it = mappingAdjNbv0.begin(); it != mappingAdjNbv0.end(); it++)
	{
		if (dejaTraiter.empty())
		{
			vector<int> inter;
			inter.push_back(it->first);

			for (map<int, vector<int> >::iterator it2 = it; it2 != mappingAdjNbv0.end(); it2++)
			{
				if (it != it2 && comparerAdj(mappingAdjNbv0[it->first], mappingAdjNbv0[it2->first]))
				{
					inter.push_back(it2->first);
					dejaTraiter.push_back(it2->first);
				}
			}
			trace2.push_back(inter);
		}
		else if (find(dejaTraiter.begin(), dejaTraiter.end(), it->first) == dejaTraiter.end())
		{
			vector<int> inter;
			inter.push_back(it->first);

			for (map<int, vector<int> >::iterator it2 = it; it2 != mappingAdjNbv0.end(); it2++)
			{
				if (it != it2 && comparerAdj(mappingAdjNbv0[it->first], mappingAdjNbv0[it2->first]))
				{
					inter.push_back(it2->first);
					dejaTraiter.push_back(it2->first);
				}
			}
			trace2.push_back(inter);
		}
	}
	//cout << "Trace2 primos : " << endl;
	//afficher(trace2);
	bool backTrace2(false);
BACK2:
	for (vector<vector<int> >::iterator it = trace2.begin(); it != trace2.end(); it++)
	{
		//cout << "premier trace : " << endl;
		//afficher(*it);
		if ((*it).size() != 1)
		{
			for (vector<vector<int> >::iterator it2 = trace2.begin(); it2 != trace2.end(); it2++)
			{
				//cout << "deuxieme trace : " << endl;
				//afficher(*it2);
				if (it != it2)
				{
					if (casseur(g, *it, *it2))
					{
						//cout << "casser" << endl;
						vector<int> inter(*it);
						vector<vector<int> > dejaTraiter2;
						map<vector<int>, vector<int> > decomposition;
						toErase.push(it);
						for (int l = 0; l < inter.size(); l++)
						{
							//cout << "inter[" << l << "]= " << inter[l] << endl;
							vector<int> key;
							for (vector<vector<int> >::iterator it3 = trace2.begin(); it3 != trace2.end(); it3++)
							{
								//cout << "troisieme trace : " << endl;
								//afficher(*it3);
								if (it != it3)
								{
									for (vector<int>::iterator it4 = (*it3).begin(); it4 != (*it3).end(); it4++)
									{
										//cout << "quatrieme trace : " << *it4 << endl;
										if (find(g[inter[l]].begin(), g[inter[l]].end(), *it4) != g[inter[l]].end())
										{
											key.push_back(*it4);
										}
									}
								}
							}
							sort(key.begin(), key.end());
							//cout << "key : " << endl;
							//afficher(key);
							if (dejaTraiter2.empty() || find(dejaTraiter2.begin(), dejaTraiter2.end(), key) == dejaTraiter2.end())
							{
								vector<int> in;
								in.push_back(inter[l]);
								decomposition[key] = in;
								dejaTraiter2.push_back(key);
								//cout << "\tin: " << endl;
								//afficher(in);
							}
							else if (find(dejaTraiter2.begin(), dejaTraiter2.end(), key) != dejaTraiter2.end())
							{
								vector<int> in(decomposition[key]);
								in.push_back(inter[l]);
								decomposition[key] = in;
								//cout << "\tin: " << endl;
								//afficher(in);
							}
						}
						for (map<vector<int>, vector<int> >::iterator keys = decomposition.begin(); keys != decomposition.end(); keys++)
						{
							toAdd.push_back(keys->second);
							//cout << "toAdd here : " << endl;
							//afficher(keys->second);
						}
						break;
					}
				}
			}
		}
	}
	//cout << "toErase : " << endl;
	while (!toErase.empty())
	{
		//afficher(*toErase.top());
		trace2.erase(toErase.top());
		toErase.pop();
	}
	if (!toAdd.empty())
	{
		backTrace2 = true;
	}
	while (!toAdd.empty())
	{
		trace2.push_back(toAdd[0]);
		toAdd.erase(toAdd.begin());
	}
	if (backTrace2)
	{
		backTrace2 = false;
		goto BACK2;
	}
	/*cout << "Trace2 : " << endl;
	afficher(trace2);
	cout << "Nom Modules now : " << nomModules << endl;*/
	char c;
	//cin >> c;
	// Supprimer les noeuds de t1 qui ne sont pas des modules de G en comparant les listes d'adjacences des sommets de chaque
	// module (on compareceux qui sont incidents aux actives edges)
	vector<int> deleted1, deleted2;
	suppressionRestriction(t1, trace1, deleted1, supprimerNom);
	//cout << "deleted 1 : " << endl;
	//afficher(deleted1);
	suppressionRestriction(t2, trace2, deleted2, supprimerNom);
	//cout << "deleted 2 : " << endl;
	//afficher(deleted2);
	groupementRestriction(t1, trace1, deleted1, g);
	//cout << "deleted 1 : " << endl;
	//afficher(deleted1);
	groupementRestriction(t2, trace2, deleted2, g);
	//cout << "deleted 2 : " << endl;
	//afficher(deleted2);
	/*map<int, vector<int> > moduleGarder, adj; vector<int> moduleSupprimer; vector<vector<int> > dejaAjoutee;
	for (int i = nivMax1 - 1; i > -1; i--)
	{
	cout << "niv a traiter : " << i << endl;
	for (int j = 0; j < t1.size(); j++)
	{
	cout << "indice module t1 : " << j << endl;
	if (t1[j].getNiveau() == i && t1[j].getSommets().size() > 1)
	{
	cout << "B1" << endl;
	for (int k = 0; k < trace1.size(); k++)
	{
	cout << "element trace 1 : " << k << endl;
	afficher(trace1[k]);
	if (trace1[k].size() > 1)
	{
	cout << "B2" << endl;
	if (contient(t1[j].getSommets(), trace1[k], moduleGarder))
	{
	cout << "B3" << endl;
	moduleGarder[t1[j].getNom()] = t1[j].getSommets();
	vector<int> inter(t1[j].getSommets());
	sort(inter.begin(), inter.end());
	dejaAjoutee.push_back(inter);
	adj[t1[j].getNom()] = mappingAdjNbv0[t1[j].getSommets()[0]];
	cout << "Module garder: " << endl;
	afficher(moduleGarder);
	break;
	}
	else if (contientAuMoinsUnElmt(t1[j].getSommets(), trace1[k], moduleGarder))
	{
	cout << "B4-1" << endl;
	// Tester si le module a plusieurs sommets de même adjacence donc il reste et on supprime juste les enfants
	vector<vector<int> > out;
	if (t1[j].getSommets().size() > 2 && resteQuandMemePlusQueDeux(t1[j].getSommets(), trace1, moduleGarder, out))
	{
	cout << "B4-2" << endl;
	for (int s = 0; s < out.size(); s++)
	{
	if (out[s].size() > 1)
	{
	cout << "B4-3" << endl;
	sort(out[s].begin(), out[s].end());
	if (dejaAjoutee.empty() || find(dejaAjoutee.begin(), dejaAjoutee.end(), out[s]) == dejaAjoutee.end())
	{
	bool trouver(false);
	for (int w = 0; w < t1.size(); w++)
	{
	vector<int> in;
	for (int j = 0; j < t1[w].getSommets().size(); j++)
	{
	recurseAdd(in, t1, t1[w].getSommets()[j]);
	}
	sort(in.begin(), in.end());
	if (contient(out[s], in))
	{
	for (int j = t1[w].getSommets().size() - 1; j > -1; j--)
	{
	vector<int> in2;
	recurseAdd(in2, t1, t1[w].getSommets()[j]);
	if (!contient(in2, out[s]))
	{
	vector<int> ss(t1[w].getSommets());
	ss.erase(ss.begin() + j);
	t1[w].setSommets(ss);
	break;
	}
	}
	trouver = true;
	}
	}
	if (!trouver)
	{
	cout << "B4-4" << endl;
	nomModules++;
	cout << "nvModulle : " << nomModules << endl;
	dejaAjoutee.push_back(out[s]);
	Module nvModule(nomModules, out[s], -1, t1[j].getDegenerate(), -1, true, false);
	t1.push_back(nvModule);
	for (int h = 0; h < out[s].size(); h++)
	{
	for (int v = 0; v < t1.size(); v++)
	{
	if (t1[v].getNom() == out[s][h])
	{
	cout << "change origine : " << t1[v].getNom() << " : " << t1[v].getBouchra() << " => " << nomModules << endl;
	t1[v].setBouchra(nomModules);
	break;
	}
	}
	}
	}
	}
	}
	}
	if (j != t1.size() - 1)
	{
	t1.erase(t1.begin() + j);
	j--;
	}
	else
	{
	t1.erase(t1.begin() + j);
	}
	/*for (int s = t1[j].getSommets().size() - 1; s > -1; s--)
	{
	cout << "s = " << s << endl;
	for (int t = 0; t < t1.size(); t++)
	{
	cout << "t = " << t << endl;
	if (t1[t].getNom() == t1[j].getSommets()[s])
	{
	cout << "B4-3" << endl;
	if (!contient(t1[t].getSommets(), trace1[posTrace], moduleGarder))
	{
	cout << "B4-4" << endl;
	vector<int> inter(t1[j].getSommets());
	inter.erase(inter.begin() + s);
	t1[j].setSommets(inter);
	t1[t].setBouchra(-1);
	}
	break;
	}
	}
	}
	moduleGarder[t1[j].getNom()] = t1[j].getSommets();
	adj[t1[j].getNom()] = mappingAdjNbv0[t1[j].getSommets()[0]];*/
	/*}
	else //  sinon on le supprime directement
	{
	if (t1[j].getBouchra() == -1)
	{
	supprimerNom.push_back(t1[j].getNom());
	for (int l = 0; l < t1[j].getSommets().size(); l++)
	{
	for (int s = 0; s < t1.size(); s++)
	{
	if (t1[s].getNom() == t1[j].getSommets()[l])
	{
	t1[s].setBouchra(-1);
	break;
	}
	}
	}
	t1.erase(t1.begin() + j);
	}
	else
	{
	cout << "B4-5" << endl;
	int p = j;
	cout << "p = " << p << endl;
	SUPPRESS1:
	for (int l = 0; l < t1.size(); l++)
	{
	cout << "element t1 in l : " << l << endl;
	cout << "nom module : " << t1[l].getNom() << endl;
	cout << "list in : " << endl;
	afficher(t1[l].getSommets());
	bool found = false, parent = false;
	if (t1[l].getSommets().size() > 1 && l != p)
	{
	cout << "B5" << endl;
	for (int s = 0; s < t1[l].getSommets().size(); s++)
	{
	cout << "element de trace 2 de l in : " << t1[l].getSommets()[s] << endl;
	if (t1[l].getSommets()[s] == t1[p].getNom())
	{
	cout << "B6" << endl;
	if (t1[l].getSommets().size() <= 2)
	{
	cout << "B7" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), l) == moduleSupprimer.end())
	{
	cout << "B8" << endl;
	moduleSupprimer.push_back(l);
	}
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B9" << endl;
	moduleSupprimer.push_back(p);
	}
	if (t1[l].getBouchra() != -1)
	{
	cout << "B10" << endl;
	p = l;
	parent = true;
	}
	cout << "Module supprimer : " << endl;
	afficher(moduleSupprimer);
	}
	else
	{
	cout << "B11" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B12" << endl;
	moduleSupprimer.push_back(p);
	cout << "Module supprimer: " << endl;
	afficher(moduleSupprimer);
	}
	}
	found = true;
	if (parent)
	{
	cout << "B13" << endl;
	goto SUPPRESS1;
	}
	sort(moduleSupprimer.begin(), moduleSupprimer.end());
	for (int h = moduleSupprimer.size() - 1; h > -1; h--)
	{
	cout << "indice module supprimer : " << h << endl;
	supprimerNom.push_back(t1[moduleSupprimer[h]].getNom());
	for (int l = 0; l < t1[moduleSupprimer[h]].getSommets().size(); l++)
	{
	for (int s = 0; s < t1.size(); s++)
	{
	if (t1[s].getNom() == t1[moduleSupprimer[h]].getSommets()[l])
	{
	t1[s].setBouchra(-1);
	break;
	}
	}
	}
	t1.erase(t1.begin() + moduleSupprimer[h]);
	}
	moduleSupprimer.clear();
	break;
	}
	}
	}
	if (found)
	{
	cout << "B14" << endl;
	break;
	}
	}
	}
	}
	break;
	}
	else if (k == trace1.size() - 1)
	{
	cout << "B26" << endl;
	int p = j;
	SUPPRESS6:
	for (int l = 0; l < t1.size(); l++)
	{
	cout << "ce l de trace 2 : " << l << endl;
	bool found = false, parent = false;
	if (t1[l].getSommets().size() > 1 && l != p)
	{
	cout << "B27" << endl;
	for (int s = 0; s<t1[l].getSommets().size(); s++)
	{
	cout << "element de l de trace 2 in : " << t1[l].getSommets()[s] << endl;
	if (t1[l].getSommets()[s] == t1[p].getNom())
	{
	cout << "B28" << endl;
	if (t1[l].getSommets().size() <= 2)
	{
	cout << "B29" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), l) == moduleSupprimer.end())
	{
	cout << "B30" << endl;
	moduleSupprimer.push_back(l);
	}
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B31" << endl;
	moduleSupprimer.push_back(p);
	}
	if (t1[l].getBouchra() != -1)
	{
	cout << "B32" << endl;
	p = l;
	parent = true;
	}
	cout << "Module supprimer: " << endl;
	afficher(moduleSupprimer);
	}
	else
	{
	cout << "B33" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B34" << endl;
	moduleSupprimer.push_back(p);
	cout << "Module supprimer: " << endl;
	afficher(moduleSupprimer);
	}
	}
	found = true;
	if (parent)
	{
	cout << "B35" << endl;
	goto SUPPRESS6;
	}
	sort(moduleSupprimer.begin(), moduleSupprimer.end());
	for (int h = moduleSupprimer.size() - 1; h > -1; h--)
	{
	cout << "module supprimer h in : " << h << endl;
	supprimerNom.push_back(t1[moduleSupprimer[h]].getNom());
	for (int l = 0; l < t1[moduleSupprimer[h]].getSommets().size(); l++)
	{
	for (int s = 0; s < t1.size(); s++)
	{
	if (t1[s].getNom() == t1[moduleSupprimer[h]].getSommets()[l])
	{
	t1[s].setBouchra(-1);
	break;
	}
	}
	}
	t1.erase(t1.begin() + moduleSupprimer[h]);
	}
	break;
	}
	}
	}
	if (found)
	{
	cout << "B36" << endl;
	break;
	}
	}
	}
	}
	else if (k == trace1.size() - 1)
	{
	cout << "B15" << endl;
	int p = j;
	SUPPRESS2:
	for (int l = 0; l < t1.size(); l++)
	{
	cout << "ce l de trace 2 : " << l << endl;
	bool found = false, parent = false;
	if (t1[l].getSommets().size() > 1 && l != p)
	{
	cout << "B16" << endl;
	for (int s = 0; s<t1[l].getSommets().size(); s++)
	{
	cout << "element de l de trace 2 in : " << t1[l].getSommets()[s] << endl;
	if (t1[l].getSommets()[s] == t1[p].getNom())
	{
	cout << "B17" << endl;
	if (t1[l].getSommets().size() <= 2)
	{
	cout << "B18" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), l) == moduleSupprimer.end())
	{
	cout << "B19" << endl;
	moduleSupprimer.push_back(l);
	}
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B20" << endl;
	moduleSupprimer.push_back(p);
	}
	if (t1[l].getBouchra() != -1)
	{
	cout << "B21" << endl;
	p = l;
	parent = true;
	}
	cout << "Module supprimer: " << endl;
	afficher(moduleSupprimer);
	}
	else
	{
	cout << "B22" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B23" << endl;
	moduleSupprimer.push_back(p);
	cout << "Module supprimer: " << endl;
	afficher(moduleSupprimer);
	}
	}
	found = true;
	if (parent)
	{
	cout << "B24" << endl;
	goto SUPPRESS2;
	}
	sort(moduleSupprimer.begin(), moduleSupprimer.end());
	for (int h = moduleSupprimer.size() - 1; h > -1; h--)
	{
	cout << "module supprimer h in : " << h << endl;
	supprimerNom.push_back(t1[moduleSupprimer[h]].getNom());
	for (int l = 0; l < t1[moduleSupprimer[h]].getSommets().size(); l++)
	{
	for (int s = 0; s < t1.size(); s++)
	{
	if (t1[s].getNom() == t1[moduleSupprimer[h]].getSommets()[l])
	{
	t1[s].setBouchra(-1);
	break;
	}
	}
	}
	t1.erase(t1.begin() + moduleSupprimer[h]);
	}
	break;
	}
	}
	}
	if (found)
	{
	cout << "B25" << endl;
	break;
	}
	}
	}
	}
	}
	}
	}
	// Supprimer les noeuds de t2 qui ne sont pas des modules de G de la même manière que t1
	moduleGarder.clear(); moduleSupprimer.clear();
	for (int i = nivMax2 - 1; i > -1; i--)
	{
	cout << "niv a traiter : " << i << endl;
	for (int j = 0; j < t2.size(); j++)
	{
	cout << "indice module t2 : " << j << " , nom : " << t2[j].getNom() << endl;
	if (t2[j].getNiveau() == i && t2[j].getSommets().size() > 1)
	{
	cout << "B1" << endl;
	for (int k = 0; k < trace2.size(); k++)
	{
	cout << "element trace 2 : " << k << endl;
	afficher(trace2[k]);
	if (trace2[k].size() > 1)
	{
	cout << "B2" << endl;
	if (contient(t2[j].getSommets(), trace2[k], moduleGarder))
	{
	cout << "B3" << endl;
	moduleGarder[t2[j].getNom()] = t2[j].getSommets();
	vector<int> inter(t2[j].getSommets());
	sort(inter.begin(), inter.end());
	dejaAjoutee.push_back(inter);
	adj[t2[j].getNom()] = mappingAdjNbv0[t2[j].getSommets()[0]];
	cout << "Module garder: " << endl;
	afficher(moduleGarder);
	break;
	}
	else if (contientAuMoinsUnElmt(t2[j].getSommets(), trace2[k], moduleGarder))
	{
	cout << "B4-1" << endl;
	// Tester si le module a plusieurs sommets de même adjacence donc il reste et on supprime juste les enfants
	vector<vector<int> > out;
	if (t2[j].getSommets().size() > 2 && resteQuandMemePlusQueDeux(t2[j].getSommets(), trace2, moduleGarder, out))
	{
	cout << "B4-2\nout:" << endl;
	afficher(out);
	moduleSupprimer.push_back(t2[j].getNom());
	for (int s = 0; s < out.size(); s++)
	{
	if (out[s].size() > 1)
	{
	cout << "B4-3" << endl;
	sort(out[s].begin(), out[s].end());
	if (dejaAjoutee.empty() || find(dejaAjoutee.begin(), dejaAjoutee.end(), out[s]) == dejaAjoutee.end())
	{
	cout << "B4-3-1" << endl;
	bool trouver(false);
	for (int w = 0; w < t2.size(); w++)
	{
	vector<int> in;
	for (int j = 0; j < t2[w].getSommets().size(); j++)
	{
	recurseAdd(in, t2, t2[w].getSommets()[j]);
	}
	sort(in.begin(), in.end());
	cout << "in : " << endl;
	afficher(in);
	if (contient(out[s], in))
	{
	cout << "B4-3-2" << endl;
	for (int j = t2[w].getSommets().size() - 1; j > -1; j--)
	{
	vector<int> in2;
	recurseAdd(in2, t2, t2[w].getSommets()[j]);
	cout << "in2 : " << endl;
	afficher(in2);
	if (!contient(in2, out[s]))
	{
	cout << "B4-3-3" << endl;
	vector<int> ss(t2[w].getSommets());
	ss.erase(ss.begin() + j);
	t2[w].setSommets(ss);
	break;
	}
	}
	trouver = true;
	}
	}
	if (!trouver)
	{
	cout << "B4-4" << endl;
	nomModules++;
	cout << "nvModulle : " << nomModules << endl;
	dejaAjoutee.push_back(out[s]);
	Module nvModule(nomModules, out[s], -1, t2[j].getDegenerate(), -1, true, false);
	t2.push_back(nvModule);
	for (int h = 0; h < out[s].size(); h++)
	{
	for (int v = 0; v < t2.size(); v++)
	{
	if (t2[v].getNom() == out[s][h])
	{
	cout << "change origine : " << t2[v].getNom() << " : " << t2[v].getBouchra() << " => " << nomModules << endl;
	t2[v].setBouchra(nomModules);
	break;
	}
	}
	}
	}
	}
	}
	}
	if (j != t2.size() - 1)
	{
	t2.erase(t2.begin() + j);
	j--;
	}
	else
	{
	t2.erase(t2.begin() + j);
	}
	/*for (int s = t2[j].getSommets().size() - 1; s > -1; s--)
	{
	cout << "s = " << s << " => " << t2[j].getSommets()[s] << endl;
	for (int t = 0; t < t2.size(); t++)
	{
	cout << "t = " << t << endl;
	if (t2[t].getNom() == t2[j].getSommets()[s])
	{
	cout << "B4-3" << endl;
	cout << "t2 de t : " << endl;
	afficher(t2[t].getSommets());
	cout << "trace2 de postrace : " << endl;
	afficher(trace2[posTrace]);
	cout << "moduleGarder : " << endl;
	afficher(moduleGarder);
	if (!contient(t2[t].getSommets(), trace2[posTrace], moduleGarder))
	{
	cout << "B4-4" << endl;
	vector<int> inter(t2[j].getSommets());
	inter.erase(inter.begin() + s);
	t2[j].setSommets(inter);
	t2[t].setBouchra(-1);
	cout << "t2 now : " << endl;
	afficher(t2);
	}
	break;
	}
	}
	}
	moduleGarder[t2[j].getNom()] = t2[j].getSommets();
	adj[t2[j].getNom()] = mappingAdjNbv0[t2[j].getSommets()[0]];*/
	/*}
	else //  sinon on le supprime directement
	{
	if (t2[j].getBouchra() == -1)
	{
	supprimerNom.push_back(t2[j].getNom());
	for (int l = 0; l < t2[j].getSommets().size(); l++)
	{
	for (int s = 0; s < t2.size(); s++)
	{
	if (t2[s].getNom() == t2[j].getSommets()[l])
	{
	t2[s].setBouchra(-1);
	break;
	}
	}
	}
	t2.erase(t2.begin() + j);
	}
	else
	{
	cout << "B4-5" << endl;
	int p = j;
	cout << "p = " << p << endl;
	SUPPRESS3:
	for (int l = 0; l < t2.size(); l++)
	{
	cout << "element t2 in l : " << l << endl;
	cout << "nom module : " << t2[l].getNom() << endl;
	cout << "list in : " << endl;
	afficher(t2[l].getSommets());
	bool found = false, parent = false;
	if (t2[l].getSommets().size() > 1 && l != p)
	{
	cout << "B5" << endl;
	for (int s = 0; s < t2[l].getSommets().size(); s++)
	{
	cout << "element de trace 2 de l in : " << t2[l].getSommets()[s] << endl;
	if (t2[l].getSommets()[s] == t2[p].getNom())
	{
	cout << "B6" << endl;
	if (t2[l].getSommets().size() <= 2)
	{
	cout << "B7" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), l) == moduleSupprimer.end())
	{
	cout << "B8" << endl;
	moduleSupprimer.push_back(l);
	}
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B9" << endl;
	moduleSupprimer.push_back(p);
	}
	if (t2[l].getBouchra() != -1)
	{
	cout << "B10" << endl;
	p = l;
	parent = true;
	}
	cout << "Module supprimer : " << endl;
	afficher(moduleSupprimer);
	}
	else
	{
	cout << "B11" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B12" << endl;
	moduleSupprimer.push_back(p);
	cout << "Module supprimer: " << endl;
	afficher(moduleSupprimer);
	}
	}
	found = true;
	if (parent)
	{
	cout << "B13" << endl;
	goto SUPPRESS3;
	}
	sort(moduleSupprimer.begin(), moduleSupprimer.end());
	for (int h = moduleSupprimer.size() - 1; h > -1; h--)
	{
	cout << "indice module supprimer : " << h << endl;
	supprimerNom.push_back(t2[moduleSupprimer[h]].getNom());
	for (int l = 0; l < t2[moduleSupprimer[h]].getSommets().size(); l++)
	{
	for (int s = 0; s < t2.size(); s++)
	{
	if (t2[s].getNom() == t2[moduleSupprimer[h]].getSommets()[l])
	{
	t2[s].setBouchra(-1);
	break;
	}
	}
	}
	t2.erase(t2.begin() + moduleSupprimer[h]);
	}
	moduleSupprimer.clear();
	break;
	}
	}
	}
	if (found)
	{
	cout << "B14" << endl;
	break;
	}
	}
	}
	}
	break;
	}
	else if (k == trace2.size() - 1)
	{
	cout << "B26" << endl;
	int p = j;
	SUPPRESS5:
	for (int l = 0; l < t2.size(); l++)
	{
	cout << "ce l de trace 2 : " << l << endl;
	bool found = false, parent = false;
	if (t2[l].getSommets().size() > 1 && l != p)
	{
	cout << "B27" << endl;
	for (int s = 0; s<t2[l].getSommets().size(); s++)
	{
	cout << "element de l de trace 2 in : " << t2[l].getSommets()[s] << endl;
	if (t2[l].getSommets()[s] == t2[p].getNom())
	{
	cout << "B28" << endl;
	if (t2[l].getSommets().size() <= 2)
	{
	cout << "B29" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), l) == moduleSupprimer.end())
	{
	cout << "B30" << endl;
	moduleSupprimer.push_back(l);
	}
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B31" << endl;
	moduleSupprimer.push_back(p);
	}
	if (t2[l].getBouchra() != -1)
	{
	cout << "B32" << endl;
	p = l;
	parent = true;
	}
	cout << "Module supprimer: " << endl;
	afficher(moduleSupprimer);
	}
	else
	{
	cout << "B33" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B34" << endl;
	moduleSupprimer.push_back(p);
	cout << "Module supprimer: " << endl;
	afficher(moduleSupprimer);
	}
	}
	found = true;
	if (parent)
	{
	cout << "B35" << endl;
	goto SUPPRESS5;
	}
	sort(moduleSupprimer.begin(), moduleSupprimer.end());
	for (int h = moduleSupprimer.size() - 1; h > -1; h--)
	{
	cout << "module supprimer h in : " << h << endl;
	supprimerNom.push_back(t2[moduleSupprimer[h]].getNom());
	for (int l = 0; l < t2[moduleSupprimer[h]].getSommets().size(); l++)
	{
	for (int s = 0; s < t2.size(); s++)
	{
	if (t2[s].getNom() == t2[moduleSupprimer[h]].getSommets()[l])
	{
	t2[s].setBouchra(-1);
	break;
	}
	}
	}
	t2.erase(t2.begin() + moduleSupprimer[h]);
	}
	break;
	}
	}
	}
	if (found)
	{
	cout << "B36" << endl;
	break;
	}
	}
	}
	}
	else if (k == trace2.size() - 1)
	{
	cout << "B15" << endl;
	int p = j;
	SUPPRESS4:
	for (int l = 0; l < t2.size(); l++)
	{
	cout << "ce l de trace 2 : " << l << endl;
	bool found = false, parent = false;
	if (t2[l].getSommets().size() > 1 && l != p)
	{
	cout << "B16" << endl;
	for (int s = 0; s<t2[l].getSommets().size(); s++)
	{
	cout << "element de l de trace 2 in : " << t2[l].getSommets()[s] << endl;
	if (t2[l].getSommets()[s] == t2[p].getNom())
	{
	cout << "B17" << endl;
	if (t2[l].getSommets().size() <= 2)
	{
	cout << "B18" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), l) == moduleSupprimer.end())
	{
	cout << "B19" << endl;
	moduleSupprimer.push_back(l);
	}
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B20" << endl;
	moduleSupprimer.push_back(p);
	}
	if (t2[l].getBouchra() != -1)
	{
	cout << "B21" << endl;
	p = l;
	parent = true;
	}
	cout << "Module supprimer: " << endl;
	afficher(moduleSupprimer);
	}
	else
	{
	cout << "B22" << endl;
	if (find(moduleSupprimer.begin(), moduleSupprimer.end(), p) == moduleSupprimer.end())
	{
	cout << "B23" << endl;
	moduleSupprimer.push_back(p);
	cout << "Module supprimer: " << endl;
	afficher(moduleSupprimer);
	}
	}
	found = true;
	if (parent)
	{
	cout << "B24" << endl;
	goto SUPPRESS4;
	}
	sort(moduleSupprimer.begin(), moduleSupprimer.end());
	for (int h = moduleSupprimer.size() - 1; h > -1; h--)
	{
	cout << "module supprimer h in : " << h << endl;
	supprimerNom.push_back(t2[moduleSupprimer[h]].getNom());
	for (int l = 0; l < t2[moduleSupprimer[h]].getSommets().size(); l++)
	{
	for (int s = 0; s < t2.size(); s++)
	{
	if (t2[s].getNom() == t2[moduleSupprimer[h]].getSommets()[l])
	{
	t2[s].setBouchra(-1);
	break;
	}
	}
	}
	t2.erase(t2.begin() + moduleSupprimer[h]);
	}
	break;
	}
	}
	}
	if (found)
	{
	cout << "B25" << endl;
	break;
	}
	}
	}
	}
	}
	}
	}*/
	cout << "T1 after suppress : " << endl;
	afficher(t1);
	cout << "T2 after suppress : " << endl;
	afficher(t2);
	/*for (int i = 0; i < t1.size(); i++)
	{
	bool trouver = false;
	for (int j = 0; j < t1.size(); j++)
	{
	if (t1[i].getBouchra() == t1[j].getNom())
	{
	trouver = true;
	break;
	}
	}
	if (!trouver)
	{
	for (int j = 0; j < t2.size(); j++)
	{
	if (t2[j].getNom() == t1[i].getBouchra())
	{
	trouver = true;
	break;
	}
	}
	if (!trouver)
	{
	t1[i].setBouchra(-1);
	}
	}
	}
	for (int i = 0; i < t2.size(); i++)
	{
	bool trouver = false;
	for (int j = 0; j < t2.size(); j++)
	{
	if (t2[i].getBouchra() == t2[j].getNom())
	{
	trouver = true;
	break;
	}
	}
	if (!trouver)
	{
	for (int j = 0; j < t1.size(); j++)
	{
	if (t1[j].getNom() == t2[i].getBouchra())
	{
	trouver = true;
	break;
	}
	}
	if (!trouver)
	{
	t2[i].setBouchra(-1);
	}
	}
	}*/
	// Rassembler les noeuds de t1 qui ont des ressemblances et faire attention aux noeuds qui ont la même dégénérescence
	/*for (int i = 0; i < t1.size(); i++)
	{
	if (t1[i].getSommets().size() > 1)
	{
	for (int j = 0; j < t1.size(); j++)
	{
	if (t1[j].getSommets().size() > 1 && i != j)
	{
	if (adj[t1[i].getNom()] == adj[t1[j].getNom()])
	{
	if (t1[i].getDegenerate() == 1 && t1[j].getDegenerate() == 1)
	{
	if (find(g[t1[j].getSommets()[0]].begin(), g[t1[j].getSommets()[0]].end(), t1[i].getSommets()[0]) == g[t1[j].getSommets()[0]].end())
	{
	if (t1[i].getNom() < t1[j].getNom())
	{
	for (int k = 0; k < t1[j].getSommets().size(); k++)
	{
	t1[i].getSommets().push_back(t1[j].getSommets()[k]);
	for (int l = 0; l < t1.size(); l++)
	{
	if (t1[j].getSommets()[k] == t1[l].getNom())
	{
	t1[l].setBouchra(t1[i].getNom());
	break;
	}
	}
	}
	supprimerNom.push_back(t1[j].getNom());
	t1.erase(t1.begin() + j);
	}
	else
	{
	for (int k = 0; k < t1[i].getSommets().size(); k++)
	{
	t1[j].getSommets().push_back(t1[i].getSommets()[k]);
	for (int l = 0; l < t1.size(); l++)
	{
	if (t1[i].getSommets()[k] == t1[l].getNom())
	{
	t1[l].setBouchra(t1[j].getNom());
	break;
	}
	}
	}
	supprimerNom.push_back(t1[i].getNom());
	t1.erase(t1.begin() + i);
	}
	}
	else
	{
	int nom;
	if (supprimerNom.empty())
	{
	nomModules++;
	nom = nomModules;
	}
	else
	{
	sort(supprimerNom.begin(), supprimerNom.end());
	nom = supprimerNom[0];
	supprimerNom.erase(supprimerNom.begin());
	}
	vector<int> li;
	li.push_back(t1[i].getNom());
	li.push_back(t1[j].getNom());
	Module nvModule(nom, li, -1, 2, -1, true, false);
	t1.push_back(nvModule);
	}
	}
	else if(t1[i].getDegenerate() == 2 && t1[j].getDegenerate() == 2)
	{
	if (find(g[t1[j].getSommets()[0]].begin(), g[t1[j].getSommets()[0]].end(), t1[i].getSommets()[0]) == g[t1[j].getSommets()[0]].end())
	{
	int nom;
	if (supprimerNom.empty())
	{
	nomModules++;
	nom = nomModules;
	}
	else
	{
	sort(supprimerNom.begin(), supprimerNom.end());
	nom = supprimerNom[0];
	supprimerNom.erase(supprimerNom.begin());
	}
	vector<int> li;
	li.push_back(t1[i].getNom());
	li.push_back(t1[j].getNom());
	Module nvModule(nom, li, -1, 1, -1, true, false);
	t1.push_back(nvModule);
	}
	else
	{
	if (t1[i].getNom() < t1[j].getNom())
	{
	for (int k = 0; k < t1[j].getSommets().size(); k++)
	{
	t1[i].getSommets().push_back(t1[j].getSommets()[k]);
	for (int l = 0; l < t1.size(); l++)
	{
	if (t1[j].getSommets()[k] == t1[l].getNom())
	{
	t1[l].setBouchra(t1[i].getNom());
	break;
	}
	}
	}
	supprimerNom.push_back(t1[j].getNom());
	t1.erase(t1.begin() + j);
	}
	else
	{
	for (int k = 0; k < t1[i].getSommets().size(); k++)
	{
	t1[j].getSommets().push_back(t1[i].getSommets()[k]);
	for (int l = 0; l < t1.size(); l++)
	{
	if (t1[i].getSommets()[k] == t1[l].getNom())
	{
	t1[l].setBouchra(t1[j].getNom());
	break;
	}
	}
	}
	supprimerNom.push_back(t1[i].getNom());
	t1.erase(t1.begin() + i);
	}
	}
	}
	else if((t1[i].getDegenerate() == 2 && t1[j].getDegenerate() == 1) || (t1[i].getDegenerate() == 1 && t1[j].getDegenerate() == 2))
	{
	if (find(g[t1[j].getSommets()[0]].begin(), g[t1[j].getSommets()[0]].end(), t1[i].getSommets()[0]) == g[t1[j].getSommets()[0]].end())
	{
	if (t1[i].getDegenerate() == 1)
	{
	t1[i].getSommets().push_back(t1[j].getNom());
	t1[j].setBouchra(t1[i].getNom());
	}
	else
	{
	t1[j].getSommets().push_back(t1[i].getNom());
	t1[i].setBouchra(t1[j].getNom());
	}
	}
	else
	{
	if (t1[i].getDegenerate() == 2)
	{
	t1[i].getSommets().push_back(t1[j].getNom());
	t1[j].setBouchra(t1[i].getNom());
	}
	else
	{
	t1[j].getSommets().push_back(t1[i].getNom());
	t1[i].setBouchra(t1[j].getNom());
	}
	}
	}
	else if ((t1[i].getDegenerate() == 0 && t1[j].getDegenerate() == 1) || (t1[i].getDegenerate() == 1 && t1[j].getDegenerate() == 0))
	{

	}
	else if ((t1[i].getDegenerate() == 0 && t1[j].getDegenerate() == 2) || (t1[i].getDegenerate() == 2 && t1[j].getDegenerate() == 0))
	{

	}
	else if (t1[i].getDegenerate() == 0 && t1[j].getDegenerate() == 0)
	{

	}
	}
	}
	}
	}
	}*/
	// Rassembler les noeuds de t2 qui ont des ressemblances et faire attention aux noeuds qui ont la même dégénérescence
	/*for (int i = 0; i < t2.size(); i++)
	{
	if (t2[i].getSommets().size() > 1)
	{
	for (int j = 0; j < t2.size(); j++)
	{
	if (t2[j].getSommets().size() > 1 && i != j)
	{
	if (adj[t2[i].getNom()] == adj[t2[j].getNom()])
	{
	if (t2[i].getDegenerate() == 1 && t2[j].getDegenerate() == 1)
	{
	if (find(g[t2[j].getSommets()[0]].begin(), g[t2[j].getSommets()[0]].end(), t2[i].getSommets()[0]) == g[t2[j].getSommets()[0]].end())
	{
	if (t2[i].getNom() < t2[j].getNom())
	{
	for (int k = 0; k < t2[j].getSommets().size(); k++)
	{
	t2[i].getSommets().push_back(t2[j].getSommets()[k]);
	for (int l = 0; l < t2.size(); l++)
	{
	if (t2[j].getSommets()[k] == t2[l].getNom())
	{
	t2[l].setBouchra(t2[i].getNom());
	break;
	}
	}
	}
	supprimerNom.push_back(t2[j].getNom());
	t2.erase(t2.begin() + j);
	}
	else
	{
	for (int k = 0; k < t2[i].getSommets().size(); k++)
	{
	t2[j].getSommets().push_back(t2[i].getSommets()[k]);
	for (int l = 0; l < t2.size(); l++)
	{
	if (t2[i].getSommets()[k] == t2[l].getNom())
	{
	t2[l].setBouchra(t2[j].getNom());
	break;
	}
	}
	}
	supprimerNom.push_back(t2[i].getNom());
	t2.erase(t2.begin() + i);
	}
	}
	else
	{
	int nom;
	if (supprimerNom.empty())
	{
	nomModules++;
	nom = nomModules;
	}
	else
	{
	sort(supprimerNom.begin(), supprimerNom.end());
	nom = supprimerNom[0];
	supprimerNom.erase(supprimerNom.begin());
	}
	vector<int> li;
	li.push_back(t2[i].getNom());
	li.push_back(t2[j].getNom());
	Module nvModule(nom, li, -1, 2, -1, true, false);
	t2.push_back(nvModule);
	}
	}
	else if (t2[i].getDegenerate() == 2 && t2[j].getDegenerate() == 2)
	{
	if (find(g[t2[j].getSommets()[0]].begin(), g[t2[j].getSommets()[0]].end(), t2[i].getSommets()[0]) == g[t2[j].getSommets()[0]].end())
	{
	int nom;
	if (supprimerNom.empty())
	{
	nomModules++;
	nom = nomModules;
	}
	else
	{
	sort(supprimerNom.begin(), supprimerNom.end());
	nom = supprimerNom[0];
	supprimerNom.erase(supprimerNom.begin());
	}
	vector<int> li;
	li.push_back(t2[i].getNom());
	li.push_back(t2[j].getNom());
	Module nvModule(nom, li, -1, 1, -1, true, false);
	t2.push_back(nvModule);
	}
	else
	{
	if (t2[i].getNom() < t2[j].getNom())
	{
	for (int k = 0; k < t2[j].getSommets().size(); k++)
	{
	t2[i].getSommets().push_back(t2[j].getSommets()[k]);
	for (int l = 0; l < t2.size(); l++)
	{
	if (t2[j].getSommets()[k] == t2[l].getNom())
	{
	t2[l].setBouchra(t2[i].getNom());
	break;
	}
	}
	}
	supprimerNom.push_back(t2[j].getNom());
	t2.erase(t2.begin() + j);
	}
	else
	{
	for (int k = 0; k < t2[i].getSommets().size(); k++)
	{
	t2[j].getSommets().push_back(t2[i].getSommets()[k]);
	for (int l = 0; l < t2.size(); l++)
	{
	if (t2[i].getSommets()[k] == t2[l].getNom())
	{
	t2[l].setBouchra(t2[j].getNom());
	break;
	}
	}
	}
	supprimerNom.push_back(t2[i].getNom());
	t2.erase(t2.begin() + i);
	}
	}
	}
	else if ((t2[i].getDegenerate() == 2 && t2[j].getDegenerate() == 1) || (t2[i].getDegenerate() == 1 && t2[j].getDegenerate() == 2))
	{
	if (find(g[t2[j].getSommets()[0]].begin(), g[t2[j].getSommets()[0]].end(), t2[i].getSommets()[0]) == g[t2[j].getSommets()[0]].end())
	{
	if (t2[i].getDegenerate() == 1)
	{
	t2[i].getSommets().push_back(t2[j].getNom());
	t2[j].setBouchra(t2[i].getNom());
	}
	else
	{
	t2[j].getSommets().push_back(t2[i].getNom());
	t2[i].setBouchra(t2[j].getNom());
	}
	}
	else
	{
	if (t2[i].getDegenerate() == 2)
	{
	t2[i].getSommets().push_back(t2[j].getNom());
	t2[j].setBouchra(t2[i].getNom());
	}
	else
	{
	t2[j].getSommets().push_back(t2[i].getNom());
	t2[i].setBouchra(t2[j].getNom());
	}
	}
	}
	else if ((t2[i].getDegenerate() == 0 && t2[j].getDegenerate() == 1) || (t2[i].getDegenerate() == 1 && t2[j].getDegenerate() == 0))
	{

	}
	else if ((t2[i].getDegenerate() == 0 && t2[j].getDegenerate() == 2) || (t2[i].getDegenerate() == 2 && t2[j].getDegenerate() == 0))
	{

	}
	else if (t2[i].getDegenerate() == 0 && t2[j].getDegenerate() == 0)
	{

	}
	}
	}
	}
	}
	}*/
	/*cout << "T1 after rassemble : " << endl;
	afficher(t1);
	cout << "T2 after rassemble : " << endl;
	afficher(t2);*/
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

void calculCFC(vector<vector<int> >& CFC, map<int, vector<int> >& g, map<int, vector<int> > forcingG)
{
	vector<int> positive, negative, posTest, negTest;
	while (!forcingG.empty())
	{
		vector<int> cc;
		// Marquage initial
		positive.push_back(forcingG.begin()->first);
		negative.push_back(forcingG.begin()->first);
		// Marquage positive
		int ind = forcingG.begin()->first;
	POSITIVE:
		for (vector<int>::iterator it = forcingG[ind].begin(); it != forcingG[ind].end(); it++)
		{
			if (find(positive.begin(), positive.end(), *it) == positive.end())
			{
				positive.push_back(*it);
				posTest.push_back(*it);
			}
		}
		if (!posTest.empty())
		{
			ind = posTest[posTest.size() - 1];
			posTest.pop_back();
			goto POSITIVE;
		}
		// Marquage négative
		ind = forcingG.begin()->first;
	NEGATIVE:
		for (map<int, vector<int> >::iterator it = forcingG.begin(); it != forcingG.end(); it++)
		{
			if (find(it->second.begin(), it->second.end(), ind) != it->second.end() && find(negative.begin(), negative.end(), it->first) == negative.end())
			{
				negative.push_back(it->first);
				negTest.push_back(it->first);
			}
		}
		if (!negTest.empty())
		{
			ind = negTest[negTest.size() - 1];
			negTest.pop_back();
			goto NEGATIVE;
		}
		// Detecting CFC
		for (int i = 0; i < positive.size(); i++)
		{
			for (int j = 0; j < negative.size(); j++)
			{
				if (positive[i] == negative[j])
				{
					cc.push_back(positive[i]);
					for (map<int, vector<int> >::iterator it = forcingG.begin(); it != forcingG.end(); it++)
					{
						for (vector<int>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
						{
							if (*it2 == positive[i])
							{
								it->second.erase(it2);
								break;
							}
						}
					}
					for (map<int, vector<int> >::iterator it = forcingG.begin(); it != forcingG.end(); it++)
					{
						if (it->first == positive[i])
						{
							forcingG.erase(it);
							break;
						}
					}
					break;
				}
			}
			positive.clear();
			negative.clear();
		}
		CFC.push_back(cc);
		/*cout << "forcing Grph : " << endl;
		afficher(forcingG);
		cout << "CFC de i : " << endl;
		afficher(CFC);*/
	}
}

bool contient(vector<Module>& t, int& nom)
{
	for (int i = 0; i < t.size(); i++)
	{
		if (t[i].getNom() == nom)
		{
			return true;
		}
	}
	return false;
}

void renumeroter(vector<Module>& t, vector<int>& supprimerNom)
{
	if (!supprimerNom.empty())
	{
		sort(supprimerNom.begin(), supprimerNom.end());
		int max = -1, ind;
		while (true)
		{
			for (int i = 0; i < t.size(); i++)
			{
				if (t[i].getNom() > max)
				{
					max = t[i].getNom();
					ind = i;
				}
			}
			if (max < supprimerNom[0])
			{
				return;
			}
			else if (contient(t, supprimerNom[0]))
			{
				supprimerNom.erase(supprimerNom.begin());
				if (supprimerNom.empty())
				{
					return;
				}
			}
			else
			{
				for (int i = 0; i < t[ind].getSommets().size(); i++)
				{
					for (int j = 0; j < t.size(); j++)
					{
						if (t[j].getNom() == t[ind].getSommets()[i])
						{
							t[j].setBouchra(supprimerNom[0]);
							break;
						}
					}
				}
				for (int i = 0; i < t.size(); i++)
				{
					if (t[i].getNom() == t[ind].getBouchra())
					{
						for (int j = 0; j < t[i].getSommets().size(); j++)
						{
							if (t[i].getSommets()[j] == max)
							{
								vector<int> inter(t[i].getSommets());
								inter[j] = supprimerNom[0];
								t[i].setSommets(inter);
								break;
							}
						}
						break;
					}
				}
				t[ind].setNom(supprimerNom[0]);
				supprimerNom.erase(supprimerNom.begin());
				if (supprimerNom.empty())
				{
					return;
				}
				else
				{
					max = -1;
				}
			}
		}
	}
}

void v0Module(int& v0, vector<int>& nv0, vector<int>& nbv0, map<int, vector<int> >& g, map<int, vector<int> >& gbar, vector<Module>& t1, vector<Module>& t2, vector<Module>& t, vector<int>& supprimerNom)
{
	cout << "\tV0Module" << endl;
	vector<vector<int> > B, Bprime; vector<int> isoles;
	for (int i = 0; i < t1.size(); i++)
	{
		if (t1[i].getBouchra() == -1 && t1[i].getSommets().size() > 1)
		{
			vector<int> inter;
			for (int j = 0; j < t1[i].getSommets().size(); j++)
			{
				recurseAdd(inter, t1, t1[i].getSommets()[j]);
			}
			B.push_back(inter);
		}
		else if (t1[i].getBouchra() == -1 && t1[i].getSommets().size() == 1)
		{
			isoles.push_back(t1[i].getNom());
		}
	}
	if (!isoles.empty())
	{
		map<int, vector<int> > isolesBar(grapheInduit(gbar, isoles));
		vector<map<int, vector<int> > > CCGB = dCConnexe(isolesBar);
		for (int i = 0; i < CCGB.size(); i++)
		{
			vector<int> inter;
			for (map<int, vector<int> >::iterator it = CCGB[i].begin(); it != CCGB[i].end(); it++)
			{
				inter.push_back(it->first);
			}
			B.push_back(inter);
		}
	}
	for (int i = 0; i < t2.size(); i++)
	{
		if (t2[i].getBouchra() == -1)
		{
			vector<int> inter;
			for (int j = 0; j < t2[i].getSommets().size(); j++)
			{
				recurseAdd(inter, t2, t2[i].getSommets()[j]);
			}
			Bprime.push_back(inter);
		}
	}
	cout << "B : " << endl;
	afficher(B);
	cout << "Bprime : " << endl;
	afficher(Bprime);
	map<int, vector<int> > forcingGraph;
	for (int i = 0; i < B.size(); i++)
	{
		vector<int> inter;
		for (int j = 0; j < Bprime.size(); j++)
		{
			for (int k = 0; k < B[i].size(); k++)
			{
				bool found = false;
				for (int l = 0; l < Bprime[j].size(); l++)
				{
					for (vector<int>::iterator it = g[B[i][k]].begin(); it != g[B[i][k]].end(); it++)
					{
						if (*it == Bprime[j][l])
						{
							inter.push_back(j + B.size());
							found = true;
							break;
						}
					}
					if (found)
					{
						break;
					}
				}
				if (found)
				{
					break;
				}
			}
		}
		forcingGraph[i] = inter;
	}
	for (int i = 0; i < Bprime.size(); i++)
	{
		vector<int> inter;
		for (int j = 0; j < B.size(); j++)
		{
			for (int k = 0; k < Bprime[i].size(); k++)
			{
				bool found = false;
				for (int l = 0; l < B[j].size(); l++)
				{
					for (vector<int>::iterator it = gbar[Bprime[i][k]].begin(); it != gbar[Bprime[i][k]].end(); it++)
					{
						if (*it == B[j][l])
						{
							inter.push_back(j);
							found = true;
							break;
						}
					}
					if (found)
					{
						break;
					}
				}
				if (found)
				{
					break;
				}
			}
		}
		forcingGraph[i + B.size()] = inter;
	}
	cout << "Forcing Graph : " << endl;
	afficher(forcingGraph);
	vector<vector<int> > CFC; vector<bool> outDegCFC;
	calculCFC(CFC, g, forcingGraph);
	cout << "CFC : " << endl;
	afficher(CFC);
	if (CFC.size() == 1)
	{
		vector<int> inter; inter.push_back(v0);
		Module nvModule(v0, inter, -1, 0, -1, true, true);
		t1.push_back(nvModule);
		return;
	}
	for (int i = 0; i < CFC.size(); i++)
	{
		bool found = false;
		for (int j = 0; j < CFC[i].size(); j++)
		{
			for (int k = 0; k < CFC.size(); k++)
			{
				if (i != k)
				{
					for (int l = 0; l < CFC[k].size(); l++)
					{
						if (find(forcingGraph[CFC[i][j]].begin(), forcingGraph[CFC[i][j]].end(), CFC[k][l]) != forcingGraph[CFC[i][j]].end())
						{
							outDegCFC.push_back(true);
							found = true;
							break;
						}
					}
				}
				if (found)
				{
					break;
				}
			}
			if (found)
			{
				break;
			}
		}
		if (!found)
		{
			outDegCFC.push_back(false);
		}
	}
	/*cout << "Forcing Graph : " << endl;
	afficher(forcingGraph);
	cout << "outDegCFC : " << endl;
	for (int i = 0; i < outDegCFC.size(); i++)
	{
	cout << outDegCFC[i] << endl;
	}*/
	bool unionModules = false;
	for (int i = 0; i < outDegCFC.size(); i++)
	{
		if (!outDegCFC[i])
		{
			vector<int> adjV0(g[v0]), listSommets, adjList;
			for (int j = 0; j < CFC[i].size(); j++)
			{
				if (CFC[i][j] < B.size())
				{
					for (int k = 0; k < B[CFC[i][j]].size(); k++)
					{
						listSommets.push_back(B[CFC[i][j]][k]);
					}
				}
				else
				{
					for (int k = 0; k < Bprime[CFC[i][j] - B.size()].size(); k++)
					{
						listSommets.push_back(Bprime[CFC[i][j] - B.size()][k]);
					}
				}
			}
			cout << "retour list sommets : " << endl;
			afficher(listSommets);
			cout << "adjV0 : " << endl;
			afficher(adjV0);
			for (vector<int>::iterator it = listSommets.begin(); it != listSommets.end(); it++)
			{
				for (vector<int>::iterator it2 = adjV0.begin(); it2 != adjV0.end(); it2++)
				{
					if (*it == *it2)
					{
						adjV0.erase(it2);
						break;
					}
				}
			}
			cout << "Nouveau adjV0 sans list sommets : " << endl;
			afficher(adjV0);
			for (vector<int>::iterator it = listSommets.begin(); it != listSommets.end(); it++)
			{
				for (vector<int>::iterator it2 = g[*it].begin(); it2 != g[*it].end(); it2++)
				{
					if (*it2 != v0 && find(listSommets.begin(), listSommets.end(), *it2) == listSommets.end())
					{
						if (find(adjList.begin(), adjList.end(), *it2) == adjList.end())
						{
							adjList.push_back(*it2);
						}
					}
				}
			}
			cout << "list adj des sommets : " << endl;
			afficher(adjList);
			if (adjList.size() == adjV0.size())
			{
				bool found;
				for (int j = 0; j < adjList.size(); j++)
				{
					found = false;
					for (int k = 0; k < adjV0.size(); k++)
					{
						if (adjList[j] == adjV0[k])
						{
							found = true;
							break;
						}
					}
					if (!found)
					{
						break;
					}
				}
				if (found)
				{
					bool found2 = false;
					unionModules = true;
					for (vector<Module>::iterator it2 = t1.begin(); it2 != t1.end(); it2++)
					{
						if (listSommets[0] == (*it2).getNom())
						{
							if ((*it2).getBouchra() == -1)
							{
								// Si c'est un sommet isolé, alors on formme le nouveau module
								if (find(g[v0].begin(), g[v0].end(), listSommets[0]) == g[v0].end())
								{
									int nom;
									if (supprimerNom.empty())
									{
										nomModules++;
										nom = nomModules;
									}
									else
									{
										sort(supprimerNom.begin(), supprimerNom.end());
										nom = supprimerNom[0];
										supprimerNom.erase(supprimerNom.begin());
									}
									listSommets.push_back(v0);
									for (int k = 0; k < listSommets.size(); k++)
									{
										for (int l = 0; l < t1.size(); l++)
										{
											if (listSommets[k] == t1[l].getNom())
											{
												t1[l].setBouchra(nom);
												break;
											}
										}
									}
									// On construit le nouveau module
									Module nvModule(nom, listSommets, -1, 1, -1, true, false);
									// On l'ajoute à la liste des modules
									t1.push_back(nvModule);
									vector<int> inter2; inter2.push_back(v0);
									Module nvModule2(v0, inter2, nom, 0, -1, true, true);
									t1.push_back(nvModule2);
									goto RASSEMBLE;
								}
								else
								{
									int nom;
									if (supprimerNom.empty())
									{
										nomModules++;
										nom = nomModules;
									}
									else
									{
										sort(supprimerNom.begin(), supprimerNom.end());
										nom = supprimerNom[0];
										supprimerNom.erase(supprimerNom.begin());
									}
									listSommets.push_back(v0);
									for (int k = 0; k < listSommets.size(); k++)
									{
										for (int l = 0; l < t1.size(); l++)
										{
											if (listSommets[k] == t1[l].getNom())
											{
												t1[l].setBouchra(nom);
												break;
											}
										}
									}
									// On construit le nouveau module
									Module nvModule(nom, listSommets, -1, 2, -1, false, false);
									// On l'ajoute à la liste des modules
									t1.push_back(nvModule);
									vector<int> inter2; inter2.push_back(v0);
									Module nvModule2(v0, inter2, nom, 0, -1, true, true);
									t1.push_back(nvModule2);
									goto RASSEMBLE;
								}
							}
							else
							{
								int parent = (*it2).getBouchra();
								while (parent != -1)
								{
									for (vector<Module>::iterator it = t1.begin(); it != t1.end(); it++)
									{
										if (parent == (*it).getNom())
										{
											if ((*it).getBouchra() == -1)
											{
												if (find(g[v0].begin(), g[v0].end(), listSommets[0]) == g[v0].end())
												{
													if ((*it).getDegenerate() == 1)
													{
														vector<int> inte((*it).getSommets());
														inte.push_back(v0);
														(*it).setSommets(inte);
														vector<int> inter; inter.push_back(v0);
														Module nvModule(v0, inter, (*it).getNom(), 0, -1, true, true);
														t1.push_back(nvModule);
														goto RASSEMBLE;
													}
													else
													{
														vector<int> inter;
														inter.push_back(v0);
														inter.push_back((*it).getNom());
														int nom;
														if (supprimerNom.empty())
														{
															nomModules++;
															nom = nomModules;
														}
														else
														{
															sort(supprimerNom.begin(), supprimerNom.end());
															nom = supprimerNom[0];
															supprimerNom.erase(supprimerNom.begin());
														}
														(*it).setBouchra(nom);
														Module nvModule(nom, inter, -1, 1, -1, false, false);
														t1.push_back(nvModule);
														vector<int> inter2; inter2.push_back(v0);
														Module nvModule2(v0, inter2, nom, 0, -1, true, true);
														t1.push_back(nvModule2);
														goto RASSEMBLE;
													}
												}
												else
												{
													if ((*it).getDegenerate() == 2)
													{
														vector<int> inte((*it).getSommets());
														inte.push_back(v0);
														(*it).setSommets(inte);
														vector<int> inter; inter.push_back(v0);
														Module nvModule(v0, inter, (*it).getNom(), 0, -1, true, true);
														t1.push_back(nvModule);
														goto RASSEMBLE;
													}
													else
													{
														vector<int> inter;
														inter.push_back(v0);
														inter.push_back((*it).getNom());
														int nom;
														if (supprimerNom.empty())
														{
															nomModules++;
															nom = nomModules;
														}
														else
														{
															sort(supprimerNom.begin(), supprimerNom.end());
															nom = supprimerNom[0];
															supprimerNom.erase(supprimerNom.begin());
														}
														(*it).setBouchra(nom);
														Module nvModule(nom, inter, -1, 2, -1, false, false);
														t1.push_back(nvModule);
														vector<int> inter2; inter2.push_back(v0);
														Module nvModule2(v0, inter2, nom, 0, -1, true, true);
														t1.push_back(nvModule2);
														goto RASSEMBLE;
													}
												}
											}
											parent = (*it).getBouchra();
											break;
										}
									}
								}
							}
							found2 = true;
						}
					}
					if (!found2)
					{
						for (vector<Module>::iterator it2 = t2.begin(); it2 != t2.end(); it2++)
						{
							//cout << "soomet : " << (*it2).getNom() << endl;
							if (listSommets[0] == (*it2).getNom())
							{
								//cout << "C1" << endl;
								if ((*it2).getBouchra() == -1)
								{
									//cout << "C2" << endl;
									// Si c'est un sommet isolé, alors on formme le nouveau module
									if (find(g[v0].begin(), g[v0].end(), listSommets[0]) == g[v0].end())
									{
										//cout << "C3" << endl;
										int nom;
										if (supprimerNom.empty())
										{
											nomModules++;
											nom = nomModules;
										}
										else
										{
											sort(supprimerNom.begin(), supprimerNom.end());
											nom = supprimerNom[0];
											supprimerNom.erase(supprimerNom.begin());
										}
										listSommets.push_back(v0);
										for (int k = 0; k < listSommets.size(); k++)
										{
											for (int l = 0; l < t2.size(); l++)
											{
												if (listSommets[k] == t2[l].getNom())
												{
													t2[l].setBouchra(nom);
													break;
												}
											}
										}
										// On construit le nouveau module
										Module nvModule(nom, listSommets, -1, 1, -1, false, false);
										// On l'ajoute à la liste des modules
										t2.push_back(nvModule);
										vector<int> inter2; inter2.push_back(v0);
										Module nvModule2(v0, inter2, nom, 0, -1, true, true);
										t2.push_back(nvModule2);
										goto RASSEMBLE;
									}
									else
									{
										//cout << "C4" << endl;
										int nom;
										if (supprimerNom.empty())
										{
											nomModules++;
											nom = nomModules;
										}
										else
										{
											sort(supprimerNom.begin(), supprimerNom.end());
											nom = supprimerNom[0];
											supprimerNom.erase(supprimerNom.begin());
										}
										listSommets.push_back(v0);
										for (int k = 0; k < listSommets.size(); k++)
										{
											for (int l = 0; l < t2.size(); l++)
											{
												if (listSommets[k] == t2[l].getNom())
												{
													t2[l].setBouchra(nom);
													break;
												}
											}
										}
										// On construit le nouveau module
										Module nvModule(nom, listSommets, -1, 2, -1, false, false);
										// On l'ajoute à la liste des modules
										t2.push_back(nvModule);
										vector<int> inter2; inter2.push_back(v0);
										Module nvModule2(v0, inter2, nom, 0, -1, true, true);
										t2.push_back(nvModule2);
										goto RASSEMBLE;
									}
								}
								else
								{
									//cout << "C5" << endl;
									int parent = (*it2).getBouchra();
									//cout << "parent = " << parent << endl;
									while (parent != -1)
									{
										for (vector<Module>::iterator it = t2.begin(); it != t2.end(); it++)
										{
											if (parent == (*it).getNom())
											{
												//cout << "C6" << endl;
												if ((*it).getBouchra() == -1)
												{
													//cout << "C7" << endl;
													if (find(g[v0].begin(), g[v0].end(), listSommets[0]) == g[v0].end())
													{
														//cout << "C8" << endl;
														if ((*it).getDegenerate() == 1)
														{
															//cout << "C9" << endl;
															vector<int> inte((*it).getSommets());
															inte.push_back(v0);
															(*it).setSommets(inte);
															vector<int> inter; inter.push_back(v0);
															Module nvModule(v0, inter, (*it).getNom(), 0, -1, true, true);
															t2.push_back(nvModule);
															//cout << "t2 here : " << endl;
															//afficher(t2);
															goto RASSEMBLE;
														}
														else
														{
															//cout << "C10" << endl;
															vector<int> inter;
															inter.push_back(v0);
															inter.push_back((*it).getNom());
															int nom;
															if (supprimerNom.empty())
															{
																nomModules++;
																nom = nomModules;
															}
															else
															{
																sort(supprimerNom.begin(), supprimerNom.end());
																nom = supprimerNom[0];
																supprimerNom.erase(supprimerNom.begin());
															}
															(*it).setBouchra(nom);
															Module nvModule(nom, inter, -1, 1, -1, false, false);
															t2.push_back(nvModule);
															vector<int> inter2; inter2.push_back(v0);
															Module nvModule2(v0, inter2, nom, 0, -1, true, true);
															t2.push_back(nvModule2);
															goto RASSEMBLE;
														}
													}
													else
													{
														//cout << "C11" << endl;
														if ((*it).getDegenerate() == 2)
														{
															//cout << "C12" << endl;
															vector<int> inte((*it).getSommets());
															inte.push_back(v0);
															(*it).setSommets(inte);
															vector<int> inter; inter.push_back(v0);
															Module nvModule(v0, inter, (*it).getNom(), 0, -1, true, true);
															t2.push_back(nvModule);
															goto RASSEMBLE;
														}
														else
														{
															//cout << "C13" << endl;
															vector<int> inter;
															inter.push_back(v0);
															inter.push_back((*it).getNom());
															int nom;
															if (supprimerNom.empty())
															{
																nomModules++;
																nom = nomModules;
															}
															else
															{
																sort(supprimerNom.begin(), supprimerNom.end());
																nom = supprimerNom[0];
																supprimerNom.erase(supprimerNom.begin());
															}
															(*it).setBouchra(nom);
															Module nvModule(nom, inter, -1, 2, -1, false, false);
															t2.push_back(nvModule);
															vector<int> inter2; inter2.push_back(v0);
															Module nvModule2(v0, inter2, nom, 0, -1, true, true);
															t2.push_back(nvModule2);
															goto RASSEMBLE;
														}
													}
												}
												parent = (*it).getBouchra();
												//cout << "parent now = " << parent << endl;
												break;
											}
										}
									}
								}
							}
						}
					}
				}
				else
				{
					// Essayer de mettre prime dans prime
					cout << "Test prime in prime 1" << endl;
					vector<int> mayModule;
					mayModule.push_back(v0);
					for (int h = 0; h < listSommets.size(); h++)
					{
						mayModule.push_back(listSommets[h]);
					}
					cout << "InitialMayModule : " << endl;
					afficher(mayModule);
					while (true)
					{
						set<int> difference;
						for (int h = 0; h < mayModule.size() - 1; h++)
						{
							for (int f = h + 1; f < mayModule.size(); f++)
							{
								for (int hh = 0; hh < g[mayModule[h]].size(); hh++)
								{
									bool trouver = false;
									for (int ff = 0; ff < g[mayModule[f]].size(); ff++)
									{
										if (g[mayModule[h]][hh] == g[mayModule[f]][ff])
										{
											trouver = true;
											break;
										}
									}
									if (!trouver && find(mayModule.begin(), mayModule.end(), g[mayModule[h]][hh]) == mayModule.end())
									{
										difference.insert(g[mayModule[h]][hh]);
									}
								}
								for (int ff = 0; ff < g[mayModule[f]].size(); ff++)
								{
									bool trouver = false;
									for (int hh = 0; hh < g[mayModule[h]].size(); hh++)
									{
										if (g[mayModule[h]][hh] == g[mayModule[f]][ff])
										{
											trouver = true;
											break;
										}
									}
									if (!trouver && find(mayModule.begin(), mayModule.end(), g[mayModule[f]][ff]) == mayModule.end())
									{
										difference.insert(g[mayModule[f]][ff]);
									}
								}
							}
						}
						cout << "Difference : ";
						for (set<int>::iterator it = difference.begin(); it != difference.end(); it++)
						{
							cout << *it << " ";
						}
						cout << endl;
						if (difference.empty())
						{
							for (int h = 0; h < mayModule.size(); h++)
							{
								for (vector<Module>::iterator it2 = t2.begin(); it2 != t2.end(); it2++)
								{
									if (mayModule[h] == (*it2).getNom())
									{
										t1.push_back(*it2);
										t2.erase(it2);
										break;
									}
								}
							}
							int nom;
							if (supprimerNom.empty())
							{
								nomModules++;
								nom = nomModules;
							}
							else
							{
								sort(supprimerNom.begin(), supprimerNom.end());
								nom = supprimerNom[0];
								supprimerNom.erase(supprimerNom.begin());
							}
							for (int k = 0; k < mayModule.size(); k++)
							{
								for (int l = 0; l < t1.size(); l++)
								{
									if (mayModule[k] == t1[l].getNom())
									{
										t1[l].setBouchra(nom);
										break;
									}
								}
							}
							// On construit le nouveau module
							Module nvModule(nom, mayModule, -1, 0, -1, true, false);
							// On l'ajoute à la liste des modules
							t1.push_back(nvModule);
							vector<int> inter; inter.push_back(v0);
							Module nvModule2(v0, inter, nom, 0, -1, true, true);
							t1.push_back(nvModule2);
							goto RASSEMBLE;
						}
						else
						{
							for (set<int>::iterator h = difference.begin(); h != difference.end(); h++)
							{
								mayModule.push_back(*h);
							}
							if (mayModule.size() == g.size())
							{
								break;
							}
						}
					}
				}
			}
			else
			{
				// Essayer de mettre prime dans prime
				cout << "Test prime in prime 2" << endl;
				vector<int> mayModule;
				mayModule.push_back(v0);
				for (int h = 0; h < listSommets.size(); h++)
				{
					mayModule.push_back(listSommets[h]);
				}
				cout << "InitialMayModule : " << endl;
				afficher(mayModule);
				while (true)
				{
					set<int> difference;
					for (int h = 0; h < mayModule.size() - 1; h++)
					{
						for (int f = h + 1; f < mayModule.size(); f++)
						{
							for (int hh = 0; hh < g[mayModule[h]].size(); hh++)
							{
								bool trouver = false;
								for (int ff = 0; ff < g[mayModule[f]].size(); ff++)
								{
									if (g[mayModule[h]][hh] == g[mayModule[f]][ff])
									{
										trouver = true;
										break;
									}
								}
								if (!trouver && find(mayModule.begin(), mayModule.end(), g[mayModule[h]][hh]) == mayModule.end())
								{
									difference.insert(g[mayModule[h]][hh]);
								}
							}
							for (int ff = 0; ff < g[mayModule[f]].size(); ff++)
							{
								bool trouver = false;
								for (int hh = 0; hh < g[mayModule[h]].size(); hh++)
								{
									if (g[mayModule[h]][hh] == g[mayModule[f]][ff])
									{
										trouver = true;
										break;
									}
								}
								if (!trouver && find(mayModule.begin(), mayModule.end(), g[mayModule[f]][ff]) == mayModule.end())
								{
									difference.insert(g[mayModule[f]][ff]);
								}
							}
						}
					}
					cout << "Difference : ";
					for (set<int>::iterator it = difference.begin(); it != difference.end(); it++)
					{
						cout << *it << " ";
					}
					cout << endl;
					if (difference.empty())
					{
						for (int h = 0; h < mayModule.size(); h++)
						{
							for (vector<Module>::iterator it2 = t2.begin(); it2 != t2.end(); it2++)
							{
								if (mayModule[h] == (*it2).getNom())
								{
									t1.push_back(*it2);
									t2.erase(it2);
									break;
								}
							}
						}
						int nom;
						if (supprimerNom.empty())
						{
							nomModules++;
							nom = nomModules;
						}
						else
						{
							sort(supprimerNom.begin(), supprimerNom.end());
							nom = supprimerNom[0];
							supprimerNom.erase(supprimerNom.begin());
						}
						for (int k = 0; k < mayModule.size(); k++)
						{
							for (int l = 0; l < t1.size(); l++)
							{
								if (mayModule[k] == t1[l].getNom())
								{
									t1[l].setBouchra(nom);
									break;
								}
							}
						}
						// On construit le nouveau module
						Module nvModule(nom, mayModule, -1, 0, -1, true, false);
						// On l'ajoute à la liste des modules
						t1.push_back(nvModule);
						vector<int> inter; inter.push_back(v0);
						Module nvModule2(v0, inter, nom, 0, -1, true, true);
						t1.push_back(nvModule2);
						goto RASSEMBLE;
					}
					else
					{
						for (set<int>::iterator h = difference.begin(); h != difference.end(); h++)
						{
							mayModule.push_back(*h);
						}
						if (mayModule.size() == g.size())
						{
							break;
						}
					}
				}
			}
		}
	}
	if (!unionModules)
	{
		//cout << "C14" << endl;
		vector<int> inter; inter.push_back(v0);
		Module nvModule(v0, inter, -1, 0, -1, true, true);
		t1.push_back(nvModule);
		/*bool trouver = false;
		for (int l = 0; l < t1.size(); l++)
		{
		if (t1[l].getNom() == listSommets[0])
		{
		t1.push_back(nvModule);
		trouver = true;
		break;
		}
		}
		if (!trouver)
		{
		cout << "C15" << endl;
		t2.push_back(nvModule);
		}
		//goto RASSEMBLE;*/
	}
	//cout << "T1 after v0Module : " << endl;
	//afficher(t1);
	//cout << "T2 after v0Module : " << endl;
	//afficher(t2);
RASSEMBLE:
	for (int i = 0; i < t1.size(); i++)
	{
		t.push_back(t1[i]);
	}
	for (int i = 0; i < t2.size(); i++)
	{
		t.push_back(t2[i]);
	}
	//cout << "T after v0Module : " << endl;
	//afficher(t);
}

void assemble(int& v0, map<int, vector<int> >& g, vector<Module>& t, int& bouchra, vector<int>& supprimerNom, int& nomAPrendre)
{
	cout << "\tAssemble" << endl;
	cout << "T avant assemble : " << endl;
	afficher(t);
	cout << "v0= " << v0 << endl;
	int v0Module;
	for (vector<Module>::iterator it = t.begin(); it != t.end(); it++)
	{
		cout << "module in : " << (*it).getNom() << endl;
		bool trouver = false;
		for (int i = 0; i < (*it).getSommets().size(); i++)
		{
			if ((*it).getSommets()[i] == v0)
			{
				trouver = true;
				if ((*it).getBouchra() == -1)
				{
					v0Module = (*it).getNom();
				}
				else
				{
					int parent = (*it).getBouchra();
					while (parent != -1)
					{
						for (vector<Module>::iterator it2 = t.begin(); it2 != t.end(); it2++)
						{
							if ((*it2).getNom() == parent)
							{
								parent = (*it2).getBouchra();
								if (parent == -1)
	 							{
									v0Module = (*it2).getNom();
								}
								break;
							}
						}
					}
				}
				break;
			}
		}
		if (trouver)
		{
			break;
		}
	}
	cout << "V0Module num = " << v0Module << endl;
	vector<int> inter;
	int posV0Module;
ASSEMBLE:
	for (int i = 0; i < t.size(); i++)
	{
		if (t[i].getNom() == v0Module)
		{
			for (int j = 0; j < t[i].getSommets().size(); j++)
			{
				recurseAdd(inter, t, t[i].getSommets()[j]);
			}
			posV0Module = i;
			break;
		}
	}
	cout << "interV0 : " << endl;
	afficher(inter);
	for (int i = 0; i < t.size(); i++)
	{
		if (i != posV0Module && t[i].getBouchra() == -1)
		{
			cout << "Module a tester : " << endl;
			cout << t[i] << endl;
			vector<int> inter2, adjInter, adjInter2;
			for (int j = 0; j < t[i].getSommets().size(); j++)
			{
				recurseAdd(inter2, t, t[i].getSommets()[j]);
			}
			for (int j = inter2.size() - 1; j > -1; j--)
			{
				if (find(inter.begin(), inter.end(), inter2[j]) != inter.end())
				{
					inter2.erase(inter2.begin() + j);
				}
			}
			cout << "interV0 2 : " << endl;
			afficher(inter2);
			if (!inter2.empty())
			{
				for (vector<int>::iterator it = inter.begin(); it != inter.end(); it++)
				{
					for (vector<int>::iterator it2 = g[*it].begin(); it2 != g[*it].end(); it2++)
					{
						if (find(inter.begin(), inter.end(), *it2) == inter.end() && find(inter2.begin(), inter2.end(), *it2) == inter2.end())
						{
							if (find(adjInter.begin(), adjInter.end(), *it2) == adjInter.end())
							{
								adjInter.push_back(*it2);
							}
						}
					}
				}
				cout << "adj interV0 1 : " << endl;
				afficher(adjInter);
				for (vector<int>::iterator it = inter2.begin(); it != inter2.end(); it++)
				{
					for (vector<int>::iterator it2 = g[*it].begin(); it2 != g[*it].end(); it2++)
					{
						if (find(inter.begin(), inter.end(), *it2) == inter.end() && find(inter2.begin(), inter2.end(), *it2) == inter2.end())
						{
							if (find(adjInter2.begin(), adjInter2.end(), *it2) == adjInter2.end())
							{
								adjInter2.push_back(*it2);
							}
						}
					}
				}
				cout << "adj interV0 2 : " << endl;
				afficher(adjInter2);
				if (adjInter.size() == adjInter2.size())
				{
					cout << "K1" << endl;
					bool found;
					for (int j = 0; j < adjInter.size(); j++)
					{
						found = false;
						for (int k = 0; k < adjInter2.size(); k++)
						{
							if (adjInter[j] == adjInter2[k])
							{
								cout << "K2" << endl;
								found = true;
								break;
							}
						}
						if (!found)
						{
							cout << "K3" << endl;
							break;
						}
					}
					if (found)
					{
						cout << "K4" << endl;
						if (find(g[inter[0]].begin(), g[inter[0]].end(), inter2[0]) == g[inter[0]].end())
						{
							cout << "K5" << endl;
							cout << "Degenerate i = " << t[i].getDegenerate() << endl;
							cout << "Degenerate v0 = " << t[posV0Module].getDegenerate() << endl;
							if (t[i].getDegenerate() == 1 && t[posV0Module].getDegenerate() == 1)
							{
								cout << "K6" << endl;
								vector<int> interIn(t[posV0Module].getSommets());
								interIn.push_back(t[i].getNom());
								t[posV0Module].setSommets(interIn);
								t[i].setBouchra(t[posV0Module].getNom());
							}
							else if (t[i].getDegenerate() == 1 && t[posV0Module].getDegenerate() != 1)
							{
								cout << "K7" << endl;
								vector<int> interIn(t[i].getSommets());
								interIn.push_back(v0Module);
								t[i].setSommets(interIn);
								t[posV0Module].setBouchra(t[i].getNom());
								cout << t[i] << endl;
								cout << t[posV0Module] << endl;
								v0Module = t[i].getNom();
								posV0Module = i;
							}
							else if (t[i].getDegenerate() != 1 && t[posV0Module].getDegenerate() == 1)
							{
								cout << "K8" << endl;
								vector<int> interIn(t[posV0Module].getSommets());
								interIn.push_back(t[i].getNom());
								t[posV0Module].setSommets(interIn);
								t[i].setBouchra(t[posV0Module].getNom());
							}
							else if (t[i].getDegenerate() != 1 && t[posV0Module].getDegenerate() != 1)
							{
								cout << "K9" << endl;
								int nom;
								if (supprimerNom.empty())
								{
									cout << "K10" << endl;
									nomModules++;
									nom = nomModules;
								}
								else
								{
									cout << "K11" << endl;
									sort(supprimerNom.begin(), supprimerNom.end());
									nom = supprimerNom[0];
									supprimerNom.erase(supprimerNom.begin());
								}
								vector<int> sommets;
								sommets.push_back(t[i].getNom());
								sommets.push_back(v0Module);
								Module nvModule(nom, sommets, -1, 1, -1, true, false);
								t.push_back(nvModule);
								t[i].setBouchra(nom);
								t[posV0Module].setBouchra(nom);
								v0Module = nom;
								posV0Module = t.size() - 1;
							}
						}
						else
						{
							cout << "K12" << endl;
							if (t[i].getDegenerate() == 2 && t[posV0Module].getDegenerate() == 2)
							{
								cout << "K13" << endl;
								vector<int> interIn(t[posV0Module].getSommets());
								interIn.push_back(t[i].getNom());
								t[posV0Module].setSommets(interIn);
								t[i].setBouchra(t[posV0Module].getNom());
							}
							else if (t[i].getDegenerate() == 2 && t[posV0Module].getDegenerate() != 2)
							{
								cout << "K14" << endl;
								vector<int> interIn(t[i].getSommets());
								interIn.push_back(t[posV0Module].getNom());
								t[i].setSommets(interIn);
								t[posV0Module].setBouchra(t[i].getNom());
								v0Module = t[i].getNom();
								posV0Module = i;
							}
							else if (t[i].getDegenerate() != 2 && t[posV0Module].getDegenerate() == 2)
							{
								cout << "K15" << endl;
								vector<int> interIn(t[posV0Module].getSommets());
								interIn.push_back(t[i].getNom());
								t[posV0Module].setSommets(interIn);
								t[i].setBouchra(t[posV0Module].getNom());
							}
							else if (t[i].getDegenerate() != 2 && t[posV0Module].getDegenerate() != 2)
							{
								cout << "K16" << endl;
								int nom;
								if (supprimerNom.empty())
								{
									cout << "K17" << endl;
									nomModules++;
									nom = nomModules;
								}
								else
								{
									cout << "K18" << endl;
									sort(supprimerNom.begin(), supprimerNom.end());
									nom = supprimerNom[0];
									supprimerNom.erase(supprimerNom.begin());
								}
								vector<int> sommets;
								sommets.push_back(t[i].getNom());
								sommets.push_back(v0Module);
								Module nvModule(nom, sommets, -1, 2, -1, true, false);
								t.push_back(nvModule);
								t[i].setBouchra(nom);
								t[posV0Module].setBouchra(nom);
								v0Module = nom;
								posV0Module = t.size() - 1;
							}
						}
						inter.clear();
						goto ASSEMBLE;
					}
				}
			}
		}
	}
	vector<int> sommets;
	//nomModules++;
	for (int i = 0; i < t.size(); i++)
	{
		if (t[i].getBouchra() == -1)
		{
			sommets.push_back(t[i].getNom());
			t[i].setBouchra(/*nomModules*/nomAPrendre);
		}
	}
	Module nvModule(nomAPrendre, sommets, bouchra, 0, -1, false, false);
	t.push_back(nvModule);
	cout << "T after assemble avant renumeroter : " << endl;
	afficher(t);
	cout << "supprimerNom : " << endl;
	afficher(supprimerNom);
	renumeroter(t, supprimerNom);
}

void primePhase(map<int, vector<int> >& graphe, map<int, vector<int> >& grapheBar, vector<Module>& listModules, int& bouchra, int& nomAPrendre)
{
	cout << "Prime Phase" << endl;
	char c;
	//cout << "Prime Phase : " << endl;
	vector<int> listSommets;
	// Fixer le v0
	int v0 = graphe.begin()->first;
	//cout << "v0 = " << v0 << endl;
	// Avoir le N(v0)
	vector<int> nv0(graphe[v0]), nbv0;
	// Avoir le Nb(v0)
	for (map<int, vector<int> >::iterator it = graphe.begin(); it != graphe.end(); it++)
	{
		if (it != graphe.begin())
		{
			bool found = false;
			for (int i = 0; i < nv0.size(); i++)
			{
				if (nv0[i] == it->first)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				nbv0.push_back(it->first);
			}
		}
	}
	cout << "nv0 : " << endl;
	afficher(nv0);
	cout << "nbv0 : " << endl;
	afficher(nbv0);
	//cin >> c;
	// Avoir les graphes induits G|N(v0) et G|Nb(v0)
	map<int, vector<int> > indGNV0(grapheInduit(graphe, nv0));
	map<int, vector<int> > indGNBV0(grapheInduit(graphe, nbv0));
	//cout << "indGNV0 : " << endl;
	//afficher(indGNV0);
	//cout << "indGNBV0 : " << endl;
	//afficher(indGNBV0);
	//cin >> c;
	vector<Module> T1, T2, T;
	int nivMax1, nivMax2;
	// Exécuter les 4 steps de l'algorithme
	recursion(indGNV0, T1, nivMax1);
	recursion(indGNBV0, T2, nivMax2);
	//cin >> c;
	// Calculer les niveaux des modules
	map<int, vector<int> > mappingNiveaux1 = getNiveaux(T1);
	map<int, vector<int> > mappingNiveaux2 = getNiveaux(T2);
	nivMax1 = 0;
	for (map<int, vector<int> >::iterator it = mappingNiveaux1.begin(); it != mappingNiveaux1.end(); it++)
	{
		if (it->first > nivMax1)
		{
			nivMax1 = it->first;
		}
	}
	cout << "Mapping Niveaux 1 : " << endl;
	afficher(mappingNiveaux1);
	nivMax2 = 0;
	for (map<int, vector<int> >::iterator it = mappingNiveaux2.begin(); it != mappingNiveaux2.end(); it++)
	{
		if (it->first > nivMax2)
		{
			nivMax2 = it->first;
		}
	}
	cout << "Mapping Niveaux 2 : " << endl;
	afficher(mappingNiveaux2);
	cin >> c;
	vector<int> supprimerNom;
	/*int max = -1;
	for (int i = 0; i < T1.size(); i++)
	{
	if (max < T1[i].getNom())
	{
	max = T1[i].getNom();
	}
	}
	for (int i = 0; i < T2.size(); i++)
	{
	if (max < T2[i].getNom())
	{
	max = T2[i].getNom();
	}
	}
	if (max > nomModules)
	{
	nomModules = max;
	}*/
	restriction(v0, nv0, nbv0, graphe, T1, T2, nivMax1, nivMax2, supprimerNom);
	cout << "Supprimer Noms :" << endl;
	afficher(supprimerNom);
	//cin >> c;
	v0Module(v0, nv0, nbv0, graphe, grapheBar, T1, T2, T, supprimerNom);
	cout << "Supprimer Noms :" << endl;
	afficher(supprimerNom);
	//cin >> c;
	assemble(v0, graphe, T, bouchra, supprimerNom, nomAPrendre);
	// Récupérer les solutions trouvées
	for (int i = 0; i < T.size(); i++)
	{
		listModules.push_back(T[i]);
	}
}

void decompositionSecondaire(map<int, vector<int> >& graphe, map<int, vector<int> >& grapheBar, vector<map<int, vector<int> > >& ECC, vector<Module>& ancienModules, vector<int>& listBouchra, vector<int>& suiviBouchra)
{
	cout << "decomposition" << endl;
	// Une liste qui contient les sommets actuels du graphe
	vector<int> listSommetsGraphe;
	cout << "nomModules : " << nomModules << endl;
	//cout << "nomProchainsModules : " << nomProchainsModules << endl;
	//cout << "list bouchra : " << endl;
	//afficher(listBouchra);
	/*cout << "suivi bouchra : " << endl;
	afficher(suiviBouchra);*/
	for (map<int, vector<int> >::iterator it = graphe.begin(); it != graphe.end(); it++)
	{
		listSommetsGraphe.push_back(it->first);
	}
	afficher(listSommetsGraphe);
	char c;
	cout << "PAUSE 1 : Intro" << endl;
	cin >> c;	
	if (graphe.size() == 1)
	{
		// On récupère ici le bouchra du graphe qu'on est en train de traité, c'est le dernier de listBouchra
		int bouchra(listBouchra[listBouchra.size() - 1]);
		//cout << "ORIGINE in 1 = " << bouchra << endl;
		// On le supprime de la liste une fois stocké
		listBouchra.pop_back();
		if (!suiviBouchra.empty())
		{
			suiviBouchra.pop_back();
		}
		// On construit le nouveau module
		Module nvModule(listSommetsGraphe[0], listSommetsGraphe, bouchra, 0, -1, true, true);
		// On l'ajoute à la liste des modules
		ancienModules.push_back(nvModule);
		// Donner le fils à son père
		//cout << "Donner le fils: " << listSommetsGraphe[0] << " au pere : " << bouchra << endl;
		for (int k = 0; k < ancienModules.size(); k++)
		{
			if (ancienModules[k].getNom() == bouchra)
			{
				vector<int> ss(ancienModules[k].getSommets());
				ss.push_back(listSommetsGraphe[0]);
				ancienModules[k].setSommets(ss);
				break;
			}
		}
		cout << "actuel = " << listSommetsGraphe[0] << "\tpere = " << bouchra << endl;
		cout << "PAUSE 2 : SOMMET ISOLE" << endl;
		cin >> c;
		return;
	}
	if (vgStable(graphe))
	{
		// On récupère ici le bouchra du graphe qu'on est en train de traité, c'est le dernier de listBouchra
		int bouchra(listBouchra[listBouchra.size() - 1]);
		//cout << "ORIGINE in 2 = " << bouchra << endl;
		/*int nomNiv;
		if (listBouchra.empty() || (listBouchra.size() == 1 && listBouchra[0] == -1))
		{
		nomModules++;
		nomNiv = 0;
		}
		else
		{
		nomModules = listBouchra[listBouchra.size() - 1];
		if (suiviBouchra.empty())
		{
		nomNiv = 0;
		}
		else
		{
		nomNiv = suiviBouchra[suiviBouchra.size() - 1];
		}
		}
		int nomAPrendre = nomModules + nomNiv;*/
		nomModules++;
		// On le supprime de la liste une fois stocké
		listBouchra.pop_back();
		if (!suiviBouchra.empty())
		{
			suiviBouchra.pop_back();
		}
		// On construit le nouveau module
		Module nvModule(/*nomAPrendre*/nomModules, listSommetsGraphe, bouchra, 1, -1, false, false);
		// On l'ajoute à la liste des modules
		ancienModules.push_back(nvModule);
		// Donner le fils à son père
		//cout << "Donner le fils: " << nomModules << " au pere : " << bouchra << endl;
		for (int k = 0; k < ancienModules.size(); k++)
		{
			if (ancienModules[k].getNom() == bouchra)
			{
				vector<int> ss(ancienModules[k].getSommets());
				ss.push_back(nomModules);
				ancienModules[k].setSommets(ss);
				break;
			}
		}
		// Création des modules triviaux des fils du stable
		for (int i = 0; i < listSommetsGraphe.size(); i++)
		{
			vector<int> inter; inter.push_back(listSommetsGraphe[i]);
			// On construit le nouveau module
			Module nvModule_(listSommetsGraphe[i], inter, /*nomAPrendre*/ nomModules, 0, -1, true, true);
			// On l'ajoute à la liste des modules
			ancienModules.push_back(nvModule_);
		}
		cout << "actuel = " << nomModules << "\tpere = " << bouchra << endl;
		cout << "PAUSE 3 : STABLE" << endl;
		cin >> c;
		return;
	}
	// On construit l'ensemble des composantes connexes de G
	vector<map<int, vector<int> > > CCG = dCConnexe(graphe);
	// On construit l'ensemble des composantes connexes de G barre
	vector<map<int, vector<int> > > CCGB = dCConnexe(grapheBar);
	// On récupère ici le bouchra du graphe qu'on est en train de traité, c'est le dernier de listBouchra
	int bouchra(listBouchra[listBouchra.size() - 1]);
	//cout << "ORIGINE 3 = " << bouchra << endl;
	// On le supprime de la liste une fois stocké
	listBouchra.pop_back();
	// On s'assure que nomProchainModules est le même que nomModules (je ne suis pas sûr de cette instruction)
	//nomProchainsModules = nomModules;
	// Si la taille de l'ensemble des composantes connexes est différente de 1 alors G non connexe, sinon on vérifie GBarre
	if (CCG.size() != 1)
	{
		//cout << "in 1" << endl;
		// G non connexe, on ajoute le nouveau module qui est en parallèle
		//cout << "G non connexe" << endl;
		/*for (int i = 0; i<CCG.size(); i++)
		{
		cout << "G[" << i << "]:" << endl;
		afficher(CCG[i]);
		}*/
		// list des modules construisant le module qu'on va créer
		vector<int> listSommets;
		// Le nom du nouveau module
		/*int nomNiv;
		if (listBouchra.empty())
		{
		nomModules++;
		nomNiv = 0;
		}
		else
		{
		nomModules = bouchra;
		nomNiv = suiviBouchra[suiviBouchra.size() - 1];
		}
		int nomAPrendre = nomModules + nomNiv;
		if (!suiviBouchra.empty())
		{
		suiviBouchra.pop_back();
		}
		// Il faut que le nomProchainModules ne soit pas dépasser par nomModules
		if (nomModules > nomProchainsModules)
		{
		nomProchainsModules = nomModules;
		}
		nomNiv = 0;*/
		nomModules++;
		//cout << "nouveau module : " << nomModules << endl;
		cout << "Liste des composantes : " << endl;
		// On ajoute chaque composante connexe à la liste des sommets construisant le module
		for (int i = 0; i<CCG.size(); i++)
		{
			// Si la composant connexe contient un seul sommet alors on l'ajoute directement et sauvegarde son bouchra
			if (CCG[i].size() == 1)
			{
				//listSommets.push_back(CCG[i].begin()->first);
				listBouchra.push_back(/*nomAPrendre*/nomModules);
				//suiviBouchra.push_back(nomNiv);
			}
			else // Sinon, la composante connexe construit un nouveau graphe à traiter dans les prochaines itérations
			{
				// On crée dès maintenant son nom, on le prévoie psk nomProchainModules représente les noms futures à donner
				//nomProchainsModules++;
				//nomNiv++;
				//listSommets.push_back(nomProchainsModules);
				// On sauvegarde aussi son bouchra
				listBouchra.push_back(/*nomAPrendre*/nomModules);
				//suiviBouchra.push_back(nomNiv);
			}
			// On stocke chaque composante à traiter prochainement dans ECC
			ECC.push_back(CCG[i]);
			cout << "{";
			for (map<int, vector<int> >::iterator af = CCG[i].begin(); af != CCG[i].end(); af++)
			{
				cout << af->first << " ";
			}
			cout << "}" << endl;
		}
		bool strong, trivial;
		// On fixe les propriétés du module qu'on va créer
		if (bouchra == 1 || bouchra == 0)
		{
			strong = true;
		}
		else
		{
			strong = false;
		}
		if (bouchra == 0)
		{
			trivial = true;
		}
		else
		{
			trivial = false;
		}
		// On construit le nouveau module
		Module nvModule(/*nomAPrendre*/nomModules, listSommets, bouchra, 1, -1, strong, trivial);
		// On l'ajoute à la liste des modules
		ancienModules.push_back(nvModule);
		// Donner le fils à son père
		//cout << "Donner le fils: " << nomModules << " au pere : " << bouchra << endl;
		for (int k = 0; k < ancienModules.size(); k++)
		{
			if (ancienModules[k].getNom() == bouchra)
			{
				vector<int> ss(ancienModules[k].getSommets());
				ss.push_back(nomModules);
				ancienModules[k].setSommets(ss);
				break;
			}
		}
		// Si la taille de l'ensemble des composantes connexes de GBarre est différente de 1 alors GBarre non connexe, sinon G est prime
		cout << "actuel = " << nomModules << "\tpere = " << bouchra << endl;
		cout << "PAUSE 4 : PARALLELE" << endl;
		cin >> c;
	}
	else if (CCGB.size() != 1)
	{
		//cout << "in 2" << endl;
		// GBarre est non connexe, , on ajoute le nouveau module qui est en série
		//cout << "GBar non connexe" << endl;
		/*for (int i = 0; i<CCGB.size(); i++)
		{
		cout << "GBar[" << i << "]:" << endl;
		afficher(CCGB[i]);
		}*/
		// Le nom du nouveau module
		/*int nomNiv;
		if (listBouchra.empty())
		{
		nomModules++;
		nomNiv = 0;
		}
		else
		{
		nomModules = bouchra;
		nomNiv = suiviBouchra[suiviBouchra.size() - 1];
		}
		int nomAPrendre = nomModules + nomNiv;
		if (!suiviBouchra.empty())
		{
		suiviBouchra.pop_back();
		}
		// Il faut que le nomProchainModules ne soit pas dépasser par nomModules
		if (nomModules > nomProchainsModules)
		{
		nomProchainsModules = nomModules;
		}
		nomNiv = 0;*/
		nomModules++;
		//cout << "nouveau module : " << nomModules << endl;
		// list des modules construisant le module qu'on va créer
		vector<int> listSommets;
		cout << "Liste des omposantes : " << endl;
		// On ajoute chaque composante connexe à la liste des sommets construisant le module
		for (int i = 0; i<CCGB.size(); i++)
		{
			// Si la composant connexe contient un seul sommet alors on l'ajoute directement et sauvegarde son bouchra
			if (CCGB[i].size() == 1)
			{
				//listSommets.push_back(CCGB[i].begin()->first);
				listBouchra.push_back(/*nomAPrendre*/nomModules);
				//suiviBouchra.push_back(nomNiv);
			}
			else // Sinon, la composante connexe construit un nouveau graphe à traiter dans les prochaines itérations
			{
				// On crée dès maintenant son nom, on le prévoie psk nomProchainModules représente les noms futures à donner
				//nomProchainsModules++;
				//nomNiv++;
				//listSommets.push_back(nomProchainsModules);
				// On sauvegarde aussi son bouchra
				listBouchra.push_back(/*nomAPrendre*/nomModules);
				//suiviBouchra.push_back(nomNiv);
			}
			// On stocke chaque composante à traiter prochainement dans ECC
			ECC.push_back(gToGBar(CCGB[i]));
			cout << "{";
			for (map<int, vector<int> >::iterator af = CCGB[i].begin(); af != CCGB[i].end(); af++)
			{
				cout << af->first << " ";
			}
			cout << "}" << endl;
		}
		bool strong, trivial;
		// On fixe les propriété du module qu'on va créer
		if (bouchra == 1 || bouchra == 0)
		{
			strong = true;
		}
		else
		{
			strong = false;
		}
		if (bouchra == 0)
		{
			trivial = true;
		}
		else
		{
			trivial = false;
		}
		// On construit le nouveau module
		Module nvModule(/*nomAPrendre*/nomModules, listSommets, bouchra, 2, -1, strong, trivial);
		// On l'ajoute à la liste des modules
		ancienModules.push_back(nvModule);
		// Donner le fils à son père
		//cout << "Donner le fils: " << nomModules << " au pere : " << bouchra << endl;
		for (int k = 0; k < ancienModules.size(); k++)
		{
			if (ancienModules[k].getNom() == bouchra)
			{
				vector<int> ss(ancienModules[k].getSommets());
				ss.push_back(nomModules);
				ancienModules[k].setSommets(ss);
				break;
			}
		}
		cout << "actuel = " << nomModules << "\tpere = " << bouchra << endl;
		cout << "PAUSE 5 : SERIE" << endl;
		cin >> c;
	}
	else // G est sûrement prime, on crée le module correspondant
	{
		/*cout << "in 3" << endl;
		cout << "G et Gbar sont connexes" << endl;
		for (int i = 0; i<CCG.size(); i++)
		{
		cout << "G[" << i << "]:" << endl;
		afficher(CCG[i]);
		}
		for (int i = 0; i<CCGB.size(); i++)
		{
		cout << "GBar[" << i << "]:" << endl;
		afficher(CCGB[i]);
		}*/
		if (graphe.size() == 4)
		{
			//cout << "in 4" << endl;
			// Le nom du nouveau module
			/*int nomNiv;
			if (listBouchra.empty())
			{
			nomModules++;
			nomNiv = 0;
			}
			else
			{
			nomModules = bouchra;
			nomNiv = suiviBouchra[suiviBouchra.size() - 1];
			}
			int nomAPrendre = nomModules + nomNiv;
			if (!suiviBouchra.empty())
			{
			suiviBouchra.pop_back();
			}*/
			nomModules++;
			//cout << "nouveau module : " << nomModules << endl;
			// list des modules construisant le module qu'on va créer
			vector<int> listSommets;
			for (map<int, vector<int> >::iterator it = graphe.begin(); it != graphe.end(); it++)
			{
				listSommets.push_back(it->first);
			}
			// On construit le nouveau module
			Module nvModule(/*nomAPrendre*/nomModules, listSommets, bouchra, 0, -1, true, false);
			// On l'ajoute à la liste des modules
			ancienModules.push_back(nvModule);
			// Donner le fils à son père
			//cout << "Donner le fils: " << nomModules << " au pere : " << bouchra << endl;
			for (int k = 0; k < ancienModules.size(); k++)
			{
				if (ancienModules[k].getNom() == bouchra)
				{
					vector<int> ss(ancienModules[k].getSommets());
					ss.push_back(nomModules);
					ancienModules[k].setSommets(ss);
					break;
				}
			}
			for (int i = 0; i < listSommets.size(); i++)
			{
				vector<int> list;
				list.push_back(listSommets[i]);
				// On construit le nouveau module
				Module nvModule(listSommets[i], list, /*nomAPrendre*/nomModules, 0, -1, true, true);
				// On l'ajoute à la liste des modules
				ancienModules.push_back(nvModule);
			}
			cout << "actuel = " << nomModules << "\tpere = " << bouchra << endl;
			cout << "PAUSE 6 : PRIME A 4" << endl;
			cin >> c;
		}
		else
		{
			//cout << "in 5" << endl;
			//int maxAppelation = nomProchainsModules + graphe.size() + 1;
			/*int nomNiv;
			if (listBouchra.empty())
			{
			nomModules++;
			nomNiv = 0;
			}
			else
			{
			nomModules = bouchra;
			nomNiv = suiviBouchra[suiviBouchra.size() - 1];
			}
			int nomAPrendre = nomModules + nomNiv;*/
			nomModules++;
			int nomAPrendre = nomModules;
			cout << "actuel = " << nomModules << "\tpere = " << bouchra << endl;
			cout << "PAUSE 7 : TO PRIME PHASE" << endl;
			cin >> c;
			primePhase(graphe, grapheBar, ancienModules, bouchra, nomAPrendre);
			// Donner le fils à son père
			//cout << "Donner le fils: " << nomAPrendre << " au pere : " << bouchra << endl;
			for (int k = 0; k < ancienModules.size(); k++)
			{
				if (ancienModules[k].getNom() == bouchra)
				{
					vector<int> ss(ancienModules[k].getSommets());
					ss.push_back(nomAPrendre);
					ancienModules[k].setSommets(ss);
					break;
				}
			}
			cout << "List modules after assemble : " << endl;
			afficher(ancienModules);
			cout << "PAUSE 8 : END PRIME" << endl;
			cin >> c;
		}
		/*int max = -1;
		for (int i = 0; i < ancienModules.size(); i++)
		{
		if (ancienModules[i].getNom() > max)
		{
		max = ancienModules[i].getNom();
		}
		}
		nomModules = max;
		nomProchainsModules = max;*/
	}
	// test non conecté g
	// detecte connex sur g
	// liste composantes conex parallel
	// else test g bar
	// detecte connex sur g
	// liste composantes conex serie
	// else prime
}

void arborescence(map<int, vector<int> >& G, vector<Module>& listModule)
{
	cout << "Arborescence" << endl;
	// Ensemble des composantes connexes à traiter à partir de la ra//cine, un fois vide on termine l'algorithme (tous les cas traité)
	vector<map<int, vector<int> > > ECC;
	// Vecteur des bouchras de chaque composante connexe qu'on va traiter
	vector<int> bouchra, suiviBouchra;
	// Initialemnt on traite la ra//cine ('est le graphe  concatainé de la dernière étape faite)
	ECC.push_back(G);
	// La racine, son bouchra est -1
	bouchra.push_back(-1);
	// NomProchainModules: est les noms des modules qu'on va utiliser si la G est non connexe ou G barre est non connexe
	// psk ce seront de nouveaux modules ajouté à part l'actuel, alors on les prévoit avant leur création
	//nomProchainsModules = nomModules;
	cout << "nomModules : " << nomModules << endl;
	//cout << "nomProchainsModules : " << nomProchainsModules << endl;
	//cout << "pass 1" << endl;
	// Si ECC vide, alors on termine l'algorithme, tous les cas sont traités
	while (!ECC.empty())
	{
		//cout << "pass 2" << endl;
		//cout << "Le graphe à traiter : " << endl;
		//afficher(ECC[ECC.size() - 1]);
		//cout << "Liste des Bouchra :" << endl;
		//afficher(bouchra);
		// On traite le dernier graphe ajouté à ECC
		map<int, vector<int> > GPrime(ECC[ECC.size() - 1]);
		cout << "pass 3" << endl;
		//afficher(GPrime);
		// On crée G barre de GPrime
		map<int, vector<int> > GPrimeBar(gToGBar(GPrime));
		cout << "pass 4 : Gbar:" << endl;
		//afficher(GPrimeBar);
		// On supprime de ECC le graphe qu'on va traiter
		ECC.pop_back();
		//cout << "pass 5" << endl;
		// On traite ici GPrime et GPrimeBar
		decompositionSecondaire(GPrime, GPrimeBar, ECC, listModule, bouchra, suiviBouchra);
	}
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

						//cout << '\n';
						break;
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

void recursionNiveaux(int nv, vector<int> sommets, vector<Module>& listModule, map<int, vector<int> >& niv)
{
	nv++;
	int ind;
	for (int i = 0; i < sommets.size(); i++)
	{
		//cout << "sommets [" << i << "]= " << sommets[i] << endl;
		for (int j = 0; j < listModule.size(); j++)
		{
			//cout << "nom module [" << j << "]= " << listModule[j].getNom() << endl;
			if (listModule[j].getNom() == sommets[i])
			{
				ind = j;
				break;
			}
		}
		vector<int> listSommets;
		for (int j = 0; j < listModule[ind].getSommets().size(); j++)
		{
			listSommets.push_back(listModule[ind].getSommets()[j]);
		}
		if (listSommets.size() == 1)
		{
			//cout << "feuille: " << sommets[i] << endl;
			listModule[ind].setNiveau(nv);
			map<int, vector<int> >::iterator it = niv.find(nv);
			if (it != niv.end())
			{
				it->second.push_back(sommets[i]);
			}
			else
			{
				vector<int> list;
				list.push_back(sommets[i]);
				niv[nv] = list;
			}
		}
		else
		{
			recursionNiveaux(nv, listSommets, listModule, niv);
			cout << "noeud: " << sommets[i] << endl;
			listModule[ind].setNiveau(nv);
			map<int, vector<int> >::iterator it = niv.find(nv);
			if (it != niv.end())
			{
				it->second.push_back(sommets[i]);
			}
			else
			{
				vector<int> list;
				list.push_back(sommets[i]);
				niv[nv] = list;
			}
		}
	}
}

map<int, vector<int> > getNiveaux(vector<Module>& listModule)
{
	map<int, vector<int> > niv;
	for (int i = 0; i < listModule.size(); i++)
	{
		if (listModule[i].getBouchra() == -1)
		{
			listModule[i].setNiveau(0);
			if (listModule[i].getSommets().size() == 1)
			{
				if (niv.empty())
				{
					niv[0] = listModule[i].getSommets();
				}
				else
				{
					niv[0].push_back(listModule[i].getNom());
				}
			}
			else
			{
				int nv(0);
				vector<int> racine;
				racine.push_back(listModule[i].getNom());
				niv[nv] = racine;
				//cout << "niv = " << nv << endl;
				recursionNiveaux(nv, listModule[i].getSommets(), listModule, niv);
			}
		}
	}
	return niv;
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

map<int, vector<int> > construireGrapheQuotient(vector<Module>& modules, map<int, vector<int> >& G)
{
	cout << "Construire Graphe : " << endl;
	int racine, ind;
	map<int, vector<int> > Gq;
	for (int i = 0; i < modules.size(); i++)
	{
		if (modules[i].getBouchra() == -1)
		{
			racine = modules[i].getNom();
			ind = i;
			break;
		}
	}
	if (modules[ind].getDegenerate() == 0)
	{
		vector<int> keys;
		vector<vector<int> > primalLists;
		for (int i = 0; i < modules.size(); i++)
		{
			if (modules[i].getNiveau() == 1)
			{
				keys.push_back(modules[i].getNom());
				vector<int> inter; int depart = modules[i].getNom();
				recurseAdd(inter, modules, depart);
				primalLists.push_back(inter);
			}
		}
		cout << "keys : " << endl;
		afficher(keys);
		cout << "primalLists : " << endl;
		afficher(primalLists);
		for (int i = 0; i < keys.size(); i++)
		{
			vector<int> adj;
			for (int j = 0; j < keys.size(); j++)
			{
				if (i != j)
				{
					if (find(G[primalLists[i][0]].begin(), G[primalLists[i][0]].end(), primalLists[j][0]) != G[primalLists[i][0]].end())
					{
						adj.push_back(keys[j]);
					}
				}
			}
			Gq[keys[i]] = adj;
		}
	}
	else if (modules[ind].getDegenerate() == 1)
	{
		for (int i = 0; i < modules.size(); i++)
		{
			if (modules[i].getBouchra() == racine)
			{
				vector<int> inter;
				Gq[modules[i].getNom()] = inter;
			}
		}
	}
	else
	{
		vector<int> inter;
		for (int i = 0; i < modules.size(); i++)
		{
			if (modules[i].getBouchra() == racine)
			{
				inter.push_back(modules[i].getNom());
			}
		}
		for (int i = 0; i < inter.size(); i++)
		{
			vector<int> inter2;
			for (int j = 0; j < inter.size(); j++) {
				if (i != j)
				{
					inter2.push_back(inter[j]);
				}
			}
			Gq[inter[i]] = inter2;
		}
	}
	return Gq;
}

int decomposition(map<int, vector<int> >& G, map<int, vector<int> >& Gq, vector<Module>& listModules, unsigned int& n)
{
	unsigned int z, nivMax;
	char c;

	// Initialization
	nomModules = n - 1;
	//nomProchainsModules = n - 1;


	cout << "BEGIN" << endl;
	arborescence(G, listModules);
	// Renumérotation
	int tailleModule = listModules.size();
	for (int i = tailleModule - 1; i > -1; i--)
	{
		bool trouver(false);
		for (int j = 0; j < listModules.size(); j++)
		{
			if (listModules[j].getNom() == i)
			{
				trouver = true;
				break;
			}
		}
		if (!trouver)
		{
			int max = -1, ind;
			for (int j = 0; j < listModules.size(); j++)
			{
				if (max < listModules[j].getNom())
				{
					max = listModules[j].getNom();
					ind = j;
				}
			}
			for (int j = 0; j < listModules.size(); j++)
			{
				if (listModules[j].getNom() == listModules[ind].getBouchra())
				{
					vector<int> inter(listModules[j].getSommets());
					inter.erase(find(inter.begin(), inter.end(), max));
					inter.push_back(i);
					listModules[j].setSommets(inter);
					break;
				}
			}
			for (int j = 0; j < listModules[ind].getSommets().size(); j++)
			{
				for (int k = 0; k < listModules.size(); k++)
				{
					if (listModules[ind].getSommets()[j] == listModules[k].getNom())
					{
						listModules[k].setBouchra(i);
						break;
					}
				}
			}
			listModules[ind].setNom(i);
		}
	}
	cout << "List avant Niveau : " << endl;
	afficher(listModules);
	map<int, vector<int> > mappingNiveaux = getNiveaux(listModules);
	nivMax = 0;
	for (map<int, vector<int> >::iterator it = mappingNiveaux.begin(); it != mappingNiveaux.end(); it++)
	{
		if (it->first > nivMax)
		{
			nivMax = it->first;
		}
	}
	cout << "Mapping Niveaux : " << endl;
	afficher(mappingNiveaux);

	Gq = construireGrapheQuotient(listModules, G);
	cout << "Graphe quotient :" << endl;
	afficher(Gq);
	cin >> c;

	cout << "Solution Finale:" << endl;
	afficher(listModules);
	
	cout << "ENTRER UN CARACTERE PUIS CLIQUER SUR ENTRER POUR FERMER." << endl;
	cin >> c;
	return 0;
}