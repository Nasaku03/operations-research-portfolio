#ifndef INCLUDED_NOEUD_HPP
#define INCLUDED_NOEUD_HPP

#include <vector>

class Noeud
{
private:
	unsigned int num;
	std::vector<double> uppers;
	std::vector<double> lowers;
	std::vector<std::vector<int> > Hl;
	std::vector<std::vector<double> > Hl_origine;
	std::vector<std::vector<double> > Hl_constraint;

public:
	const int HL = 0;
	const int HL_ORIGINE = 1;
	const int HL_CONSTRAINT = 2;
	Noeud(unsigned int, std::vector<std::vector<int> >&, std::vector<std::vector<double> >&, std::vector<std::vector<double> >&, std::vector<double>, std::vector<double>);
	~Noeud();

	unsigned int getNum()const;
	void setNum(unsigned int&);
	std::vector<std::vector<int> > getHl()const;
	void setHl(std::vector<std::vector<int> >&);
	std::vector<std::vector<double> > getHl_origine()const;
	void setHl_origine(std::vector<std::vector<double> >&);
	std::vector<std::vector<double> > getHl_constraint()const;
	void setHl_constraint(std::vector<std::vector<double> >&);
	std::vector<double> getUppers()const;
	void setUppers(std::vector<double>&);
	std::vector<double> getLowers()const;
	void setLowers(std::vector<double>&);
	void addElement(const int&, std::vector<double>&, std::vector<int> = std::vector<int>());
};

#endif // !INCLUDED_NOEUD_HPP
