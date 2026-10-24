#include <ilcplex/ilocplex.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "Noeud.h"

#define _PATH_EFFICIENT_BOLAND_ "C:\\Users\\m-ela.NINO\\Documents\\efficient_boland.gms"
//#define _PATH_OPT_BOLAND_ "C:\\Users\\CLIENT\\Documents\\Visual Studio 2015\\Projects\\LinearIntegerOptimizationOverMOQIPEpsilon\\x64\\Release\\optimal_boland.gms"
#define _PATH_BEST_OPT_ "C:\\Users\\m-ela.NINO\\Documents\\best_optimal_boland.gms"

using namespace std;

double f_u = numeric_limits<double>::infinity(), f_l = -numeric_limits<double>::infinity(), eps1 = 0.00000001, eps2 = 0.000000001;
vector<vector<double> > A, C, CM;
vector<vector<vector<double> > > M;
vector<double> b, obj;
vector<string> sign;
string model_opt = "";
int nb_eff = 0, nb_non_eff = 0;

void afficher(vector<vector<double> > mat)
{
	cout << '[' << endl;
	for (unsigned int i = 0; i < mat.size(); i++)
	{
		cout << '[';
		for (unsigned int j = 0; j < mat[0].size(); j++)
		{
			cout << mat[i][j] << ' ';
		}
		cout << "]\n";
	}
	cout << ']' << endl;
}

void afficher(vector<double> vec)
{
	cout << '[' << endl;
	for (unsigned int j = 0; j < vec.size(); j++)
	{
		cout << vec[j] << ' ';
	}
	cout << ']' << endl;
}

