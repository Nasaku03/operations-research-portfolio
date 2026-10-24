#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include <iostream>
#include <vector>

class Module
{
private:
	int nom;
	// List des noms des sommets (nom du module => module = sommet) qui construisent le module complet
	std::vector<int> sommets;
	// List des noms des  sous modules qui construisent le module
	//std::vector<Module> sous;
	int bouchra;
	// 0: prime ; 1: parallele; 2: série
	int degenerate;
	int niveau;
	bool strong, trivial;

public:
	Module();
	Module(int const&);
	Module(int const&, std::vector<int> const&, /*std::vector<Module> const&,*/ int const&, int const&, int const&, bool const&, bool const&);
	virtual ~Module();

	int getNom()const;
	std::vector<int> getSommets()const;
	//std::vector<Module> getSous() const;
	int getBouchra() const;
	int getDegenerate() const;
	int getNiveau() const;
	bool getStrong() const;
	bool getTrivial()const;
	void setBouchra(int const&);
	void setNom(int const&);
	void setSommets(std::vector<int> const&);
	//void setSous(std::vector<int> const&);
	void setDegenerate(int const&);
	void setNiveau(int const&);
	void setStrong(bool const&);
	void setTrivial(bool const&);
	friend std::ostream& operator<<(std::ostream&, Module&);

};

#endif // MODULE_H_INCLUDED
