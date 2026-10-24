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
	int nbCon, nbVar, nbObj;
	// Matrice du simplexe + contraintes (originales + ajoutées) + , Ens. des sol. efficaces + Ens. des sol. non dominées
	std::vector<std::vector<double> > S, A;
	// Vecteur second membre
	std::vector<double> b;
	// Sens des contraintes
	std::vector<std::string> sign;
	// Base
	std::vector<int> base;

public:
	NoeudQuadratic(unsigned int, unsigned int, std::vector<std::vector<double> >&, std::vector<std::vector<double> >&, std::vector<double>&, std::vector<std::string>&, std::vector<int>&, int, int, int);
	virtual~NoeudQuadratic();
	unsigned int getNum()const;
	unsigned int getLevel()const;
	std::vector<std::vector<double>> getS()const;
	std::vector<std::vector<double>> getA()const;
	std::vector<int> getBase()const;
	std::vector<double> getB()const;
	std::vector<std::string> getSign()const;
	void setS(std::vector<std::vector<double> >&);
	void setA(std::vector<std::vector<double> >&);
	void setBase(std::vector<int>&);
	void setSign(std::vector<std::string>&);
	void setB(std::vector<double>&);
	int getNbCon()const;
	int getNbVar()const;
	int getNbObj()const;
	void setNbCon(int&);
	void setNbVar(int&);
	void setNbObj(int&);
	//friend std::ostream& operator<< (std::ostream&, NoeudQuadratic const&);
};