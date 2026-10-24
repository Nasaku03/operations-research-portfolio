#ifndef NOEUDQUADRATIC_H_INCLUDED
#define NOEUDQUADRATIC_H_INCLUDED
#endif

#include <map>
#include <vector>
#include <string>

class NoeudQuadratic
{
	// Numéro du noeud
	unsigned int num, level;
	// Paramètrres
	int n_S, m_S, max_n_S, max_m_S, n_A, m_A, max_n_A, max_m_A, nbObj;
	// Matrice du simplexe + contraintes (originales + ajoutées)
	double** S;
	double** A;
	// Vecteur second membre
	double* b;
	// Sens des contraintes
	std::string *sign;
	// Base
	std::vector<int> base;

public:
	NoeudQuadratic(unsigned int, unsigned int, double**, double**, double*, std::string*, std::vector<int>&, int, int, int, int, int, int, int, int, int);
	NoeudQuadratic(NoeudQuadratic&);
	virtual~NoeudQuadratic();
	unsigned int getNum()const;
	unsigned int getLevel()const;
	double** getS()const;
	double** getA()const;
	std::vector<int> getBase()const;
	double* getB()const;
	std::string* getSign()const;
	void setS(double**);
	void setA(double**);
	void setBase(std::vector<int>&);
	void setSign(std::string*);
	void setB(double*);
	int getn_S()const;
	int getm_S()const;
	int getmax_n_S()const;
	int getmax_m_S()const;
	int getn_A()const;
	int getm_A()const;
	int getmax_n_A()const;
	int getmax_m_A()const;
	int getNbObj()const;
	void setn_S(int&);
	void setm_S(int&);
	void setmax_n_S(int&);
	void setmax_m_S(int&);
	void setn_A(int&);
	void setm_A(int&);
	void setmax_n_A(int&);
	void setmax_m_A(int&);
	void setNbObj(int&);
	friend std::ostream& operator<< (std::ostream&, NoeudQuadratic const&);
};