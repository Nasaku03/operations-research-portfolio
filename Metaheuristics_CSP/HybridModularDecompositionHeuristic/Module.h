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
	Module();
	Module(int const&, std::map<int, std::vector<int> > &, int const&, Module*);
	virtual ~Module();

	inline int getNum()const;
	inline std::map<int, std::vector<int> > getG()const;
	inline std::vector<std::vector<int> > getListStables() const;
	inline int getType()const;
	inline Module* getParent() const;
	inline std::vector<Module*> getEnfants() const;

	inline void setNum(int const&);
	inline void setG(std::map<int, std::vector<int> > &);
	inline void setListStables(std::vector<std::vector<int> > &);
	inline void setType(int const&);
	inline void setParent(Module*);
	inline void setEnfants(std::vector<Module*>&);

	inline void addEnfant(Module*);
	inline void addStable(std::vector<int>&);
	inline void addToStable(int const&, int const&);

	friend std::ostream& operator<<(std::ostream&, Module&);

};

#endif // MODULE_H_INCLUDED