void readData()
{
	//cout << "read data" << endl;
	unsigned int n, m, p; char c;
	ifstream fichier("input.txt");
	//ofstream model(_PATH_EFFICIENT_BOLAND_);
	//ofstream model_best(_PATH_BEST_OPT_);
	if (fichier)
	{
		double mot; //Une variable pour stocker les lignes lues
		fichier >> n;
		fichier >> m;
		fichier >> p;
		//model << "Set j 'set of variables of indices' /1*" << n << "/;\nAlias (j,k);\nSet i 'set of constraints' /1*" << m << "/;\n";
		model_opt = model_opt + "Set j 'set of variables of indices' /1*" + to_string(n) + "/;\nAlias (j,k);\nSet i 'set of constraints' /1*" + to_string(m) + "/;\n";
		//model_best << "Set j 'set of variables of indices' /1*" << n << "/;\nAlias (j,k);\nSet i 'set of constraints' /1*" << m << "/;\n";
		//model << "Set q 'set of quad objectives' /1*" << q << "/;\nSet l 'set of lin objectives' /1*" << p << "/;\n";
		model_opt = model_opt + "Set q 'set of quad objectives' /1*" + to_string(q) + "/;\nSet l 'set of lin objectives' /1*" + to_string(p) + "/;\n";
		//model_best << "Set q 'set of quad objectives' /1*" << q << "/;\nSet l 'set of lin objectives' /1*" << p << "/;\n";
		//model << "Parameters\n\ty0(q) 'Upper Quad Bound'\n\ty1(q) 'Lower Quad Bound'\n\ty2(l) 'Upper Lin Bound'\n\ty3(l) 'Lower lin Bound'\n";
		model_opt = model_opt + "Parameter\n\ty1(q) 'Lower Quad Bound on y_L'\n\ty3(l) 'Lower lin Bound on y_L'\n";
		model_opt = model_opt + "\ty4(q) 'Upper bound quad on y'\n\ty5(l) 'upper bound lin on y'\n";
		//model_best << "Parameter\n\ty0(q) 'Upper Quad Bound on Y_Tild'\n\ty2(l) 'Upper Lin Bound on Y_Tild'\n";
		//model << "\tb(i) 'Second member vector'\n\tc(l,j) 'Lin obj vector'\n\tcm(q,j) 'Quad obj vector';\n";
		model_opt = model_opt + "\tb(i) 'Second member vector'\n\tc(l,j) 'Lin obj vector'\n\tcm(q,j) 'Quad obj vector'\n\ttarget(j) 'Objective Opt'\n\tM(q) 'Big Mq'\n\tMM(l) 'Big Ml';\n";
		model_opt = model_opt + "Scalar epsilon /0.1/;\n\n";
		//model_best << "\tb(i) 'Second member vector'\n\tc(l,j) 'Lin obj vector'\n\tcm(q,j) 'Quad obj vector'\n\ttarget(j) 'Objective Opt';\n";
		vector<double> inter;
		for (unsigned int k = 0; k < n; k++)
		{
			fichier >> mot;
			obj.push_back(mot);
		}
		for (unsigned int i = 0; i < m; i++)
		{
			for (unsigned int j = 0; j < n; j++)
			{
				fichier >> mot;
				inter.push_back(mot);
			}
			A.push_back(inter);
			inter.clear();
			fichier >> c;
			fichier >> mot;
			sign.push_back((c == '<' ? "<=" : (c == '>' ? ">=" : "==")));
			b.push_back(mot);
		}
		//model << "Table A(i,j) 'Lin constraints matrix'\n\t";
		model_opt = model_opt + "Table A(i,j) 'Lin constraints matrix'\n\t";
		//model_best << "Table A(i,j) 'Lin constraints matrix'\n\t";
		for (unsigned int i = 1; i <= n; i++)
		{
			//model << i << "\t";
			model_opt = model_opt + to_string(i) + "\t";
			//model_best << i << "\t";
		}
		for (unsigned int i = 0; i < m; i++)
		{
			//model << "\n" << (i + 1) << "\t";
			model_opt = model_opt + "\n" + to_string(i + 1) + "\t";
			//model_best << "\n" << (i + 1) << "\t";
			for (unsigned int j = 0; j < n; j++)
			{
				//model << (int)A[i][j] << "\t";
				model_opt = model_opt + to_string((int)A[i][j]) + "\t";
				//model_best << (int)A[i][j] << "\t";
			}
		}
		//model << ";\nTable QL(j,q) 'Matrice des objectifs lin de la partie quad'\n\t";
		model_opt = model_opt + ";\nTable QL(j,q) 'Matrice des objectifs lin de la partie quad'\n\t";
		//model_best << ";\nTable QL(j,q) 'Matrice des objectifs lin de la partie quad'\n\t";
		for (unsigned int i = 1; i <= q; i++)
		{
			//model << i << "\t";
			model_opt = model_opt + to_string(i) + "\t";
			//model_best << i << "\t";
		}
		for (int h = 0; h < q; h++)
		{
			vector<vector<double> > MM;
			for (unsigned int j = 0; j < n; j++)
			{
				for (unsigned int jj = 0; jj < n; jj++)
				{
					fichier >> mot;
					inter.push_back(mot);
				}
				MM.push_back(inter);
				inter.clear();
			}
			M.push_back(MM);
		}
		for (unsigned int k = 0; k < q; k++)
		{
			for (unsigned int j = 0; j < n; j++)
			{
				fichier >> mot;
				inter.push_back(mot);
			}
			CM.push_back(inter);
			inter.clear();
		}
		for (unsigned int j = 0; j < n; j++)
		{
			//model << "\n" <<  (j + 1) << "\t";
			model_opt = model_opt + "\n" + to_string(j + 1) + "\t";
			//model_best << "\n" << (j + 1) << "\t";
			for (unsigned int k = 0; k < q; k++)
			{
				//model << (int)CM[k][j] << "\t";
				model_opt = model_opt + to_string((int)CM[k][j]) + "\t";
				//model_best << (int)CM[k][j] << "\t";
			}
		}
		//model << ";\nTable LL(j,l) 'Matrice des objectifs lin de la partie lin'\n\t";
		model_opt = model_opt + ";\nTable LL(j,l) 'Matrice des objectifs lin de la partie lin'\n\t";
		//model_best << ";\nTable LL(j,l) 'Matrice des objectifs lin de la partie lin'\n\t";
		for (unsigned int i = 1; i <= p; i++)
		{
			//model << i << "\t";
			model_opt = model_opt + to_string(i) + "\t";
			//model_best << i << "\t";
		}
		for (unsigned int k = 0; k < p; k++)
		{
			for (unsigned int j = 0; j < n; j++)
			{
				fichier >> mot;
				inter.push_back(mot);
			}
			C.push_back(inter);
			inter.clear();
		}
		for (unsigned int j = 0; j < n; j++)
		{
			//model << "\n" << (j + 1) << "\t";
			model_opt = model_opt + "\n" + to_string(j + 1) + "\t";
			//model_best << "\n" << (j + 1) << "\t";
			for (unsigned int k = 0; k < p; k++)
			{
				//model << (int)C[k][j] << "\t";
				model_opt = model_opt + to_string((int)C[k][j]) + "\t";
				//model_best << (int)C[k][j] << "\t";
			}
		}
		//model << ";\nTable QQ(q,j,j) 'Objectif quad';\n";
		model_opt = model_opt + ";\nTable QQ(q,j,j) 'Objectif quad';\n";
		//model_best << ";\nTable QQ(q,j,j) 'Objectif quad';\n";
		for (unsigned int h = 1; h <= q; h++)
		{
			for (unsigned int j = 1; j <= n; j++)
			{
				for (unsigned int jj = 1; jj <= n; jj++)
				{
					//model << "QQ('" << h << "','" << j << "','" << jj << "') = " << (int)M[h - 1][j - 1][jj - 1] << ";\n";
					model_opt = model_opt + "QQ('" + to_string(h) + "','" + to_string(j) + "','" + to_string(jj) + "') = " + to_string((int)M[h - 1][j - 1][jj - 1]) + ";\n";
					//model_best << "QQ('" << h << "','" << j << "','" << jj << "') = " << (int)M[h - 1][j - 1][jj - 1] << ";\n";
				}
			}
		}
		for (unsigned int h = 1; h <= q; h++)
		{
			//model << "cm('" << h << "', j) = QL(j, '" << h << "');\n";
			model_opt = model_opt + "cm('" + to_string(h) + "', j) = QL(j, '" + to_string(h) + "');\n";
			//model_best << "cm('" << h << "', j) = QL(j, '" << h << "');\n";
		}
		for (unsigned int h = 1; h <= p; h++)
		{
			//model << "c('" << h << "', j) = LL(j, '" << h << "');\n";
			model_opt = model_opt + "c('" + to_string(h) + "', j) = LL(j, '" + to_string(h) + "');\n";
			//model_best << "c('" << h << "', j) = LL(j, '" << h << "');\n";
		}
		for (unsigned int i = 0; i < b.size(); i++)
		{
			//model << "b('" << (i + 1) << "') = " << b[i] << ";\n";
			model_opt = model_opt + "b('" + to_string(i + 1) + "') = " + to_string(b[i]) + ";\n";
			//model_best << "b('" << (i + 1) << "') = " << b[i] << ";\n";
		}
		for (unsigned int i = 1; i <= q; i++)
		{
			model_opt = model_opt + "M('" + to_string(i) + "') = 10000000;\n";
		}
		for (unsigned int i = 1; i <= p; i++)
		{
			model_opt = model_opt + "MM('" + to_string(i) + "') = 10000000;\n";
		}
		for (unsigned int i = 1; i <= n; i++)
		{
			model_opt = model_opt + "target('" + to_string(i) + "') = " + to_string(obj[i - 1]) + ";\n";
			//model_best << "target('" << i << "') = " << obj[i - 1] << ";\n";
		}
		//model << "Variables\n\tx(j) 'variables'\n\tsoq(q) 'sub obj q'\n\tsol(l) 'sub obj l'\n\tobj 'objectif';\nInteger Variable x;\n";
		model_opt = model_opt + "Variable\n\tx(j) 'variables'\n\tsoq(q) 'sub obj q'\n\tsol(l) 'sub obj l'\n\tobj 'objectif'\n";
		model_opt = model_opt + "\tb3(q) 'choice1 on y'\n\tbc(l) 'choice2 on y';\nInteger Variable x;\nBinary Variables b3, bc;\n";
		//model_best << "Variable\n\tx(j) 'variables'\n\tsoq(q) 'sub obj q'\n\tsol(l) 'sub obj l'\n\tobj 'objectif';\n";
		//model_best << "Integer Variable x;\n";
		//model << "Equations\n\tf\t\t'Fonction Objectif'\n\tsubf(q)\t'Fonction quadratique'\n\tsubl(l)\t'Function Lineaires'\n\tconst(i)'1er groupe'\n\t";
		model_opt = model_opt + "Equation\n\tf\t\t'Fonction Objectif'\n\tsubf(q)\t'Fonction quadratique'\n\tsubl(l)\t'Function Lineaires'\n\tconst(i)'1er groupe'\n\t";
		//model_best << "Equation\n\tf\t\t'Fonction Objectif'\n\tsubf(q)\t'Fonction quadratique'\n\tsubl(l)\t'Function Lineaires'\n\tconst(i)'1er groupe'\n\t";
		//model << "bsq(q)\t'borne sup quad'\n\tbiq(q)\t'borne inf quad'\n\tbsl(l)\t'borne sup lin'\n\tbil(l)\t'born inf lin';\n";
		model_opt = model_opt + "biq(q)\t'borne inf quad on y_L'\n\tbil(l)\t'born inf lin on y_L'\n";
		model_opt = model_opt + "\tbsq2(q)\t'borne sup quad on y'\n\tbsl2(l)'borne sup lin on y'\n\tfixb2\t'fix b to 1 _ 2';\n";
		//model_best << "bsq(q)'borne sup quad on Y_Tild'\n\tbsl(l)'borne sup lin on Y_Tild';\n";
		//model << "subf(q)..\tsoq(q) =e= sum((j,k), QQ(q,j,k)*x(j)*x(k)) + sum(j, cm(q,j)*x(j));\n";
		model_opt = model_opt + "subf(q)..\tsoq(q) =e= sum((j,k), QQ(q,j,k)*x(j)*x(k)) + sum(j, cm(q,j)*x(j));\n";
		//model_best << "subf(q)..\tsoq(q) =e= sum((j,k), QQ(q,j,k)*x(j)*x(k)) + sum(j, cm(q,j)*x(j));\n";
		//model << "subl(l)..\tsol(l) =e= sum(j, c(l,j)*x(j));\n";
		model_opt = model_opt + "subl(l)..\tsol(l) =e= sum(j, c(l,j)*x(j));\n";
		//model_best << "subl(l)..\tsol(l) =e= sum(j, c(l,j)*x(j));\n";
		//model << "f..\t\t\tobj =e= sum(q, soq(q)) + sum(l, sol(l));\n";
		model_opt = model_opt + "f..\t\t\tobj =e= sum(j, target(j)*x(j));\n";
		//model_best << "f..\t\t\tobj =e= sum(j, target(j)*x(j));\n";
		//model << "const(i)..\tsum(j, A(i, j) * x(j)) =l= b(i);\n";
		model_opt = model_opt + "const(i)..\tsum(j, A(i, j) * x(j)) =l= b(i);\n";
		//model_best << "const(i)..\tsum(j, A(i, j) * x(j)) =l= b(i);\n";
		//model << "bsq(q)..\tsoq(q) =l= y0(q);\n";
		//model_best << "bsq(q)..\tsoq(q) =l= y0(q);\n";
		//model << "biq(q)..\tsoq(q) =g= y1(q);\n";
		model_opt = model_opt + "biq(q)..\tsoq(q) =g= y1(q);\n";
		//model << "bsl(l)..\tsol(l) =l= y2(l);\n";
		//model_best << "bsl(l)..\tsol(l) =l= y2(l);\n";
		//model << "bil(l)..\tsol(l) =g= y3(l);\n";
		model_opt = model_opt + "bil(l)..\tsol(l) =g= y3(l);\n";
		model_opt = model_opt + "bsq2(q)..\tsoq(q) =l= (y4(q) - epsilon - M(q))*b3(q) + M(q);\n";
		model_opt = model_opt + "bsl2(l)..\tsol(l) =l= (y5(l) - epsilon - MM(l))*bc(l) + MM(l);\n";
		model_opt = model_opt + "fixb2..\t\tsum(q, b3(q)) + sum(l, bc(l)) =e= 1;\n";
		//model << "Model boland /all/;\noption MINLP = gurobi;\n";
		//model_best << "Model boland /all/;\noption MINLP = gurobi;\n";
	}
	else
	{
		//cout << "We are out" << endl;
	}
	//model.close();
	fichier.close();
}

