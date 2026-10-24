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

void getCout(vector<vector<int> >& solution, unsigned int& z)
{
	z = 0;
	for (int i = 0; i<solution.size(); i++)
	{
		z += solution[i].size()*(i + 1);
	}
}

void createOCCPModel(IloModel model, IloNumVarArray x, IloRangeArray c, vector<vector<int> >& A, unsigned int& m, unsigned int& n, unsigned int& delta, vector<int>& p)
{
	IloEnv env = model.getEnv();
	for (int j = 1; j <= delta; j++)	
	{
		for (int i = 1; i <= n; i++)
		{
			ostringstream oss;
			oss << "x_" << i << "_" << j;
			x.add(IloBoolVar(env, oss.str().c_str()));
		}
	}
	model.add(x);
	for (int j = 0; j < delta; j++)
	{
		for (int i = 0; i < m; i++)
		{
			ostringstream oss;
			oss << "adj_" << i << "_" << j;
			IloExpr exp(env);
			for (int k = 0; k < n; k++)
			{
				exp += A[i][k] * x[j*n+k];
			}
			c.add(IloRange(env, exp, 1, oss.str().c_str()));
		}
	}
	for (int k = 0; k < n; k++)
	{
		ostringstream oss;
		oss << "col_" << (k + 1);
		IloExpr exp(env);
		for (int j = 0; j < delta; j++)
		{
			exp += x[j*n + k];
		}
		c.add(IloRange(env, 1, exp, 1, oss.str().c_str()));
	}
	model.add(c);
	IloExpr obj(env);
	for(int i = 0; i < delta; i++)
	{
		for (int j = 0; j < n; j++)
		{
			obj += (i + 1)*p[j] * x[i*n + j];
		}
	}
	model.add(IloMinimize(env, obj));
}

