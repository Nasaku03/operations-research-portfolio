#ifndef NOEUDLEMKE_H_INCLUDED
#define NOEUDLEMKE_H_INCLUDED
#endif

#include <map>
#include <vector>
#include <string>

class NoeudLemke
{
	// Numéro du noeud
	unsigned int num;
	// Sondé ou non
	bool etat;
	// Matrice des contraintes (originales + ajoutées)
	std::vector<std::vector<double> > A;
	// Vecteur du second membre (originale + ajoutée)
	std::vector<double> b;
	// Sense des contraintes
	std::vector<std::string> sign;
	// Base
	std::vector<int> base;

public:
	NoeudLemke(unsigned int, std::vector<std::vector<double> >&, std::vector<double>&, std::vector<std::string>&, std::vector<int>&, bool);
	virtual~NoeudLemke();
	unsigned int getNum()const;
	bool getEtat()const;
	void setEtat(bool);
	std::vector<std::vector<double>> getA()const;
	std::vector<double> getB()const;
	std::vector<int> getBase()const;
	void setA(std::vector<std::vector<double> >&);
	void setBase(std::vector<int>&);
	void setB(std::vector<double>&);
	std::vector<std::string> getSign()const;
	void setSign(std::vector<std::string>&);
	friend std::ostream& operator<< (std::ostream&, NoeudLemke const&);
};