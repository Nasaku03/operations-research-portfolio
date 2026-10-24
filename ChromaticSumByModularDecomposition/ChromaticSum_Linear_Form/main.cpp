#include <ilopl/iloopl.h>

#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <string>

using namespace std;

void lectureDimacs(unsigned int& n, unsigned int& m, unsigned int& delta, vector<vector<int> > & A)
{
	ifstream data("dimacs.txt");
	string info;
	data >> info;
	data >> info;
	data >> n;
	data >> m;
	vector<int> ligneA;
	for (unsigned int j = 0; j<n; j++)
	{
		ligneA.push_back(0);
	}
	for (unsigned int i = 0; i<m; i++)
	{
		A.push_back(ligneA);
	}
	int s1, s2;
	for (int i = 0; i<m; i++)
	{
		data >> info;
		data >> s1;
		data >> s2;
		A[i][s1 - 1] = 1;
		A[i][s2 - 1] = 1;
	}
	unsigned int max_;
	//unsigned int somme[n];
	vector<unsigned int> somme(n, 0);
	for (unsigned int j = 0; j<n; j++)
	{
		for (unsigned int i = 0; i<m; i++)
		{
			somme[j] += A[i][j];
		}
	}
	max_ = somme[0];
	for (unsigned int j = 1; j<n; j++)
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
	for (unsigned int i = 0; i<contr; i++)
	{
		for (unsigned int j = 0; j<var; j++)
		{
			fichier >> mot;
			inter.push_back(mot);
		}
		A.push_back(inter);
		inter.clear();
	}
	n = var; m = contr;
	/*
	cout << var << '\n' << contr << endl;

	cout << "A=\n" <<endl;

	for(unsigned int i=0; i<contr; i++)
	{
	for(unsigned int j=0; j<var; j++)
	{
	cout << A[i][j] << '\t';
	}
	cout << '\n';
	}
	*/
	unsigned int max_;
	//unsigned int somme[n];
	vector<unsigned int> somme(n, 0);
	for (unsigned int j = 0; j<n; j++)
	{
		for (unsigned int i = 0; i<m; i++)
		{
			somme[j] += A[i][j];
		}
	}
	max_ = somme[0];
	for (unsigned int j = 1; j<n; j++)
	{
		if (max_ < somme[j])
		{
			max_ = somme[j];
		}
	}
	delta = max_ + 1;
}

static void populatebyrow(IloModel model, IloIntVarArray x, IloRangeArray c, vector<vector<int> >& A, unsigned int& delta)
{
	IloEnv env = model.getEnv();

	for (int i = 0; i < A[0].size(); i++)
	{
		for (int j = 0; j < delta; j++)
		{
			stringstream sstm;
			sstm << "x" << i << "_" << j;
			string result = sstm.str();
			x.add(IloIntVar(env, 0, 1, result.data()));
		}
	}
	
	IloNumExpr obj(env);
	for (int i = 0; i < A[0].size(); i++)
	{
		for (int j = 0; j < delta; j++)
		{
			obj += (j + 1)*x[i*A[0].size()+j];
		}
	}
	model.add(IloMinimize(env, obj));

	for (int i = 0; i < A[0].size(); i++)
	{
		IloNumExpr cons(env);
		for (int j = 0; j < delta; j++)
		{
			cons += x[i*A[0].size() + j];
		}
		c.add(cons == 1);
	}

	for (int k = 0; k < A.size(); k++)
	{
		for (int j = 0; j < delta; j++)
		{
			IloNumExpr cons(env);
			for (int i = 0; i < A[0].size(); i++)
			{
				cons += A[k][i] * x[i*A[0].size() + j];
			}
			c.add(cons <= 1);
		}
	}
	for (int j = 0; j < delta; j++)
	{
		IloNumExpr cons(env);
		for (int i = 0; i < A[0].size(); i++)
		{
			cons += x[i*A[0].size() + j];
		}
		c.add(cons >= 0);
	}
	model.add(c);

}

void writeDatFileEfficience(vector<vector<int> >& A, unsigned int& delta)
{
	//cout << "WriteFileEfficience" << endl;
	ofstream data;
	data.open("ChromaticSum.dat", ofstream::trunc);
	data << "n = " << A[0].size() << ";\nm = " << A.size() << ";\ndelta = " << delta << ";\nA = [";
	for (unsigned int i = 0; i < A.size(); i++)
	{
		data << "[";
		for (int j = 0; j < A[i].size(); j++)
		{
			data << A[i][j] << " ";
		}
		if (i == A.size() - 1)
		{
			data << "]";
		}
		else
		{
			data << "],\n";
		}
	}
	data << "];" << endl;
}

int getCoutSequence(vector<int>& sequence)
{
	int cout(0);
	for (int i = 0; i < sequence.size(); i++)
	{
		cout += sequence[i] * (i + 1);
	}
	return cout;
}

