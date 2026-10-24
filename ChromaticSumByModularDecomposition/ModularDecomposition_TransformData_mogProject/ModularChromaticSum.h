#ifndef MODULARCHROMATICSUM_H_INCLUDED
#define MODULARCHROMATICSUM_H_INCLUDED

#include <vector>
#include <map>

#include "Module.h"

struct classcomp {
	bool operator() (const int& lhs, const int& rhs) const
	{
		return lhs > rhs;
	}
};

void ecrireData2(std::vector<std::vector<int> >&, unsigned int&, unsigned int&, unsigned int&, std::vector<int>&);
void afficher(std::vector<int>);
void afficher(std::map<int, std::vector<int> >&);
void afficher(std::vector<std::vector<int> >&);
void afficher(std::map<int, std::vector<std::vector<int> > >&);
void afficher(std::vector<Module>&);
void afficher(std::map<int, std::vector<std::vector<Module>::iterator>, classcomp >&);
void recurseAdd(std::vector<int>&, std::vector<Module>&, int&);
std::vector<std::vector<int> > modulesToList(std::vector<Module>&);
std::map<int, std::vector<std::vector<Module>::iterator>, classcomp > trieModuleParNiveau(std::vector<Module>&, int&);
bool containsValue(std::vector<int>&, int);
bool contains(std::vector<int>&, std::vector<int>&);
void getCout(std::vector<std::vector<int> >&, unsigned int&);
void maxVec(std::vector<int>&, unsigned int&, unsigned int&);
int SMSC_DNTS2(std::vector<std::vector<int> >&, unsigned int&, unsigned int&, unsigned int&, std::vector<std::vector<int> >&, std::vector<int>&);
std::map<int, std::vector<int> > gToListAdj2(std::vector<std::vector<int> >&);
void resolution_finale(std::vector<std::vector<int> >&, std::map<int, std::vector<int> >&, std::map<int, std::vector<std::vector<int> > >&, std::vector<std::vector<int> >&, std::vector<std::vector<int> >&, std::vector<std::vector<int> >&);
void cliquer(std::map<int, std::vector<int> >&, std::vector<std::vector<int> >&, std::map<int, std::vector<std::vector<int> > >&, int&, std::vector<int>&, std::vector<std::vector<int> >&);
void graphe_de_solution(std::map<int, std::vector<int> >&, std::map<int, std::vector<std::vector<int> > >&, std::vector<std::vector<int> >&, std::vector<std::vector<int> >&);
std::vector<std::vector<int> > resolutionBottomUp(std::vector<std::vector<int> >&, std::map<int, std::vector<int> >&, std::vector<Module>&, std::vector<std::vector<int> >&);


#endif