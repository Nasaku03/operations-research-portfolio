#include <ilopl/iloopl.h>

#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <string>

#include "ModularDecomposition.h"
#include "Module.h"

using namespace std;

struct classcomp {
	bool operator() (const int& lhs, const int& rhs) const
	{
		return lhs>rhs;
	}
};

struct myclass {
	bool operator() (vector<int> i, vector<int> j) { return (i.size()>j.size()); }
} sortBySize;

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

void ecrireData2(vector<vector<int> >& A, unsigned int& m, unsigned int& n, unsigned int& delta, vector<int>& p)
{
	ofstream model;
	model.open("OCCP.dat", ofstream::trunc);
	model << "n= " << n << ";\nm= " << m << ";\ndelta= " << delta << ";\nA =[";
	for (unsigned int i = 0; i<m; i++)
	{
		model << "[ ";
		for (unsigned int j = 0; j<n; j++)
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

void cliquer(map<int, vector<int> >& Gq, vector<vector<int> >& Amodulaire, map<int, vector<vector<int> > >& listSolutionsModules, int& ind, vector<int>& adj, vector<vector<int> >& listX)
{
	for (int i = 0; i<adj.size(); i++)
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
		for (int j = 0; j<solutionModulaire[it->first].size(); j++)
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

map<int, vector<int> > gToListAdj2(vector<vector<int> >& A)
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

int SMSC_DNTS2(vector<vector<int> >& A, unsigned int& n, unsigned int& m, unsigned int& delta, vector<vector<int> > &solution, vector<int>& p)
{
	IloEnv env;
	int status(0);
	try
	{
		ecrireData2(A, m, n, delta, p);
		IloOplRunConfiguration rc1(env, "OCCP.mod", "OCCP.dat");
		rc1.getCplex().setParam(IloCplex::NodeSel, CPX_NODESEL_DFS);
		rc1.getCplex().setOut(env.getNullStream());
		rc1.getOplModel().generate();
		if (rc1.getCplex().solve())
		{
			IloIntMap t = rc1.getOplModel().getElement("x").asIntMap();
			solution.clear();
			for (IloInt i = 1; i <= t.getSize(); i++)
			{
				IloIntMap t_sub(t.getSub(i));
				vector<int> sous;
				for (IloInt j = 1; j <= t_sub.getSize(); j++)
				{
					sous.push_back(t_sub.get(j));
				}
				solution.push_back(sous);
			}

		}
		rc1.end();
	}
	catch (IloOplException & e) {
		cout << "### OPL exception: " << e.getMessage() << endl;
		status = 1;
	}
	catch (IloException & e) {
		cout << "### CONCERT exception: ";
		e.print(cout);
		status = 2;
	}
	catch (...) {
		cout << "### UNEXPECTED ERROR ..." << endl;
		status = 3;
	}
	env.end();
	return status;
}

void resolution_finale(vector<vector<int> >& listModules, map<int, vector<int> >& Gq, map<int, vector<vector<int> > >& solutionModulaire, vector<vector<int> >& Amodulaire, vector<vector<int> >& solution, vector<vector<int> >& listX)
{
	if (!Amodulaire.empty())
	{
		unsigned int n_(0), m_(0), delta_(0), coloring; vector<vector<int> > sousSolution, solutionFinale;
		for (int i = 0; i < solutionModulaire.size(); i++)
		{
			n_ += solutionModulaire[i].size();
		}
		//calculer somme mode

		vector<int> p;
		m_ = Amodulaire.size();
		//caclcule le degré de chaque sommet
		vector<int> deg; // Vecteur des degrés du graphe de la solution
		map<int, vector<int> > g(gToListAdj2(Amodulaire));   // graphe solution
															 //cout << "graphsolution" << endl;
															 //afficher(g);
															 //papillon(g, coloring, n_);
		for (int i = 0; i < g.size(); i++)
		{
			deg.push_back(g[i].size());
		}
		unsigned int ind;
		maxVec(deg, delta_, ind);
		//z_actuel = 326;

		//cout << "delta = " << delta_ << endl;
		//delta_ = (int)floor(((double)coloring+(double)delta_-1.0)/2.0);
		//delta_ = (int)ceil(((double)coloring + (double)delta_) / 2.0);
		//cout << "delta modifier = " << delta_ << endl;
		for (int i = 0; i < listX.size(); i++)
		{
			p.push_back(listX[i].size());
		}

		SMSC_DNTS2(Amodulaire, n_, m_, delta_, sousSolution, p);
		//cout << "***************************FIN SMSC2****************************" << endl;
		vector<vector<int> > recapSol;
		for (int k = 0; k < delta_; k++)
		{
			vector<int> inter;
			recapSol.push_back(inter);
		}
		for (int j = 0; j < sousSolution.size(); j++)
		{
			vector<int> inter;
			for (int k = 0; k < sousSolution[j].size(); k++)
			{
				if (sousSolution[j][k] == 1)
				{
					recapSol[k].push_back(j);
				}
			}
		}
		for (int k = delta_ - 1; k > -1; k--)
		{
			if (recapSol[k].empty())
			{
				recapSol.erase(recapSol.end() - 1);
			}
			else
			{
				break;
			}
		}
		//cout << "Solution SMSC2 " << endl;

		// Récupération de la  solution finale
		for (unsigned int i = 0; i < recapSol.size(); i++)
		{
			vector<int> sous;
			for (unsigned int j = 0; j < recapSol[i].size(); j++)
			{
				for (unsigned int k = 0; k < listX[recapSol[i][j]].size(); k++)
				{
					sous.push_back(listX[recapSol[i][j]][k]);
				}
			}
			solution.push_back(sous);
		}
	}
	else
	{
		//cout << "solution modulaire" << endl;
		//afficher(solutionModulaire);
		int max = 0;
		for (map<int, vector<vector<int> > >::iterator it = solutionModulaire.begin(); it != solutionModulaire.end(); it++)
		{
			//cout << "taille : " << it->second.size() << endl;
			if (max < it->second.size())
			{
				//cout << "here" << endl;
				max = it->second.size();
			}
			else
			{
				//cout << "no" << endl;
			}
		}
		//cout << "max = " << max << endl;
		for (int i = 0; i < max; i++)
		{
			vector<int> sous;
			for (map<int, vector<vector<int> > >::iterator it = solutionModulaire.begin(); it != solutionModulaire.end(); it++)
			{
				if (i < it->second.size())
				{
					for (int j = 0; j < it->second[i].size(); j++)
					{
						sous.push_back(it->second[i][j]);
					}
				}
			}
			solution.push_back(sous);
		}
		//cout << "sol : " << endl;
		//afficher(solution);
	}
}

vector<vector<int> > resolutionBottomUp(vector<vector<int> >& A, map<int, vector<int> >& G, vector<Module>& listModules, vector<vector<int> >& listModule_)
{
	int nivMax;
	vector<vector<int> > stables;
	map<int, vector<vector<int> > > linkBottomUp;
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
	{
		vector<int> inter(1, it->first);
		stables.push_back(inter);
	}
	//cout << "Stables 0 : " << endl;
	//afficher(stables);
	// Trier les modules par niveau bottom-up
	map<int, vector<vector<Module>::iterator>, classcomp> indicesModulesTrieParNiveau(trieModuleParNiveau(listModules, nivMax));
	// On commence l'attribution des couleurs aux sommets selon les règles fixées
	for (int i = nivMax - 1; i > -1; i--)
	{
		//cout << "niv = " << i << endl;
		for (int j = 0; j < indicesModulesTrieParNiveau[i].size(); j++)
		{
			//cout << "\tj = " << j << endl;
			//cout << *(indicesModulesTrieParNiveau[i][j]) << endl;
			if (!indicesModulesTrieParNiveau[i][j]->getTrivial())
			{
				if (indicesModulesTrieParNiveau[i][j]->getDegenerate() == 1)
				{
					//cout << "Parallele" << endl;
					if (linkBottomUp.empty())
					{
						//cout << "First Link" << endl;
						vector<int> inter;
						for (int k = 0; k < indicesModulesTrieParNiveau[i][j]->getSommets().size(); k++)
						{
							inter.push_back(indicesModulesTrieParNiveau[i][j]->getSommets()[k]);
							vector<int> f(1, indicesModulesTrieParNiveau[i][j]->getSommets()[k]);
							for (vector<vector<int> >::iterator it = stables.begin(); it != stables.end(); it++)
							{
								if (*it == f)
								{
									stables.erase(it);
									break;
								}
							}
						}
						linkBottomUp[indicesModulesTrieParNiveau[i][j]->getNom()].push_back(indicesModulesTrieParNiveau[i][j]->getSommets());
						stables.push_back(inter);
						//cout << "Link 0 :" << endl;
						//afficher(linkBottomUp);
						//cout << "Stables 1 : " << endl;
						//afficher(stables);
					}
					else
					{
						//cout << "In Link" << endl;
						vector<vector<int> > inter; int size(1);
						for (int k = 0; k < indicesModulesTrieParNiveau[i][j]->getSommets().size(); k++)
						{
							if (indicesModulesTrieParNiveau[i][j]->getSommets()[k] >= G.size())
							{
								if (linkBottomUp[indicesModulesTrieParNiveau[i][j]->getSommets()[k]].size() > size)
								{
									size = linkBottomUp[indicesModulesTrieParNiveau[i][j]->getSommets()[k]].size();
								}
							}
						}
						//cout << "size = " << size << endl;
						for (int k = 0; k < size; k++)
						{
							vector<int> in;
							inter.push_back(in);
						}
						for (int k = 0; k < indicesModulesTrieParNiveau[i][j]->getSommets().size(); k++)
						{
							//cout << "k = " << k << endl;
							if (indicesModulesTrieParNiveau[i][j]->getSommets()[k] < G.size())
							{
								//cout << "vrai sommet : " << indicesModulesTrieParNiveau[i][j]->getSommets()[k] << endl;
								inter[0].push_back(indicesModulesTrieParNiveau[i][j]->getSommets()[k]);
								vector<int> f(1, indicesModulesTrieParNiveau[i][j]->getSommets()[k]);
								for (vector<vector<int> >::iterator it = stables.begin(); it != stables.end(); it++)
								{
									if (*it == f)
									{
										stables.erase(it);
										break;
									}
								}
								//cout << "Stables 2 : " << endl;
								//afficher(stables);
							}
							else
							{
								//cout << "tree sommet : " << indicesModulesTrieParNiveau[i][j]->getSommets()[k] << endl;
								int pos(0);
								vector<vector<int> > vertices(linkBottomUp[indicesModulesTrieParNiveau[i][j]->getSommets()[k]]);
								//cout << "vertices : " << endl;
								//afficher(vertices);
								while (!vertices.empty())
								{
									int max = 0, ind = -1;
									for (int k = 0; k < vertices.size(); k++)
									{
										if (max < vertices[k].size())
										{
											max = vertices[k].size();
											ind = k;
										}
									}
									//cout << "max = " << max << ", ind = " << ind << endl;
									//cout << "list : " << endl;
									//afficher(vertices[ind]);
									for (int k = 0; k < vertices[ind].size(); k++)
									{
										inter[pos].push_back(vertices[ind][k]);
									}
									//cout << "inter here : " << endl;
									//afficher(inter);
									pos++;
									for (vector<vector<int> >::iterator it = stables.begin(); it != stables.end(); it++)
									{
										if (*it == vertices[ind])
										{
											stables.erase(it);
											break;
										}
									}
									vertices.erase(vertices.begin() + ind);
								}
							}
						}
						linkBottomUp[indicesModulesTrieParNiveau[i][j]->getNom()] = inter;
						for (int k = 0; k < inter.size(); k++)
						{
							stables.push_back(inter[k]);
						}
						//cout << "Link 1 :" << endl;
						//afficher(linkBottomUp);
						//cout << "Stables 3 : " << endl;
						//afficher(stables);
					}
				}
				else if (indicesModulesTrieParNiveau[i][j]->getDegenerate() == 2)
				{
					//cout << "serie" << endl;
					vector<vector<int> > inter;
					for (int k = 0; k < indicesModulesTrieParNiveau[i][j]->getSommets().size(); k++)
					{
						if (indicesModulesTrieParNiveau[i][j]->getSommets()[k] < G.size())
						{
							vector<int> f(1, indicesModulesTrieParNiveau[i][j]->getSommets()[k]);
							inter.push_back(f);
						}
						else
						{
							for (int l = 0; l < linkBottomUp[indicesModulesTrieParNiveau[i][j]->getSommets()[k]].size(); l++)
							{
								inter.push_back(linkBottomUp[indicesModulesTrieParNiveau[i][j]->getSommets()[k]][l]);
							}
						}
					}
					linkBottomUp[indicesModulesTrieParNiveau[i][j]->getNom()] = inter;
					//cout << "Link 2 :" << endl;
					//afficher(linkBottomUp);
				}
				else
				{
					//cout << "Prime" << endl;
					vector<vector<int> > solution;
					map<int, vector<vector<int> > > solutionModulaire;
					vector<vector<int> > listX;
					vector<vector<int> > Amodulaire;
					map<int, vector<int> > Gq;
					for (int k = 0; k < indicesModulesTrieParNiveau[i][j]->getSommets().size(); k++)
					{
						if (indicesModulesTrieParNiveau[i][j]->getSommets()[k] >= G.size())
						{
							solutionModulaire[indicesModulesTrieParNiveau[i][j]->getSommets()[k]] = linkBottomUp[indicesModulesTrieParNiveau[i][j]->getSommets()[k]];
						}
						else
						{
							vector<int> inter(1, indicesModulesTrieParNiveau[i][j]->getSommets()[k]);
							vector<vector<int> > inter2(1, inter);
							solutionModulaire[indicesModulesTrieParNiveau[i][j]->getSommets()[k]] = inter2;
						}
						vector<int> inter;
						Gq[indicesModulesTrieParNiveau[i][j]->getSommets()[k]] = inter;
					}
					for (int k = 0; k < indicesModulesTrieParNiveau[i][j]->getSommets().size(); k++)
					{
						int indS, indI;
						if (indicesModulesTrieParNiveau[i][j]->getSommets()[k] < G.size())
						{
							indS = indicesModulesTrieParNiveau[i][j]->getSommets()[k];
						}
						else
						{
							indS = linkBottomUp[indicesModulesTrieParNiveau[i][j]->getSommets()[k]][0][0];
						}
						for (int l = k + 1; l < indicesModulesTrieParNiveau[i][j]->getSommets().size(); l++)
						{
							if (indicesModulesTrieParNiveau[i][j]->getSommets()[l] < G.size())
							{
								indI = indicesModulesTrieParNiveau[i][j]->getSommets()[l];
							}
							else
							{
								indI = linkBottomUp[indicesModulesTrieParNiveau[i][j]->getSommets()[l]][0][0];
							}
							if (find(G[indS].begin(), G[indS].end(), indI) != G[indS].end())
							{
								Gq[indicesModulesTrieParNiveau[i][j]->getSommets()[k]].push_back(indicesModulesTrieParNiveau[i][j]->getSommets()[l]);
								Gq[indicesModulesTrieParNiveau[i][j]->getSommets()[l]].push_back(indicesModulesTrieParNiveau[i][j]->getSommets()[k]);
							}
						}
					}
					//cout << "Gq : " << endl;
					//afficher(Gq);
					graphe_de_solution(Gq, solutionModulaire, Amodulaire, listX);
					resolution_finale(listModule_, Gq, solutionModulaire, Amodulaire, solution, listX);
					linkBottomUp[indicesModulesTrieParNiveau[i][j]->getNom()] = solution;
					set<vector<int> > toAdd; bool in;
					for (vector<vector<int> >::iterator rit = stables.end() - 1; rit != stables.begin(); rit--)
					{
						in = false;
						for (vector<vector<int> >::iterator it = linkBottomUp[indicesModulesTrieParNiveau[i][j]->getNom()].begin(); it != linkBottomUp[indicesModulesTrieParNiveau[i][j]->getNom()].end(); it++)
						{
							if (contains(*it, *rit))
							{
								in = true;
								stables.erase(rit);
								toAdd.insert(*it);
								break;
							}
							if (in)
							{
								break;
							}
						}
					}
					in = false;
					for (vector<vector<int> >::iterator it = linkBottomUp[indicesModulesTrieParNiveau[i][j]->getNom()].begin(); it != linkBottomUp[indicesModulesTrieParNiveau[i][j]->getNom()].end(); it++)
					{
						if (contains(*it, *(stables.begin())))
						{
							in = true;
							stables.erase(stables.begin());
							toAdd.insert(*it);
							break;
						}
						if (in)
						{
							break;
						}
					}
					for (set<vector<int> >::iterator it = toAdd.begin(); it != toAdd.end(); it++)
					{
						stables.push_back(*it);
					}
					//cout << "Link 3 :" << endl;
					//afficher(linkBottomUp);
					//cout << "Stables 4 : " << endl;
					//afficher(stables);
				}
			}
		}
	}
	sort(stables.begin(), stables.end(), sortBySize);
	return stables;
}

void getCout(vector<vector<int> >& solution, unsigned int& z)
{
	z = 0;
	for (int i = 0; i<solution.size(); i++)
	{
		z += solution[i].size()*(i + 1);
	}
}

int main()
{
	vector<vector<int> > A, solution;
	unsigned int n, m, delta;
	map<int, vector<int> > G, Gq;
	unsigned int z;
	vector<Module> listModules;
	char c;

	// Lecture des données
	//lireData(A, m, n, delta);

	lectureDimacs(n, m, delta, A);
	//simplification(A, m);

	// Initialization

	G = gToListAdj(A);

	float debut = clock();
	// décomposition et création de graphe quotion
	cout << "Decomposition:" << endl;
	decomposition(G, Gq, listModules, n);

	cout << "List des Modules Finales : " << endl;
	afficher(listModules);

	vector<vector<int> > listModules_(modulesToList(listModules));
	cout << "List des Modules Finales en listes : " << endl;
	afficher(listModules_);
	//cin >> c;

	// Début de la résolution
	solution = resolutionBottomUp(A, G, listModules, listModules_);

	float fin = (float)(clock() - debut) / CLOCKS_PER_SEC;
	// Solution finale
	cout << "Solution finale" << endl;
	afficher(solution);

	getCout(solution, z);

	cout << "Z = " << z << endl;

	cout << "Sequence : (";
	for (int i = 0; i < solution.size(); i++)
	{
		cout << solution[i].size() << " ";
	}
	cout << ")" << endl;

	cout << "s(G) = " << solution.size() << endl;

	cout << "temps = " << fin << endl;

	cout << "ENTRER UN CARACTERE POUR FERMER..." << endl;
	cin >> c;

	return 0;
}