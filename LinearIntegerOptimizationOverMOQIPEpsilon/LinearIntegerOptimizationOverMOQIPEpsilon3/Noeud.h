#ifndef NOEUD_H_INCLUDED
#define NOEUD_H_INCLUDED
#endif

#include <ilopl/iloopl.h>
#include <map>
#include <vector>
#include <string>

class Noeud
{
	// Numéro du noeud
	unsigned int num;
	// Sondé ou non
	bool etat;
	// Variables
	IloNumVarArray variables;
	// Contraintes
	IloRangeArray contraintes;
	// Objectif
	IloNumExpr objectif;
	// Modèle
	IloModel model;
	// Module CPLEX
	IloCplex cplex;
	//Environnement
	IloEnv env;
	// Valeur du cout entière ou non
	bool entier;
	// Matrice A courente
	std::vector<std::vector<int> > A;
	// Vecteur b courent
	std::vector<int> b;
	// Vecteur des signes courent
	std::vector<std::string> sign;
	// Vecteur des solutions déjà testées pour leurs efficacité pour le noeud actuel (en tant que parent)
	std::vector<double> xNonEfficace;

public:
	Noeud(unsigned int, std::vector<std::vector<int> >&, std::vector<int>&, std::vector<std::string>&, std::vector<double>&, IloNumVarArray, IloRangeArray, IloNumExpr, IloModel, IloCplex, IloEnv, bool, bool = false);
	virtual~Noeud();
	unsigned int getNum()const;
	IloNumVarArray getVariables()const;
	IloRangeArray getContraintes()const;
	IloNumExpr getObjectif()const;
	IloModel getModel()const;
	IloCplex getCplex()const;
	IloEnv getEnv()const;
	bool getEntier()const;
	bool getEtat()const;
	void setEtat(bool);
	std::vector<std::vector<int>> getA()const;
	std::vector<int> getB()const;
	std::vector<std::string> getSign()const;
	std::vector<double> getXNonEfficace()const;
	void setA(std::vector<std::vector<int> >&);
	void setB(std::vector<int>&);
	void setSign(std::vector<std::string>&);
	friend std::ostream& operator<< (std::ostream&, Noeud const&);
};