void generationFeasibleSequencesByProgramming(set<vector<int> >& sequences, unsigned int& cout, unsigned int& n, int& sg)
{
	bool first = true, firstSup = true; int couleurMin(2), couleurMax(sg - 1);
	// Détection de la couleur minimale
	// Trouver les séquences
	for (int i = couleurMax; i >= couleurMin; i--)
	{
		IloEnv env;
		IloModel model(env, "MultipleSequences");
		IloIntVarArray vars(env);
		IloRangeArray rngs(env);


		for (int j = 0; j < i; j++)
		{
			vars.add(IloIntVar(env));
		}
		model.add(vars);
		IloExpr exp(env);
		IloExpr exp2(env);
		for (int j = 0; j < i; j++)
		{
			exp += vars[j];
			exp2 += (j + 1)*vars[j];
		}
		rngs.add(IloRange(env, n, exp, n));
		rngs.add(IloRange(env, cout + 1, exp2));

		for (int j = 0; j < (i - 1); j++)
		{
			rngs.add(IloRange(env, 0, vars[j] - vars[j + 1]));
		}
		for (int j = 0; j < i; j++)
		{
			rngs.add(IloRange(env, 1, vars[j]));
		}
		model.add(rngs);

		model.add(IloMaximize(env, exp));

		IloCplex cplex(model);

		cplex.setParam(IloCplex::Param::MIP::Pool::Intensity, 4);
		cplex.setParam(IloCplex::Param::MIP::Limits::Populate, 100000000);
		if (cplex.populate())
		{
			IloNumArray vals(env);
			for (int j = 0; j < cplex.getSolnPoolNsolns(); j++)
			{
				cplex.getValues(vals, vars, j);
				vector<int> seq;
				for (int k = 0; k < i; k++)
				{
					seq.push_back(round(vals[k]));
				}
				sequences.insert(seq);
			}
		}
		else
		{
			env.end();
			break;
		}
		env.end();
	}
}

void generationSequencesByProgramming(set<vector<int> >& sequences, unsigned int& cout, unsigned int& n)
{
	bool first = true, firstSup = true; int couleurMin(0), couleurMax(0), alpha;
	// Détection de la couleur minimale
	vector<int> first_, last_;
	//std::cout << "n = " << n << endl;
	//std::cout << "cout = " << //cout << endl;
	for (int i = 1; i <= n; i++)
	{
		for (int j = 1; j <= (n%i); j++)
		{
			last_.push_back(ceil((double)n / (double)i));
		}
		for (int j = (n%i) + 1; j <= i; j++)
		{
			last_.push_back(floor((double)n / (double)i));
		}
		if (getCoutSequence(last_) > cout)
		{
			couleurMin = i;
			break;
		}
		else
		{
			last_.clear();
		}
	}
	if (couleurMin == 0)
	{
		couleurMin = n;
	}
	//std::cout << "Col min = " << couleurMin << endl;
	// Détection couleur maximum
	for (int i = 1; i <= n; i++)
	{
		first_.push_back(n - i + 1);
		for (int j = 2; j <= i; j++)
		{
			first_.push_back(1);
		}
		if (getCoutSequence(first_) > cout)
		{
			couleurMax = i;
			break;
		}
		else
		{
			first_.clear();
		}
	}
	if (couleurMax == 0)
	{
		couleurMax = n;
	}
	//std::cout << "Col max = " << couleurMax << endl;
	// Trouver les séquences
	for (int i = couleurMin; i <= couleurMax; i++)
	{
		IloEnv env;
		IloModel model(env, "MultipleSequences");
		IloIntVarArray vars(env);
		IloRangeArray rngs(env);


		for (int j = 0; j < i; j++)
		{
			vars.add(IloIntVar(env));
		}
		model.add(vars);
		IloExpr exp(env);
		IloExpr exp2(env);
		for (int j = 0; j < i; j++)
		{
			exp += vars[j];
			exp2 += (j + 1)*vars[j];
		}
		rngs.add(IloRange(env, n, exp, n));
		rngs.add(IloRange(env, cout, exp2, cout));

		for (int j = 0; j < (i - 1); j++)
		{
			rngs.add(IloRange(env, 0, vars[j] - vars[j + 1]));
		}
		for (int j = 0; j < i; j++)
		{
			rngs.add(IloRange(env, 1, vars[j]));
		}
		model.add(rngs);

		model.add(IloMaximize(env, exp));

		IloCplex cplex(model);

		cplex.setParam(IloCplex::Param::MIP::Pool::Intensity, 4);
		cplex.setParam(IloCplex::Param::MIP::Limits::Populate, 100000000);
		if (cplex.populate())
		{
			IloNumArray vals(env);
			for (int j = 0; j < cplex.getSolnPoolNsolns(); j++)
			{
				cplex.getValues(vals, vars, j);
				vector<int> seq;
				for (int k = 0; k < i; k++)
				{
					seq.push_back(round(vals[k]));
				}
				sequences.insert(seq);
			}
		}
		env.end();
	}
}

