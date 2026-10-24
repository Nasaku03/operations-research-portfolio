#include "gams.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstring>

#include "Noeud.hpp"

using namespace std;
using namespace gams;

vector<vector<double> > A, C, CM, SND;
vector<vector<vector<double> > > M;
vector<double> b, obj;
vector<string> sign;
string model_opt = "", efficiency = "", epsilon_model = "", inverse_beg = "", inverse_end = "", ip_model = "";
unsigned int num_noeud = 0, dom_delete = 0, hl_delete = 0, cost_delete = 0, eff_delete = 0, empty_delete = 0, int_sol = 0, int_eff_sol = 0, int_epsilon_positive = 0, int_epsilon_null = 0, int_epsilon_abs = 0, int_epsilon_error = 0, epsilon_efficace_delete = 0, exist_find = 0, exist_not_find = 0;
clock_t quadTime(0), idealTime(0), idealLinTime(0), idealQuadTime(0), constructTime(0), existTime(0), epsilonTime(0), fileTime(0);
vector<Noeud> tree;
vector<Noeud> queue;

void afficher(vector<vector<double> >& mat)
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

void afficher(vector<double>& vec)
{
	cout << '[' << endl;
	for (unsigned int j = 0; j < vec.size(); j++)
	{
		cout << vec[j] << ' ';
	}
	cout << ']' << endl;
}

void afficher(vector<vector<int> >& mat)
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

