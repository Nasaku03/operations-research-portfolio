#include <iostream>

#include "Module.h"

using namespace std;

Module::Module(int const & num, std::map<int, std::vector<int>>& G, int const & type, Module * parent)
{
	this->num = num;
	this->G = G;
	this->type = type;
	this->parent = parent;
}

Module::~Module()
{
	delete parent;
	parent = 0;
	for (int i = 0; i < enfants.size(); i++)
	{
		delete enfants[i];
		enfants[i] = 0;
	}
}

int Module::getNum() const
{
	return num;
}

std::map<int, std::vector<int>> Module::getG() const
{
	return G;
}

std::vector<std::vector<int>> Module::getListStables() const
{
	return listStables;
}

int Module::getType() const
{
	return type;
}

Module * Module::getParent() const
{
	return parent;
}

std::vector<Module*> Module::getEnfants() const
{
	return enfants;
}

void Module::setNum(int const & num)
{
	this->num = num;
}

void Module::setG(std::map<int, std::vector<int>>& G)
{
	this->G = G;
}

void Module::setListStables(std::vector<std::vector<int>>& listStables)
{
	this->listStables = listStables;
}

void Module::setType(int const & type)
{
	this->type = type;
}

void Module::setParent(Module * parent)
{
	this->parent = parent;
}

void Module::setEnfants(std::vector<Module*>& enfants)
{
	this->enfants = enfants;
}

void Module::addEnfant(Module * enfant)
{
	(this->enfants).push_back(enfant);
}

ostream& operator<<(ostream& flux, Module& module)
{

	flux << "Module : " << module.getNum() << "\nNombre des sommets : " << module.G.size() << endl;
	flux << "Nombre de stables : " << module.listStables.size() << endl;
	if (module.parent != nullptr)
	{
		flux << "Parent : " << module.parent->num << endl;
	}
	else
	{
		flux << "Parent : Null" << endl;
	}
	flux << "Liste des modules enfants du module : \n";
	for (int i = 0; i < module.enfants.size(); i++)
	{
		flux << *module.enfants[i] << endl;
	}
	return flux;
}