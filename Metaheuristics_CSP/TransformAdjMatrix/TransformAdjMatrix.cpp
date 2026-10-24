// TransformAdjMatrix.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

void lectureDimacs(unsigned int& n, unsigned int& m, unsigned int& delta, vector<vector<int> >& A)
{
	ifstream data("dimacs.txt");
	string info;
	data >> info;
	data >> info;
	data >> n;
	data >> m;
	vector<int> ligneA;
	for (unsigned int j = 0; j < n; j++)
	{
		ligneA.push_back(0);
	}
	for (unsigned int i = 0; i < m; i++)
	{
		A.push_back(ligneA);
	}
	int s1, s2;
	for (int i = 0; i < m; i++)
	{
		data >> info;
		data >> s1;
		data >> s2;
		A[i][s1 - 1] = 1;
		A[i][s2 - 1] = 1;
	}
	unsigned int max_;
	vector<unsigned int> somme(n, 0);
	for (unsigned int j = 0; j < n; j++)
	{
		for (unsigned int i = 0; i < A.size(); i++)
		{
			somme[j] += A[i][j];
		}
	}
	max_ = somme[0];
	for (unsigned int j = 1; j < n; j++)
	{
		if (max_ < somme[j])
		{
			max_ = somme[j];
		}
	}
	delta = max_ + 1;
}

void lireData(vector<vector<int> >& A, unsigned int& m, unsigned int& n, unsigned int& delta)
{
	ifstream fichier("input.txt");
	unsigned int var, contr, mot; //Une variable pour stocker les lignes lues
	fichier >> var;
	fichier >> contr;
	vector<int> inter;
	cout << var << endl;
	cout << contr << endl;
	for (unsigned int i = 0; i < contr; i++)
	{
		for (unsigned int j = 0; j < var; j++)
		{
			fichier >> mot;
			inter.push_back(mot);
		}
		A.push_back(inter);
		inter.clear();
	}
	n = var; m = contr;
	unsigned int max_;
	vector<unsigned int> somme(n, 0);
	for (unsigned int j = 0; j < n; j++)
	{
		for (unsigned int i = 0; i < m; i++)
		{
			somme[j] += A[i][j];
		}
	}
	max_ = somme[0];
	for (unsigned int j = 1; j < n; j++)
	{
		if (max_ < somme[j])
		{
			max_ = somme[j];
		}
	}
	delta = max_ + 1;
}


int main()
{
	unsigned int n, m, delta;
	int Z;
	vector<vector<int> > A;

	ofstream out("out.out");

	lireData(A, m, n, delta);
	//lectureDimacs(n, m, delta, A);

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			if (j != m - 1)
			{
				out << A[j][i] << ",";
			}
			else
			{
				out << A[j][i] << endl;
			}			
		}
	}
	return 0;
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