void afficher(vector<int>& vec)
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
	unsigned int n, m, p, q; char c;
	ifstream fichier("input.txt");
	if (fichier)
	{
		double mot; //Une variable pour stocker les lignes lues
		fichier >> n;
		fichier >> m;
		fichier >> p;
		fichier >> q;
		model_opt = model_opt + "Set j 'Variables' /1*" + to_string(n) + "/;\nSet i 'Constraints' /1*" + to_string(m) + "/;\n";
		model_opt = model_opt + "Parameters\n\tf(j) 'function to optimize'\n\tA(i, j) 'matrix of constraints'\n\tb(i) 'second Member vector'\n\tuppers(j) 'upper bounds of variables'";
		model_opt = model_opt + "\n\tlowers(j) 'lower bounds of variables';\nPositive Variables\n\tx(j) 'principle variables';\nVariable\n\tobj 'Objective';\n";
		model_opt = model_opt + "Equations\n\tff 'Objective function'\n\tconst(i)'Principle constraints'\n\tup(j) 'Upper bounds'\n\tlo(j) 'Lower bounds';\n";
		model_opt = model_opt + "ff..    obj =e= sum(j, f(j)*x(j));\nconst(i)..sum(j, A(i, j) * x(j)) =l= b(i);\nup(j)..x(j) =l= uppers(j);\nlo(j)..x(j) =g= lowers(j);\n";
		efficiency = efficiency + "Set j 'Variables' /1*" + to_string(n) + "/;\nSet i 'Constraints' /1*" + to_string(m) + "/;\nSet q 'Quadratic Objectives' /1*" + to_string(q) + "/;\n";
		efficiency = efficiency + "Set l 'linear Objectives' /1*" + to_string(p) + "/;\nAlias(j,k);\n";
		efficiency = efficiency + "Parameters\n\tA(i, j) 'matrix of constraints'\n\tb(i) 'second Member vector'";
		efficiency = efficiency + "\n\tQQ(q,j,j) 'Quadratic objectives matrix'\n\tLQ(q,j) 'Linear part of quadratic objectives'\n\t";
		ip_model = efficiency;
		efficiency = efficiency + "LL(l,j) 'Linear objectives'\n\ty(j) 'Given solution to test';\nVariable\n\tx(j) 'principle variables'\n\tobj 'Objective';\nInteger Variable x;\n";
		efficiency = efficiency + "Positive Variables\n\tt(q) 'added variables for quadratic'\n\tt2(l)'added variables for linear';\n";
		efficiency = efficiency + "Equations\n\tff 'Objective function'\n\tconst(i)'Principle constraints'\n\t";
		efficiency = efficiency + "up(q) 'Upper bounds of quadratic objectives'\n\tlo(l) 'Lower bounds of linear objectives';\n";
		efficiency = efficiency + "ff..    obj =e= sum(q, t(q)) + sum(l, t2(l));\nconst(i)..sum(j, A(i, j) * x(j)) =l= b(i);\n";
		efficiency = efficiency + "up(q)..    sum((j,k), QQ(q,j,k)*x(j)*x(k)) + sum(j, LQ(q,j)*x(j)) =e= sum((j,k), QQ(q,j,k)*y(j)*y(k)) + sum(j, LQ(q,j)*y(j)) - t(q);\n";
		efficiency = efficiency + "lo(l)..    sum(j, LL(l,j)*x(j)) =e= sum(j, LL(l,j)*y(j)) - t2(l);\n";
		epsilon_model = epsilon_model + "Set j 'Variables' /1*" + to_string(n) + "/;\nSet i 'Constraints' /1*" + to_string(m) + "/;\n";
		epsilon_model = epsilon_model + "Parameters\n\tA(i, j) 'matrix of constraints'\n\tb(i) 'second Member vector'\n\tnHl(j) 'new Hl'\n\tuppers(j) 'upper bounds of variables'";
		epsilon_model = epsilon_model + "\n\tlowers(j) 'lower bounds of variables';\nScalar nHl_SM 'new Second member of Hl';\nVariables\n\tx(j) 'principle variables'\n\tobj 'Objective';\n";
		epsilon_model = epsilon_model + "Positive Variables\n\tepsilon 'epsilon';\nInteger Variable x;\n";
		epsilon_model = epsilon_model + "Equations\n\tff'Objective function'\n\tconst(i)'Principle constraints'\n\tup(j)'Upper bounds'\n\tlo(j)'Lower bounds'\n\tnec'new efficient cut';\n";
		epsilon_model = epsilon_model + "ff..    obj =e= epsilon;\nconst(i)..sum(j, A(i, j) * x(j)) =l= b(i);\nup(j)..x(j) =l= uppers(j);\nlo(j)..x(j) =g= lowers(j);\n";
		epsilon_model = epsilon_model + "nec..   sum(j, nHl(j)*x(j)) =e= nHl_SM - epsilon;\n";
		ip_model = ip_model + "LL(l,j) 'Linear objectives'\n\tuppers(j) 'upper bounds of variables'\n\tlowers(j) 'lower bounds of variables';\nVariable\n\tx(j) 'principle variables'";
		ip_model = ip_model + "\n\tobj 'Objective';\nInteger Variable x;\nEquations\n\tconst(i)'Principle constraints'\n\tup(j) 'Upper bounds'\n\tlo(j) 'Lower bounds';\n";
		ip_model = ip_model + "const(i)..sum(j, A(i, j) * x(j)) =l= b(i);\nup(j)..x(j) =l= uppers(j);\nlo(j)..x(j) =g= lowers(j);\n";
		inverse_beg = inverse_beg + "Alias (i,ip), (j,jp);\nParameter\n\ta(i, j) 'matrix to be inverted'\n\tb(j, i) 'inverse of a'\n\tbp(i, j)'permuted and transposed inverse of a'\n\t";
		inverse_beg = inverse_beg + "pair(i, j) 'pivoting sequence and permutation'\n\trank 'rank of matrix a'\n\tadet 'absolute value of determinant of matrix a'\n\tpiv\n\tbig\n\ttol;\n";
		inverse_beg = inverse_beg + "Set\n\tr(i) 'pivot row candidates'\n\tnpr(i) 'non pivot rows'\n\ts(j) 'pivot column candidates'\n\tnps(j) 'non pivot columns';\n";
		inverse_end = inverse_end + "r(i) = yes;\ns(j) = yes;\nbp(i, j) = a(i, j);\nrank = 0;\nadet = 1;\ntol = 1e-5;\nloop(j,\n\tbig = smax((r, s), abs(bp(r, s)));\n\t";
		inverse_end = inverse_end + "big$(big < tol) = 0; \n\tnpr(i) = yes; \n\tnps(jp) = yes; \n\t";
		inverse_end = inverse_end + "loop((r, s)$(big and big = abs(bp(r, s))),\n\t\trank = rank + 1;\n\t\tpair(r, s) = rank;\n\t\tpiv = 1 / bp(r, s);\n\t\tbig = 0;\n\t\t";
		inverse_end = inverse_end + "adet = adet / piv;\n\t\tnpr(r) = no;\n\t\tnps(s) = no;\n\t\tbp(r, nps) = bp(r, nps) * piv;\n\t\t";
		inverse_end = inverse_end + "bp(npr, nps) = bp(npr, nps) - bp(r, nps) * bp(npr, s);\n\t\tbp(npr, s) = -bp(npr, s) * piv;\n\t\tbp(r, s) = piv;\n\t);\n\tr(r) = npr(r);\n\t";
		inverse_end = inverse_end + "s(s) = nps(s);\n);\nb(j, i) = sum((ip, jp)$(pair(i, jp) and pair(ip, j)), bp(ip, jp));\nadet = abs(adet);\nloop((i,j)$(abs(b(j,i)) < tol), b(j,i) = 0);\n";

		vector<double> inter;
		for (unsigned int k = 0; k < n; k++)
		{
			fichier >> mot;
			obj.push_back(mot);
			model_opt = model_opt + "f('" + to_string(k + 1) + "') = " + to_string(obj[k]) + ";\n";
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
		for (unsigned int i = 0; i < m; i++)
		{
			for (unsigned int j = 0; j < n; j++)
			{
				model_opt = model_opt + "A('" + to_string(i + 1) + "','" + to_string(j + 1) + "') = " + to_string((int)A[i][j]) + ";\n";
				efficiency = efficiency + "A('" + to_string(i + 1) + "','" + to_string(j + 1) + "') = " + to_string((int)A[i][j]) + ";\n";
				epsilon_model = epsilon_model + "A('" + to_string(i + 1) + "','" + to_string(j + 1) + "') = " + to_string((int)A[i][j]) + ";\n";
				ip_model = ip_model + "A('" + to_string(i + 1) + "','" + to_string(j + 1) + "') = " + to_string((int)A[i][j]) + ";\n";
			}
		}
		
		for (unsigned int i = 1; i <= m; i++)
		{
			model_opt = model_opt + "b('" + to_string(i) + "') = " + to_string(b[i - 1]) + ";\n";
			efficiency = efficiency + "b('" + to_string(i) + "') = " + to_string(b[i - 1]) + ";\n";
			epsilon_model = epsilon_model + "b('" + to_string(i) + "') = " + to_string(b[i - 1]) + ";\n";
			ip_model = ip_model + "b('" + to_string(i) + "') = " + to_string(b[i - 1]) + ";\n";
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
		for (unsigned int h = 1; h <= q; h++)
		{
			for (unsigned int j = 1; j <= n; j++)
			{
				for (unsigned int jj = 1; jj <= n; jj++)
				{
					efficiency = efficiency + "QQ('" + to_string(h) + "','" + to_string(j) + "','" + to_string(jj) + "') = " + to_string((int)M[h - 1][j - 1][jj - 1]) + ";\n";
					ip_model = ip_model + "QQ('" + to_string(h) + "','" + to_string(j) + "','" + to_string(jj) + "') = " + to_string((int)M[h - 1][j - 1][jj - 1]) + ";\n";
				}
			}
		}
		for (unsigned int h = 1; h <= q; h++)
		{
			for (unsigned int j = 1; j <= n; j++)
			{
				efficiency = efficiency + "LQ('" + to_string(h) + "','" + to_string(j) + "') = " + to_string((int)CM[h - 1][j - 1]) + ";\n";
				ip_model = ip_model + "LQ('" + to_string(h) + "','" + to_string(j) + "') = " + to_string((int)CM[h - 1][j - 1]) + ";\n";
			}
		}
		for (unsigned int h = 1; h <= p; h++)
		{
			for (unsigned int j = 1; j <= n; j++)
			{
				efficiency = efficiency + "LL('" + to_string(h) + "','" + to_string(j) + "') = " + to_string((int)C[h - 1][j - 1]) + ";\n";
				ip_model = ip_model + "LL('" + to_string(h) + "','" + to_string(j) + "') = " + to_string((int)C[h - 1][j - 1]) + ";\n";
			}
		}
		efficiency = efficiency + "model principle /all/;\n";
	}
	else
	{
		//cout << "We are out" << endl;
	}
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

double ZQuad(vector<vector<double> >& Q, vector<double>& CM, vector<double>& x)
{
	double z(0.0);
	for (unsigned int i = 0; i < Q.size(); i++)
	{
		for (unsigned int j = 0; j < Q[i].size(); j++)
		{
			z += Q[i][j] * x[i] * x[j];
		}
	}
	for (unsigned int i = 0; i < CM.size(); i++)
	{
		z += CM[i] * x[i];
	}
	return z;
}

vector<double> Z(vector<vector<vector<double> > >& M, vector<vector<double> >& CM, vector<vector<double> >& C, vector<double> x)
{
	vector<double> vec;
	for (int k = 0; k < M.size(); k++)
	{
		vec.push_back(ZQuad(M[k], CM[k], x));
	}
	for (int i = 0; i < C.size(); i++)
	{
		vec.push_back(ZLinear(C[i], x));
	}
	return vec;
}

bool entiere(vector<double>& solution, int& ind)
{
	for (unsigned int i = 0; i < solution.size(); i++)
	{
		if ((solution[i] - floor(solution[i])) > 0)
		{
			ind = i;
			return false;
		}
	}
	return true;
}

void detectBases(GAMSJob& t, vector<int>& base, vector<int>& outBase, bool Hl_const)
{
	int i = 1;
	for (GAMSVariableRecord rec : t.outDB().getVariable("x"))
	{
		if (rec.marginal() != 0)
		{
			outBase.push_back(stoi(rec.key(0)));
		}
		else
		{
			base.push_back(stoi(rec.key(0)));
		}
		//cout << "x_" << i << " = " << rec.marginal() << endl;
		i++;
	}
	i = 1;
	for (GAMSEquationRecord rec : t.outDB().getEquation("const"))
	{
		if (rec.marginal() != 0)
		{
			outBase.push_back(stoi(rec.key(0)) + C[0].size());
		}
		else
		{
			base.push_back(stoi(rec.key(0)) + C[0].size());
		}
		//cout << "const_" << i << " = " << rec.marginal() << endl;
		i++;
	}
	i = 1;
	for (GAMSEquationRecord rec : t.outDB().getEquation("up"))
	{
		if (rec.marginal() != 0)
		{
			outBase.push_back(stoi(rec.key(0)) + C[0].size() + A.size());
		}
		else
		{
			base.push_back(stoi(rec.key(0)) + C[0].size() + A.size());
		}
		//cout << "up_" << i << " = " << rec.marginal() << endl;
		i++;
	}
	i = 1;
	for (GAMSEquationRecord rec : t.outDB().getEquation("lo"))
	{
		if (rec.marginal() != 0)
		{
			outBase.push_back(stoi(rec.key(0)) + 2 * C[0].size() + A.size());
		}
		else
		{
			base.push_back(stoi(rec.key(0)) + 2 * C[0].size() + A.size());
		}
		//cout << "lo_" << i << " = " << rec.marginal() << endl;
		i++;
	}
	i = 1;
	if (!Hl_const)
	{
		for (GAMSEquationRecord rec : t.outDB().getEquation("ec"))
		{
			if (rec.marginal() != 0)
			{
				outBase.push_back(stoi(rec.key(0)) + 3 * C[0].size() + A.size());
			}
			else
			{
				base.push_back(stoi(rec.key(0)) + 3 * C[0].size() + A.size());
			}
			//cout << "ec_" << i << " = " << rec.marginal() << endl;
			i++;
		}
	}
}

void extractMatrixBase(vector<int>& base, vector<vector<double> >& matrixBase, int rank, vector<vector<double> >& Hl)
{
	for (int i = 0; i < rank; i++)
	{
		matrixBase.push_back(vector<double>(rank, 0.0));
	}
	for (int j = 0; j < base.size(); j++)
	{
		if (base[j] <= C[0].size())
		{
			for (int i = 0; i < A.size(); i++)
			{
				matrixBase[i][j] = A[i][base[j] - 1];
			}
			for (int i = 0; i < C[0].size(); i++)
			{
				if (i + 1 == base[j])
				{
					matrixBase[A.size() + i][j] = 1;
				}
				else
				{
					matrixBase[A.size() + i][j] = 0;
				}
			}
			for (int i = 0; i < C[0].size(); i++)
			{
				if (i + 1 == base[j])
				{
					matrixBase[C[0].size() + A.size() + i][j] = -1;
				}
				else
				{
					matrixBase[C[0].size() + A.size() + i][j] = 0;
				}
			}
			if (!Hl.empty())
			{
				for (int i = 0; i < Hl.size(); i++)
				{
					/*cout << "i = " << i << endl;
					cout << "j = " << j << endl;
					cout << "in matrix base = " << (2 * C[0].size() + A.size() + i) << endl;
					cout << "base_j = " << base[j] << endl;
					cout << "Hl = " << Hl[i][base[j] - 1] << endl;*/
					matrixBase[2 * C[0].size() + A.size() + i][j] = -Hl[i][base[j] - 1];
				}
			}
		}
		else
		{
			for (int i = 0; i < A.size(); i++)
			{
				if (base[j] == C[0].size() + i + 1)
				{
					matrixBase[i][j] = 1;
				}
				else
				{
					matrixBase[i][j] = 0;
				}				
			}
			for (int i = 0; i < C[0].size(); i++)
			{
				if (C[0].size() + A.size() + i + 1 == base[j])
				{
					matrixBase[A.size() + i][j] = 1;
				}
				else
				{
					matrixBase[A.size() + i][j] = 0;
				}
			}
			for (int i = 0; i < C[0].size(); i++)
			{
				if (2 * C[0].size() + A.size() + i + 1 == base[j])
				{
					matrixBase[C[0].size() + A.size() + i][j] = 1;
				}
				else
				{
					matrixBase[C[0].size() + A.size() + i][j] = 0;
				}
			}
			if (!Hl.empty())
			{
				for (int i = 0; i < Hl.size(); i++)
				{
					if (base[j] == 3 * C[0].size() + A.size() + i + 1)
					{
						matrixBase[2 * C[0].size() + A.size() + i][j] = 1;
					}
					else
					{
						matrixBase[2 * C[0].size() + A.size() + i][j] = 0.0;
					}					
				}
			}
		}
	}
	for (int i = 0; i < matrixBase.size(); i++)
	{
		for (int j = 0; j < matrixBase.size(); j++)
		{
			matrixBase[i][j] = (abs(matrixBase[i][j]) < 0.0000001) ? 0.0 : matrixBase[i][j];
		}
	}
}

double deriverQuadratic(vector<vector<double> >& M, vector<double>& c, vector<double>& solution, int k)
{
	double der = 0.0;
	for (unsigned int i = 0; i < solution.size(); i++)
	{
		der += (M[i][k] + M[k][i]) * solution[i];
	}
	return (der + c[k]);
}

double functionBase(double** S, vector<vector<double> >& M, vector<double>& CM, vector<double>& x, int j, vector<int>& bases)
{
	double val(0.0);
	for (int i = 0; i < bases.size(); i++)
	{
		if (bases[i] <= x.size())
		{
			val += deriverQuadratic(M, CM, x, bases[i] - 1) * S[i][j];
		}
	}
	return val;
}

void computeS(double** S, int const& size_S_row, int const& size_S_col, vector<vector<double> >& inverseMatrix, vector<vector<double> >& Hl, vector<double>& uppers, vector<double>& lowers, vector<int>& base)
{	
	for (int i = 0; i < inverseMatrix.size(); i++)
	{
		for (int j = 0; j < size_S_col - 1; j++)
		{
			for (int k = 0; k < inverseMatrix.size(); k++)
			{
				if (k < A.size())
				{
					if (j < A[0].size())
					{
						S[i][j] += inverseMatrix[i][k] * A[k][j];
					}
					else if (j < A[0].size() + A.size() && j == A[0].size() + k)
					{
						S[i][j] += inverseMatrix[i][k];
					}
				}
				else if (k < A.size() + A[0].size())
				{
					if (j < A[0].size() && j == k - A.size())
					{
						S[i][j] += inverseMatrix[i][k];
					}
					else if (j >= A[0].size() && j == A[0].size() + k)
					{
						S[i][j] += inverseMatrix[i][k];
					}
				}
				else if (k < A.size() + 2 * A[0].size())
				{
					if (j < A[0].size() && j == k - A.size() - A[0].size())
					{
						S[i][j] -= inverseMatrix[i][k];
					}
					else if (j >= A[0].size() && j == A[0].size() + k)
					{
						S[i][j] += inverseMatrix[i][k];
					}
				}
				else
				{
					if (j < A[0].size())
					{
						S[i][j] -= inverseMatrix[i][k] * Hl[k - A.size() - 2 * A[0].size()][j];
					}
					else if(j >= A[0].size() && j == A[0].size() + k)
					{
						S[i][j] += inverseMatrix[i][k];
					}
				}
			}
			S[i][j] = (abs(S[i][j]) < 0.0000001) ? 0 : S[i][j];
		}
	}
	for (int i = 0; i < inverseMatrix.size(); i++)
	{
		for (int h = 0; h < inverseMatrix.size(); h++)
		{
			if (h < b.size())
			{
				S[i][size_S_col - 1] += inverseMatrix[i][h] * b[h];
			}
			else if(h < b.size() + A[0].size())
			{
				S[i][size_S_col - 1] += inverseMatrix[i][h] * uppers[h - b.size()];
			}
			else if (h < b.size() + 2 * A[0].size())
			{
				S[i][size_S_col - 1] -= inverseMatrix[i][h] * lowers[h - b.size() - A[0].size()];
			}
			else
			{
				S[i][size_S_col - 1] -= inverseMatrix[i][h] * Hl[h - b.size() - 2 * A[0].size()][A[0].size()];
			}
			S[i][h] = (abs(S[i][h]) < 0.0000001) ? 0 : S[i][h];
		}
	}
	vector<double> pi(inverseMatrix.size(), 0.0);
	for (int i = 0; i < inverseMatrix.size(); i++)
	{
		for (int j = 0; j < inverseMatrix.size(); j++)
		{
			if (base[j] - 1 < obj.size())
			{
				pi[i] += obj[base[j] - 1] * inverseMatrix[j][i];
			}
		}
	}
	for (int i = 0; i < size_S_col - 1; i++)
	{
		double value = 0.0;
		for (int j = 0; j < inverseMatrix.size(); j++)
		{
			if (j < A.size())
			{
				if (i < A[0].size())
				{
					value += pi[j] * A[j][i];
				}
				else if(i < A[0].size() + A.size() && i == A[0].size() + j)
				{
					value += pi[j];
				}
			}
			else if(j < A.size() + A[0].size())
			{
				if (i < A[0].size() && j == A.size() + i)
				{
					value += pi[j];
				}
				else if(i >=A[0].size() && i == A[0].size() + j)
				{
					value += pi[j];
				}
			}
			else if(j < A.size() + 2 * A[0].size())
			{
				if (i < A[0].size() && j == A.size() + A[0].size() + i)
				{
					value -= pi[j];
				}
				else if(i >= A[0].size() && i == A[0].size() + j)
				{
					value += pi[j];
				}
			}
			else
			{
				if (i < A[0].size())
				{
					value -= pi[j] * Hl[j - b.size() - 2 * A[0].size()][i];
				}
				else if (i >= A[0].size() && i == A[0].size() + j)
				{
					value += pi[j];
				}
			}
			if (i < obj.size())
			{
				S[size_S_row - CM.size() - C.size() - 1][i] = obj[i] - value;
			}
			else
			{
				S[size_S_row - CM.size() - C.size() - 1][i] = - value;
			}
			S[size_S_row - CM.size() - C.size() - 1][i] = (abs(S[size_S_row - CM.size() - C.size() - 1][i]) < 0.0000001) ? 0 : S[size_S_row - CM.size() - C.size() - 1][i];
		}
	}
	for (int k = 0; k < C.size(); k++)
	{
		vector<double> pi_(inverseMatrix.size(), 0.0);
		for (int i = 0; i < inverseMatrix.size(); i++)
		{
			for (int j = 0; j < inverseMatrix.size(); j++)
			{
				if (base[j] - 1 < obj.size())
				{
					pi_[i] += C[k][base[j] - 1] * inverseMatrix[j][i];
				}
			}
		}
		for (int i = 0; i < size_S_col - 1; i++)
		{
			double value = 0.0;
			for (int j = 0; j < inverseMatrix.size(); j++)
			{
				if (j < A.size())
				{
					if (i < A[0].size())
					{
						value += pi_[j] * A[j][i];
					}
					else if (i < A[0].size() + A.size() && i == A[0].size() + j)
					{
						value += pi_[j];
					}
				}
				else if (j < A.size() + A[0].size())
				{
					if (i < A[0].size() && j == A.size() + i)
					{
						value += pi_[j];
					}
					else if (i >= A[0].size() && i == A[0].size() + j)
					{
						value += pi_[j];
					}
				}
				else if (j < A.size() + 2 * A[0].size())
				{
					if (i < A[0].size() && j == A.size() + A[0].size() + i)
					{
						value -= pi_[j];
					}
					else if (i >= A[0].size() && i == A[0].size() + j)
					{
						value += pi_[j];
					}
				}
				else
				{
					if (i < A[0].size())
					{
						value -= pi_[j] * Hl[j - b.size() - 2 * A[0].size()][i];
					}
					else if (i >= A[0].size() && i == A[0].size() + j)
					{
						value += pi_[j];
					}
				}
				if (i < obj.size())
				{
					S[size_S_row - CM.size() - C.size() + k][i] = C[k][i] - value;
				}
				else
				{
					S[size_S_row - CM.size() - C.size() + k][i] = -value;
				}
				S[size_S_row - CM.size() - C.size() + k][i] = (abs(S[size_S_row - CM.size() - C.size() + k][i]) < 0.0000001) ? 0 : S[size_S_row - CM.size() - C.size() + k][i];
			}
		}
		for (int i = 0; i < pi_.size(); i++)
		{
			S[size_S_row - CM.size() - C.size() + k][size_S_col - 1] -= pi_[i] * b[i];
		}
		S[size_S_row - CM.size() - C.size() - 1][size_S_col + k] = (abs(S[size_S_row - CM.size() - C.size() + k][size_S_col - 1]) < 0.0000001) ? 0 : S[size_S_row - CM.size() - C.size() + k][size_S_col - 1];
	}
}

void detectHl(double** S, int const& S_row, int const& S_col, vector<int>& outBases, vector<double> uppers, vector<double> lowers, vector<vector<double> > exist_Hl, vector<int>& Hl, vector<double>& Hl_const, vector<double>& Hl_origine, int nbCon)
{
	// Compute Hl_const and Hl_origine
	for (int i = 0; i < S_col - 1; i++)
	{
		Hl_const.push_back(0.0);
	}
	for (int i = 0; i < A[0].size(); i++)
	{
		Hl_origine.push_back(0.0);
	}
	Hl_origine.push_back(1.0);
	for (int j = 0; j < outBases.size(); j++)
	{
		//int nbZero = 0;
		for (int i = nbCon + 1; i < S_row; i++)
		{
			if (S[i][outBases[j] - 1] < 0)
			{
				Hl.push_back(outBases[j] - 1);
				Hl_const[outBases[j] - 1] = 1.0;
				if (outBases[j] > A[0].size())
				{
					if (outBases[j] <= A[0].size() + A.size())
					{
						for (int k = 0; k < A[0].size(); k++)
						{
							Hl_origine[k] -= A[outBases[j] - A[0].size() - 1][k];
						}
						Hl_origine[A[0].size()] -= b[outBases[j] - A[0].size() - 1];
					}
					else if(outBases[j] <= 2 * A[0].size() + A.size())
					{
						Hl_origine[outBases[j] - A[0].size() - A.size() - 1]--;
						Hl_origine[A[0].size()] -= uppers[outBases[j] - A[0].size() - A.size() - 1];
					}
					else if (outBases[j] <= 3 * A[0].size() + A.size())
					{
						Hl_origine[outBases[j] - 2 * A[0].size() - A.size() - 1]++;
						Hl_origine[A[0].size()] += lowers[outBases[j] - 2 * A[0].size() - A.size() - 1];
					}
					else
					{
						for (int h = 0; h < exist_Hl.size(); h++)
						{
							for (int k = 0; k <= A[0].size(); k++)
							{
								Hl_origine[k] += exist_Hl[h][k];
							}
						}
					}
				}
				else
				{
					Hl_origine[outBases[j] - 1]++;
				}
				break;
			}
			/*else if (S[i][outBases[j] - 1] == 0)
			{
			nbZero++;
			}*/
		}
		/*if (nbZero == (S.size() - nbCon - 1))
		{
		Hl.push_back(outBases[j] - 1);
		}*/
	}
	//cout << "Hl :" << endl;
	//afficher(Hl);
}

bool getEpsilon(GAMSWorkspace& eps, GAMSJob& t, vector<vector<double> > Hls, vector<double>& Hl_origine, vector<double> up, vector<double> lo, double& epsilon, vector<double>& x)
{
	//cout << "TestEfficency" << endl;
	clock_t db = clock();
	string str = epsilon_model + "\n";
	if (!Hls.empty())
	{
		str.append("Set h 'old Hls' /1*" + to_string(Hls.size()) + "/;\nEquation\n\tec(h) 'efficient cuts';\nParameter\n\tHl(h,j) 'old Hls'\n\t");
		str.append("Hl_SM(h) 'old SM of Hl';\nec(h)..    sum(j, Hl(h,j)*x(j)) =g= Hl_SM(h);\n");
		for (int i = 0; i < Hls.size(); i++)
		{
			for (int j = 1; j < Hl_origine.size(); j++)
			{
				str.append("Hl('" + to_string(i + 1) + "','" + to_string(j) + "') = " + to_string(Hls[i][j - 1]) + ";\n");
			}
		}
		for (int i = 0; i < Hls.size(); i++)
		{
			str.append("Hl_SM('" + to_string(i + 1) + "') = " + to_string(Hls[i][Hl_origine.size() - 1]) + ";\n");
		}
	}
	for (int i = 1; i < Hl_origine.size(); i++)
	{
		str.append("nHl('" + to_string(i) + "') = " + to_string(Hl_origine[i - 1]) + ";\n");
	}
	str.append("nHl_SM = " + to_string(Hl_origine[Hl_origine.size() - 1]) + ";\n");
		
	for (int i = 0; i < up.size(); i++)
	{
		str.append("uppers('" + to_string(i + 1) + "') = " + to_string(up[i]) + ";\n");
	}
	for (int i = 0; i < lo.size(); i++)
	{
		str.append("lowers('" + to_string(i + 1) + "') = " + to_string(lo[i]) + ";\n");
	}
	str.append("Scalar ms 'model status', ss 'solve status';\nmodel principle /all/;\nsolve principle using mip min obj; ms = principle.modelstat; ss = principle.solvestat;");
	//cout << "str epsilon: " << endl << str << endl;
	t = eps.addJobFromString(str);
	t.run();
	if (t.outDB().getParameter("ms").findRecord().value() == 1 || t.outDB().getParameter("ms").findRecord().value() == 8)
	{
		epsilon = t.outDB().getVariable("obj").firstRecord().level();
		for (GAMSVariableRecord rec : t.outDB().getVariable("x"))
			x[stoi(rec.key(0)) - 1] = rec.level();
		if (epsilon == 0.0)
		{
			int_epsilon_null++;
		}
		else
		{
			int_epsilon_positive++;
		}
		epsilonTime = epsilonTime + (clock() - db);
		return true;
	}
	else
	{
		epsilonTime = epsilonTime + (clock() - db);
		int_epsilon_abs++;
		return false;
	}
	return false;
}

void computeInverseInfo(GAMSWorkspace& ws, GAMSJob& tInverse, GAMSJob& t, vector<double>& x,  vector<int>& base, vector<int>& outBase, vector<vector<double> > Hls_origine, vector<double> up, vector<double> lo, vector<int>& Hl, vector<double>& Hl_const, vector<double>& Hl_origine)
{
	detectBases(t, base, outBase, Hls_origine.empty());
	vector<vector<double> > matrixBase;
	int rank = A.size() + 2 * C[0].size() + Hls_origine.size();
	extractMatrixBase(base, matrixBase, rank, Hls_origine);
	//cout << "Matrix base : " << endl;
	//afficher(matrixBase);
	vector<vector<double> > inverseMatrixBase(matrixBase.size(), vector<double>(matrixBase.size(), 0.0));
	string str3 = "Set j 'Variables' /1*" + to_string(rank) + "/;\nSet i 'Constraints' /1*" + to_string(rank) + "/;\n" + inverse_beg;
	for (int i = 0; i < rank; i++)
	{
		for (int j = 0; j < rank; j++)
		{
			str3.append("a('" + to_string(i + 1) + "','" + to_string(j + 1) + "') = " + to_string(matrixBase[i][j]) + ";\n");
		}
	}
	str3.append(inverse_end);
	tInverse = ws.addJobFromString(str3);
	tInverse.run();
	int i = 0, j = 0;
	for (GAMSParameterRecord rec : tInverse.outDB().getParameter("b"))
	{
		inverseMatrixBase[stoi(rec.key(0)) - 1][stoi(rec.key(1)) - 1] = rec.value();
	}
	//cout << "Inverse Matrix : " << endl;
	//afficher(inverseMatrixBase);
	//vector<vector<double> > S;
	double** S;
	int const size_S_row = inverseMatrixBase.size() + CM.size() + C.size() + 1;
	S = new double* [size_S_row];
	int const size_S_col = A[0].size() + inverseMatrixBase.size() + 1;
	for (int i = 0; i < size_S_row; i++)
	{
		S[i] = new double[size_S_col];
	}
	computeS(S, size_S_row, size_S_col, inverseMatrixBase, Hls_origine, up, lo, base);
	S[size_S_row - CM.size() - C.size() - 1][size_S_col - 1] = -ZLinear(obj, x);
	for (int i = 0; i < C.size(); i++)
	{
		S[size_S_row - CM.size() - C.size() + i][size_S_col - 1] = -ZLinear(C[i], x);
	}
	//Compute quadratic terms
	int ind(0);
	for (int i = size_S_row - CM.size(); i < size_S_row; i++)
	{
		for (int j = 0; j < size_S_col - 1; j++)
		{
			//cout << " j = " << j << endl;
			if (find(base.begin(), base.end(), j + 1) != base.end())
			{
				S[i][j] = 0.0;
			}
			else if (j < CM[0].size())
			{
				S[i][j] = deriverQuadratic(M[ind], CM[ind], x, j) - functionBase(S, M[ind], CM[ind], x, j, base);
			}
			else
			{
				S[i][j] = -functionBase(S, M[ind], CM[ind], x, j, base);
			}
			S[i][j] = (abs(S[i][j]) < 0.0000001) ? 0 : S[i][j];
		}
		S[i][size_S_col - 1] = -ZQuad(M[ind], CM[ind], x);
		S[i][size_S_col - 1] = (abs(S[i][size_S_col - 1]) < 0.0000001) ? 0 : S[i][size_S_col - 1];
		ind++;
	}
	detectHl(S, size_S_row, size_S_col, outBase, up, lo, Hls_origine, Hl, Hl_const, Hl_origine, size_S_row - CM.size() - C.size() - 1);
	delete[] S;
}

bool initialPhase(vector<double>& x, double& z)
{
	cout << "initial phase start" << endl;
	GAMSWorkspaceInfo wsi(GAMSEnum::ShowLog);
	GAMSWorkspace ws(wsi);
	GAMSWorkspaceInfo wsiInverse(GAMSEnum::ShowLog);
	GAMSWorkspace wsInverse(wsiInverse);
	GAMSJob tInverse;
	string str = model_opt + "\nScalar ms 'model status', ss 'solve status';\n";
	for (unsigned int i = 1; i <= CM[0].size(); i++)
	{
		str.append("uppers('" + to_string(i) + "') = 1000;\n");
		str.append("lowers('" + to_string(i) + "') = 0;\n");
	}
	str.append("Model principle /all/;\nsolve principle using lp min obj; ms = principle.modelstat; ss = principle.solvestat;");
	//cout << "str = " << str << endl;

	GAMSJob t = ws.addJobFromString(str);
	t.run();
	if (t.outDB().getParameter("ms").findRecord().value() == 1)
	{
		cout << "3" << endl;
		vector<double> xx(CM[0].size(), 0.0);
		int indNotInt;
		for (GAMSVariableRecord rec : t.outDB().getVariable("x"))
			xx[stoi(rec.key(0)) - 1] = rec.level();
		cout << "xx : ";
		afficher(xx);
		if (entiere(xx, indNotInt))
		{
			cout << "4" << endl;
			string str2 = efficiency + "\nScalar ms 'model status', ss 'solve status'; \n";
			for (unsigned int i = 1; i <= CM.size(); i++)
			{
				str2.append("y('" + to_string(i) + "') = " + to_string(xx[i - 1]) + "; \n");
			}
			str2.append("option miqcp = cplex;\nsolve principle using miqcp max obj; ms = principle.modelstat; ss = principle.solvestat;");
			GAMSWorkspaceInfo wsInfo(GAMSEnum::ShowLog);
			GAMSWorkspace efficient(wsInfo);
			GAMSJob t2 = efficient.addJobFromString(str2);
			t2.run();
			if (t2.outDB().getParameter("ms").findRecord().value() == 1 || t2.outDB().getParameter("ms").findRecord().value() == 8)
			{
				cout << "5" << endl;
				double zz = t2.outDB().getVariable("obj").firstRecord().level();
				if (zz == 0.0) // efficient solution
				{
					cout << "6" << endl;
					x = xx;
					z = t.outDB().getVariable("obj").firstRecord().level();
					SND.push_back(Z(M, CM, C, x));
					return false;
				}
				else // Not effcient
				{
					cout << "7" << endl;
					for (GAMSVariableRecord rec : t.outDB().getVariable("x"))
						x[stoi(rec.key(0)) - 1] = rec.level();
					z = ZLinear(obj, x);
					SND.push_back(Z(M, CM, C, x));
					vector<int> base, outBase;
					vector<double> up(A[0].size(), 1000), lo(A[0].size(), 0.0);
					vector<vector<double> > empty;
					vector<int> Hl;
					vector<double> Hl_origine, Hl_const;
					computeInverseInfo(wsInverse, tInverse, t, xx, base, outBase, empty, up, lo, Hl, Hl_const, Hl_origine); // empty just for the execution
					if (Hl.empty())
					{
						cout << "8" << endl;
						hl_delete++;
					}
					else
					{
						cout << "9" << endl;
						double epsilon(0);
						vector<double> xEpsilon(xx.size(), 0.0);
						vector<vector<double> > Hls_origine;
						GAMSJob t3;
						if (getEpsilon(efficient, t3, Hls_origine, Hl_origine, up, lo, epsilon, xEpsilon)) // Hls is just for the execution of the method
						{
							cout << "10" << endl;
							vector<double> xCopy(xEpsilon);
							string str3 = efficiency + "\nScalar ms 'model status', ss 'solve status'; \n";
							for (unsigned int i = 1; i <= CM[0].size(); i++)
							{
								str3.append("y('" + to_string(i) + "') = " + to_string(xEpsilon[i - 1]) + "; \n");
							}
							str3.append("option miqcp = cplex;\nsolve principle using miqcp max obj; ms = principle.modelstat; ss = principle.solvestat;");
							GAMSWorkspaceInfo wsInfo(GAMSEnum::ShowLog);
							GAMSWorkspace efficient(wsInfo);
							GAMSJob t3 = efficient.addJobFromString(str3);
							t3.run();
							if (t3.outDB().getParameter("ms").findRecord().value() == 1 || t2.outDB().getParameter("ms").findRecord().value() == 8)
							{
								cout << "11" << endl;
								double zz = t3.outDB().getVariable("obj").firstRecord().level();
								if (zz == 0.0) // efficient solution
								{			
									cout << "12" << endl;
									x = xEpsilon;
									z = ZLinear(obj, xEpsilon);
									epsilon_efficace_delete++;
									SND.push_back(Z(M, CM, C, xEpsilon));
									int_sol++;
									int_eff_sol++;
								}
								else
								{
									cout << "13" << endl;
									for (GAMSVariableRecord rec : t3.outDB().getVariable("x"))
										xCopy[stoi(rec.key(0)) - 1] = rec.level();
									x = xCopy;
									z = ZLinear(obj, xCopy);
									SND.push_back(Z(M, CM, C, xCopy));
								}
								Hls_origine.push_back(Hl_origine);
								vector<vector<double> > Hls_const;
								Hls_const.push_back(vector<double>(Hl_const));
								vector<vector<int> > Hls;
								Hls.push_back(vector<int>(Hl));
								Noeud noeud(num_noeud, Hls, Hls_origine, Hls_const, up, lo);
								tree.push_back(noeud);
								num_noeud++;
							}
						}
						else // No epsilon
						{
							cout << "14" << endl;
							return false;
						}
					}
				}
			}
		}
		else // fractional solution
		{
			cout << "23" << endl;
			vector<double> up(A[0].size(), 1000), lo(A[0].size(), 0.0);
			vector<vector<double> > Hls_origine;
			vector<vector<int> > Hls;
			lo[indNotInt] = ceil(xx[indNotInt]);
			Noeud noeud(num_noeud, Hls, Hls_origine, Hls_origine, up, lo);
			tree.push_back(noeud);
			num_noeud++;
			lo[indNotInt] = 0;
			up[indNotInt] = floor(xx[indNotInt]);
			Noeud noeud2(num_noeud, Hls, Hls_origine, Hls_origine, up, lo);
			tree.push_back(noeud2);
			num_noeud++;
		}
	}
	else
	{
		cout << "24" << endl;
		return false;
	}
}

bool domine(vector<double> v1, vector<double> v2)
{
	for (unsigned int i = 0; i < v1.size(); i++)
	{
		if (v1[i] > v2[i])
		{
			return false;
		}
	}
	return true;
}

vector<double> approximationIP(GAMSWorkspace& ws, GAMSJob& t, string& str)
{
	vector<double> idealPoint;
	for (int i = 0; i < M.size(); i++)
	{
		string model(str);
		model.append("Scalar ms 'model status', ss 'solve status';\n");
		model.append("Equation\n\tnewObj 'New objective';\nVariable\n\tnobj 'new objective variable';\n");
		model.append("newObj..\tnobj =e= sum((j,k), QQ('" + to_string(i + 1) + "',j, k)* x(j)* x(k)) + sum(j, LQ('" + to_string(i + 1) + "',j)* x(j)); \n");
		model.append("Model ip /all/;\noption miqcp = cplex;\nsolve ip using miqcp min nobj; ms = ip.modelstat; ss = ip.solvestat;\n");
		t = ws.addJobFromString(model);
		t.run();
		if (t.outDB().getParameter("ms").findRecord().value() == 1 || t.outDB().getParameter("ms").findRecord().value() == 8)
		{
			idealPoint.push_back(t.outDB().getVariable("nobj").firstRecord().level());
		}
	}
	for (int i = 0; i < C.size(); i++)
	{
		string model(str);
		model.append("\nScalar ms 'model status', ss 'solve status';\n");
		model.append("Equation\n\tnewObj 'New objective';\nVariable\n\tnobj 'new objective variable';\n");
		model.append("newObj..\tnobj =e= sum(j, LL('" + to_string(i + 1) + "',j)* x(j)); \n");
		model.append("Model ip /all/;\noption mip = cplex;\nsolve ip using mip min nobj; ms = ip.modelstat; ss = ip.solvestat;\n");
		t = ws.addJobFromString(model);
		t.run();
		if (t.outDB().getParameter("ms").findRecord().value() == 1 || t.outDB().getParameter("ms").findRecord().value() == 8)
		{
			idealPoint.push_back(t.outDB().getVariable("nobj").firstRecord().level());
		}
	}
	return idealPoint;
}

void principalPhase(vector<double>& x, double& z)
{
	cout << "Priciple phase start" << endl;
	vector<double> Hl_origine, Hl_const, IP;
	vector<int> Hl, base, outBase;
	vector<double> xx(CM[0].size(), 0.0);
	vector<double> xEpsilon(xx.size(), 0.0);
	vector<double> y(CM[0].size(), 0.0);
	vector<double> xCopy(xx.size(), 0.0);
	GAMSWorkspaceInfo wsi(GAMSEnum::ShowLog);
	GAMSWorkspace ws(wsi);
	GAMSWorkspaceInfo wsInfo(GAMSEnum::ShowLog);
	GAMSWorkspace efficient(wsInfo);
	GAMSWorkspaceInfo wsInfoIP(GAMSEnum::ShowLog);
	GAMSWorkspace wsIP(wsInfoIP);
	GAMSWorkspaceInfo wsInfoInverse(GAMSEnum::ShowLog);
	GAMSWorkspace wsInverse(wsInfoInverse);
	GAMSJob t, t2, t3, t4, t5, t6;
	while (!tree.empty())
	{
		cout << "num_noeud : " << num_noeud << endl;
		Noeud noeud = tree[tree.size() - 1];
		tree.pop_back();
		string str = model_opt + "\nScalar ms 'model status', ss 'solve status';\n";
		for (unsigned int i = 1; i <= CM[0].size(); i++)
		{
			str.append("uppers('" + to_string(i) + "') = " + to_string(noeud.getUppers()[i - 1]) + "; \n");
			str.append("lowers('" + to_string(i) + "') = " + to_string(noeud.getLowers()[i - 1]) + ";\n");
		}
		if (!noeud.getHl().empty())
		{
			cout << "25" << endl;
			str.append("Set h 'old Hls' /1*" + to_string(noeud.getHl_origine().size()) + "/;\nEquation\n\tec(h) 'efficient cuts';\nParameter\n\tHl(h,j) 'old Hls'\n\t");
			str.append("Hl_SM(h) 'old SM of Hl';\nec(h)..    sum(j, Hl(h,j)*x(j)) =g= Hl_SM(h);\n");
			for (int i = 0; i < noeud.getHl_origine().size(); i++)
			{
				for (int j = 1; j < noeud.getHl_origine()[0].size(); j++)
				{
					str.append("Hl('" + to_string(i + 1) + "','" + to_string(j) + "') = " + to_string(noeud.getHl_origine()[i][j - 1]) + ";\n");
				}
			}
			for (int i = 0; i < noeud.getHl_origine().size(); i++)
			{
				str.append("Hl_SM('" + to_string(i + 1) + "') = " + to_string(noeud.getHl_origine()[i][noeud.getHl_origine()[0].size() - 1]) + ";\n");
			}
		}
		str.append("Model principle /all/;\noption lp = cplex;\nsolve principle using lp min obj; ms = principle.modelstat; ss = principle.solvestat;");
		//cout << "str = " << str << endl;
		t = ws.addJobFromString(str);
		t.run();
		if (t.outDB().getParameter("ms").findRecord().value() == 1)
		{
			cout << "26" << endl;
			int indNotInt;
			for (GAMSVariableRecord rec : t.outDB().getVariable("x"))
				xx[stoi(rec.key(0)) - 1] = rec.level();
			double z2 = t.outDB().getVariable("obj").firstRecord().level();
			//cout << "xx : ";
			//afficher(xx);
			cout << "z = " << z2 << endl;
			if (z == numeric_limits<double>::infinity())
			{
				cout << "27" << endl;
				if (entiere(xx, indNotInt))
				{
					cout << "28" << endl;
					string str2 = efficiency + "\nScalar ms 'model status', ss 'solve status'; \n";
					for (unsigned int i = 1; i <= CM[0].size(); i++)
					{
						str2.append("y('" + to_string(i) + "') = " + to_string(xx[i - 1]) + "; \n");
					}
					str2.append("option miqcp = cplex;\nsolve principle using miqcp max obj; ms = principle.modelstat; ss = principle.solvestat;");
					//cout << "str2 : " << endl << str2 << endl;
					t2 = efficient.addJobFromString(str2);
					t2.run();
					if (t2.outDB().getParameter("ms").findRecord().value() == 1 || t2.outDB().getParameter("ms").findRecord().value() == 8)
					{
						cout << "29" << endl;
						double zz = t2.outDB().getVariable("obj").firstRecord().level();
						if (zz == 0.0) // efficient solution
						{
							cout << "30" << endl;
							if (z > z2)
							{
								cout << "31" << endl;
								x = xx;
								z = z2;
							}
						}
						else // Not efficient
						{
							cout << "32" << endl;
							for (GAMSVariableRecord rec : t2.outDB().getVariable("x"))
								y[stoi(rec.key(0)) - 1] = rec.level();
							double z_ = ZLinear(obj, y);
							SND.push_back(Z(M, CM, C, y));
							//cout << "y = ";
							//afficher(y);
							cout << "z = " << z_ << endl;
							if (z > z_)
							{
								cout << "43" << endl;
								x = y;
								z = z_;
							}
							base = vector<int>();
							outBase = vector<int>();
							Hl = vector<int>();
							Hl_const = vector<double>();
							Hl_origine = vector<double>();
							computeInverseInfo(wsInverse, t6, t, xx, base, outBase, noeud.getHl_origine(), noeud.getUppers(), noeud.getLowers(), Hl, Hl_const, Hl_origine);
							//cout << "S : " << endl;
							//afficher(S);
							//cout << "Hl:" << endl;
							//afficher(Hl);
							//cout << "Hl_const:" << endl;
							//afficher(Hl_const);
							//cout << "Hl_origine:" << endl;
							//afficher(Hl_origine);
							if (Hl.empty())
							{
								cout << "33" << endl;
								hl_delete++;
							}
							else
							{
								cout << "34" << endl;
								double epsilon(0);
								xEpsilon = vector<double>(xEpsilon.size(), 0.0);
								if (getEpsilon(efficient, t4, noeud.getHl_origine(), Hl_origine, noeud.getUppers(), noeud.getLowers(), epsilon, xEpsilon))
								{
									cout << "35" << endl;
									xCopy = vector<double>(xEpsilon);
									cout << "epsilon = " << epsilon << endl;
									//cout << "xEpsilon:" << endl;
									//afficher(xEpsilon);
									string str3 = efficiency + "\nScalar ms 'model status', ss 'solve status'; \n";
									for (unsigned int i = 1; i <= CM.size(); i++)
									{
										str3.append("y('" + to_string(i) + "') = " + to_string(xEpsilon[i - 1]) + "; \n");
									}
									str3.append("option miqcp = cplex;\nsolve principle using miqcp max obj; ms = principle.modelstat; ss = principle.solvestat;");
									t3 = efficient.addJobFromString(str3);
									t3.run();
									if (t3.outDB().getParameter("ms").findRecord().value() == 1 || t3.outDB().getParameter("ms").findRecord().value() == 8)
									{
										cout << "36" << endl;
										double zz = t3.outDB().getVariable("obj").firstRecord().level();
										if (zz == 0.0) // efficient solution
										{
											cout << "37" << endl;
											double z_ = ZLinear(obj, xEpsilon);
											//cout << "z_eps = " << z_ << endl;
											if (z > z_)
											{
												cout << "38" << endl;
												x = xEpsilon;
												z = z_;
											}
											epsilon_efficace_delete++;
											SND.push_back(Z(M, CM, C, xEpsilon));
											int_sol++;
											int_eff_sol++;
										}
										else
										{
											cout << "39" << endl;
											for (GAMSVariableRecord rec : t3.outDB().getVariable("x"))
												xCopy[stoi(rec.key(0)) - 1] = rec.level();
											double z_ = ZLinear(obj, xCopy);
											//cout << "xCopy:" << endl;
											//afficher(xCopy);
											//cout << "z_eps_copy = " << z_ << endl;
											if (z > z_)
											{
												cout << "40" << endl;
												x = xCopy;
												z = z_;
											}
											SND.push_back(Z(M, CM, C, xCopy));
											int_sol++;
											int_eff_sol++;
										}
										noeud.addElement(noeud.HL_ORIGINE, Hl_origine);
										noeud.addElement(noeud.HL_CONSTRAINT, Hl_const);
										noeud.addElement(noeud.HL, Hl_origine, Hl);
										tree.push_back(noeud);
										num_noeud++;
									}
								}
								else // No epsilon
								{
									cout << "41" << endl;
									int_epsilon_abs++;
								}
							}
						}
					}
				}
				else // fractional solution
				{
					cout << "53" << endl;
					vector<double> up(noeud.getUppers()), lo(noeud.getLowers());
					lo[indNotInt] = ceil(xx[indNotInt]);
					noeud.setLowers(lo);
					noeud.setNum(num_noeud);
					tree.push_back(noeud);
					num_noeud++;
					lo[indNotInt] = 0;
					up[indNotInt] = floor(xx[indNotInt]);
					noeud.setLowers(lo);
					noeud.setUppers(up);
					noeud.setNum(num_noeud);
					tree.push_back(noeud);
					num_noeud++;
				}
			}
			else if(z2 >= z)
			{
				cout << "54" << endl;
				cost_delete++;
			}
			else
			{
				cout << "55" << endl;
				string str4 = ip_model;
				for (unsigned int i = 1; i <= CM[0].size(); i++)
				{
					str4.append("uppers('" + to_string(i) + "') = " + to_string(noeud.getUppers()[i - 1]) + "; \n");
					str4.append("lowers('" + to_string(i) + "') = " + to_string(noeud.getLowers()[i - 1]) + ";\n");
				}
				if (!noeud.getHl().empty())
				{
					cout << "55_1" << endl;
					str4.append("Set h 'old Hls' /1*" + to_string(noeud.getHl_origine().size()) + "/;\nEquation\n\tec(h) 'efficient cuts';\nParameter\n\tHl(h,j) 'old Hls'\n\t");
					str4.append("Hl_SM(h) 'old SM of Hl';\nec(h)..    sum(j, Hl(h,j)*x(j)) =g= Hl_SM(h);\n");
					for (int i = 0; i < noeud.getHl_origine().size(); i++)
					{
						for (int j = 1; j < noeud.getHl_origine()[0].size(); j++)
						{
							str4.append("Hl('" + to_string(i + 1) + "','" + to_string(j) + "') = " + to_string(noeud.getHl_origine()[i][j - 1]) + ";\n");
						}
					}
					for (int i = 0; i < noeud.getHl_origine().size(); i++)
					{
						str4.append("Hl_SM('" + to_string(i + 1) + "') = " + to_string(noeud.getHl_origine()[i][noeud.getHl_origine()[0].size() - 1]) + ";\n");
					}
				}
				IP = approximationIP(wsIP, t5, str4);
				//cout << "IP:";
				//afficher(IP);
				bool domination(false);
				if (!IP.empty())
				{
					cout << "56" << endl;
					for (unsigned int i = 0; i < SND.size(); i++)
					{
						if (domine(SND[i], IP))
						{
							cout << "57" << endl;
							domination = true;
							break;
						}
					}
				}
				else
				{
					cout << "58" << endl;
					empty_delete++;
					continue;
				}
				if (!domination)
				{
					cout << "59" << endl;
					if (entiere(xx, indNotInt))
					{
						cout << "60" << endl;
						string str5 = efficiency + "\nScalar ms 'model status', ss 'solve status'; \n";
						for (unsigned int i = 1; i <= CM[0].size(); i++)
						{
							str5.append("y('" + to_string(i) + "') = " + to_string(xx[i - 1]) + "; \n");
						}
						str5.append("option miqcp = cplex;\nsolve principle using miqcp max obj; ms = principle.modelstat; ss = principle.solvestat;");
						t2 = efficient.addJobFromString(str5);
						t2.run();
						if (t2.outDB().getParameter("ms").findRecord().value() == 1 || t2.outDB().getParameter("ms").findRecord().value() == 8)
						{
							cout << "61" << endl;
							double zz = t2.outDB().getVariable("obj").firstRecord().level();
							if (zz == 0.0) // efficient solution
							{
								cout << "62" << endl;
								if (z > t.outDB().getVariable("obj").firstRecord().level())
								{
									cout << "63" << endl;
									x = xx;
									z = t.outDB().getVariable("obj").firstRecord().level();
								}
							}
							else
							{
								cout << "64" << endl;
								for (GAMSVariableRecord rec : t2.outDB().getVariable("x"))
									y[stoi(rec.key(0)) - 1] = rec.level();
								double z_ = ZLinear(obj, y);
								SND.push_back(Z(M, CM, C, y));
								//cout << "y:";
								//afficher(y);
								//cout << "z_y = " << z_ << endl;
								if (z > z_)
								{
									cout << "64_1" << endl;
									x = y;
									z = z_;
								}
								base = vector<int>();
								outBase = vector<int>();
								Hl = vector<int>();
								Hl_const = vector<double>();
								Hl_origine = vector<double>();
								computeInverseInfo(wsInverse, t6, t, xx, base, outBase, noeud.getHl_origine(), noeud.getUppers(), noeud.getLowers(), Hl, Hl_const, Hl_origine);
								//cout << "S : " << endl;
								//afficher(S);
								//cout << "Hl:" << endl;
								//afficher(Hl);
								//cout << "Hl_const:" << endl;
								//afficher(Hl_const);
								//cout << "Hl_origine:" << endl;
								//afficher(Hl_origine);
								if (Hl.empty())
								{
									cout << "65" << endl;
									hl_delete++;
								}
								else
								{
									cout << "66" << endl;
									double epsilon(0);
									xEpsilon = vector<double>(xEpsilon.size(), 0.0);
									if (getEpsilon(efficient, t4, noeud.getHl_origine(), Hl_origine, noeud.getUppers(), noeud.getLowers(), epsilon, xEpsilon))
									{
										cout << "67" << endl;
										cout << "epsilon = " << epsilon << endl;
										//cout << "xEpsilon:";
										//afficher(xEpsilon);
										xCopy = vector<double>(xEpsilon);
										string str6 = efficiency + "\nScalar ms 'model status', ss 'solve status'; \n";
										for (unsigned int i = 1; i <= CM[0].size(); i++)
										{
											str6.append("y('" + to_string(i) + "') = " + to_string(xEpsilon[i - 1]) + "; \n");
										}
										str6.append("option miqcp = cplex;\nsolve principle using miqcp max obj; ms = principle.modelstat; ss = principle.solvestat;");
										t3 = efficient.addJobFromString(str6);
										t3.run();
										if (t3.outDB().getParameter("ms").findRecord().value() == 1 || t3.outDB().getParameter("ms").findRecord().value() == 8)
										{
											cout << "68" << endl;
											double zz = t3.outDB().getVariable("obj").firstRecord().level();
											if (zz == 0.0) // efficient solution
											{
												cout << "69" << endl;
												double z_ = ZLinear(obj, xEpsilon);
												if (z > z_)
												{
													cout << "70" << endl;
													x = xEpsilon;
													z = z_;
												}
												epsilon_efficace_delete++;
												SND.push_back(Z(M, CM, C, xEpsilon));
												int_sol++;
												int_eff_sol++;
												eff_delete++;
											}
											else
											{
												cout << "71" << endl;
												for (GAMSVariableRecord rec : t3.outDB().getVariable("x"))
													xCopy[stoi(rec.key(0)) - 1] = rec.level();
												double z_ = ZLinear(obj, xCopy);
												//cout << "xCopy:";
												//afficher(xCopy);
												//cout << "z_xCopy = " << z_ << endl;
												if (z > z_)
												{
													cout << "72" << endl;
													x = xCopy;
													z = z_;
												}
												SND.push_back(Z(M, CM, C, xCopy));
											}
											noeud.addElement(noeud.HL_ORIGINE, Hl_origine);
											noeud.addElement(noeud.HL_CONSTRAINT, Hl_const);
											noeud.addElement(noeud.HL, Hl_origine, Hl);
											tree.push_back(noeud);
											num_noeud++;
										}
									}
									else // No epsilon
									{
										cout << "73" << endl;
										int_epsilon_abs++;
									}
								}
							}
						}
					}
					else // fractional solution
					{
						cout << "85" << endl;
						vector<double> up(noeud.getUppers()), lo(noeud.getLowers());
						lo[indNotInt] = ceil(xx[indNotInt]);
						noeud.setLowers(lo);
						noeud.setNum(num_noeud);
						tree.push_back(noeud);
						num_noeud++;
						lo[indNotInt] = 0;
						up[indNotInt] = floor(xx[indNotInt]);
						noeud.setLowers(lo);
						noeud.setUppers(up);
						noeud.setNum(num_noeud);
						tree.push_back(noeud);
						num_noeud++;
					}
				}
				else
				{
					cout << "86" << endl;
					dom_delete++;
				}
			}
		}
		else
		{
			cout << "87" << endl;
			empty_delete++;
		}
	}
}

int main(int argc, char* argv[])
{
	
	vector<double> x;
	clock_t beg, ed, tm;
	double z = numeric_limits<double>::infinity();
	beg = clock();
	readData();
	try {
		
		cout << "1" << endl;
		if (initialPhase(x, z))
		{
			cout << "2" << endl;
			principalPhase(x, z);
		}
		
		cout << "9" << endl;

	}
	catch (GAMSException& ex) {
		cout << "GAMSException occured: " << ex.what() << endl;
	}
	catch (exception& ex) {
		cout << ex.what() << endl;
	}
	
	ed = clock();
	tm = ed - beg;

	cout << "Solution Optimale :";
	for (unsigned int i = 0; i < x.size(); i++)
	{
		cout << x[i] << " ";
	}
	vector<double> zz = Z(M, CM, C, x);
	cout << "Z opt : ";
	afficher(zz);
	cout << "\nf = " << z << endl;
	cout << "Execution time : " << tm / 1000.0 << " sec" << endl;
	cout << "Quadratic time : " << quadTime / 1000.0 << " sec" << endl;
	cout << "File time : " << fileTime / 1000.0 << " sec" << endl;
	cout << "Ideal time : " << idealTime / 1000.0 << " sec" << endl;
	cout << "Ideal Quad time : " << idealQuadTime / 1000.0 << " sec" << endl;
	cout << "Ideal Lin time : " << idealLinTime / 1000.0 << " sec" << endl;
	cout << "Construct Ideal time : " << constructTime / 1000.0 << " sec" << endl;
	cout << "Exist time : " << existTime / 1000.0 << " sec" << endl;
	cout << "Epsilon time : " << epsilonTime / 1000.0 << " sec" << endl;
	cout << "Real time : " << (tm - constructTime) / 1000.0 << " sec" << endl;
	char h;
	cout << "Number of nodes : " << num_noeud << endl;
	cout << "Number of nodes deleted because efficiency : \t\t\t" << eff_delete << endl;
	cout << "Number of nodes deleted because Hl empty : \t\t\t" << hl_delete << endl;
	cout << "Number of nodes deleted because dominanced sub region : \t" << dom_delete << endl;
	cout << "Number of nodes deleted because cost : \t\t\t\t" << cost_delete << endl;
	cout << "Number of nodes deleted because sub region empty : \t\t" << empty_delete << endl;
	cout << "Number of nodes deleted because solution auxiliare program efficiency : " << epsilon_efficace_delete << endl;
	cout << "Number of found solutions find in exist : \t\t\t" << exist_find << endl;
	cout << "Number of found solutions not find in exist : \t\t\t" << exist_not_find << endl;
	cout << "Number of integer solutions found : \t\t\t\t" << int_sol << endl;
	cout << "Number of integer efficient solutions found : \t\t\t" << int_eff_sol << endl;
	cout << "Number of positive Epsilon : \t\t\t\t\t" << int_epsilon_positive << endl;
	cout << "Number of null Epsilon : \t\t\t\t\t" << int_epsilon_null << endl;
	cout << "Number of abs Epsilon : \t\t\t\t\t" << int_epsilon_abs << endl;
	cout << "Number of error Epsilon : \t\t\t\t\t" << int_epsilon_error << endl;
	cout << "ENTRER UN CARACTERE ET CLIQUER ENTRER POUR SORTIR ... " << endl;
	cin >> h;
	//levels.close();
	//allLevels.close();
	return 0;
}