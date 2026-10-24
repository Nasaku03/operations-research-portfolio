#include <ilopl/iloopl.h>

#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <vector>
#include <map>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <string>
#include <limits>
#include <queue>
#include <utility>
#include <set>

#include "NoeudLemke.h"
#include "NoeudQuadratic.h"
#include "CMatrix.h"

using namespace std;

class mycomparison
{
	bool reverse;
public:
	mycomparison(const bool& revparam = false)
	{
		reverse = revparam;
	}
	bool operator() (const NoeudLemke& lhs, const NoeudLemke& rhs) const
	{
		if (reverse) return (lhs.getNum() >= rhs.getNum());
		else return (lhs.getNum()<rhs.getNum());
	}
};

class mycomparison2
{
	bool reverse;
public:
	mycomparison2(const bool& revparam = false)
	{
		reverse = revparam;
	}
	bool operator() (const NoeudQuadratic& lhs, const NoeudQuadratic& rhs) const
	{
		if (reverse) return (lhs.getNum() >= rhs.getNum());
		else return (lhs.getNum()<rhs.getNum());
	}
};

ofstream output, output2;
//ofstream levels, allLevels;
unsigned int num_noeud = 0, dom_delete = 0, hl_delete = 0, cost_delete = 0, eff_delete = 0, empty_delete = 0, int_sol = 0, int_eff_sol = 0;
unsigned int int_epsilon_positive = 0, int_epsilon_null = 0, int_epsilon_abs = 0, int_epsilon_error = 0;

//priority_queue<NoeudLemke, vector<NoeudLemke>, mycomparison> tree;
priority_queue<NoeudQuadratic, vector<NoeudQuadratic>, mycomparison2> tree;
clock_t quadTime(0), idealTime(0), idealLinTime(0), idealQuadTime(0), constructTime(0), alphaTime(0), fileTime(0);
clock_t epsilonTime(0);
vector<int> epsilonsPositive;

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

