#include<string>
#include <vector>

#include "NoeudLemke.h"

using namespace std;

NoeudLemke::NoeudLemke(unsigned int num, vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<int>& base, bool etat)
{
	this->num = num;
	this->etat = etat;
	this->A = A;
	this->b = b;
	this->sign = sign;
	this->base = base;
}

NoeudLemke::~NoeudLemke()
{
}

unsigned int NoeudLemke::getNum() const
{
	return num;
}

bool NoeudLemke::getEtat() const
{
	return etat;
}

void NoeudLemke::setEtat(bool etat)
{
	this->etat = etat;
}

vector<vector<double>> NoeudLemke::getA() const
{
	return A;
}

vector<double> NoeudLemke::getB() const
{
	return b;
}

vector<int> NoeudLemke::getBase() const
{
	return base;
}

void NoeudLemke::setA(vector<vector<double>>& A)
{
	this->A = A;
}

void NoeudLemke::setBase(vector<int>& base)
{
	this->base = base;
}

void NoeudLemke::setB(std::vector<double>& b)
{
	this->b = b;
}

vector<string> NoeudLemke::getSign() const
{
	return sign;
}

void NoeudLemke::setSign(vector<string>&)
{
	this->sign = sign;
}

/*ostream& operator<<(ostream& flux, NoeudLemke const& noeud)
{
	flux << noeud.getNum();
	return flux;
}*/