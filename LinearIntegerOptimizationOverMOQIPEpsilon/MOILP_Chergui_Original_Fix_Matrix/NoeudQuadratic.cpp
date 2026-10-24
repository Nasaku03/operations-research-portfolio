#include<string>
#include <vector>

#include "NoeudQuadratic.h"

using namespace std;

NoeudQuadratic::NoeudQuadratic(unsigned int num, unsigned int level, double** S, double** A, double* b, string* sign, vector<int>& base, int n_S, int m_S, int max_n_S, int max_m_S, int n_A, int m_A, int max_n_A, int max_m_A, int nbObj)
{
	this->num = num;
	this->level = level + 1;
	this->S = new double*[max_m_S];
	for (int i = 0; i < max_m_S; i++)
	{
		this->S[i] = new double[max_n_S];
	}
	for (int i = 0; i < m_S; i++)
	{
		for (int j = 0; j < n_S; j++)
		{
			this->S[i][j] = S[i][j];
		}
	}
	this->A = new double*[max_m_A];
	this->b = new double[max_m_A];
	this->sign = new string[max_m_A];
	for (int i = 0; i < max_m_A; i++)
	{
		this->A[i] = new double[max_n_A];
	}
	for (int i = 0; i < m_A; i++)
	{
		for (int j = 0; j < n_A; j++)
		{
			this->A[i][j] = A[i][j];
		}
		this->b[i] = b[i];
		this->sign[i] = sign[i];
	}
	this->n_S = n_S;
	this->m_S = m_S;
	this->max_n_S = max_n_S;
	this->max_m_S = max_m_S;
	this->n_A = n_A;
	this->m_A = m_A;
	this->max_n_A = max_n_A;
	this->max_m_A = max_m_A;
	this->nbObj = nbObj;
	this->base = base;
}

NoeudQuadratic::NoeudQuadratic(NoeudQuadratic &noeud)
{
	this->num = noeud.num;
	this->level = noeud.level + 1;
	this->S = new double*[noeud.max_m_S];
	for (int i = 0; i < noeud.max_m_S; i++)
	{
		this->S[i] = new double[noeud.max_n_S];
	}
	for (int i = 0; i < noeud.m_S; i++)
	{
		for (int j = 0; j < noeud.n_S; j++)
		{
			this->S[i][j] = noeud.S[i][j];
		}
	}
	this->A = new double*[noeud.max_m_A];
	this->b = new double[noeud.max_m_A];
	this->sign = new string[noeud.max_m_A];
	for (int i = 0; i < noeud.max_m_A; i++)
	{
		this->A[i] = new double[noeud.max_n_A];
	}
	for (int i = 0; i < noeud.m_A; i++)
	{
		for (int j = 0; j < noeud.n_A; j++)
		{
			this->A[i][j] = noeud.A[i][j];
		}
		this->b[i] = noeud.b[i];
		this->sign[i] = noeud.sign[i];
	}
	this->n_S = noeud.n_S;
	this->m_S = noeud.m_S;
	this->max_n_S = noeud.max_n_S;
	this->max_m_S = noeud.max_m_S;
	this->n_A = noeud.n_A;
	this->m_A = noeud.m_A;
	this->max_n_A = noeud.max_n_A;
	this->max_m_A = noeud.max_m_A;
	this->nbObj = noeud.nbObj;
	this->base = noeud.base;
}

NoeudQuadratic::~NoeudQuadratic()
{
	for (int i = 0; i < max_m_S; i++)
	{
		delete[] S[i];
		//S[i] = nullptr;
	}
	delete[] S;
	for (int i = 0; i < max_m_A; i++)
	{
		delete[] A[i];
		//A[i] = nullptr;
	}
	delete[] A;
	delete[] b;
	//b = nullptr;
	delete[] sign;
	//sign = nullptr;
}

unsigned int NoeudQuadratic::getNum() const
{
	return num;
}

unsigned int NoeudQuadratic::getLevel() const
{
	return level;
}

double** NoeudQuadratic::getS() const
{
	return S;
}

double** NoeudQuadratic::getA() const
{
	return A;
}

vector<int> NoeudQuadratic::getBase() const
{
	return base;
}

double* NoeudQuadratic::getB() const
{
	return b;
}

void NoeudQuadratic::setS(double** S)
{
	this->S = new double*[max_m_S];
	for (int i = 0; i < max_m_S; i++)
	{
		this->S[i] = new double[max_n_S];
	}
	for (int i = 0; i < m_S; i++)
	{
		for (int j = 0; j < n_S; j++)
		{
			this->S[i][j] = S[i][j];
		}
	}
}

void NoeudQuadratic::setA(double** A)
{
	this->A = new double*[max_m_A];
	for (int i = 0; i < max_m_A; i++)
	{
		this->A[i] = new double[max_n_A];
	}
	for (int i = 0; i < m_A; i++)
	{
		for (int j = 0; j < n_A; j++)
		{
			this->A[i][j] = A[i][j];
		}
	}
}

void NoeudQuadratic::setB(double* b)
{
	this->b = new double[max_m_A];
	
	for (int i = 0; i < m_A; i++)
	{
		this->b[i] = b[i];
	}
}

void NoeudQuadratic::setBase(vector<int>& b)
{
	this->base = base;
}

string* NoeudQuadratic::getSign() const
{
	return sign;
}

void NoeudQuadratic::setSign(string* sign)
{
	this->sign = new string[max_m_A];
	for (int i = 0; i < m_A; i++)
	{
		this->sign[i] = sign[i];
	}
}

int NoeudQuadratic::getn_S() const
{
	return n_S;
}

int NoeudQuadratic::getm_S() const
{
	return m_S;
}

int NoeudQuadratic::getmax_m_S() const
{
	return max_m_S;
}

int NoeudQuadratic::getmax_n_S() const
{
	return max_n_S;
}

int NoeudQuadratic::getn_A() const
{
	return n_A;
}

int NoeudQuadratic::getm_A() const
{
	return m_A;
}

int NoeudQuadratic::getmax_m_A() const
{
	return max_m_A;
}

int NoeudQuadratic::getmax_n_A() const
{
	return max_n_A;
}

int NoeudQuadratic::getNbObj() const
{
	return nbObj;
}

void NoeudQuadratic::setn_S(int& n_S)
{
	this->n_S = n_S;
}

void NoeudQuadratic::setm_S(int& m_S)
{
	this->m_S = m_S;
}

void NoeudQuadratic::setmax_m_S(int& max_m_S)
{
	this->max_m_S = max_m_S;
}

void NoeudQuadratic::setmax_n_S(int& max_n_S)
{
	this->max_n_S = max_n_S;
}

void NoeudQuadratic::setn_A(int& n_A)
{
	this->n_A = n_A;
}

void NoeudQuadratic::setm_A(int& m_A)
{
	this->m_A = m_A;
}

void NoeudQuadratic::setmax_m_A(int& max_m_A)
{
	this->max_m_A = max_m_A;
}

void NoeudQuadratic::setmax_n_A(int& max_n_A)
{
	this->max_n_A = max_n_A;
}

void NoeudQuadratic::setNbObj(int& nbObj)
{
	this->nbObj = nbObj;
}

ostream & operator<<(ostream & flux, NoeudQuadratic const & noeud)
{
	flux << noeud.getNum();
	return flux;
}