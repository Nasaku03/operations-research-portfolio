#include <iostream>
#include <vector>

#include "ModuleMOG.h"

using namespace std;

ModuleMOG::ModuleMOG()
{

}

ModuleMOG::ModuleMOG(int const& n)
{
	nom = n;
}

ModuleMOG::ModuleMOG(int const& n, vector<int> const& v, int const& b, int const& d, int const& niv, bool const& r, bool const& t)
{
	nom = n;
	sommets = v;
	bouchra = b;
	degenerate = d;
	niveau = niv;
	strong = r;
	trivial = t;
}

ModuleMOG::~ModuleMOG()
{

}

int ModuleMOG::getNom()const
{
	return nom;
}

vector<int> ModuleMOG::getSommets()const
{
	return sommets;
}

int ModuleMOG::getBouchra()const
{
	return bouchra;
}

int ModuleMOG::getDegenerate()const
{
	return degenerate;
}

int ModuleMOG::getNiveau()const
{
	return niveau;
}

bool ModuleMOG::getStrong()const
{
	return strong;
}

bool ModuleMOG::getTrivial()const
{
	return trivial;
}

void ModuleMOG::setNom(int const& n)
{
	nom = n;
}

void ModuleMOG::setSommets(vector<int> const& s)
{
	sommets = s;
}

void ModuleMOG::setBouchra(int const& b)
{
	bouchra = b;
}

void ModuleMOG::setDegenerate(int const& d)
{
	degenerate = d;
}

void ModuleMOG::setNiveau(int const& niv)
{
	niveau = niv;
}

void ModuleMOG::setStrong(bool const& s)
{
	strong = s;
}

void ModuleMOG::setTrivial(bool const& t)
{
	trivial = t;
}

ostream& operator<<(ostream& flux, ModuleMOG& m)
{
	flux << "nom : " << m.getNom() << "\t: " << ((m.getDegenerate() == 0) ? "Prime" : ((m.getDegenerate() == 1) ? "Parallele" : "Serie")) << "\tOrigine: " << m.getBouchra() << "\tNiveau: " << m.getNiveau() << "\nList sommets : ";
	for (int i = 0; i < m.getSommets().size(); i++)
	{
		flux << m.getSommets()[i] << " ";
	}
	flux << "\n";
	return flux;
}
