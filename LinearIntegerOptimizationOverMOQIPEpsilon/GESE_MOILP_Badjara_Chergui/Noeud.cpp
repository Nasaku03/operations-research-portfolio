#include<string>
#include <vector>

#include "Noeud.h"

using namespace std;

Noeud::Noeud(unsigned int num, vector<vector<int> >& A, vector<int>& b, vector<string>& sign, IloNumVarArray var, IloRangeArray con, IloNumExpr objectif, IloModel model, IloCplex cplex, IloEnv env, bool entier, bool etat)
{
	this->num = num;
	this->variables = var;
	this->contraintes = con;
	this->objectif = objectif;
	this->model = model;
	this->cplex = cplex;
	this->env = env;
	this->entier = entier;
	this->etat = etat;
	this->A = A;
	this->b = b;
	this->sign = sign;
}

Noeud::~Noeud()
{
}

unsigned int Noeud::getNum() const
{
	return num;
}

IloNumVarArray Noeud::getVariables() const
{
	return variables;
}

IloRangeArray Noeud::getContraintes() const
{
	return contraintes;
}

IloNumExpr Noeud::getObjectif() const
{
	return objectif;
}

IloModel Noeud::getModel() const
{
	return model;
}

IloCplex Noeud::getCplex() const
{
	return cplex;
}

IloEnv Noeud::getEnv() const
{
	return env;
}

bool Noeud::getEntier() const
{
	return entier;
}

bool Noeud::getEtat() const
{
	return etat;
}

void Noeud::setEtat(bool etat)
{
	this->etat = etat;
}

vector<vector<int>> Noeud::getA() const
{
	return A;
}

vector<int> Noeud::getB() const
{
	return b;
}

vector<string> Noeud::getSign() const
{
	return sign;
}

void Noeud::setA(vector<vector<int>>& A)
{
	this->A = A;
}

void Noeud::setB(vector<int>& b)
{
	this->b = b;
}

void Noeud::setSign(vector<string>& sign)
{
	this->sign = sign;
}

ostream& operator<<(ostream& flux, Noeud const& noeud)
{
	flux << noeud.getNum();
	return flux;
}
