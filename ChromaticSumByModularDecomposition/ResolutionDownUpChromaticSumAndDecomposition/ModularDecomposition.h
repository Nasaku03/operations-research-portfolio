#ifndef MODULARDECOMPOSITION_H_INCLUDED
#define MODULARDECOMPOSITION_H_INCLUDED

#include <vector>
#include <map>

#include "Module.h"


void lectureDimacs(unsigned int& n, unsigned int& m, unsigned int& delta, std::vector<std::vector<int> >& A);

void lireData(std::vector<std::vector<int> >& A, unsigned int& m, unsigned int& n, unsigned int& delta);

void afficher(std::vector<std::vector<int> >& P);

void afficher(std::map<int, std::vector<int> >& P);

void afficher(std::vector<int> P);

void afficher(std::map<int, int>& P);

void afficher(std::vector<Module>& P);

void afficher(std::map<int, std::vector<std::vector<int> > >& P);

void minVec(std::vector<int>& deg, int& min_, unsigned int& ind);

void maxVec(std::vector<int>& deg, unsigned int& max_, unsigned int& ind);

bool vgStable(std::map<int, std::vector<int> > graphqlq);

std::map<int, std::vector<int> > gToGBar(std::map<int, std::vector<int> > G);

bool containsValue(std::vector<int>& c, int val);

std::vector<std::map<int, std::vector<int> > > dCConnexe(std::map<int, std::vector<int> > g_);

std::map<int, std::vector<int> > grapheInduit(std::map<int, std::vector<int> >& graphe, std::vector<int>& list);

bool comparerAdj(std::vector<int>& adj, std::vector<int>& pj);

bool contient(std::vector<int>& sommets, std::vector<int>& trace, std::map<int, std::vector<int> >& moduleGarder);

bool contientAuMoinsUnElmt(std::vector<int>& sommets, std::vector<int>& trace);

void recursion(std::map<int, std::vector<int> >& g, std::vector<Module>& t, int& nivMax);

void restriction(int& v0, std::vector<int>& nv0, std::vector<int>& nbv0, std::map<int, std::vector<int> >& g, std::vector<Module>& t1, std::vector<Module>& t2, std::vector<Module>& t, int& nivMax1, int& nivMax2, std::vector<int>& supprimerNom);

void recurseAdd(std::vector<int>& B, std::vector<Module>& t, int& ind);

void calculCFC(std::vector<std::vector<int> >& CFC, std::map<int, std::vector<int> >& g, std::map<int, std::vector<int> > forcingG);

void v0Module(int& v0, std::vector<int>& nv0, std::vector<int>& nbv0, std::map<int, std::vector<int> >& g, std::map<int, std::vector<int> >& gbar, std::vector<Module>& t1, std::vector<Module>& t2, std::vector<Module>& t, std::vector<int>& supprimerNom);

void assemble(int& v0, std::map<int, std::vector<int> >& g, std::vector<Module>& t, std::vector<int>& supprimerNom, int& nomAPrendre);

void primePhase(std::map<int, std::vector<int> >& graphe, std::map<int, std::vector<int> >& grapheBar, std::vector<Module>& listModules, int& bouchra);

void decompositionSecondaire(std::map<int, std::vector<int> >& graphe, std::map<int, std::vector<int> >& grapheBar, std::vector<std::map<int, std::vector<int> > >& ECC, std::vector<Module>& ancienModules, std::vector<int>& listBouchra, std::vector<int>& suiviBouchra);

void arborescence(std::map<int, std::vector<int> >& G, std::vector<Module>& listModule);

std::map<int, std::vector<int> > gToListAdj(std::vector<std::vector<int> >& A);

void recursionNiveaux(int nv, std::vector<int> sommets, std::vector<Module>& listModule, std::map<int, std::vector<int> >& niv);

std::map<int, std::vector<int> > getNiveaux(std::vector<Module>& listModule);

std::map<int, std::vector<int> > construireGrapheQuotient(std::vector<Module>& modules, std::map<int, std::vector<int> >& G);

std::vector<std::vector<int> > modulesToList(std::vector<Module>& modules);

int decomposition(std::map<int, std::vector<int> >& G, std::map<int, std::vector<int> >& Gq, std::vector<Module>& listModules, unsigned int& n);

#endif 