void getBestSolution(vector<vector<vector<int> > >& solution)
{
	vector<int> sizes;
	for (int i = 0; i < solution.size(); i++)
	{
		sizes.push_back(solution[i].size());
	}
	int m = *min_element(sizes.begin(), sizes.end());
	for (vector<vector<vector<int> > >::iterator it = solution.end() - 1; it != solution.begin(); it--)
	{
		if (it->size() != m)
		{
			solution.erase(it);
		}
	}
	if (solution[0].size() != m)
	{
		solution.erase(solution.begin());
	}
}

void afficher(vector<int> vec)
{
	cout << '[';
	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec[i] << " ";
	}
	cout << ']' << endl;
}

void lectureDimacs0(unsigned int& n, unsigned int& m, unsigned int& delta, vector<vector<int> > & A)
{
	ifstream data("dimacs.txt");
	string info;
	data >> info;
	data >> info;
	data >> n;
	data >> m;
	vector<int> ligneA;
	for (unsigned int j = 0; j<n; j++)
	{
		ligneA.push_back(0);
	}
	for (unsigned int i = 0; i<m; i++)
	{
		A.push_back(ligneA);
	}
	int s1, s2;
	for (int i = 0; i<m; i++)
	{
		data >> info;
		data >> s1;
		data >> s2;
		A[i][s1] = 1;
		A[i][s2] = 1;
	}
	unsigned int max_;
	//unsigned int somme[n];
	vector<unsigned int> somme(n, 0);
	for (unsigned int j = 0; j<n; j++)
	{
		for (unsigned int i = 0; i<m; i++)
		{
			somme[j] += A[i][j];
		}
	}
	max_ = somme[0];
	for (unsigned int j = 1; j<n; j++)
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
	vector<vector<int> > A, solution;
	unsigned int n, m, delta;
	unsigned int z;
	char c;

	// Lecture des données
	//lireData(A, m, n, delta);

	//lectureDimacs(n, m, delta, A);
	lectureDimacs0(n, m, delta, A);
	// simplification(A, m);
	//delta = 92;
	writeDatFileEfficience(A, delta);
	IloEnv env;
	set<vector<int> > sequences, sequencesFin;
	float debut = clock();
	try
	{
		IloOplRunConfiguration rc1(env, "ChromaticSum.mod", "ChromaticSum.dat");
		rc1.getCplex().setParam(IloCplex::NodeSel, CPX_NODESEL_DFS);
		rc1.getCplex().setParam(IloCplex::Param::MIP::Strategy::MIQCPStrat, 1);
		rc1.getCplex().setOut(env.getNullStream());
		rc1.getOplModel().generate();
		if (rc1.getCplex().solve())
		{
			IloIntMap t = rc1.getOplModel().getElement("x").asIntMap();
			IloNumArray vals(env);
			//cplex.getValues(vals, var);
			for (IloInt i = 1; i <= t.getSize(); i++)
			{
				IloIntMap t_sub(t.getSub(i));
				vector<int> sous;
				for (IloInt j = 1; j <= t_sub.getSize(); j++)
				{
					sous.push_back(t_sub.get(j));
				}
				solution.push_back(sous);
			}
		}
		cout << "Solution: [";
		for (int i = 0; i < solution.size(); i++)
		{
			cout << "[";
			for (int j = 0; j < solution[i].size(); j++)
			{
				cout << solution[i][j] << " ";
			}
			if (i == solution.size() - 1)
			{
				cout << "]";
			}
			else
			{
				cout << "], ";
			}
		}
		cout << "]" << endl;
		vector<vector<int> > sol;
		for (int i = 0; i < solution.size(); i++)
		{
			vector<int> inter;
			sol.push_back(inter);
		}
		for (int i = 0; i < solution.size(); i++)
		{
			for (int j = 0; j < solution[i].size(); j++)
			{
				if (solution[i][j] == 1)
				{
					sol[j].push_back(i + 1);
					break;
				}
			}
		}
		for (int i = sol.size() - 1; i > -1; i--)
		{
			if (sol[i].empty())
			{
				sol.erase(sol.begin() + i);
			}
			else
			{
				break;
			}
		}
		cout << "Solution finale: [";
		for (int i = 0; i < sol.size(); i++)
		{
			cout << "[";
			for (int j = 0; j < sol[i].size(); j++)
			{
				cout << sol[i][j] << " ";
			}
			if (i == sol.size() - 1)
			{
				cout << "]";
			}
			else
			{
				cout << "], ";
			}
		}
		cout << "]\nSequence : [" << endl;
		for (int i = 0; i < sol.size(); i++)
		{
			cout << sol[i].size() << " ";
		}
		cout << "]\ns(G) = " << sol.size() << endl;
		cout << "Objective = " << rc1.getCplex().getObjValue() << endl;
		env.out() << "Temps CPLEX : " << rc1.getCplex().getTime() << endl;
		float fin = (float)(clock() - debut) / CLOCKS_PER_SEC;
		cout << "Temps mine = " << fin << " seconds" << endl;
		rc1.end();
	}
	catch (IloException& e) {
		cerr << "Concert exception caught: " << e << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
	}
	env.end();
	cout << "ENTRER UN CARACTERE POUR  FERMER" << endl;
	cin >> c;
}