int SMSC_DNTS2(vector<vector<int> >& A, unsigned int& n, unsigned int& m, unsigned int& delta, vector<vector<vector<int> > >& solution, vector<int>& p)
{
	IloEnv env;
	int status(0);
	try
	{
		IloModel model(env, "example");

		IloNumVarArray var(env);
		IloRangeArray  rng(env);
		//populatebycolumn(model, var, rng);
		createOCCPModel(model, var, rng, A, m, n, delta, p);

		IloCplex cplex(model);
		cplex.setParam(IloCplex::NodeSel, CPX_NODESEL_DFS);
		cplex.setOut(env.getNullStream());

		vector<vector<int> > souSol;
		cplex.exportModel("model.lp");
		if (cplex.solve())
		{
			IloNumArray vals(env);
			cplex.getValues(vals, var);
			solution.clear();
			vector<vector<int> > sousSolution;
			for (IloInt i = 0; i < vals.getSize(); i+=n)
			{
				vector<int> sous;
				for (IloInt j = 0; j < n; j++)
				{
					sous.push_back(vals[i + j]);
				}
				souSol.push_back(sous);
			}
			unsigned int z = cplex.getObjValue();
			int ind(0); unsigned int newZ(z);
			do
			{
				if (newZ == z)
				{
					vector<vector<int> > in(souSol);
					solution.push_back(in);
					// Ajouter la contrainte en éliminant les solutions actuelles
					IloExpr newConst(model.getEnv());
					for (int i = 0; i < vals.getSize(); i++)
					{
						if (vals[i] == 0)
						{
							newConst += var[i];
						}
					}
					souSol.clear();
					rng.add(IloRange(env, 1, newConst, IloInfinity));
					model.add(rng);
					if (cplex.solve())
					{
						cplex.getValues(vals, var);
						for (IloInt i = 0; i < vals.getSize(); i += n)
						{
							vector<int> sous;
							for (IloInt j = 0; j < n; j++)
							{
								sous.push_back(vals[i + j]);
							}
							souSol.push_back(sous);
						}
						newZ = cplex.getObjValue();
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			} while (true);
		}
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

void resolution_finale(vector<vector<int> >& listModules, map<int, vector<int> >& Gq, map<int, vector<vector<int> > >& solutionModulaire, vector<vector<int> >& Amodulaire, vector<vector<vector<int> > >& solution, vector<vector<int> >& listX)
{
	if (!Amodulaire.empty())
	{
		unsigned int n_(0), m_(0), delta_(0), coloring; vector<vector<vector<int> > > sousSolution;
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
		for (int b = 0; b < sousSolution.size(); b++)
		{
			//cout << "b = " << b << endl;
			//afficher(sousSolution[b]);
			vector<vector<int> > recapSol;
			for (int k = 0; k < delta_; k++)
			{
				vector<int> inter;
				recapSol.push_back(inter);
			}
			//cout << "delta_ = " << delta_ << endl;
			//cout << "recapSol debut : " << endl;
			//afficher(recapSol);
			//cout << "here 1 " << endl;
			for (int j = 0; j < sousSolution[b].size(); j++)
			{
				//cout << "j = " << j << endl;
				//cout << "sousSolution[b][j].size() = " << sousSolution[b][j].size() << endl;
				for (int k = 0; k < sousSolution[b][j].size(); k++)
				{
					//cout << "k = " << k << endl;
					//cout << "sousSolution[b][j][k] = " << sousSolution[b][j][k] << endl;
					if (sousSolution[b][j][k] == 1)
					{
						recapSol[j].push_back(k);
						//cout << "d5alna" << endl;
					}
				}
			}
			//cout << "recapSol debut : " << endl;
			//afficher(recapSol);
			//cout << "here 2 " << endl;
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
			//cout << "here 3 " << endl;
			//cout << "Solution SMSC2 " << endl;

			vector<vector<int> > sousSol;
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
				sousSol.push_back(sous);
			}
			solution.push_back(sousSol);
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
		vector<vector<int> > sousSol;
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
			sousSol.push_back(sous);
		}
		solution.push_back(sousSol);
		//cout << "sol : " << endl;
		//afficher(solution);
	}
}

void updateSolutions(vector<vector<vector<int> > >& stables, vector<map<int, vector<vector<int> > > >& linkBottomUp)
{
	if (stables.size() != 1)
	{
		vector<int> couts;
		for (int i = 0; i < stables.size(); i++)
		{
			sort(stables[i].begin(), stables[i].end(), sortBySize);
			int somme(0);
			for (int j = 0; j < stables[i].size(); j++)
			{
				somme += stables[i][j].size() * (j + 1);
			}
			couts.push_back(somme);
		}
		int min = couts[0];
		for (int i = 1; i < couts.size(); i++)
		{
			if (min > couts[i])
			{
				min = couts[i];
			}
		}
		for (int i = couts.size() - 1; i > -1; i--)
		{
			if (couts[i] != min)
			{
				stables.erase(stables.begin() + i);
				linkBottomUp.erase(linkBottomUp.begin() + i);
			}
		}
	}
}

vector<vector<int> > resolutionBottomUp(vector<vector<int> >& A, map<int, vector<int> >& G, vector<Module>& listModules, vector<vector<int> >& listModule_)
{
	int nivMax;
	vector<vector<vector<int> > > stables; vector<vector<int> > souStable; stables.push_back(souStable);
	vector<map<int, vector<vector<int> > > > linkBottomUp; map<int, vector<vector<int> > > sousLink; linkBottomUp.push_back(sousLink);
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
	{
		vector<int> inter(1, it->first);
		stables[0].push_back(inter);
	}
	char c;
	cout << "Debut Resolution : " << endl;
	cin >> c;
	cout << "Stables 0 : " << endl;
	afficher(stables[0]);
	// Trier les modules par niveau bottom-up
	map<int, vector<vector<Module>::iterator>, classcomp> indicesModulesTrieParNiveau(trieModuleParNiveau(listModules, nivMax));
	// On commence l'attribution des couleurs aux sommets selon les règles fixées
	for (int i = nivMax - 1; i > -1; i--)
	{
		cout << "niv = " << i << endl;
		for (int j = 0; j < indicesModulesTrieParNiveau[i].size(); j++)
		{
			cout << "\tj = " << j << endl;
			cout << *(indicesModulesTrieParNiveau[i][j]) << endl;
			if (!indicesModulesTrieParNiveau[i][j]->getTrivial())
			{
				if (indicesModulesTrieParNiveau[i][j]->getDegenerate() == 1)
				{
					cout << "Parallele" << endl;
					if (linkBottomUp[0].empty())
					{
						cout << "First Link" << endl;
						vector<int> inter;
						for (int k = 0; k < indicesModulesTrieParNiveau[i][j]->getSommets().size(); k++)
						{
							inter.push_back(indicesModulesTrieParNiveau[i][j]->getSommets()[k]);
							vector<int> f(1, indicesModulesTrieParNiveau[i][j]->getSommets()[k]);
							for (int b = 0; b < stables.size(); b++) 
							{
								for (vector<vector<int> >::iterator it = stables[b].begin(); it != stables[b].end(); it++)
								{
									if (*it == f)
									{
										stables[b].erase(it);
										break;
									}
								}
							}
						}
						for (int b = 0; b < linkBottomUp.size(); b++) 
						{
							linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getNom()].push_back(indicesModulesTrieParNiveau[i][j]->getSommets());
							stables[b].push_back(inter);
							cout << "Link 0 :" << b << endl;
							afficher(linkBottomUp[b]);
							cout << "Stables 1 : " << b << endl;
							afficher(stables[b]);
						}
					}
					else
					{
						cout << "In Link" << endl;
						for (int b = 0; b < stables.size(); b++) 
						{
							vector<vector<int> > inter; int size(1);
							for (int k = 0; k < indicesModulesTrieParNiveau[i][j]->getSommets().size(); k++)
							{
								if (indicesModulesTrieParNiveau[i][j]->getSommets()[k] >= G.size())
								{
									if (linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getSommets()[k]].size() > size)
									{
										size = linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getSommets()[k]].size();
									}
								}
							}
							cout << "size = " << size << endl;
							for (int k = 0; k < size; k++)
							{
								vector<int> in;
								inter.push_back(in);
							}
							for (int k = 0; k < indicesModulesTrieParNiveau[i][j]->getSommets().size(); k++)
							{
								cout << "k = " << k << endl;
								if (indicesModulesTrieParNiveau[i][j]->getSommets()[k] < G.size())
								{
									cout << "vrai sommet : " << indicesModulesTrieParNiveau[i][j]->getSommets()[k] << endl;
									inter[0].push_back(indicesModulesTrieParNiveau[i][j]->getSommets()[k]);
									vector<int> f(1, indicesModulesTrieParNiveau[i][j]->getSommets()[k]);									for (vector<vector<int> >::iterator it = stables[b].begin(); it != stables[b].end(); it++)
									{
										if (*it == f)
										{
											stables[b].erase(it);
											break;
										}
									}
									cout << "Stables 2 : " << endl;
									afficher(stables[b]);
								}
								else
								{
									cout << "tree sommet : " << indicesModulesTrieParNiveau[i][j]->getSommets()[k] << endl;
									int pos(0);
									vector<vector<int> > vertices(linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getSommets()[k]]);
									cout << "vertices : " << endl;
									afficher(vertices);
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
										cout << "max = " << max << ", ind = " << ind << endl;
										cout << "list : " << endl;
										afficher(vertices[ind]);
										for (int k = 0; k < vertices[ind].size(); k++)
										{
											inter[pos].push_back(vertices[ind][k]);
										}
										cout << "inter here : " << endl;
										afficher(inter);
										pos++;
										for (vector<vector<int> >::iterator it = stables[b].begin(); it != stables[b].end(); it++)
										{
											if (*it == vertices[ind])
											{
												stables[b].erase(it);
												break;
											}
										}
										vertices.erase(vertices.begin() + ind);
									}
								}
							}
							linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getNom()] = inter;
							for (int k = 0; k < inter.size(); k++)
							{
								stables[b].push_back(inter[k]);
							}
							cout << "Link 1 :" << endl;
							afficher(linkBottomUp[b]);
							cout << "Stables 3 : " << endl;
							afficher(stables[b]);
						}
						// Mise à jour des solutions
						updateSolutions(stables, linkBottomUp);
					}
				}
				else if (indicesModulesTrieParNiveau[i][j]->getDegenerate() == 2)
				{
					cout << "serie" << endl;
					for (int b = 0; b < stables.size(); b++) {
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
								for (int l = 0; l < linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getSommets()[k]].size(); l++)
								{
									inter.push_back(linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getSommets()[k]][l]);
								}
							}
						}
						linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getNom()] = inter;
						//cout << "Link 2 :" << endl;
						//afficher(linkBottomUp[b]);
					}
					// mise à jour des solutions
					updateSolutions(stables, linkBottomUp);
				}
				else
				{
					cout << "Prime" << endl;
					int bSize = stables.size();
					for (int b = 0; b < bSize; b++) {
						vector<vector<vector<int> > > solution;
						map<int, vector<vector<int> > > solutionModulaire;
						vector<vector<int> > listX;
						vector<vector<int> > Amodulaire;
						map<int, vector<int> > Gq;
						for (int k = 0; k < indicesModulesTrieParNiveau[i][j]->getSommets().size(); k++)
						{
							if (indicesModulesTrieParNiveau[i][j]->getSommets()[k] >= G.size())
							{
								solutionModulaire[indicesModulesTrieParNiveau[i][j]->getSommets()[k]] = linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getSommets()[k]];
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
								indS = linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getSommets()[k]][0][0];
							}
							for (int l = k + 1; l < indicesModulesTrieParNiveau[i][j]->getSommets().size(); l++)
							{
								if (indicesModulesTrieParNiveau[i][j]->getSommets()[l] < G.size())
								{
									indI = indicesModulesTrieParNiveau[i][j]->getSommets()[l];
								}
								else
								{
									indI = linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getSommets()[l]][0][0];
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
						vector<vector<int> > stables2(stables[b]);
						map<int, vector<vector<int> > > linkBottomUp2(linkBottomUp[b]);
						for (int h = 0; h < solution.size(); h++)
						{
							if (h == 0)
							{
								linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getNom()] = solution[h];
								set<vector<int> > toAdd; bool in;
								for (vector<vector<int> >::iterator rit = stables[b].end() - 1; rit != stables[b].begin(); rit--)
								{
									in = false;
									for (vector<vector<int> >::iterator it = linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getNom()].begin(); it != linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getNom()].end(); it++)
									{
										if (contains(*it, *rit))
										{
											in = true;
											stables[b].erase(rit);
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
								for (vector<vector<int> >::iterator it = linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getNom()].begin(); it != linkBottomUp[b][indicesModulesTrieParNiveau[i][j]->getNom()].end(); it++)
								{
									if (contains(*it, *(stables[b].begin())))
									{
										in = true;
										stables[b].erase(stables[b].begin());
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
									stables[b].push_back(*it);
								}
								//cout << "Link 3 :" << endl;
								//afficher(linkBottomUp[b]);
								//cout << "Stables 4 : " << endl;
								//afficher(stables[b]);
							}
							else
							{
								//cout << "stables2: " << endl;
								//afficher(stables2);
								//cout << "linkBottomUp2: " << endl;
								//afficher(linkBottomUp2);
								stables.push_back(stables2);
								linkBottomUp.push_back(linkBottomUp2);
								linkBottomUp[linkBottomUp.size()-1][indicesModulesTrieParNiveau[i][j]->getNom()] = solution[h];
								set<vector<int> > toAdd; bool in;
								for (vector<vector<int> >::iterator rit = stables[stables.size() - 1].end() - 1; rit != stables[stables.size() - 1].begin(); rit--)
								{
									in = false;
									for (vector<vector<int> >::iterator it = linkBottomUp[linkBottomUp.size() - 1][indicesModulesTrieParNiveau[i][j]->getNom()].begin(); it != linkBottomUp[linkBottomUp.size() - 1][indicesModulesTrieParNiveau[i][j]->getNom()].end(); it++)
									{
										if (contains(*it, *rit))
										{
											in = true;
											stables[stables.size() - 1].erase(rit);
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
								for (vector<vector<int> >::iterator it = linkBottomUp[linkBottomUp.size() - 1][indicesModulesTrieParNiveau[i][j]->getNom()].begin(); it != linkBottomUp[linkBottomUp.size() - 1][indicesModulesTrieParNiveau[i][j]->getNom()].end(); it++)
								{
									if (contains(*it, *(stables[stables.size() - 1].begin())))
									{
										in = true;
										stables[stables.size() - 1].erase(stables[stables.size() - 1].begin());
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
									stables[stables.size() - 1].push_back(*it);
								}
								//cout << "Link 4 :" << endl;
								//afficher(linkBottomUp[stables.size() - 1]);
								//cout << "Stables 5 : " << endl;
								//afficher(stables[stables.size() - 1]);
							}
						}						
					}
				}
			}
		}
	}
	sort(stables[0].begin(), stables[0].end(), sortBySize);
	return stables[0];
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