double ZLinear(vector<double>& c, vector<double>& x)
{
	double z(0.0);
	for (int i = 0; i < x.size(); i++)
	{
		z += c[i] * x[i];
	}
	return z;
}

bool initialPhase(vector<double>& c, vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<double> >& C, vector<vector<vector<double> > >& M, vector<vector<double> >& CM, vector<double>& x, double& z, vector<vector<double> >& SND)
{
	//cout << "initial phase" << endl;
	return true;
}

void generalPhase(vector<double>& c, vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<double> >& C, vector<vector<vector<double> > >& M, vector<vector<double> >& CM, vector<double>& x, double& z, vector<vector<double> >& SND)
{
	//cout << "general phase" << endl;
}

int main(int argc, char* argv[])
{
	vector<vector<double> > A, C, CM;
	vector<vector<vector<double> > > M;
	vector<vector<double> > SND;
	vector<int> H, N;
	vector<double> b, c, x;
	vector<string> sign;
	clock_t beg, ed, tm;

	double z = numeric_limits<double>::infinity();
	vector<Noeud> queue;

	beg = clock();
	cout << "0" << endl;
	readData();
	vector<double> x(C[0].size(), 0);
	beg = clock();

	if (initialPhase(c, A, b, sign, C, M, CM, x, z, SND))
	{
		generalPhase(c, A, b, sign, C, M, CM, x, z, SND);
	}
	ed = clock();
	tm = ed - beg;
	cout << "Solution Optimale :";
	for (unsigned int i = 0; i < x.size(); i++)
	{
		cout << x[i] << " ";
	}
	z = ZLinear(obj, x);
	cout << endl << "Cout = " << z << endl;
	cout << "Temps : " << (tm / 1000.0) << " seconds" << endl;
	cout << "Nombre de sol. eff. = " << nb_eff << endl;
	cout << "Nombre de sol. non eff. = " << nb_non_eff << endl;
	char h;
	cin >> h;
	return 0;
}