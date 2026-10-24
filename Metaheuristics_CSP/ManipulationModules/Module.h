#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include <iostream>
#include <vector>
#include <map>

class Module
{
private:
	// Numéro du module
	int num;
	// Graphe du module
	std::map<int, std::vector<int> > G;
	// Liste des stables actuels
	std::vector<std::vector<int> > listStables;
	// Le type du module (prime: 0, parallèle : 1, série : 2)
	int type;
	// Le parent
	Module *parent;
	// Liste des enfants
	std::vector<Module*> enfants;

public:
	Module(int const&, std::map<int, std::vector<int> > &, int const&, Module*);
	virtual ~Module();

	int getNum()const;
	std::map<int, std::vector<int> > getG()const;
	std::vector<std::vector<int> > getListStables() const;
	int getType()const;
	Module* getParent() const;
	std::vector<Module*> getEnfants() const;

	void setNum(int const&);
	void setG(std::map<int, std::vector<int> > &);
	void setListStables(std::vector<std::vector<int> > &);
	void setType(int const&);
	void setParent(Module*);
	void setEnfants(std::vector<Module*>&);

	void addEnfant(Module*);

	friend std::ostream& operator<<(std::ostream&, Module&);

};

#endif // MODULE_H_INCLUDED
