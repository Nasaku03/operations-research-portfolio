#include "Noeud.hpp"

Noeud::Noeud(unsigned int num, std::vector<std::vector<int> >& Hl, std::vector<std::vector<double> >& Hl_origine, std::vector<std::vector<double> >& Hl_constraint, std::vector<double> up, std::vector<double> lo)
{
	this->num = num;
	this->Hl = std::vector<std::vector<int> >(Hl);
	this->Hl_origine = std::vector<std::vector<double> >(Hl_origine);
	this->Hl_constraint = std::vector<std::vector<double> >(Hl_constraint);
	this->uppers = std::vector<double>(up);
	this->lowers = std::vector<double>(lo);
}

Noeud::~Noeud()
{

}

unsigned int Noeud::getNum() const
{
	return this->num;
}

void Noeud::setNum(unsigned int& num)
{
	this->num = num;
}

std::vector<std::vector<int> > Noeud::getHl()const
{
	return this->Hl;
}

void Noeud::setHl(std::vector<std::vector<int> >& Hl)
{
	this->Hl = Hl;
}

std::vector<std::vector<double> > Noeud::getHl_origine()const
{
	return this->Hl_origine;
}

void Noeud::setHl_origine(std::vector<std::vector<double> >& Hl_origine)
{
	this->Hl_origine = Hl_origine;
}

std::vector<std::vector<double> > Noeud::getHl_constraint()const
{
	return this->Hl_constraint;
}

void Noeud::setHl_constraint(std::vector<std::vector<double> >& Hl_constraint)
{
	this->Hl_constraint = Hl_constraint;
}

std::vector<double> Noeud::getUppers()const
{
	return this->uppers;
}

void Noeud::setUppers(std::vector<double>& up)
{
	this->uppers = up;
}

std::vector<double> Noeud::getLowers()const
{
	return this->lowers;
}

void Noeud::setLowers(std::vector<double>& lo)
{
	this->lowers = lo;
}

void Noeud::addElement(const int& name, std::vector<double>& element, std::vector<int> element2)
{
	if (name == Noeud::HL)
	{
		this->Hl.push_back(element2);
	}
	else if(name == Noeud::HL_ORIGINE)
	{
		this->Hl_origine.push_back(element);
	}
	else if(name == Noeud::HL_CONSTRAINT)
	{
		this->Hl_constraint.push_back(element);
	}
}