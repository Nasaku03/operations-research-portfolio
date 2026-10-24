#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <fstream>
#include <string>
#include <ctime>
#include <unordered_map>

#include "Module.h"
#include "ModularChromaticSum.h"

using namespace std;

void afficher(vector<int> P)
{
	cout << '{';
	for (int i = 0; i < P.size(); i++)
	{
		cout << P[i] << ' ';
	}
	cout << '}' << endl;
}

void afficher(map<int, vector<int> >& P)
{
	cout << '{' << endl;
	for (map<int, vector<int> >::iterator it = P.begin(); it != P.end(); it++)
	{
		cout << it->first << " : ";
		for (int j = 0; j < (it->second).size(); j++)
		{
			cout << (it->second)[j] << ' ';
		}
		cout << endl;
	}
	cout << '}' << endl;
}

void afficher(vector<Module>& P)
{
	cout << '{';
	for (int i = 0; i < P.size(); i++)
	{
		cout << P[i] << ' ';
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
		A[i][s1] = 1;
		A[i][s2] = 1;
	}
	unsigned int max_;
	//unsigned int somme[n];
	vector<unsigned int> somme(n, 0);
	for (unsigned int j = 0; j < n; j++)
	{
		for (unsigned int i = 0; i < m; i++)
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

void lireData(vector<vector<int> >& A, unsigned int& m, unsigned int& n, unsigned int& delta)
{
	ifstream fichier("input.txt");
	unsigned int var, contr, mot; //Une variable pour stocker les lignes lues
	fichier >> var;
	fichier >> contr;
	vector<int> inter;
	cout << var << endl;
	cout << contr << endl;
	for (unsigned int i = 0; i < contr; i++)
	{
		for (unsigned int j = 0; j < var; j++)
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
	for (unsigned int j = 0; j < n; j++)
	{
		for (unsigned int i = 0; i < m; i++)
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

map<int, vector<int> > gToListAdj(vector<vector<int> >& A)
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

void transformGraph(map<int, vector<int> >& G)
{
	ofstream graph("graph.dat");
	if (graph)
	{
		for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
		{
			if (it == G.begin())
			{
				graph << it->first << "|";
			}
			else
			{
				graph << endl << it->first << "|";
			}
			for (int i = 0; i < (it->second).size(); i++)
			{
				if (i != (it->second).size() - 1)
				{
					graph << it->second[i] << ",";
				}
				else
				{
					graph << it->second[i];
				}
			}
		}
	}
	graph.close();
}

void getModules(vector<Module>& listModules, string& md, int& nn)
{
	if (md.empty())
	{
		stack<Module> modules;
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
					modules.push(Module());
					modules.top().setBouchra(-1);
				}
				else
				{
					int origin = modules.top().getNom();
					modules.push(Module());
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
		stack<Module> modules;
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
					modules.push(Module());
					modules.top().setBouchra(-1);
				}
				else
				{
					int origin = modules.top().getNom();
					modules.push(Module());
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
			//cout << "noeud: " << sommets[i] << endl;
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

map<int, vector<int> > construireGrapheQuotient(vector<Module>& modules, map<int, vector<int> >& G)
{
	//cout << "Construire Graphe : " << endl;
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
		//cout << "keys : " << endl;
		//afficher(keys);
		//cout << "primalLists : " << endl;
		//afficher(primalLists);
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

void detectSubCograph(vector<Module>& listModule, int n)
{
	// Indexation rapide: nom -> module
	unordered_map<int, int> index;
	int maxLevel = 0;

	for (int i = 0; i < listModule.size(); i++)
	{
		index[listModule[i].getNom()] = i;
		maxLevel = max(maxLevel, listModule[i].getNiveau());
	}

	// Parcours bottom-up par niveau
	for (int lvl = maxLevel; lvl >= 0; lvl--)
	{
		for (Module& m : listModule)
		{
			if (m.getNiveau() != lvl)
				continue;

			// Règle 1
			if (m.getNom() < n)
			{
				m.setCographe(true);
				continue;
			}

			// Règle 3
			if (m.getDegenerate() == 0)
			{
				m.setCographe(false);
				continue;
			}

			// Règle 2
			bool allChildrenCograph = true;

			for (int childName : m.getSommets())
			{
				auto it = index.find(childName);
				if (it == index.end() ||
					!listModule[it->second].getCographe())
				{
					allChildrenCograph = false;
					break;
				}
			}

			m.setCographe(allChildrenCograph);
		}
	}
}


int main()
{
	vector<vector<int> > A, solution;
	unsigned int n, m, delta;
	map<int, vector<int> > G, Gq;
	unsigned int z;
	vector<Module> listModules;

	lectureDimacs(n, m, delta, A);

	G = gToListAdj(A);

	char c;

	cout << "Would you like transform the graph G (y/n) ? ";
	cin >> c;
	if (c == 'y' || c == 'Y')
	{
		transformGraph(G);
	}
	else
	{
		float debut = clock();
		string md = "";
		int taille = n;
		getModules(listModules, md, taille);
		//afficher(listModules);
		map<int, vector<int> > mappingNiveaux = getNiveaux(listModules);
		unsigned int nivMax = 0;
		for (map<int, vector<int> >::iterator it = mappingNiveaux.begin(); it != mappingNiveaux.end(); it++)
		{
			if (it->first > nivMax)
			{
				nivMax = it->first;
			}
		}
		//cout << "Mapping Niveaux : " << endl;
		//afficher(mappingNiveaux);

		// Détection des sous-cographes
		detectSubCograph(listModules, n);

		Gq = construireGrapheQuotient(listModules, G);
		afficher(listModules);
		//afficher(Gq);

		int prime = 0, parallel = 0, series = 0;
		vector<int> primes;
		for (int i = 0; i < listModules.size(); i++)
		{
			if (listModules[i].getDegenerate() == 0 && listModules[i].getSommets().size() != 1)
			{
				prime++;
				primes.push_back(listModules[i].getSommets().size());
			}
			else if (listModules[i].getDegenerate() == 1)
			{
				parallel++;
			}
			else if (listModules[i].getDegenerate() == 2)
			{
				series++;
			}
		}

		vector<vector<int> > listModules_(modulesToList(listModules));
		//cout << "List des Modules Finales en listes : " << endl;
		//afficher(listModules_);
		cout << "prime = " << prime << endl;
		cout << "parallel = " << parallel << endl;
		cout << "series = " << series << endl;
		cout << "Prime sizes : " << endl;
		afficher(primes);
		//cin >> c;



		// Début de la résolution
		solution = resolutionBottomUp(A, G, listModules, listModules_);

		// Solution finale
		cout << "Solution finale" << endl;
		afficher(solution);

		getCout(solution, z);

		cout << "Z = " << z << endl;

		float fin = (float)(clock() - debut) / CLOCKS_PER_SEC;

		int nn = 0;
		cout << "Sequence : (";
		for (int i = 0; i < solution.size(); i++)
		{
			cout << solution[i].size() << " ";
			nn += solution[i].size();
		}
		cout << ")" << endl;

		cout << "s(G) = " << solution.size() << endl;

		cout << "nn = " << nn << endl;

		cout << "temps = " << fin << " seconds" << endl;

	}
	return 0;
}