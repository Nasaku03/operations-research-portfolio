#ifndef INCLUDED_NOEUD_HPP
#define INCLUDED_NOEUD_HPP

#include <vector>

class Noeud
{
private:
	double f_l;
	int q;
	int l;
	std::vector<double> x_0;
	std::vector<double> y;
	std::vector<double> y_L;
	std::vector<std::vector<double> > YTild;

public:
	Noeud(int, int, double&, std::vector<double>, std::vector<std::vector<double> >, std::vector<double>, std::vector<double>);
	~Noeud();

	double getF_l() const;
	void setF_l(double&);
	int getQ() const;
	void setQ(int&);
	int getL() const;
	void setL(int&);
	std::vector<double> getX_0() const;
	void setX_0(std::vector<double>&);
	std::vector<std::vector<double> > getYTild()const;
	void setYTild(std::vector<std::vector<double> >&);
	std::vector<double> getY()const;
	void setY(std::vector<double>&);
	std::vector<double> getY_L()const;
	void setY_L(std::vector<double>&);
};

bool operator==(std::vector<double>&, std::vector<double>&);
bool operator!=(std::vector<double>&, std::vector<double>&);
bool operator<=(std::vector<double>&, std::vector<double>&);
bool operator>=(std::vector<double>&, std::vector<double>&);
bool operator<(std::vector<double>&, std::vector<double>&);
bool operator>(std::vector<double>&, std::vector<double>&);

#endif // !INCLUDED_NOEUD_HPP