void afficher(set<vector<double> > mat)
{
	cout << '[' << endl;
	for (set<vector<double> >::iterator i = mat.begin(); i != mat.end(); i++)
	{
		cout << '[';
		for (unsigned int j = 0; j < (*i).size(); j++)
		{
			cout << (*i)[j] << ' ';
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

void afficher(vector<int> vec)
{
	cout << '[' << endl;
	for (unsigned int j = 0; j < vec.size(); j++)
	{
		cout << vec[j] << ' ';
	}
	cout << ']' << endl;
}

void readData(vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<double> >& C, vector<vector<vector<double> > >& M, vector<vector<double> >& CM)
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
		vector<double> inter;
		output << "n = " << n << endl;
		output << "m = " << m << endl;
		output << "p = " << p << endl;
		output << "q = " << q << endl;
		//cout << n << endl;
		//cout << m << endl;
		//cout << p << endl;
		//cout << q << endl;
		output << "Ax ~ b = [" << endl;
		for (unsigned int i = 0; i<m; i++)
		{
			output << "[";
			for (unsigned int j = 0; j<n; j++)
			{
				fichier >> mot;
				inter.push_back(mot);
				output << mot << " ";
			}
			A.push_back(inter);
			inter.clear();
			fichier >> c;
			fichier >> mot;
			output << c << " " << mot << "]" << endl;
			sign.push_back((c == '<' ? "<=" : (c == '>' ? ">=" : "==")));
			b.push_back(mot);
		}
		output << "]" << endl;
		output << "M = [" << endl;
		for (int h = 0; h < q; h++)
		{
			output << "[";
			vector<vector<double> > MM;
			for (unsigned int j = 0; j < n; j++)
			{
				output << "[";
				for (unsigned int jj = 0; jj < n; jj++)
				{
					fichier >> mot;
					inter.push_back(mot);
					output << mot << " ";
				}
				MM.push_back(inter);
				inter.clear();
				output << "]" << endl;
			}
			output << "]" << endl;
			M.push_back(MM);
		}
		output << "]" << endl;
		output << "CM = [" << endl;
		for (unsigned int k = 0; k<q; k++)
		{
			output << "[";
			for (unsigned int j = 0; j<n; j++)
			{
				fichier >> mot;
				inter.push_back(mot);
				output << mot << " ";
			}
			CM.push_back(inter);
			inter.clear();
			output << "]" << endl;
		}
		output << "]" << endl;
		output << "C = [" << endl;
		for (unsigned int k = 0; k<p; k++)
		{
			output << "[";
			for (unsigned int j = 0; j<n; j++)
			{
				fichier >> mot;
				inter.push_back(mot);
				output << mot << " ";
			}
			C.push_back(inter);
			inter.clear();
			output << "]" << endl;
		}
		output << "]" << endl;
	}
	else
	{
		//cout << "We are out" << endl;
	}
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

vector<vector<double> > buildSimplexMatrix(vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<double> >& C, vector<vector<vector<double> > >& M, vector<vector<double> >& CM)
{
	// matrice construite uniquement pour les contraintes de type "<="
	vector<vector<double> > S;
	for (int i = 0; i < A.size(); i++)
	{
		S.push_back(A[i]);
	}
	for (int i = 0; i < A.size(); i++)
	{
		for (int j = 0; j < A.size(); j++)
		{
			if (i == j)
			{
				S[i].push_back(1.0);
			}
			else
			{
				S[i].push_back(0.0);
			}
		}
		S[i].push_back(b[i]);
	}
	for (int i = 0; i < C.size(); i++)
	{
		S.push_back(C[i]);
	}
	for (int i = 0; i < C.size(); i++)
	{
		for (int j = 0; j <= A.size(); j++)
		{
			S[A.size() + i].push_back(0.0);
		}
	}
	for (int i = 0; i < CM.size(); i++)
	{
		S.push_back(CM[i]);
	}
	for (int i = 0; i < CM.size(); i++)
	{
		for (int j = 0; j <= A.size(); j++)
		{
			S[A.size() + C.size() + i].push_back(0.0);
		}
	}
	return S;
}

void pivoting(vector<vector<double> >& M, int const& line, int const& column, int& nbObjQ)
{
	// Divide line pivot by pivot
	double pivot = M[line][column];
	for (int j = 0; j < M[line].size(); j++)
	{
		if (abs((M[line][j] / pivot) - round(M[line][j] / pivot)) > (numeric_limits<double>::epsilon() * 10000000))
		{
			M[line][j] /= pivot;
		}
		else
		{
			M[line][j] = round(M[line][j] / pivot);
		}
		
	}
	// calculate the rest lines
	//cout << "actual solution : " << endl;
	for (int i = 0; i < (M.size() - nbObjQ); i++)
	{
		if (i != line)
		{
			double alpha = M[i][column];
			for (int j = 0; j < M[i].size(); j++)
			{
				if (abs((M[i][j] - M[line][j] * alpha) - round(M[i][j] - M[line][j] * alpha)) >(numeric_limits<double>::epsilon() * 10000000))
				{
					M[i][j] -= M[line][j] * alpha;
				}
				else
				{
					M[i][j] = round(M[i][j] - M[line][j] * alpha);
				}
			}
		}
		//cout << M[i][M[0].size() - 1] << " ";
	}
	//cout << endl;
}

double deriverQuadratic(vector<vector<double> >& M, vector<double>& c, vector<double>& solution, int k)
{
	double der = 0.0;
	for (unsigned int i = 0; i < solution.size(); i++)
	{
		der += M[i][k] * solution[i];
	}
	return (2 * der) + c[k];
}

double functionBase(vector<vector<double> >& S, vector<vector<double> >& M, vector<double>& CM, vector<double>& x, int j, vector<int>& bases)
{
	double val(0.0);
	for (int i = 0; i < bases.size(); i++)
	{
		if (bases[i] <= x.size())
		{
			val += deriverQuadratic(M, CM, x, bases[i] - 1)*S[i][j];
		}		
	}
	return val;
}

void initialSolution(vector<double>& x, int nbVar)
{
	x.clear();
	for (int i = 0; i < nbVar; i++)
	{
		x.push_back(0.0);
	}
}

void extractSolution(vector<vector<double> >& S, vector<int>& bases, int nbCon, int nbVar, vector<double>& x)
{
	initialSolution(x, nbVar);
	for (int i = 0; i < bases.size(); i++)
	{
		if (bases[i] <= nbVar)
		{
			x[bases[i] - 1] = S[i][nbCon + nbVar];
		}
	}
}

bool simplex(vector<vector<double> >& S, int nbCon, int nbVar, int nbObjL, int nbObjQ, vector<int>& bases, vector<vector<vector<double> > >& M, vector<vector<double> >& CM, vector<double>& x)
{
	//cout << "SIMPLEXE (ONLY MINIMIZATION PROBLEMS)" << endl;
	while (true)
	{
		double minCol = numeric_limits<double>::infinity(), minRow = numeric_limits<double>::infinity(); int indCol = -1, indRow = -1;
		// Detect pivot
		// column
		for (int i = 0; i < S[0].size(); i++)
		{
			if ((i != (S[0].size() - 1)) && (S[nbCon][i] < 0) && (S[nbCon][i] < minCol))
			{
				minCol = S[nbCon][i];
				indCol = i;
			}
		}
		//cout << "indCol = " << indCol << endl;
		//cout << "Bases :";
		//afficher(bases);
		// row
		if (indCol != -1)
		{
			for (int i = 0; i < nbCon; i++)
			{
				//cout << "b/l: " << S[i][nbCon + nbVar] / S[i][indCol] << endl;
				if ((S[i][indCol] > 0) && ((S[i][nbCon + nbVar] / S[i][indCol]) < minRow))
				{
					minRow = S[i][nbCon + nbVar] / S[i][indCol];
					indRow = i;
				}
			}
			//cout << "indRow = " << indRow << endl;
			if (indRow != -1)
			{
				//cout << "Pivot = (" << indRow << ", " << indCol << ") = " << S[indRow][indCol] << endl;
				pivoting(S, indRow, indCol, nbObjQ);
				bases[indRow] = indCol + 1;
				//cout << "S after pivoting = " << endl;
				//afficher(S);
				//cout << "CLICK TO CONTINUE ... " << endl;
				char c;
				////cin >> c;
			}
			else
			{
				// Unbounded region
				return false;
			}
		}
		else
		{
			// Dual optimal, must verify primal optimal
			for (int i = 0; i < nbCon; i++)
			{
				if (S[i][nbCon + nbVar] < 0)
				{
					return false;
				}
			}
			// Get solution into x
			extractSolution(S, bases, nbCon, nbVar, x);
			//cout << "Base = ";
			//afficher(bases);
			// Compute quadratic terms
			int ind(0);
			for (int i = nbCon + nbObjL + 1; i < S.size(); i++)
			{
				for (int j = 0; j < S[0].size()-1; j++)
				{
					//cout << " j = " << j << endl;
					if (find(bases.begin(), bases.end(), j+1) != bases.end())
					{
						S[i][j] = 0.0;
					}
					else if(j < nbVar)
					{
						S[i][j] = deriverQuadratic(M[ind], CM[ind], x, j) - functionBase(S, M[ind], CM[ind], x, j, bases);
					}
					else
					{
						S[i][j] = - functionBase(S, M[ind], CM[ind], x, j, bases);
					}
				}
				S[i][S[0].size() - 1] = ZQuad(M[ind], CM[ind], x);
				ind++;
			}
			//cout << "SOLUTION = ";
			//afficher(x);
			//cout << "obj = " << S[nbCon][S[0].size() - 1] << endl;
			char c;
			//cin >> c;
			return true;
		}
	}
}

bool entiere(vector<double> solution, int& ind)
{
	for (unsigned int i = 0; i < solution.size(); i++)
	{
		////cout << "x(" << i << ")= " << solution[i] << endl;
		////cout << "floor(x)(" << i << ")= " << round(solution[i]) << endl;
		////cout << solution[i] - floor(solution[i]) << endl;
		////cout << (solution[i] - round(solution[i])) << endl;
		if ((solution[i] - floor(solution[i])) > 0)
		{
			ind = i;
			return false;
		}
	}
	return true;
}

void writeDatFileEfficience(vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<vector<double> > >& M, vector<vector<double> >& CM, vector<vector<double> >& C, vector<double>& x0)
{
	//cout << "WriteFileEfficience" << endl;
	ofstream model;
	model.open("TestEfficience.mod", ofstream::trunc);
	model << "int n = ...;\nint m = ...;\nint me = ...;\nint p = ...;\nrange vare = 1..n;\nrange con = 1..m;\nrange cone = 0..me;\nrange obj = 1..p;\nint A[con][vare] = ...;\nint b[con] = ...;\nint Ae[cone][vare] = ...;\nint be[cone] = ...;" << endl;
	for (int i = 1; i <= M.size(); i++)
	{
		model << "int Q" << i << "[vare][vare] = ...;" << endl;
	}
	for (int i = 1; i <= CM.size(); i++)
	{
		model << "int cm" << i << "[vare] = ...;" << endl;
	}
	for (int i = 1; i <= C.size(); i++)
	{
		model << "int c" << i << "[vare] = ...;" << endl;
	}
	model << "int x0[vare] = ...;" << endl;
	model << "dvar int x[vare];\ndvar float t[obj];\nmaximize\n  sum(k in obj) t[k];" << endl;
	model << "subject to {" << endl;
	model << "\tforall(i in con)\n\t  const:\n\t\tsum(j in vare) A[i][j]*x[j] <= b[i];" << endl;
	model << "\tforall(i in cone)\n\t  consteg:\n\t\tsum(j in vare) Ae[i][j]*x[j] == be[i];" << endl;
	for (int i = 1; i <= M.size(); i++)
	{
		model << "\t  objqcons" << i << ":\n\t\tt[" << i << "] + sum(j, k in vare) Q" << i << "[k][j]*x[j]*x[k] + sum(j in vare) cm" << i << "[j]*x[j] == sum(j, k in vare) Q" << i << "[k][j]*x0[j]*x0[k] + sum(j in vare) cm" << i << "[j]*x0[j];" << endl;
	}
	for (int i = 1; i <= C.size(); i++)
	{
		model << "\t  objlcons" << i << ":\n\t\tt[" << (M.size() + i) << "] + sum(j in vare) c" << i << "[j]*x[j] == sum(j in vare) c" << i << "[j]*x0[j];" << endl;
	}
	model << "\tforall(j in vare)\n\t  positive:\n\t\tx[j] >= 0;" << endl;
	model << "\tforall(j in obj)\n\t  positiveT:\n\t\tt[j] >= 0;" << endl;
	model << '}';
	model.close();
	ofstream data;
	data.open("TestEfficience.dat", ofstream::trunc);
	unsigned int m(0);
	for (unsigned int i = 0; i < sign.size(); i++)
	{
		if (sign[i] != "==")
		{
			m++;
		}
	}
	data << "n = " << A[0].size() << ";\nm = " << m << ";\nme = " << (sign.size() - m) << ";\np = " << (M.size() + C.size()) << ";\nA = [";
	for (unsigned int i = 0; i < A.size(); i++)
	{
		if (sign[i] != "==")
		{
			data << "[";
			for (unsigned int j = 0; j < A[0].size(); j++)
			{
				data << (sign[i] == "<=" ? A[i][j] : -A[i][j]) << " ";
			}
			unsigned int k(1);
			while ((i + k) < sign.size())
			{
				if (sign[i + k] != "==")
				{
					data << "],\n";
					break;
				}
				k++;
			}
		}
	}
	if (m == 0)
	{
		data << "];\nAe = [ ";
	}
	else
	{
		data << "]];\nAe = [ ";
	}
	for (unsigned int i = 0; i < A.size(); i++)
	{
		if (sign[i] == "==")
		{
			data << "[";
			for (unsigned int j = 0; j < A[0].size(); j++)
			{
				data << A[i][j] << " ";
			}
			unsigned int k(1);
			while ((i + k) < sign.size())
			{
				if (sign[i + k] == "==")
				{
					data << "],\n";
					break;
				}
				k++;
			}
		}
	}
	if ((sign.size() - m) == 0)
	{
		data << "];\nb = [ ";
	}
	else
	{
		data << "]];\nb = [ ";
	}
	for (unsigned int i = 0; i < b.size(); i++)
	{
		if (sign[i] == "<=")
		{
			data << b[i] << " ";
		}
		else if (sign[i] == ">=")
		{
			data << -b[i] << " ";
		}
	}
	data << "];\nbe = [ ";
	for (unsigned int i = 0; i < b.size(); i++)
	{
		if (sign[i] == "==")
		{
			data << b[i] << " ";
		}
	}
	data << "];" << endl;
	for (int k = 1; k <= M.size(); k++)
	{
		data << "Q" << k << " = [" << endl;
		for (int i = 0; i < x0.size(); i++)
		{
			data << "[ ";
			for (int j = 0; j < x0.size(); j++)
			{
				data << M[k-1][i][j] << " ";
			}
			if (i == x0.size()-1)
			{
				data << "]" << endl;
			}
			else
			{
				data << "]," << endl;
			}
		}
		data << "];" << endl;
	}
	for (int k = 1; k <= CM.size(); k++)
	{
		data << "cm" << k << " = [ ";
		for (int i = 0; i < x0.size(); i++)
		{
			data << CM[k - 1][i] << " ";
		}
		data << "];" << endl;
	}
	for (unsigned int i = 1; i <= C.size(); i++)
	{
		data << "c" << i << " = [ ";
		for (unsigned int j = 0; j < C[0].size(); j++)
		{
			data << C[i - 1][j] << " ";
		}
		data << "];" << endl;
	}
	data << "x0 = [ ";
	for (unsigned int i = 0; i < x0.size(); i++)
	{
		data << x0[i] << " ";
	}
	data << "];" << endl;
	data.close();
}

void writeDatFileAlpha(vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<int>& hl)
{
	//cout << "WriteFileAlpha" << endl;	
	ofstream data;
	data.open("getAlpha.dat", ofstream::trunc);
	unsigned int m(0);
	for (unsigned int i = 0; i < sign.size(); i++)
	{
		if (sign[i] != "==")
		{
			m++;
		}
	}
	data << "n = " << A[0].size() << ";\nm = " << m << ";\nme = " << (sign.size() - m) << ";\nA = [";
	for (unsigned int i = 0; i < A.size(); i++)
	{
		if (sign[i] != "==")
		{
			data << "[";
			for (unsigned int j = 0; j < A[0].size(); j++)
			{
				data << (sign[i] == "<=" ? A[i][j] : -A[i][j]) << " ";
			}
			unsigned int k(1);
			while ((i + k) < sign.size())
			{
				if (sign[i + k] != "==")
				{
					data << "],\n";
					break;
				}
				k++;
			}
		}
	}
	if (m == 0)
	{
		data << "];\nAe = [ ";
	}
	else
	{
		data << "]];\nAe = [ ";
	}
	for (unsigned int i = 0; i < A.size(); i++)
	{
		if (sign[i] == "==")
		{
			data << "[";
			for (unsigned int j = 0; j < A[0].size(); j++)
			{
				data << A[i][j] << " ";
			}
			unsigned int k(1);
			while ((i + k) < sign.size())
			{
				if (sign[i + k] == "==")
				{
					data << "],\n";
					break;
				}
				k++;
			}
		}
	}
	if ((sign.size() - m) == 0)
	{
		data << "];\nb = [ ";
	}
	else
	{
		data << "]];\nb = [ ";
	}
	for (unsigned int i = 0; i < b.size(); i++)
	{
		if (sign[i] == "<=")
		{
			data << b[i] << " ";
		}
		else if (sign[i] == ">=")
		{
			data << -b[i] << " ";
		}
	}
	data << "];\nbe = [ ";
	for (unsigned int i = 0; i < b.size(); i++)
	{
		if (sign[i] == "==")
		{
			data << b[i] << " ";
		}
	}
	data << "];\nhl = [" << endl;
	vector<int> inter(A[0].size(), 0);
	for (int k = 0; k < hl.size(); k++)
	{
		inter[hl[k]] = 1;
	}
	data << "];" << endl;
	data.close();
}

double ZLinear(vector<double>& c, vector<double>& x)
{
	double z(0.0);
	for(int i = 0; i < x.size(); i++)
	{
		z += c[i] * x[i];
	}
	return z;
}

vector<double> Z(vector<vector<vector<double> > >& M, vector<vector<double> >& CM, vector<vector<double> >& C, vector<double>& x)
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

void detectOutBasesSimplex(vector<int>& bases, vector<int>& outBases, int nbCon, int nbVar)
{
	outBases.clear();
	for (int i = 1; i <= (nbCon + nbVar); i++)
	{
		vector<int>::iterator it = find(bases.begin(), bases.end(), i);
		if (it == bases.end()) {
			outBases.push_back(i);
		}
	}
}

void detectHl(vector<vector<double> >& S, vector<int>& outBases, vector<int>& Hl, int nbCon)
{
	for (int j = 0; j < outBases.size(); j++)
	{
		int nbZero = 0;
		for (int i = nbCon + 1; i < S.size(); i++)
		{
			if (S[i][outBases[j] - 1] < 0)
			{
				Hl.push_back(outBases[j] - 1);
				break;
			}
			else if (S[i][outBases[j] - 1] == 0)
			{
				nbZero++;
			}
		}
		if (nbZero == (S.size() - nbCon - 1))
		{
			Hl.push_back(outBases[j] - 1);
		}
	}
}

void addEfficientCutSimplex(vector<vector<double> >& S, vector<int>& Hl, int nbCon, vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<int>& outBases)
{
	for (int i = 0; i < S.size(); i++)
	{
		S[i].insert(S[i].end() - 1, 0.0);
	}
	vector<double> inter(S[0].size(), 0.0);
	for (int i = 0; i < Hl.size(); i++)
	{
		inter[Hl[i]] = -1.0;
	}
	inter[S[0].size() - 2] = 1.0;
	inter[S[0].size() - 1] = -1.0;
	S.insert(S.begin() + nbCon, inter);

	vector<double> cut; double scdB = -1.0;
	for (int i = 0; i < A[0].size(); i++)
	{
		cut.push_back(0.0);
	}
	for (int i = 0; i < Hl.size(); i++)
	{
		if (Hl[i] < A[0].size())
		{
			cut[Hl[i]]--;
		}
		else
		{
			for (int j = 0; j < A[0].size(); j++)
			{
				cut[j] += A[Hl[i] - A[0].size()][j];
			}
			scdB += b[Hl[i] - A[0].size()];
		}
	}
	
	A.push_back(cut);
	b.push_back(scdB);
	sign.push_back("<=");
}

void addCutInfSimplex(vector<vector<double> >& S, vector<int>& bases, int nbCon, vector<vector<double> >& A, vector<double>& b, vector<string>& sign, int indVarFrac)
{
	int indVar;
	for (int i = 0; i < nbCon; i++)
	{
		if ((bases[i] - 1) == indVarFrac)
		{
			indVar = i;
			break;
		}
	}
	for (int i = 0; i < S.size(); i++)
	{
		S[i].insert(S[i].end() - 1, 0.0);
	}
	vector<double> inter(S[0].size(), 0.0);
	for (int i = 0; i < (inter.size() - 1); i++)
	{
		if (indVarFrac != i)
		{
			inter[i] = -S[indVar][i];
		}
	}
	inter[S[0].size() - 2] = 1.0;
	inter[S[0].size() - 1] = floor(S[indVar][S[0].size() - 1]) - S[indVar][S[0].size() - 1];
	S.insert(S.begin() + nbCon, inter);
	bases.push_back(S[0].size() - 1);

	vector<double> cut(A[0].size(), 0.0);
	cut[indVarFrac] = 1.0;
	A.push_back(cut);
	b.push_back(floor(S[indVar][S[0].size() - 1]));
	sign.push_back("<=");
}

void addCutSupSimplex(vector<vector<double> >& S, vector<int>& bases, int nbCon, vector<vector<double> >& A, vector<double>& b, vector<string>& sign, int indVarFrac)
{
	int indVar;
	for (int i = 0; i < nbCon; i++)
	{
		if ((bases[i] - 1) == indVarFrac)
		{
			indVar = i;
			break;
		}
	}
	for (int i = 0; i < S.size(); i++)
	{
		S[i].insert(S[i].end() - 1, 0.0);
	}
	vector<double> inter(S[0].size(), 0.0);
	for (int i = 0; i < (inter.size() - 1); i++)
	{
		if (indVarFrac != i)
		{
			inter[i] = S[indVar][i];
		}
	}
	inter[S[0].size() - 2] = 1.0;
	inter[S[0].size() - 1] = S[indVar][S[0].size() - 1] - ceil(S[indVar][S[0].size() - 1]);
	S.insert(S.begin() + nbCon, inter);
	bases.push_back(S[0].size() - 1);

	vector<double> cut(A[0].size(), 0.0);
	cut[indVarFrac] = -1.0;
	A.push_back(cut);
	b.push_back(-ceil(S[indVar][S[0].size() - 1]));
	sign.push_back("<=");
}

void addEpsilonEfficientCutSimplex(vector<vector<double> >& S, vector<int>& Hl, int nbCon, vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<int>& outBases, double& epsilon)
{
	for (int i = 0; i < S.size(); i++)
	{
		S[i].insert(S[i].end() - 1, 0.0);
	}
	vector<double> inter(S[0].size(), 0.0);
	for (int i = 0; i < Hl.size(); i++)
	{
		inter[Hl[i]] = -1.0;
	}
	inter[S[0].size() - 2] = 1.0;
	inter[S[0].size() - 1] = -1.0;
	S.insert(S.begin() + nbCon, inter);

	vector<double> cut; double scdB = -1.0 - epsilon;
	for (int i = 0; i < A[0].size(); i++)
	{
		cut.push_back(0.0);
	}
	for (int i = 0; i < Hl.size(); i++)
	{
		if (Hl[i] < A[0].size())
		{
			cut[Hl[i]]--;
		}
		else
		{
			for (int j = 0; j < A[0].size(); j++)
			{
				cut[j] += A[Hl[i] - A[0].size()][j];
			}
			scdB += b[Hl[i] - A[0].size()];
		}
	}

	A.push_back(cut);
	b.push_back(scdB);
	sign.push_back("<=");
}

void writeDatFileEpsilon(vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<double>& hl, double& b1)
{
	//cout << "WriteFileEfficience" << endl;
	ofstream data;
	data.open("getEpsilon.dat", ofstream::trunc);
	unsigned int m(0);
	for (unsigned int i = 0; i < sign.size(); i++)
	{
		if (sign[i] != "==")
		{
			m++;
		}
	}
	data << "n = " << A[0].size() << ";\nm = " << m << ";\nA = [";
	for (unsigned int i = 0; i < A.size(); i++)
	{
		if (sign[i] != "==")
		{
			data << "[";
			for (unsigned int j = 0; j < A[0].size(); j++)
			{
				data << (sign[i] == "<=" ? A[i][j] : -A[i][j]) << " ";
			}
			unsigned int k(1);
			while ((i + k) < sign.size())
			{
				if (sign[i + k] != "==")
				{
					data << "],\n";
					break;
				}
				k++;
			}
		}
	}
	if (m == 0)
	{
		data << "];\nb = [ ";
	}
	else
	{
		data << "]];\nb = [ ";
	}
	for (unsigned int i = 0; i < b.size(); i++)
	{
		if (sign[i] == "<=")
		{
			data << b[i] << " ";
		}
		else if (sign[i] == ">=")
		{
			data << -b[i] << " ";
		}
	}
	data << "];\nhl = [" << endl;
	for (unsigned int i = 0; i < hl.size(); i++)
	{
		data << hl[i] << " ";
	}
	data << "];\nscb = " << b1 << ";" << endl;
	data.close();
}

bool getEpsilon(IloEnv env, vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<double>& cut, double& b1, double& epsilon, int& status)
{
	//cout << "TestEfficency" << endl;
	clock_t db = clock();
	status = 6;
	try
	{
		//cout << "getEpsilon" << endl;
		clock_t db2 = clock();
		writeDatFileEpsilon(A, b, sign, cut, b1);
		IloOplRunConfiguration rc1(env, "getEpsilon.mod", "getEpsilon.dat");
		fileTime = fileTime + (clock() - db2);
		rc1.getCplex().setParam(IloCplex::NodeSel, CPX_NODESEL_DFS);
		rc1.getCplex().setParam(IloCplex::Param::MIP::Strategy::MIQCPStrat, 1);
		rc1.getCplex().setOut(env.getNullStream());
		rc1.getOplModel().generate();
		if (rc1.getCplex().solve())
		{
			//if (rc1.getCplex().getObjValue() == 0)
			//cout << rc1.getCplex().getObjValue() << endl;
			//cout << abs(rc1.getCplex().getObjValue() - round(rc1.getCplex().getObjValue())) << endl;
			//afficher(x);
			//afficher(Z(M, CM, C, x));
			if (abs(rc1.getCplex().getObjValue()) < (numeric_limits<double>::epsilon() * 10000000))
			{
				//cout << "Solution efficace." << endl;
				epsilon = 0;
				int_epsilon_null++;
				//cout << "Epsilon = " << epsilon << endl;
				/*char c;
				cin >> c;*/
			}
			else
			{
				epsilon = abs(rc1.getCplex().getObjValue());
				int_epsilon_positive++;
				epsilonsPositive.push_back(epsilon);
				//cout << "Epsilon = " << epsilon << endl;
				/*char c;
				cin >> c;*/
			}
			epsilonTime = epsilonTime + (clock() - db);
			status = 4;
			return true;
		}
		else
		{
			epsilonTime = epsilonTime + (clock() - db);
			int_epsilon_abs++;
			status = 5;
			//cout << "Epsilon abs " << endl;
			/*char c;
			cin >> c;*/
		}
	}
	catch (IloOplException & e) {
		//cout << "### OPL exception: " << e.getMessage() << endl;
		status = 1;
		int_epsilon_error++;
		/*cout << "Epsilon 1 " << endl;
		char c;
		cin >> c;*/
	}
	catch (IloException & e) {
		//cout << "### CONCERT exception: ";
		e.print(cout);
		status = 2;
		int_epsilon_error++;
		/*cout << "Epsilon 2 " << endl;
		char c;
		cin >> c;*/
	}
	catch (...) {
		//cout << "### UNEXPECTED ERROR ..." << endl;
		status = 3;
		int_epsilon_error++;
		/*cout << "Epsilon 3 " << endl;
		char c;
		cin >> c;*/
	}
	return false;
}

bool testEfficiency(IloEnv env, vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<vector<double> > >& M, vector<vector<double> >& CM, vector<vector<double> >& C, vector<double>& x, int& status)
{
	//cout << "TestEfficency" << endl;
	clock_t db = clock();
	try
	{
		clock_t db2 = clock();
		writeDatFileEfficience(A, b, sign, M, CM, C, x);
		IloOplRunConfiguration rc1(env, "TestEfficience.mod", "TestEfficience.dat");
		fileTime = fileTime + (clock() - db2);
		rc1.getCplex().setParam(IloCplex::NodeSel, CPX_NODESEL_DFS);
		rc1.getCplex().setParam(IloCplex::Param::MIP::Strategy::MIQCPStrat, 1);
		rc1.getCplex().setOut(env.getNullStream());
		rc1.getOplModel().generate();
		if (rc1.getCplex().solve())
		{
			//if (rc1.getCplex().getObjValue() == 0)
			//cout << rc1.getCplex().getObjValue() << endl;
			//cout << abs(rc1.getCplex().getObjValue() - round(rc1.getCplex().getObjValue())) << endl;
			//afficher(x);
			//afficher(Z(M, CM, C, x));
			if (abs(rc1.getCplex().getObjValue()) < (numeric_limits<double>::epsilon() * 10000000))
			{
				//cout << "Solution efficace." << endl;
				quadTime = quadTime + (clock() - db);
				return true;
			}
			else
			{
				IloIntMap t = rc1.getOplModel().getElement("x").asIntMap();
				//cout << "Solution non efficace." << endl;
				//cout << "solution Y: " << endl;
				x.clear();
				for (IloInt i = 1; i <= t.getSize(); i++)
				{
					x.push_back(t.get(i));
					//cout << x[i - 1] << " ";
				}
				//cout << endl;
				//vector<double> coutZ(Z(M, CM, C, x));
				//afficher(coutZ);
				/*if (find(SND.begin(), SND.end(), coutZ) == SND.end())
				{
					SND.push_back(coutZ);
					int_eff_sol++;
					int_sol++;
				}*/
				//cout << "\nZL = " << z << endl;
				//char c;
				//cin >> c;
				quadTime = quadTime + (clock() - db);
				return false;
			}
		}
		else
		{
			quadTime = quadTime + (clock() - db);
		}
	}
	catch (IloOplException& e) {
		//cout << "### OPL exception: " << e.getMessage() << endl;
		status = 1;
	}
	catch (IloException& e) {
		//cout << "### CONCERT exception: ";
		e.print(cout);
		status = 2;
	}
	catch (...) {
		//cout << "### UNEXPECTED ERROR ..." << endl;
		status = 3;
	}
}

bool initialPhase(vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<double> >& C, vector<vector<vector<double> > >& M, vector<vector<double> >& CM, vector<double>& x, vector<vector<double> >& SND, set<vector<double> >& Eff)
{
	//cout << "Initial Phase" << endl;
	int status(0);
	unsigned int level(0);
	//allLevels << 0 << endl;
	IloEnv env;
	//cout << "obj = " << endl;
	//afficher(obj);
	//vector<vector<double> > L = buildLamkeMatrix(A, b, M, c);
	vector<vector<double> > S = buildSimplexMatrix(A, b, sign, C, M, CM);
	//cout << "S=" << endl;
	//afficher(S);
	vector<int> bases, xBases;
	for (int i = 1; i <= A.size(); i++)
	{
		bases.push_back(A[0].size() + i);
	}
	if (simplex(S, A.size(), A[0].size(), C.size(), CM.size(), bases, M, CM, x))
	{
		int indNoTInt;
		//cout << "S =" << endl;
		//afficher(S);
		if (entiere(x, indNoTInt))
		{
			if (testEfficiency(env, A, b, sign, M, CM, C, x, status))
			{
				vector<double> z = Z(M,CM,C,x);
				SND.push_back(z);
				Eff.insert(x);
				//cout << "num : " << num_noeud << endl;
				//afficher(z);
				int_eff_sol++;
			}
			// Add efficiency cut
			vector<vector<double> > A_(A);
			vector<double> b_(b);
			vector<string> sign_(sign);
			vector<int> outBases, Hl;
			detectOutBasesSimplex(bases, outBases, A.size(), x.size());
			//cout << "out bases = ";
			//afficher(outBases);
			detectHl(S, outBases, Hl, A.size());
			int_sol++;
			//cout << "Hl = "; 
			//afficher(Hl);
			if (Hl.empty())
			{
				return false;
			}
			else
			{
				double epsilon(0);
				vector<double> cut; double scdB = -1.0;
				for (int i = 0; i < A[0].size(); i++)
				{
					cut.push_back(0.0);
				}
				for (int i = 0; i < Hl.size(); i++)
				{
					if (Hl[i] < A[0].size())
					{
						cut[Hl[i]]--;
					}
					else
					{
						for (int j = 0; j < A[0].size(); j++)
						{
							cut[j] += A_[Hl[i] - A[0].size()][j];
						}
						scdB += b_[Hl[i] - A[0].size()];
					}
				}
				if (getEpsilon(env, A_, b_, sign_, cut, scdB, epsilon, status))
				{
					cout << "init Phase Epsilon" << endl;
					vector<vector<double> > A_2(A);
					vector<double> b_2(b);
					vector<string> sign_2(sign);
					int nbCon = A.size() + 1, nbVar = A[0].size(), nbObj = C.size();
					addEpsilonEfficientCutSimplex(S, Hl, A.size(), A_2, b_2, sign_2, outBases, epsilon);
					bases.push_back(S[0].size() - 1);
					// Create root
					//NoeudLemke noeud(0, A_, b_, sign_, xBases, false);
					//tree.push(NoeudLemke(noeud));
					NoeudQuadratic noeud(num_noeud, level, S, A_2, b_2, sign_2, bases, nbCon, nbVar, nbObj);

					tree.push(NoeudQuadratic(noeud));
					num_noeud++;

				}
				else
				{
					cout << "status = " << status << endl;
				}
			}
		}
		else // Not integer solution
		{
			// Add cut inf
			vector<vector<double> > A1(A);
			vector<double> b1(b);
			vector<string> sign1(sign);
			vector<vector<double> > S1(S), S2(S);
			vector<int> bases1(bases), bases2(bases);
			addCutInfSimplex(S1, bases1, A.size(), A1, b1, sign1, indNoTInt);
			// Create root
			int nbCon = A.size() + 1, nbVar = A[0].size(), nbObj = C.size();
			NoeudQuadratic noeud(num_noeud, level, S1, A1, b1, sign1, bases1, nbCon, nbVar, nbObj);
			//tree.push(NoeudQuadratic(noeud));
			num_noeud++;
			// Add cut sup
			vector<vector<double> > A2(A);
			vector<double> b2(b);
			vector<string> sign2(sign);
			addCutSupSimplex(S2, bases2, A.size(), A2, b2, sign2, indNoTInt);
			// Create root
			NoeudQuadratic noeud2(num_noeud, level, S2, A2, b2, sign2, bases2, nbCon, nbVar, nbObj);
			tree.push(NoeudQuadratic(noeud2));
			tree.push(NoeudQuadratic(noeud));
			num_noeud++;
			//cout << "add cut on : " << indNoTInt << endl;
		}
	}
	else
	{
		//cout << "No feasible solution found" << endl;
		return false;
	}
	env.end();
	return true;
}

void afficher(vector<pair<pair<int, int>, double> > memory)
{
	cout << "Memory:" << endl;
	for (int i = 0; i < memory.size(); i++)
	{
		cout << i << " : (" << memory[i].first.first << ", " << memory[i].first.second << ") = " << memory[i].second << endl;
	}
	cout << "FIN MEMORY" << endl;
}

bool dualSimplex(vector<vector<double> >& S, int nbCon, int nbVar, int nbObjL, int nbObjQ, vector<int>& bases, vector<vector<vector<double> > >& M, vector<vector<double> >& CM, vector<double>& x)
{
	//cout << "DUAL BEGIN" << endl;
	char c;
	vector<pair<pair<int, int>, double> > memory;
	while (true)
	{
		//cout << "nbCon = " << nbCon << endl;
		//cout << "nbVar = " << nbVar << endl;
		double maxCol = -numeric_limits<double>::infinity(), minRow = numeric_limits<double>::infinity(); int indCol = -1, indRow = -1; double minCol = numeric_limits<double>::infinity();
		vector<int> listRows;
		for (int i = 0; i < nbCon; i++)
		{
			/*if ((S[i][nbCon + nbVar] < 0) && (minRow > S[i][nbCon + nbVar]))
			{
				minRow = S[i][nbCon + nbVar];
				indRow = i;
			}*/
			if (S[i][nbCon + nbVar] < 0)
			{
				listRows.push_back(i);
			}
		}
		//cout << "indRow = " << indRow << endl;
		//cout << "Bases :";
		//afficher(bases);
		if (/*indRow != -1*/!listRows.empty())
		{
			indRow = listRows[0];
			for (int i = 0; i < S[0].size() - 1; i++)
			{
				pair<int, int> in(indRow, i); pair<pair<int, int>, double> out(in, S[indRow][i]);
				if ((S[indRow][i]  < 0) && (find(bases.begin(), bases.end(), i + 1) == bases.end()) && (maxCol <= (S[nbCon][i] / S[indRow][i])) && (find(memory.begin(), memory.end(), out) == memory.end()))
				{
					maxCol = S[nbCon][i] / S[indRow][i];
					indCol = i;
					memory.push_back(out);
					//afficher(memory);
				}
				else if ((S[indRow][i]  < 0) && (find(bases.begin(), bases.end(), i + 1) == bases.end()) && (maxCol < (S[nbCon][i] / S[indRow][i])) && (find(memory.begin(), memory.end(), out) != memory.end()))
				{
					maxCol = S[nbCon][i] / S[indRow][i];
					//cout << "non accepté: " << i << endl;
				}
			}
			//cout << "indCol = " << indCol << endl;
			//cout << S[indRow][indCol] << endl;
			/*for (int i = 0; i < listRows.size(); i++)
			{
				for (int j = 0; j < S[0].size(); j++)
				{
					if ((j != (nbCon + nbVar)) && (S[listRows[i]][j] < 0))
					{
						indRow = i;
						indCol = j;
						break;
					}
				}
				if (indCol != -1)
				{
					break;
				}
			}*/
			if (indCol != -1)
			{
				//cout << "Pivot = (" << indRow << ", " << indCol << ") = " << S[indRow][indCol] << endl;
				pivoting(S, indRow, indCol, nbObjQ);
				bases[indRow] = indCol + 1;
				//cout << "S after pivoting = " << endl;
				//afficher(S);
				//cout << "CLICK TO CONTINUE ... " << endl;
				////cin >> c;
			}
			else
			{
				// unbounded area
				//cout << "Dual : Unbounded area" << endl;
				//cout << "CLICK TO CONTINUE ... " << endl;
				////cin >> c;
				return false;
			}
		}
		else
		{
			//cout << "view S = " << endl;
			//afficher(S);
			// Optimum if dual optimal
			for (int i = 0; i < (S[0].size() - 1); i++)
			{
				if (S[nbCon][i] < 0)
				{
					//cout << "DUAL : No feasible solution" << endl;
					//cout << "CLICK TO CONTINUE ... " << endl;
					////cin >> c;
					return false;
				}
			}
			// Get solution into x
			extractSolution(S, bases, nbCon, nbVar, x);
			//cout << "bases before quad calcule : ";
			//afficher(bases);
			// Compute quadratic terms
			int ind(0);
			for (int i = nbCon + nbObjL + 1; i < S.size(); i++)
			{
				for (int j = 0; j < S[0].size() - 1; j++)
				{
					if (find(bases.begin(), bases.end(), j + 1) != bases.end())
					{
						S[i][j] = 0.0;
					}
					else if (j < nbVar)
					{
						S[i][j] = deriverQuadratic(M[ind], CM[ind], x, j) - functionBase(S, M[ind], CM[ind], x, j, bases);
					}
					else
					{
						S[i][j] = -functionBase(S, M[ind], CM[ind], x, j, bases);
					}
				}
				S[i][S[0].size() - 1] = ZQuad(M[ind], CM[ind], x);
				ind++;
			}
			//cout << "Solution du dual = ";
			//afficher(x);
			//cout << "CLICK TO CONTINUE ... " << endl;
			////cin >> c;
			return true;
		}
	}
	//cout << "DUAL END" << endl;
}

vector<double> approxIdealQuadratic(NoeudQuadratic n, vector<vector<vector<double> > >& M, vector<vector<double> >& CM, vector<vector<double> >& C)
{
	vector<double> idealPoint;
	IloEnv env3;
	//cout << "IQ Calculating" << endl;
	clock_t beg = clock();
	clock_t beg2 = clock();
	for (unsigned int i = 0; i < M.size(); i++)
	{
		clock_t beg4 = clock();
		//cout << "i = " << i << endl;
		IloModel model(env3);
		IloNumExpr obj(env3);
		IloNumVarArray x(env3);
		IloRangeArray ct(env3);
		//cout << "so: " << C[0].size() << endl;
		for (int j = 0; j < C[0].size(); j++)
		{
			//cout << "j= " << j << endl;
			stringstream sstm;
			sstm << "x" << j;
			string result = sstm.str();
			//cout << "result= " << result << endl;
			x.add(IloNumVar(env3, 0.0, IloInfinity, result.data()));
		}
		//cout << "after x" << endl;
		for (unsigned int j = 0; j < M[i].size(); j++)
		{
			for (unsigned int k = 0; k < M[i][j].size(); k++)
			{
				obj += M[i][j][k] * x[j] * x[k];
			}
		}
		//cout << "after first obj" << endl;
		for (unsigned int j = 0; j < CM[i].size(); j++)
		{
			obj += CM[i][j] * x[j];
		}
		//cout << "after last obj" << endl;
		model.add(IloMinimize(env3, obj));
		for (int k = 0; k < n.getA().size(); k++)
		{
			IloNumExpr c(env3);
			for (int j = 0; j < C[0].size(); j++)
			{
				c += n.getA()[k][j] * x[j];
			}
			if (n.getSign()[k] == "<=")
			{
				ct.add(c <= n.getB()[k]);
			}
			else if (n.getSign()[k] == ">=")
			{
				ct.add(c >= n.getB()[k]);
			}
			else
			{
				ct.add(c == n.getB()[k]);
			}
			stringstream sstm;
			sstm << "c" << k;
			string result = sstm.str();
			ct[k].setName(result.data());
		}
		//cout << "after ct" << endl;
		model.add(ct);
		IloCplex cpx(model);
		//cout << "Model pour point Ideal, Obj : " << (i + 1) << endl;
		//cpx.out() << model << endl;
		cpx.setOut(cpx.getEnv().getNullStream());
		clock_t ed4 = clock();
		constructTime += (ed4 - beg4);
		if (cpx.solve())
		{
			idealPoint.push_back(cpx.getObjValue());
		}
		else
		{
			cout << "No solution" << endl;
		}
	}
	clock_t ed2 = clock();
	idealQuadTime += (ed2 - beg2);
	clock_t beg3 = clock();
	for (unsigned int i = 0; i < C.size(); i++)
	{
		clock_t beg5 = clock();
		IloModel model(env3);
		IloNumExpr obj(env3);
		IloNumVarArray x(env3);
		IloRangeArray ct(env3);
		for (int j = 0; j < C[0].size(); j++)
		{
			stringstream sstm;
			sstm << "x" << j;
			string result = sstm.str();
			x.add(IloNumVar(env3, 0.0, IloInfinity, result.data()));
		}
		for (unsigned int j = 0; j < C[i].size(); j++)
		{
			obj += C[i][j] * x[j];
		}
		model.add(IloMinimize(env3, obj));
		for (int k = 0; k < n.getA().size(); k++)
		{
			IloNumExpr c(env3);
			for (int j = 0; j < C[0].size(); j++)
			{
				c += n.getA()[k][j] * x[j];
			}
			if (n.getSign()[k] == "<=")
			{
				ct.add(c <= n.getB()[k]);
			}
			else if (n.getSign()[k] == ">=")
			{
				ct.add(c >= n.getB()[k]);
			}
			else
			{
				ct.add(c == n.getB()[k]);
			}
			stringstream sstm;
			sstm << "c" << k;
			string result = sstm.str();
			ct[k].setName(result.data());
		}
		model.add(ct);
		IloCplex cpx(model);
		//cout << "Model pour point Ideal, Obj : " << (i + 1) << endl;
		//cpx.out() << model << endl;
		cpx.setOut(cpx.getEnv().getNullStream());
		clock_t ed5 = clock();
		constructTime += (ed5 - beg5);
		if (cpx.solve())
		{
			idealPoint.push_back(cpx.getObjValue());
		}
		else
		{
			cout << "No solution" << endl;
		}
	}
	env3.end();
	clock_t ed = clock();
	idealTime += (ed - beg);
	clock_t ed3 = clock();
	idealLinTime += (ed3 - beg3);
	return idealPoint;
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

void updateSND(vector<vector<double> >& Eff, vector<vector<double> >& SND, vector<double>& z, vector<double>& x)
{
	for (int i = SND.size() - 1; i >= 0; i--)
	{
		if (domine(SND[i], z))
		{
			return;
		}
		else if (domine(z, SND[i]))
		{
			SND.erase(SND.begin() + i);
			Eff.erase(Eff.begin() + i);
		}
	}
	SND.push_back(z);
	Eff.push_back(x);
}

void generalPhase(vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<double> >& C, vector<vector<vector<double> > >& M, vector<vector<double> >& CM, vector<double>& x, vector<vector<double> >& SND, set<vector<double> >& Eff)
{
	//cout << "General Phase" << endl;
	IloEnv env;
	char c;
	num_noeud = (tree.size() == 1 ? 1 : 3);
	vector<vector<int> > coutReduit;
	vector<double> x_;
	while (!tree.empty())
	{
		int status(0);
		//cout << "Noeud n : " << tree.top() << endl;
		cout << "num_noeud : " << num_noeud << endl;
		//cout << "CLICK TO CONTINUE ... " << endl;
		//cin >> c;
		vector<int> bases(tree.top().getBase()), xBases;
		vector<vector<double> > S(tree.top().getS()), A2(tree.top().getA());
		vector<double> b2(tree.top().getB());
		vector<string> sign2(tree.top().getSign());
		int nbCon(tree.top().getNbCon()), nbVar(tree.top().getNbVar());
		//allLevels << tree.top().getLevel() << endl;
		//cout << "S before dual =" << endl;
		//afficher(S);
		//cout << "dual" << endl;
		if (dualSimplex(S, nbCon, nbVar, C.size(), M.size(), bases, M, CM, x_))
		{
			//detectBase(L, bases, xBases, x, tree.top().getA());
			//extractSolution(tree.top().getS(), tree.top().getBase(), tree.top().getNbCon(), tree.top().getNbVar(), x);
			//cout << "x = " << endl;
			//afficher(x_);
			//cout << "f = " << z_ << endl;
			//cout << "S after dual =" << endl;
			//afficher(S);
			//cout << "actual f = " << z << endl;
			if (SND.empty())
			{
				int indNoTInt;
				if (entiere(x_, indNoTInt))
				{
					x = x_;
					SND.push_back(Z(M, CM, C, x));
					Eff.push_back(x);
					//cout << "Solution non efficace" << endl;
					//cout << "x = " << endl;
					//afficher(x);
					//cout << " z = " << z << endl;
					// Add efficiency cut
					vector<vector<double> > A_(A2);
					vector<double> b_(b2);
					vector<string> sign_(sign2);
					vector<int> outBases, Hl;
					//detectOutBase(bases, outBases, x.size(), A.size());
					detectOutBasesSimplex(bases, outBases, nbCon, nbVar);
					//addEfficientCut(A_, b_, sign_, outBases);
					detectHl(S, outBases, Hl, nbCon);
					int_sol++;
					//cout << "Hl : ";
					//afficher(Hl);
					if (Hl.empty())
					{
						tree.top().~NoeudQuadratic();
						tree.pop();
						hl_delete++;
					}
					else
					{
						double epsilon(0);
						vector<double> cut; double scdB = -1.0;
						for (int i = 0; i < A[0].size(); i++)
						{
							cut.push_back(0.0);
						}
						for (int i = 0; i < Hl.size(); i++)
						{
							if (Hl[i] < A[0].size())
							{
								cut[Hl[i]]--;
							}
							else
							{
								for (int j = 0; j < A[0].size(); j++)
								{
									cut[j] += A_[Hl[i] - A[0].size()][j];
								}
								scdB += b_[Hl[i] - A[0].size()];
							}
						}
						if (getEpsilon(env, A_, b_, sign_, cut, scdB, epsilon, status))
						{
							//cout << "general Phase Epsilon 1" << endl;
							//cout << "status = " << status << endl;
							//cout << "epsilon = " << epsilon << endl;
							vector<vector<double> > A_2(A2);
							vector<double> b_2(b2);
							vector<string> sign_2(sign2);
							addEpsilonEfficientCutSimplex(S, Hl, nbCon, A_2, b_2, sign_2, outBases, epsilon);
							bases.push_back(S[0].size() - 1);
							// Create root
							//NoeudLemke noeud(0, A_, b_, sign_, xBases, false);
							//tree.push(NoeudLemke(noeud));
							NoeudQuadratic noeud(num_noeud, tree.top().getLevel(), S, A_2, b_2, sign_2, bases, nbCon + 1, nbVar, tree.top().getNbObj());
							tree.top().~NoeudQuadratic();
							tree.pop();
							tree.push(NoeudQuadratic(noeud));
							num_noeud++;
						}
						else {
							//cout << "status = " << status << endl;
							tree.top().~NoeudQuadratic();
							tree.pop();
						}
					}
					//cout << "CLICK TO CONTINUE ... " << endl;
					//cin >> c;
				}
				else // Not integer solution
				{
					// Delete actual node
					tree.top().~NoeudQuadratic();
					tree.pop();
					// Add cut inf
					vector<vector<double> > A1(A2);
					vector<double> b1(b2);
					vector<string> sign1(sign2);
					//addCutInf(A1, b1, sign1, x, indNoTInt);
					vector<vector<double> > S1(S), S2(S);
					vector<int> bases1(bases), bases2(bases);
					addCutInfSimplex(S1, bases1, nbCon, A1, b1, sign1, indNoTInt);
					// Create root
					//NoeudLemke noeud1(1, A1, b1, sign1, xBases, false);
					//tree.push(NoeudLemke(noeud1));
					NoeudQuadratic noeud(num_noeud, tree.top().getLevel(), S1, A1, b1, sign1, bases1, nbCon + 1, nbVar, tree.top().getNbObj());
					//tree.push(NoeudQuadratic(noeud));
					num_noeud++;
					// Add cut sup
					vector<vector<double> > A3(A2);
					vector<double> b3(b2);
					vector<string> sign3(sign2);
					//addCutSup(A2, b2, sign2, x, indNoTInt);
					addCutSupSimplex(S2, bases2, nbCon, A3, b3, sign3, indNoTInt);
					// Create root
					//NoeudLemke noeud2(2, A2, b2, sign2, xBases, false);
					//tree.push(NoeudLemke(noeud2));
					NoeudQuadratic noeud2(num_noeud, tree.top().getLevel(), S2, A3, b3, sign3, bases2, nbCon + 1, nbVar, tree.top().getNbObj());
					tree.push(NoeudQuadratic(noeud2));
					tree.push(NoeudQuadratic(noeud));
					num_noeud++;
					//cout << "ind to add : " << indNoTInt << endl;
					//cout << "Solution non entiere" << endl;
					//cout << "x = " << endl;
					//afficher(x_);
					//cout << " z = " << z_ << endl;
					//cout << "CLICK TO CONTINUE ... " << endl;
					//cin >> c;
				}
			}
			else
			{
				//cout << "here" << endl;
				//vector<double> IQ = approxIdeal(tree.top(), C);
				//cout << "IQ" << endl;
				vector<double> IQ = approxIdealQuadratic(tree.top(), M, CM, C);
				//cout << "IQ : " << endl;
				for (unsigned int i = 0; i < IQ.size(); i++)
				{
					//cout << IQ[i] << " ";
				}
				bool domination(false);
				if (!IQ.empty())
				{
					for (unsigned int i = 0; i < SND.size(); i++)
					{
						if (domine(SND[i], IQ))
						{
							domination = true;
							break;
						}
					}
				}
				else
				{
					tree.top().~NoeudQuadratic();
					tree.pop();
					empty_delete++;
					continue;
				}
				//cout << "\ndomination = " << domination << endl;
				if (!domination)
				{
					int indNoTInt;
					//cout << "x = ";
					//afficher(x_);
					if (entiere(x_, indNoTInt))
					{
						x = x_;
						vector<double> z(Z(M, CM, C, x));
						updateSND(Eff, SND, z, x);
						//cout << "Solution non efficace" << endl;
						//cout << "x = " << endl;
						//afficher(x_);
						//cout << " z = " << z_ << endl;
						// Add efficiency cut
						vector<vector<double> > A_(A2);
						vector<double> b_(b2);
						vector<string> sign_(sign2);
						vector<int> outBases, Hl;
						//detectOutBase(bases, outBases, x.size(), A.size());
						detectOutBasesSimplex(bases, outBases, nbCon, nbVar);
						//cout << "OutBases : ";
						//afficher(outBases);
						//addEfficientCut(A_, b_, sign_, outBases);
						detectHl(S, outBases, Hl, nbCon);
						int_sol++;
						//cout << "Hl : ";
						//afficher(Hl);
						if (Hl.empty())
						{
							tree.top().~NoeudQuadratic();
							tree.pop();
							hl_delete++;
						}
						else
						{
							double epsilon(0);
							vector<double> cut; double scdB = -1.0;
							for (int i = 0; i < A[0].size(); i++)
							{
								cut.push_back(0.0);
							}
							for (int i = 0; i < Hl.size(); i++)
							{
								if (Hl[i] < A[0].size())
								{
									cut[Hl[i]]--;
								}
								else
								{
									for (int j = 0; j < A[0].size(); j++)
									{
										cut[j] += A_[Hl[i] - A[0].size()][j];
									}
									scdB += b_[Hl[i] - A[0].size()];
								}
							}
							if (getEpsilon(env, A_, b_, sign_, cut, scdB, epsilon, status))
							{
								//cout << "general Phase Epsilon 1" << endl;
								//cout << "status = " << status << endl;
								//cout << "epsilon = " << epsilon << endl;
								vector<vector<double> > A_2(A2);
								vector<double> b_2(b2);
								vector<string> sign_2(sign2);
								addEpsilonEfficientCutSimplex(S, Hl, nbCon, A_2, b_2, sign_2, outBases, epsilon);
								bases.push_back(S[0].size() - 1);
								// Create root
								//NoeudLemke noeud(0, A_, b_, sign_, xBases, false);
								//tree.push(NoeudLemke(noeud));
								NoeudQuadratic noeud(num_noeud, tree.top().getLevel(), S, A_2, b_2, sign_2, bases, nbCon + 1, nbVar, tree.top().getNbObj());
								tree.top().~NoeudQuadratic();
								tree.pop();
								tree.push(NoeudQuadratic(noeud));
								num_noeud++;
							}
							else {
								//cout << "status = " << status << endl;
								tree.top().~NoeudQuadratic();
								tree.pop();
							}
						}
						//cout << "CLICK TO CONTINUE ... " << endl;
						//cin >> c;
					}
					else // Not integer solution
					{
						//cout << "NO ENTIRE SOLUTION" << endl;
						// Delete actual node
						tree.top().~NoeudQuadratic();
						tree.pop();
						// Add cut inf
						vector<vector<double> > A1(A2);
						vector<double> b1(b2);
						vector<string> sign1(sign2);
						//addCutInf(A1, b1, sign1, x, indNoTInt);
						vector<vector<double> > S1(S), S2(S);
						vector<int> bases1(bases), bases2(bases);
						addCutInfSimplex(S1, bases1, nbCon, A1, b1, sign1, indNoTInt);
						//cout << "S after adding inf cut" << endl;
						//afficher(S1);
						// Create root
						//NoeudLemke noeud1(1, A1, b1, sign1, xBases, false);
						//tree.push(NoeudLemke(noeud1));
						NoeudQuadratic noeud(num_noeud, tree.top().getLevel(), S1, A1, b1, sign1, bases1, nbCon + 1, nbVar, tree.top().getNbObj());
						//tree.push(NoeudQuadratic(noeud));
						num_noeud++;
						// Add cut sup
						vector<vector<double> > A3(A2);
						vector<double> b3(b2);
						vector<string> sign3(sign2);
						//addCutSup(A2, b2, sign2, x, indNoTInt);
						addCutSupSimplex(S2, bases2, nbCon, A3, b3, sign3, indNoTInt);
						//cout << "S after adding inf cut" << endl;
						//afficher(S2);
						// Create root
						//NoeudLemke noeud2(2, A2, b2, sign2, xBases, false);
						//tree.push(NoeudLemke(noeud2));
						NoeudQuadratic noeud2(num_noeud, tree.top().getLevel(), S2, A3, b3, sign3, bases2, nbCon + 1, nbVar, tree.top().getNbObj());
						tree.push(NoeudQuadratic(noeud2));
						tree.push(NoeudQuadratic(noeud));
						num_noeud++;
						//cout << "ind to add : " << indNoTInt << endl;
						//cout << "Solution non entiere" << endl;
						//cout << "x = " << endl;
						//afficher(x_);
						//cout << " z = " << z_ << endl;
						//cout << "CLICK TO CONTINUE ... " << endl;
						//cin >> c;
					}
				}
				else // IQ is domined by an element of SND
				{
					//levels << tree.top().getLevel() << ";" << num_noeud << endl;
					tree.top().~NoeudQuadratic();
					tree.pop();
					dom_delete++;
					//cout << "Solution dominee par SND" << endl;
					//cout << "x = " << endl;
					//afficher(x_);
					//cout << "CLICK TO CONTINUE ... " << endl;
					//cin >> c;
				}

			}
		}
		else // No solution for this node
		{
			tree.top().~NoeudQuadratic();
			tree.pop();
			empty_delete++;
			//cout << "NO SOLUTION" << endl;
			//cout << "CLICK TO CONTINUE ... " << endl;
			//cin >> c;
		}
		//cout << "CLICK TO CONTINUE ... " << endl;
		//cin >> c;
	}
}

int main()
{
	vector<vector<double> > A, C, CM;
	vector<vector<vector<double> > > M;
	vector<vector<double> > SND;
	set<vector<double> > Eff;
	vector<int> H, N;
	vector<double> b, x;
	vector<string> sign;
	clock_t beg, ed, tm;
	output.open("output.txt", ofstream::trunc);
	output2.open("data.txt", ofstream::trunc);
	//levels.open("levels.csv", ofstream::trunc);
	//allLevels.open("allLevels.txt", ofstream::trunc);
	//cout << numeric_limits<double>::epsilon()/2 << endl;
	//cout << numeric_limits<double>::lowest() << endl;
	//cout << numeric_limits<double>::min() << endl;
	
	readData(A, b, sign, C, M, CM);
	
	beg = clock();
	if (initialPhase(A, b, sign, C, M, CM, x, SND, Eff))
	{
		generalPhase(A, b, sign, C, M, CM, x, SND, Eff);
	}
	ed = clock();
	tm = ed - beg;

	cout << "Ensemble des solutions efficaces :";
	afficher(Eff);
	cout << "Ensemble des solutions non-dominées :";
	afficher(SND);

	cout << "Execution time : " << tm/1000.0 << " sec" << endl;
	cout << "Quadratic time : " << quadTime/1000.0 << " sec" << endl;
	cout << "File time : " << fileTime / 1000.0 << " sec" << endl;
	cout << "Ideal time : " << idealTime / 1000.0 << " sec" << endl;
	cout << "Ideal Quad time : " << idealQuadTime / 1000.0 << " sec" << endl;
	cout << "Ideal Lin time : " << idealLinTime / 1000.0 << " sec" << endl;
	cout << "Construct Ideal time : " << constructTime / 1000.0 << " sec" << endl;
	cout << "Alpha time : " << alphaTime / 1000.0 << " sec" << endl;
	cout << "Real time : " << (tm - constructTime) / 1000.0 << " sec" << endl;
	char h;
	cout << "Number of nodes : " << num_noeud << endl;
	cout << "Number of nodes deleted because efficiency : \t\t\t" << eff_delete << endl;
	cout << "Number of nodes deleted because Hl empty : \t\t\t" << hl_delete << endl;
	cout << "Number of nodes deleted because dominanced sub region : \t" << dom_delete << endl;
	cout << "Number of nodes deleted because cost : \t\t\t\t" << cost_delete << endl;
	cout << "Number of nodes deleted because sub region empty : \t\t" << empty_delete << endl;
	cout << "Number of integer solutions found : \t\t\t\t" << int_sol << endl;
	cout << "Number of integer efficient solutions found : \t\t\t" << int_eff_sol << endl;
	cout << "ENTRER UN CARACTERE ET CLIQUER ENTRER POUR SORTIR ... " << endl;
	cin >> h;
	output.close();
	//levels.close();
	//allLevels.close();
	return 0;
}