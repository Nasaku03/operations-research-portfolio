#include <iostream>
#include <vector>

#include "Module.h"

using namespace std;

Module::Module(int const& n, vector<int> const& v, int const& b, int const& d, int const& niv, bool const& r, bool const& t)
{
	nom = n;
	sommets = v;
	bouchra = b;
	degenerate = d;
	niveau = niv;
	strong = r;
	trivial = t;
}

Module::~Module()
{

}

int Module::getNom()const
{
	return nom;
}

vector<int> Module::getSommets()const
{
	return sommets;
}

int Module::getBouchra()const
{
	return bouchra;
}

int Module::getDegenerate()const
{
	return degenerate;
}

int Module::getNiveau()const
{
	return niveau;
}

bool Module::getStrong()const
{
	return strong;
}

bool Module::getTrivial()const
{
	return trivial;
}

void Module::setNom(int const& n)
{
	nom = n;
}

void Module::setSommets(vector<int> const& s)
{
	sommets = s;
}

void Module::setBouchra(int const& b)
{
	bouchra = b;
}

void Module::setDegenerate(int const& d)
{
	degenerate = d;
}

void Module::setNiveau(int const& niv)
{
	niveau = niv;
}

void Module::setStrong(bool const& s)
{
	strong = s;
}

void Module::setTrivial(bool const& t)
{
	trivial = t;
}

ostream& operator<<(ostream& flux, Module& m)
{
	flux << "nom : " << m.nom << "\t: " << ((m.degenerate == 0) ? "Prime" : ((m.degenerate == 1) ? "Parallele" : "Serie")) << "\tOrigine: " << m.bouchra << "\tNiveau: " << m.niveau << "\nList sommets : ";
	for (int i = 0; i<m.sommets.size(); i++)
	{
		flux << m.sommets[i] << " ";
	}
	flux << "\n";
	return flux;
}
