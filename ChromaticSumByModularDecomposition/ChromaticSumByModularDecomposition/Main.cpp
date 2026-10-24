#include <ilopl/iloopl.h>

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cmath>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>

#include "Module.h"
#include "ModularDecomposition.h"

using namespace std;

vector<int> listNomsModules;

void ecrireData(map<int, vector<int> > G)
{
	ofstream model;
	model.open("graphe.dat", ofstream::trunc);
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
	{
		model << it->first << ": ";
		for (int i = 0; i < it->second.size(); i++)
		{
			model << it->second[i] << ", ";
		}
		model << endl;
	}
	model.close();
}

void ecrireData(vector<vector<int> >& A, unsigned int& m, unsigned int& n, unsigned int& delta)
{
	ofstream model;
	model.open("ChromaticSum.dat", ofstream::trunc);
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
			model << "]];";
		}

	}
	model.close();
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

void papillon(map<int, vector<int> >& g_, unsigned int& coloring, unsigned int& n)
{
	vector<int> deg;
	map<int, vector<int> > gInd, g(g_);
	/*for(map<int, vector<int> >::iterator it=g.begin(); it!=g.end(); it++)
	{
	cout << it->first << " : ";
	for(vector<int>::iterator it2=it->second.begin(); it2!=it->second.end(); it2++)
	{
	cout << *it2 << " ";
	}
	cout << '\n';
	}*/
	for (map<int, vector<int> >::iterator it = g.begin(); it != g.end(); it++)
	{
		deg.push_back(it->second.size());
	}/*
	 cout << "Degres : " << endl;
	 for(int i=0; i<deg.size(); i++)
	 {
	 cout << deg[i] << " ";
	 }
	 cout << '\n';*/
	unsigned int ind;
	int min_;
	for (int i = 0; i<n; i++)
	{
		/*cout << "graphe actuel : " << endl;
		for(map<int, vector<int> >::iterator it=g.begin(); it!=g.end(); it++)
		{
		cout << it->first << " : ";
		for(vector<int>::iterator it2=it->second.begin(); it2!=it->second.end(); it2++)
		{
		cout << *it2 << " ";
		}
		cout << '\n';
		}
		cout << "Degres : " << endl;
		for(int i=0; i<deg.size(); i++)
		{
		cout << deg[i] << " ";
		}*/
		minVec(deg, min_, ind);
		//cout << "min = " << min_ << endl;
		//cout << "ind = " << ind << endl;
		gInd[ind + 1] = g[ind + 1];
		deg[ind] = n;
		map<int, vector<int> >::iterator it = g.begin();
		for (int j = 1; j<ind; j++)
		{
			if (it->first != ind + 1)
			{
				it++;
			}
			else
			{
				break;
			}
		}
		min_ = it->first;
		//cout << "sommet : " << min_ << endl;
		g.erase(ind + 1);
		for (map<int, vector<int> >::iterator it = g.begin(); it != g.end(); it++)
		{
			for (vector<int>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
			{
				if (*it2 == min_)
				{
					it->second.erase(it2);
					deg[(it->first) - 1]--;
					//cout << "\tin sommet : " << it->first << endl;
					break;
				}
			}
		}
	}
	deg.clear();/*
				cout << "Graphe oriente : " << endl;
				for(map<int, vector<int> >::iterator it=gInd.begin(); it!=gInd.end(); it++)
				{
				cout << it->first << " : ";
				for(vector<int>::iterator it2=it->second.begin(); it2!=it->second.end(); it2++)
				{
				cout << *it2 << " ";
				}
				cout << '\n';
				}*/
	for (map<int, vector<int> >::iterator it = gInd.begin(); it != gInd.end(); it++)
	{
		deg.push_back(it->second.size());
	}
	maxVec(deg, coloring, ind);
	cout << " fin papillon " << endl;
}

void simplification(vector<vector<int> >& A)
{
	cout << "simplification" << endl;
	bool same; vector<int> ligne;
	for (int i = 0; i<A.size() - 1; i++)
	{
		cout << "i = " << i << endl;
		for (int j = i + 1; j<A.size(); j++)
		{
			cout << "j = " << j << endl;
			same = true;
			for (int k = 0; k<A[0].size(); k++)
			{
				if (A[i][k] != A[j][k])
				{
					same = false;
					break;
				}
			}
			if (same)
			{
				ligne.push_back(j);
			}
		}
	}
	vector<vector<int> >::iterator it = A.end() - 1;
	for (int i = A.size() - 1; i>-1; i--)
	{
		cout << "to erase = " << i << endl;
		if (containsValue(ligne, i))
		{
			cout << "YES" << endl;
			A.erase(it);
		}
		it--;
	}
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

map<int, vector<int> > grapheInduit(map<int, vector<int> >& g, vector<int>& v1, vector<int>& v2)
{
	map<int, vector<int> > gi;
	/*cout << "v1: ";
	for(int i=0; i<v1.size(); i++)
	{
	cout << v1[i] << " ";
	}
	cout << "\nv2: ";
	for(int i=0; i<v2.size(); i++)
	{
	cout << v2[i] << " ";
	}*/
	for (int i = 0; i<g.size(); i++)
	{
		if (containsValue(v1, i) || containsValue(v2, i))
		{
			gi[i] = g[i];
		}
	}

	for (int i = 0; i<v1.size(); i++)
	{
		for (vector<int>::iterator j = gi[v1[i]].begin(); j != gi[v1[i]].end(); j++)
		{
			if (!containsValue(v2, *j))
			{
				gi[v1[i]].erase(j);
				--j;
			}
		}
	}
	for (int i = 0; i<v2.size(); i++)
	{
		for (vector<int>::iterator j = gi[v2[i]].begin(); j != gi[v2[i]].end(); j++)
		{
			if (!containsValue(v1, *j))
			{
				gi[v2[i]].erase(j);
				--j;
			}
		}
	}
	/*cout << "\nGraphe gi : " << endl;
	afficher(gi);*/
	return gi;
}

void getCout(vector<vector<int> >& solution, unsigned int& z)
{
	z = 0;
	for (int i = 0; i<solution.size(); i++)
	{
		z += solution[i].size()*(i + 1);
	}
}

void getCout2(vector<vector<int> >& solution, unsigned int& z, vector<int>& p)
{
	int ss;

	for (int t = 0; t<solution.size(); t++)
	{
		ss = 0;
		for (int v = 0; v<solution[t].size(); v++)
		{
			ss += p[solution[t][v]];
		}
		z += ss*(t + 1);
	}
}

int SMSC_DNTS(vector<vector<int> >& A, unsigned int& n, unsigned int& m, unsigned int& delta, vector<vector<int> > &solution)
{
	IloEnv env;
	int status(0);
	try
	{
		ecrireData(A, m, n, delta);
		IloOplRunConfiguration rc1(env, "ChromaticSum.mod", "ChromaticSum.dat");
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

bool nonSingleton(vector<vector<int> >& P)
{
	cout << "non singleton begin" << endl;
	for (int i = 0; i<P.size(); i++)
	{
		if (P[i].size()>1)
		{
			cout << "la partie est non singleton" << endl;
			return true;
		}
	}
	cout << "la partie est singleton" << endl;
	return false;
}

bool chauvauge2(vector<int>& adj, vector<int>& pj, bool adjacent)
{
	//cout << "begin chauvauge 2" << endl;
	//cout << "Intersection : ";
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

	if (adjacent && inter.size() == adj.size() - 1 && inter.size() == pj.size() - 1)
	{
		return true;
	}
	else if (!adjacent && inter.size() == adj.size() && inter.size() == pj.size())
	{
		return true;
	}
	return false;
}

bool chauvauge3(vector<int>& adj, vector<int>& pj)
{
	//cout << "begin chauvauge 3" << endl;
	//cout << "Intersection : ";
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

	if (inter.empty())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void optimiser_Gq(map<int, vector<int> >& G, map<int, vector<int> >& Gq, vector<vector<int> >& listModules, vector<int>& ordre)
{
	vector<int> ordre2;
	for (int i = 0; i<listModules.size(); i++)
	{
		ordre2.push_back(i);
	}
	map<int, vector<int> > Gquotient;
	vector<vector<int> > listModules2;
	vector<int> module;
	module.push_back(ordre2[0]);
	// tester l'adjacence avant toutt
	for (int i = 1; i<ordre2.size(); i++) {
		bool adjacent(false);
		for (int j = 0; j<Gq[ordre2[i]].size(); j++)
		{
			if (Gq[ordre2[i]][j] == ordre2[i - 1])
			{
				adjacent = true;
				cout << ordre2[i] << " est adjacent à: " << ordre2[i - 1] << endl;
				break;
			}
		}
		if (chauvauge2(Gq[ordre2[i]], Gq[ordre2[i - 1]], adjacent))
		{
			module.push_back(ordre2[i]);
		}
		else
		{
			listModules2.push_back(module);
			module.clear();
			cout << " le premier elm de nouveau module est" << ordre2[i] << endl;
			module.push_back(ordre2[i]);
		}
	}
	listModules2.push_back(module);
	cout << "Liste des modules quotient : " << endl;
	for (int i = 0; i<listModules2.size(); i++)
	{
		cout << "M" << i << " : ";
		for (int j = 0; j<listModules2[i].size(); j++)
		{
			cout << listModules2[i][j] << " ";
		}
		cout << '\n';
	}
	vector<vector<int> > listModules3;
	for (int i = 0; i<listModules2.size(); i++)
	{
		module.clear();
		for (int j = 0; j<listModules2[i].size(); j++)
		{
			for (int k = 0; k<listModules[listModules2[i][j]].size(); k++)
			{
				module.push_back(listModules[listModules2[i][j]][k]);
			}
		}
		listModules3.push_back(module);
	}
	listModules = listModules3;
	// n5arjo Gquotient (adjacence entre les sommets)   aaa
	//on construit une copie de Gquotient apartire de liste des module
	cout << "Liste des modules : " << endl;
	for (int i = 0; i<listModules.size(); i++)
	{
		cout << "M" << i << " : ";
		for (int j = 0; j<listModules[i].size(); j++)
		{
			cout << listModules[i][j] << " ";
		}
		cout << '\n';
	}
	module.clear();
	for (int j = 0; j<listModules.size(); j++)
	{
		vector<int> adj = G[listModules[j][0]];
		for (int nb = 0; nb<listModules.size(); nb++)
		{
			if (nb != j)
			{
				if (chauvauge3(adj, listModules[nb]))
				{
					module.push_back(nb);
				}
			}
		}
		// chaque modul j en va ajouté a lui le vecteur des modules adjacent à lui

		Gquotient[j] = module;
		module.clear();
	}
	Gq = Gquotient;
}

void sousList(int cible, vector<Module>& modules, unsigned int& n, vector<int>& inter)
{
	for (int i = 0; i < modules[cible].getSommets().size(); i++)
	{
		if (modules[cible].getSommets()[i] < n)
		{
			inter.push_back(modules[cible].getSommets()[i]);
		}
		else
		{
			sousList(modules[cible].getSommets()[i], modules, n, inter);
		}
	}
}

vector<vector<int> > construireListeModuleFinale(map<int, vector<int> >& Gq, vector<Module>& modules, unsigned int& n)
{
	vector<vector<int> > listModules;
	for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); it++)
	{
		vector<int> inter;
		listNomsModules.push_back(it->first);
		if (it->first < n)
		{
			inter.push_back(it->first);
		}
		else
		{
			sousList(it->first, modules, n, inter);
		}
		listModules.push_back(inter);
	}
	return listModules;
}

void sous_graph(vector<vector<int> >& A, vector<int>& listModules, vector<vector<int> >& Ainduite, vector<int>& listIndices, unsigned int& n_, unsigned int& m_, unsigned int& delta_)
{
	cout << "sous graph" << endl;
	n_ = listModules.size();
	/*cout << "module actuel : ";
	for (int i = 0; i<listModules.size(); i++)
	{
		cout << listModules[i] << ' ';
	}
	cout << "\n";*/
	vector<int> nonExist;
	listIndices = listModules;
	sort(listIndices.begin(), listIndices.end());
	/*cout << "list Indices : ";
	for (int i = 0; i<listIndices.size(); i++)
	{
		cout << listIndices[i] << " ";
	}
	cout << '\n';*/
	for (int i = 0; i<A[0].size(); i++)
	{
		bool  exist = false;
		for (int j = 0; j<listModules.size(); j++)
		{
			if (listModules[j] == i)
			{
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			nonExist.push_back(i);
		}
	}
	cout << "suppression des colonnes : " << endl;
	/*cout << "non exist : ";
	for (int i = 0; i<nonExist.size(); i++)
	{
		cout << nonExist[i] << ' ';
	}
	cout << "\n";*/
	for (int k = nonExist.size() - 1; k>-1; k--)
	{
		for (int j = Ainduite[0].size() - 1; j>-1; j--)
		{
			if (nonExist[k] == j)
			{
				for (int i = 0; i<Ainduite.size(); i++)
				{
					vector<int>::iterator it = Ainduite[i].begin() + j;
					Ainduite[i].erase(it);
				}
			}
		}
	}
	cout << "Ainduite_1" << endl;
	//afficher(Ainduite);
	nonExist.clear();
	for (int i = 0; i<Ainduite.size(); i++)
	{
		nonExist.push_back(0);
		for (int j = 0; j<Ainduite[i].size(); j++)
		{
			nonExist[i] += Ainduite[i][j];
		}
	}
	cout << "Suppression des lignes : " << endl;
	/*cout << "non exist : ";
	for (int i = 0; i<nonExist.size(); i++)
	{
		cout << nonExist[i] << ' ';
	}
	cout << "\n";*/
	for (int i = nonExist.size() - 1; i>-1; i--)
	{
		if (nonExist[i] != 2)
		{
			vector<vector<int> >::iterator it = Ainduite.begin() + i;
			Ainduite.erase(it);
		}
	}
	cout << "Ainduite_2" << endl;
	//afficher(Ainduite);
	m_ = Ainduite.size();
	nonExist.clear();
	for (int j = 0; j<Ainduite[0].size(); j++)
	{
		nonExist.push_back(0);
		for (int i = 0; i<Ainduite.size(); i++)
		{
			nonExist[j] += Ainduite[i][j];
		}
	}
	cout << "calcul delta : " << endl;
	/*cout << "non exist : ";
	for (int i = 0; i<nonExist.size(); i++)
	{
		cout << nonExist[i] << ' ';
	}
	cout << "\n";*/
	unsigned int max_;
	maxVec(nonExist, delta_, max_);
	cout << "DELTA = " << delta_ << endl;
}

void resolution_modules(vector<vector<int> >& A, vector<vector<int> >& listModules, map<int, vector<vector<int> > >& solutionModulaire)
{
	for (int i = 0; i<listModules.size(); i++)
	{
		cout << "///////////////////////// Résolution modulaire n : " << (i + 1) << endl;
		if (listModules[i].size() != 1)
		{
			vector<vector<int> > Ainduite(A); unsigned int n_, m_, delta_, coloring; vector<vector<int> > sousSolution, solutionFinale;
			vector<int> listIndices;
			sous_graph(A, listModules[i], Ainduite, listIndices, n_, m_, delta_);
			//z_actuel = 326;
			cout << "n_ = " << n_ << endl;
			cout << "m_ = " << m_ << endl;
			cout << "Ainduite : " << endl;
			//afficher(Ainduite);
			map<int, vector<int> > g(gToListAdj(Ainduite));
			cout << "Graphe G induit : " << endl;
			afficher(g);
			if (vgStable(g))
			{
				cout << "Stable" << endl;
				solutionFinale.push_back(listModules[i]);
				solutionModulaire[i] = solutionFinale;
				continue;
			}
			else
			{
				cout << "g n'est pas stable" << endl;
			}
			delta_++;
			// A revoir
			/*papillon(g, coloring, n_);
			cout << "mahbool delta" << endl;
			cout << "delta = " << delta_ << endl;
			cout << "coloring = " << coloring << endl;
			delta_ = (int)ceil(((double)coloring + (double)delta_) / 2.0);
			cout << "delta modifie = " << delta_ << endl;
			*/
			/* delta_ = (int)floor(((double)coloring+(double)delta_-1.0)/2.0);
			papillon rek sur beli tamchi belek lé entreé diyawelha bek faux
			mala 5aliha dm1 rek ta3ban wna3sen ih 3lah nta mar9adch §3lah
			ih nhat rasi wa5laswin r9adtr9adt
			nnnormal nn ma3lich kol 3atla fiha 5ir ana flil wnata fnhar hhhm5alfiin f regimok aya bn8
			nchalah  nn bah nachfaw
			bn8
			*/
			SMSC_DNTS(Ainduite, n_, m_, delta_, sousSolution);
			cout << "***************************FIN SMSC****************************" << endl;
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
			cout << "Solution SMSC " << endl;
			for (int j = 0; j<recapSol.size(); j++)
			{
				vector<int> inter;
				for (int k = 0; k<recapSol[j].size(); k++)
				{
					inter.push_back(listIndices[recapSol[j][k]]);
					cout << listIndices[recapSol[j][k]] << " ";
				}
				cout << endl;
				solutionFinale.push_back(inter);
			}
			solutionModulaire[i] = solutionFinale;
		}
		else
		{
			vector<vector<int> > solutionFinale;
			solutionFinale.push_back(listModules[i]);
			solutionModulaire[i] = solutionFinale;
		}
		cout << "TAPE TO CONTINUE" << endl;
		char c;
		cin >> c;
	}
}

void cliquer(map<int, vector<int> >& Gq, vector<vector<int> >& Amodulaire, map<int, vector<vector<int> > >& listSolutionsModules, int& ind, vector<int>& adj, vector<vector<int> >& listX)
{
	for (int i = 0; i<adj.size(); i++) 
	{
		vector<int> inter(listX.size(), 0);
		int j = 0;
		while (listNomsModules[j] != adj[i]) 
		{
			j++;
		}
		for (int k = 0; k < listSolutionsModules[j].size(); k++)
		{
			for (int l = 0; l < listX.size(); l++)
			{
				if (listX[l] == listSolutionsModules[j][k])
				{
					inter[l] = 1;
					break;
				}
			}
		}
		for (vector<int>::iterator it = Gq[adj[i]].begin(); it != Gq[adj[i]].end(); it++)
		{
			vector<int> inter2(inter); int b = 0;
			while (listNomsModules[b] != *it)
			{
				b++;
			}
			for (int k = 0; k < listSolutionsModules[b].size(); k++)
			{
				for (int l = 0; l < listX.size(); l++)
				{
					if (listX[l] == listSolutionsModules[b][k])
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

bool sousList(vector<int>& vec, vector<int>& bigVec)
{
	for (int i = 0; i < vec.size(); i++)
	{
		bool trouver = false;
		for (int j = 0; j < bigVec.size(); j++)
		{
			if (vec[i] == bigVec[j])
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

void graphe_de_solution(map<int, vector<int> >& Gq, map<int, vector<vector<int> > >& solutionModulaire, vector<vector<int> >& Amodulaire, vector<vector<int> >& listX, vector<Module>& listModule)
{
	cout << "Ensemble des solutions : " << endl;
	afficher(solutionModulaire);
	for (int i = 0; i<solutionModulaire.size(); i++)
	{
		for (int j = 0; j<solutionModulaire[i].size(); j++)
		{
			listX.push_back(solutionModulaire[i][j]);

		}
	}
	/*vector<vector<int> > listXTrie;
	for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); it++)
	{
		for (int i = 0; i < listModule.size(); i++)
		{
			if (listModule[i].getNom() == it->first)
			{
				vector<int> inter; int depart = it->first;
				recurseAdd(inter, listModule, depart);
				for (int j = 0; j < listX.size(); j++)
				{
					if (sousList(listX[j], inter))
					{
						listXTrie.push_back(listX[j]);
					}
				}
			}
		}
	}
	cout << "\nlistX : " << endl;
	afficher(listX);
	cout << "listXTrie : " << endl;
	afficher(listXTrie);
	cout << "Gq : " << endl;
	afficher(Gq);*/
	int racine;
	for (int i = 0; i < listModule.size(); i++)
	{
		if (listModule[i].getBouchra() == -1)
		{
			racine = listModule[i].getNom();
			break;
		}
	}
	for (map<int, vector<vector<int> > >::iterator it = solutionModulaire.begin(); it != solutionModulaire.end(); it++)
	{
		for (int i = 0; i < listModule.size(); i++)
		{
			if (listModule[i].getNom() == it->second[0][0])
			{
				int noeud = listModule[i].getNom(), ind = i;
				while (listModule[ind].getBouchra() != racine)
				{
					for (int j = 0; j < listModule.size(); j++)
					{
						if (listModule[ind].getBouchra() == listModule[j].getNom())
						{
							noeud = listModule[j].getNom();
							ind = j;
							break;
						}
					}
				}
				listNomsModules.push_back(noeud);
				break;
			}
		}
	}
	/*
	for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); it++)
	{
		listNomsModules.push_back(it->first);
	}*/
	cout << "listNomsModules : " << endl;
	//afficher(listNomsModules);
	int ind(0);
	for (map<int, vector<int> >::iterator it = Gq.begin(); it != Gq.end(); it++)
	{
		cliquer(Gq, Amodulaire, solutionModulaire, ind, it->second, listX);
		ind++;
	}

	cout << "\nAmodulaire : " << endl;
	//afficher(Amodulaire);
	/*if (!Amodulaire.empty())
	{
		simplification(Amodulaire);
		cout << "\nAmodulaire simplifier : " << endl;
		//afficher(Amodulaire);
	}*/
	cout << "\nlistX : " << endl;
	afficher(listX);
	cout << "TAPE TO CONTINUE" << endl;
	char c;
	cin >> c;
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
		cout << "graphsolution" << endl;
		afficher(g);
		//papillon(g, coloring, n_);
		for (int i = 0; i < g.size(); i++)
		{
			deg.push_back(g[i].size());
		}
		unsigned int ind;
		maxVec(deg, delta_, ind);
		//z_actuel = 326;

		cout << "delta = " << delta_ << endl;
		//delta_ = (int)floor(((double)coloring+(double)delta_-1.0)/2.0);
		//delta_ = (int)ceil(((double)coloring + (double)delta_) / 2.0);
		//cout << "delta modifier = " << delta_ << endl;
		for (int i = 0; i < listX.size(); i++)
		{
			p.push_back(listX[i].size());
		}

		SMSC_DNTS2(Amodulaire, n_, m_, delta_, sousSolution, p);
		cout << "***************************FIN SMSC2****************************" << endl;
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
		cout << "Solution SMSC2 " << endl;

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
		cout << "solution modulaire" << endl;
		afficher(solutionModulaire);
		int max = 0;
		for (map<int, vector<vector<int> > >::iterator it = solutionModulaire.begin(); it != solutionModulaire.end(); it++)
		{
			cout << "taille : " << it->second.size() << endl;
			if (max < it->second.size())
			{
				cout << "here" << endl;
				max = it->second.size();
			}
			else
			{
				cout << "no" << endl;
			}
		}
		cout << "max = " << max << endl;
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
		cout << "sol : " << endl;
		afficher(solution);
	}
}

void simplification(vector<vector<int> >& A, unsigned int& m)
{
	cout << "simplification" << endl;
	bool same; vector<int> ligne;
	for (int i = 0; i<A.size() - 1; i++)
	{
		for (int j = i + 1; j<A.size(); j++)
		{
			same = true;
			for (int k = 0; k<A[0].size(); k++)
			{
				if (A[i][k] != A[j][k])
				{
					same = false;
					break;
				}
			}
			if (same)
			{
				ligne.push_back(j);
			}
		}
	}
	vector<vector<int> >::iterator it = A.end() - 1;
	for (int i = A.size() - 1; i>-1; i--)
	{
		if (containsValue(ligne, i))
		{
			A.erase(it);
			m--;
		}
		it--;
	}
}

int main()
{
	vector<vector<int> > A, solution;
	unsigned int n, m, delta;
	map<int, vector<int> > G, Gq, Gf;
	map<int, vector<vector<int> > > solutionModulaire;
	vector<vector<int> > Amodulaire;
	vector<vector<int> > listX;
	unsigned int z;
	vector<Module> listModules;
	char c;

	// Lecture des données
	//lireData(A, m, n, delta);

	//lectureDimacs(n, m, delta, A);
	lectureDimacs0(n, m, delta, A);
	//simplification(A, m);

	// Initialization
	
	G = gToListAdj(A);
	ecrireData(G);
	afficher(G);
	// décomposition et création de graphe quotion
	cout << "Decomposition:" << endl;
	decomposition(G, Gq, listModules, n);
	
	cout << "List des Modules Finales : " << endl;
	afficher(listModules);

	vector<vector<int> > listModules_(modulesToList(listModules));
	cout << "List des Modules Finales en listes : " << endl;
	afficher(listModules_);
	cin >> c;
	// résolution des modeles
	cout << "Resolution:" << endl;
	resolution_modules(A, listModules_, solutionModulaire);

	// création de graphe solution
	cout << "Graphe de solution:" << endl;
	graphe_de_solution(Gq, solutionModulaire, Amodulaire, listX, listModules);

	// résolution globale de graphe solution
	cout << "Resolution finale:" << endl;
	resolution_finale(listModules_, Gq, solutionModulaire, Amodulaire, solution, listX);

	// Solution finale
	cout << "Solution finale" << endl;
	afficher(solution);

	getCout(solution, z);

	cout << "Z = " << z << endl;

	cout << "ENTRER UN CARACTERE POUR FERMER..." << endl;
	cin >> c;

	return 0;
}
