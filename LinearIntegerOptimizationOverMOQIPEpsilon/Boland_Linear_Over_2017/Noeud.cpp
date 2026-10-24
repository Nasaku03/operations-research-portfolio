#include "Noeud.h"

Noeud::Noeud(int l, double& f_l, std::vector<double> x_0, std::vector<std::vector<double> > YTild, std::vector<double> y, std::vector<double> y_L)
{
	this->l = l;
	this->f_l = f_l;
	this->x_0 = std::vector<double>(x_0);
	this->YTild = std::vector<std::vector<double> >(YTild);
	this->y = std::vector<double>(y);
	this->y_L = std::vector<double>(y_L);
}

Noeud::~Noeud()
{

}

double Noeud::getF_l()const
{
	return this->f_l;
}

void Noeud::setF_l(double& f_l)
{
	this->f_l = f_l;
}

int Noeud::getL()const
{
	return this->l;
}

void Noeud::setL(int& l)
{
	this->l = l;
}

std::vector<double> Noeud::getX_0()const
{
	return this->x_0;
}

void Noeud::setX_0(std::vector<double>& x_0)
{
	this->x_0 = x_0;
}

std::vector<std::vector<double> > Noeud::getYTild()const
{
	return this->YTild;
}

void Noeud::setYTild(std::vector<std::vector<double> >& YTild)
{
	this->YTild = YTild;
}

std::vector<double> Noeud::getY()const
{
	return this->y;
}

void Noeud::setY(std::vector<double>& y)
{
	this->y = y;
}

std::vector<double> Noeud::getY_L()const
{
	return this->y_L;
}

void Noeud::setY_L(std::vector<double>& y_L)
{
	this->y_L = y_L;
}

bool operator==(std::vector<double>& v1, std::vector<double>& v2)
{
	if (v1.size() != v2.size())
	{
		return false;
	}
	else
	{
		for (int i = 0; i < v1.size(); i++)
		{
			if (v1[i] != v2[i])
			{
				return false;
			}
		}
	}
	return true;
}

bool operator!=(std::vector<double>& v1, std::vector<double>& v2)
{
	return !(v1 == v2);
}

bool operator<= (std::vector<double>& v1, std::vector<double>& v2)
{
	if (v1.size() != v2.size())
	{
		return false;
	}
	else
	{
		int nb = 0;
		for (int i = 0; i < v1.size(); i++)
		{
			if (v1[i] <= v2[i])
			{
				nb++;
			}
		}
		if (nb == v1.size())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool operator>= (std::vector<double>& v1, std::vector<double>& v2)
{
	if (v1.size() != v2.size())
	{
		return false;
	}
	else
	{
		int nb = 0;
		for (int i = 0; i < v1.size(); i++)
		{
			if (v1[i] >= v2[i])
			{
				nb++;
			}
		}
		if (nb == v1.size())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool operator< (std::vector<double>& v1, std::vector<double>& v2)
{
	if (v1.size() != v2.size())
	{
		return false;
	}
	else
	{
		int nb = 0;
		for (int i = 0; i < v1.size(); i++)
		{
			if (v1[i] < v2[i])
			{
				nb++;
			}
		}
		if (nb == v1.size())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool operator> (std::vector<double>& v1, std::vector<double>& v2)
{
	if (v1.size() != v2.size())
	{
		return false;
	}
	else
	{
		int nb = 0;
		for (int i = 0; i < v1.size(); i++)
		{
			if (v1[i] > v2[i])
			{
				nb++;
			}
		}
		if (nb == v1.size())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}