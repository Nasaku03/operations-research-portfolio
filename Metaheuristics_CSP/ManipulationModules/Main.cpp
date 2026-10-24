#include <iostream>
#include <vector>
#include <map>

#include "Module.h"

using namespace std;

int main()
{
	map<int, vector<int> > g;
	vector<int> adj;
	adj.push_back(3);
	g[0] = adj;
	g[1] = adj;
	g[2] = adj;
	adj.clear();
	adj.push_back(0);
	adj.push_back(1);
	adj.push_back(2);
	adj.push_back(4);
	g[3] = adj;
	adj.clear();
	adj.push_back(3);
	adj.push_back(5);
	adj.push_back(6);
	adj.push_back(7);
	g[4] = adj;
	adj.clear();
	adj.push_back(4);
	g[5] = adj;
	g[6] = adj;
	g[7] = adj;
	adj.clear();
	adj.push_back(9);
	g[8] = adj;
	adj.clear();
	adj.push_back(8);
	g[9] = adj;
	Module *racine;
	racine = new Module(0, g, 1, nullptr);

	g.clear();
	adj.clear();
	adj.push_back(3);
	g[0] = adj;
	g[1] = adj;
	g[2] = adj;
	adj.clear();
	adj.push_back(0);
	adj.push_back(1);
	adj.push_back(2);
	adj.push_back(4);
	g[3] = adj;
	adj.clear();
	adj.push_back(3);
	adj.push_back(5);
	adj.push_back(6);
	adj.push_back(7);
	g[4] = adj;
	adj.clear();
	adj.push_back(4);
	g[5] = adj;
	g[6] = adj;
	g[7] = adj;

	Module *salavatipour;
	salavatipour = new Module(1, g, 0, racine);

	racine->addEnfant(salavatipour);

	g.clear();
	adj.clear();
	adj.push_back(9);
	g[8] = adj;
	adj.clear();
	adj.push_back(8);
	g[9] = adj;

	Module *arete;
	arete = new Module(2, g, 2, racine);

	racine->addEnfant(arete);

	map<int, vector<int> > g1;
	adj.clear();
	g1[0] = adj;
	g1[1] = adj;
	g1[2] = adj;
	Module* m;
	m = new Module(3, g1, 1, salavatipour);

	map<int, vector<int> > g2;
	adj.clear();
	g2[3] = adj;
	Module* m2;
	m2 = new Module(4, g2, 1, salavatipour);

	map<int, vector<int> > g3;
	adj.clear();
	g3[4] = adj;
	Module* m3;
	m3 = new Module(5, g3, 1, salavatipour);

	map<int, vector<int> > g4;
	adj.clear();
	g4[5] = adj;
	g4[6] = adj;
	g4[7] = adj;
	Module* m4;
	m4 = new Module(6, g4, 1, salavatipour);

	salavatipour->addEnfant(m);
	salavatipour->addEnfant(m2);
	salavatipour->addEnfant(m3);
	salavatipour->addEnfant(m4);

	cout << *racine << endl;
	//cout << racine->getEnfants().size() << endl;
	system("pause");
	return 0;
}