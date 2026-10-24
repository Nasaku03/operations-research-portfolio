#include<string>
#include <vector>

#include "NoeudQuadratic.h"

using namespace std;

NoeudQuadratic::NoeudQuadratic(unsigned int num, unsigned int level, vector<vector<double> >& S, vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<double> >& xNonEfficace, vector<int>& base, int nbCon, int nbVar, int nbObj)
{
	this->num = num;
	this->level = level + 1;
	this->S = S;
	this->A = A;
	this->b = b;
	this->sign = sign;
	this->nbCon = nbCon;
	this->nbVar = nbVar;
	this->nbObj = nbObj;
	this->base = base;
	this->xNonEfficace = xNonEfficace;
}

NoeudQuadratic::~NoeudQuadratic()
{
}

unsigned int NoeudQuadratic::getNum() const
{
	return num;
}

unsigned int NoeudQuadratic::getLevel() const
{
	return level;
}

vector<vector<double>> NoeudQuadratic::getS() const
{
	return S;
}

vector<vector<double>> NoeudQuadratic::getA() const
{
	return A;
}

vector<int> NoeudQuadratic::getBase() const
{
	return base;
}

vector<double> NoeudQuadratic::getB() const
{
	return b;
}

void NoeudQuadratic::setS(vector<vector<double>>& S)
{
	this->S = S;
}

void NoeudQuadratic::setA(vector<vector<double>>& A)
{
	this->A = A;
}

void NoeudQuadratic::setB(vector<double>& b)
{
	this->b = b;
}

void NoeudQuadratic::setBase(vector<int>& b)
{
	this->base = base;
}

vector<string> NoeudQuadratic::getSign() const
{
	return sign;
}

vector<vector<double> > NoeudQuadratic::getXNonEfficace() const
{
	return xNonEfficace;
}

void NoeudQuadratic::setSign(vector<string>& sign)
{
	this->sign = sign;
}

int NoeudQuadratic::getNbCon() const
{
	return nbCon;
}

int NoeudQuadratic::getNbVar() const
{
	return nbVar;
}

int NoeudQuadratic::getNbObj() const
{
	return nbObj;
}

void NoeudQuadratic::setNbCon(int& nbCon)
{
	this->nbCon = nbCon;
}

void NoeudQuadratic::setNbVar(int& nbVar)
{
	this->nbVar = nbVar;
}

void NoeudQuadratic::setNbObj(int& nbObj)
{
	this->nbObj = nbObj;
}

ostream & operator<<(ostream & flux, NoeudQuadratic const & noeud)
{
	//flux << (int) noeud.getNum();
	return flux;
}