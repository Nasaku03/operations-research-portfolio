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

#include "NoeudQuadratic.h"

using namespace std;

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

ofstream output, output2, output3;
//ofstream levels, allLevels;
unsigned int num_noeud = 0, dom_delete = 0, hl_delete = 0, cost_delete = 0, eff_delete = 0, empty_delete = 0, int_sol = 0, int_eff_sol = 0, int_epsilon_positive = 0, int_epsilon_null = 0, int_epsilon_abs = 0, int_epsilon_error = 0;
//priority_queue<NoeudLemke, vector<NoeudLemke>, mycomparison> tree;
priority_queue<NoeudQuadratic, vector<NoeudQuadratic>, mycomparison2> tree;
clock_t fileTime(0);
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

void readData(vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<double> >& C)
{
	//cout << "read data" << endl;
	unsigned int n, m, p; char c;
	ifstream fichier("input.txt");
	if (fichier)
	{
		double mot; //Une variable pour stocker les lignes lues
		fichier >> n;
		fichier >> m;
		fichier >> p;
		vector<double> inter;
		output << "n = " << n << endl;
		output << "m = " << m << endl;
		output << "p = " << p << endl;
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

double** buildSimplexMatrix(double** A, int& n_A, int& m_A, double* b, string* sign, vector<vector<double> >& C)
{
	// matrice construite uniquement pour les contraintes de type "<="
	double** S = new double*[m_A * 2];;
	for (int i = 0; i < m_A*2; i++)
	{
		S[i] = new double[(n_A + m_A) *2];
		for (int j = 0; j < n_A; j++)
		{
			S[i][j] = A[i][j];
		}
	}
	for (int i = 0; i < m_A; i++)
	{
		for (int j = 0; j < m_A; j++)
		{
			if (i == j)
			{
				S[i][n_A + j] = 1.0;
			}
			else
			{
				S[i][n_A + j] = 0.0;
			}
		}
		S[i][n_A + m_A] = b[i];
	}
	for (int i = 0; i < C.size(); i++)
	{
		for (int j = 0; j < C[0].size(); j++)
		{
			S[m_A + i][j] = C[i][j];
		}
	}
	for (int i = 0; i < C.size(); i++)
	{
		for (int j = 0; j <= m_A; j++)
		{
			S[m_A + i][n_A + j] = 0.0;
		}
	}
	return S;
}

void pivoting(double** M, int& n_S, int& m_S, int const& line, int const& column)
{
	// Divide line pivot by pivot
	double pivot = M[line][column];
	for (int j = 0; j < n_S; j++)
	{
		if (abs((M[line][j] / pivot) - round(M[line][j] / pivot)) >(numeric_limits<double>::epsilon() * 10000000))
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
	for (int i = 0; i < m_S; i++)
	{
		if (i != line)
		{
			double alpha = M[i][column];
			for (int j = 0; j < n_S; j++)
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

void initialSolution(vector<double>& x, int nbVar)
{
	x.clear();
	for (int i = 0; i < nbVar; i++)
	{
		x.push_back(0.0);
	}
}

void extractSolution(double** S, vector<int>& bases, int nbCon, int nbVar, vector<double>& x)
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

bool simplex(double** S, int& n_S, int& m_S, int nbCon, int nbVar, int nbObjL, vector<int>& bases, vector<double>& x)
{
	//cout << "SIMPLEXE (ONLY MINIMIZATION PROBLEMS)" << endl;
	while (true)
	{
		double minCol = numeric_limits<double>::infinity(), minRow = numeric_limits<double>::infinity(); int indCol = -1, indRow = -1;
		// Detect pivot
		// column
		for (int i = 0; i < n_S; i++)
		{
			if ((i != (n_S - 1)) && (S[nbCon][i] < 0) && (S[nbCon][i] < minCol))
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
				pivoting(S, n_S, m_S, indRow, indCol);
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
	model << "int n = ...;\nint m = ...;\nint p = ...;\nrange vare = 1..n;\nrange con = 1..m;\nrange obj = 1..p;\nint A[con][vare] = ...;\nint b[con] = ...;" << endl;
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
	data << "n = " << A[0].size() << ";\nm = " << m << ";\np = " << (M.size() + C.size()) << ";\nA = [";
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
	data << "];\n";
	for (int k = 1; k <= M.size(); k++)
	{
		data << "Q" << k << " = [" << endl;
		for (int i = 0; i < x0.size(); i++)
		{
			data << "[ ";
			for (int j = 0; j < x0.size(); j++)
			{
				data << M[k - 1][i][j] << " ";
			}
			if (i == x0.size() - 1)
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

double ZLinear(vector<double>& c, vector<double>& x)
{
	double z(0.0);
	for (int i = 0; i < x.size(); i++)
	{
		z += c[i] * x[i];
	}
	return z;
}

vector<double> Z(vector<vector<double> >& C, vector<double>& x)
{
	vector<double> vec;
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

void detectHl(double** S, int& n_S, int& m_S, vector<int>& outBases, vector<int>& Hl, int nbCon)
{
	for (int j = 0; j < outBases.size(); j++)
	{
		//int nbZero = 0;
		for (int i = nbCon + 1; i < m_S; i++)
		{
			if (S[i][outBases[j] - 1] < 0)
			{
				Hl.push_back(outBases[j] - 1);
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

void addEfficientCutSimplex(double** S, int& n_S, int& m_S, int& max_n_S, int& max_m_S, vector<int>& Hl, int nbCon, double** A, int& n_A, int& m_A, int& max_n_A, int& max_m_A, double* b, string* sign, vector<int>& outBases)
{
	if (n_S == max_n_S || m_S == max_m_S)
	{
		double **S_ = new double*[max_m_S];
		for (int i = 0; i < max_m_S; i++)
		{
			S_[i] = new double(max_n_S);
		}
		for (int i = 0; i < m_S; i++)
		{
			for (int j = 0; j < n_S; j++)
			{
				S_[i][j] = S[i][j];
			}
		}
		max_n_S *= 2;
		max_m_S *= 2;
		S = new double*[max_m_S];
		for (int i = 0; i < max_m_S; i++)
		{
			S[i] = new double(max_n_S);
		}
		for (int i = 0; i < m_S; i++)
		{
			for (int j = 0; j < n_S; j++)
			{
				S[i][j] = S_[i][j];
			}
		}
	}
	for (int i = 0; i < m_S; i++)
	{
		S[i][n_S] = S[i][n_S - 1];
		S[i][n_S - 1] = 0.0;
	}
	n_S++;
	for (int i = m_S; i > nbCon; i--)
	{
		for (int j = 0; j < n_S; j++)
		{
			S[i][j] = S[i - 1][j];
		}
	}
	for (int i = 0; i < n_S; i++)
	{
		S[nbCon][i] = 0.0;
	}
	for (int i = m_S; i > Hl.size(); i--)
	{
		S[nbCon][Hl[i]] = -1.0;
	}
	S[nbCon][n_S - 2] = 1.0;
	S[nbCon][n_S - 1] = -1.0;
	m_S++;

	if (max_m_A == m_A)
	{
		double **A_ = new double*[max_m_A];
		for (int i = 0; i < max_m_A; i++)
		{
			A_[i] = new double(max_n_A);
		}
		double *b_ = new double[m_A];
		string *sign_ = new string[m_A];
		for (int i = 0; i < m_A; i++)
		{
			for (int j = 0; j < n_A; j++)
			{
				A_[i][j] = A[i][j];
			}
			b_[i] = b[i];
			sign_[i] = sign[i];
		}		
		max_n_S *= 2;
		max_m_S *= 2;
		A = new double*[max_m_A];
		for (int i = 0; i < max_m_A; i++)
		{
			A[i] = new double(max_n_A);
		}
		b = new double[max_m_A];
		sign = new string[max_m_A];
		for (int i = 0; i < m_A; i++)
		{
			for (int j = 0; j < n_A; j++)
			{
				A[i][j] = A_[i][j];
			}
			b[i] = b_[i];
			sign[i] = sign_[i];
		}
	}
	vector<double> cut; double scdB = -1.0;
	for (int i = 0; i < n_A; i++)
	{
		cut.push_back(0.0);
	}
	for (int i = 0; i < Hl.size(); i++)
	{
		if (Hl[i] < n_A)
		{
			cut[Hl[i]]--;
		}
		else
		{
			for (int j = 0; j < n_A; j++)
			{
				cut[j] += A[Hl[i] - n_A][j];
			}
			scdB += b[Hl[i] - n_A];
		}
	}

	for (int i = 0; i < n_A; i++)
	{
		A[m_A][i] = cut[i];
	}
	b[m_A] = scdB;
	sign[m_A] = "<=";
	m_A++;
}

void addCutInfSimplex(double** S, int& n_S, int& m_S, int& max_n_S, int& max_m_S, vector<int>& bases, int nbCon, double** A, int& n_A, int& m_A, int& max_n_A, int& max_m_A, double* b, string* sign, int indVarFrac)
{
	if (n_S == max_n_S || m_S == max_m_S)
	{
		double **S_ = new double*[max_m_S];
		for (int i = 0; i < max_m_S; i++)
		{
			S_[i] = new double(max_n_S);
		}
		for (int i = 0; i < m_S; i++)
		{
			for (int j = 0; j < n_S; j++)
			{
				S_[i][j] = S[i][j];
			}
		}
		max_n_S *= 2;
		max_m_S *= 2;
		S = new double*[max_m_S];
		for (int i = 0; i < max_m_S; i++)
		{
			S[i] = new double(max_n_S);
		}
		for (int i = 0; i < m_S; i++)
		{
			for (int j = 0; j < n_S; j++)
			{
				S[i][j] = S_[i][j];
			}
		}
	}
	for (int i = 0; i < m_S; i++)
	{
		S[i][n_S] = S[i][n_S - 1];
		S[i][n_S - 1] = 0.0;
	}
	n_S++;
	for (int i = m_S; i > nbCon; i--)
	{
		for (int j = 0; j < n_S; j++)
		{
			S[i][j] = S[i - 1][j];
		}
	}
	int indVar;
	for (int i = 0; i < nbCon; i++)
	{
		if ((bases[i] - 1) == indVarFrac)
		{
			indVar = i;
			break;
		}
	}
	vector<double> inter(n_S, 0.0);
	for (int i = 0; i < (inter.size() - 1); i++)
	{
		if (indVarFrac != i)
		{
			inter[i] = -S[indVar][i];
		}
	}
	inter[n_S - 2] = 1.0;
	inter[n_S - 1] = floor(S[indVar][n_S - 1]) - S[indVar][n_S - 1];
	for (int i = 0; i < n_S; i++)
	{
		S[nbCon][i] = inter[i];
	}
	bases.push_back(n_S - 1);

	vector<double> cut(n_A, 0.0);
	cut[indVarFrac] = 1.0;
	for (int i = 0; i < n_A; i++)
	{
		A[m_A][i] = cut[i];
	}
	b[m_A] = floor(S[indVar][n_S - 1]);
	sign[m_A] = "<=";
	m_A++;
	//cout << "x" << indVarFrac << "<=" << b[b.size() - 1] << endl;
}

void addCutSupSimplex(double** S, int& n_S, int& m_S, int& max_n_S, int& max_m_S, vector<int>& bases, int nbCon, double** A, int& n_A, int& m_A, int& max_n_A, int& max_m_A, double* b, string* sign, int indVarFrac)
{
	if (n_S == max_n_S || m_S == max_m_S)
	{
		double **S_ = new double*[max_m_S];
		for (int i = 0; i < max_m_S; i++)
		{
			S_[i] = new double(max_n_S);
		}
		for (int i = 0; i < m_S; i++)
		{
			for (int j = 0; j < n_S; j++)
			{
				S_[i][j] = S[i][j];
			}
		}
		max_n_S *= 2;
		max_m_S *= 2;
		S = new double*[max_m_S];
		for (int i = 0; i < max_m_S; i++)
		{
			S[i] = new double(max_n_S);
		}
		for (int i = 0; i < m_S; i++)
		{
			for (int j = 0; j < n_S; j++)
			{
				S[i][j] = S_[i][j];
			}
		}
	}
	for (int i = 0; i < m_S; i++)
	{
		S[i][n_S] = S[i][n_S - 1];
		S[i][n_S - 1] = 0.0;
	}
	n_S++;
	for (int i = m_S; i > nbCon; i--)
	{
		for (int j = 0; j < n_S; j++)
		{
			S[i][j] = S[i - 1][j];
		}
	}
	int indVar;
	for (int i = 0; i < nbCon; i++)
	{
		if ((bases[i] - 1) == indVarFrac)
		{
			indVar = i;
			break;
		}
	}
	vector<double> inter(n_S, 0.0);
	for (int i = 0; i < (inter.size() - 1); i++)
	{
		if (indVarFrac != i)
		{
			inter[i] = S[indVar][i];
		}
	}
	inter[n_S - 2] = 1.0;
	inter[n_S - 1] = S[indVar][n_S - 1] - ceil(S[indVar][n_S - 1]);
	for (int i = 0; i < n_S; i++)
	{
		S[nbCon][i] = inter[i];
	}
	bases.push_back(n_S - 1);

	/*cout << "contrainte: ";
	for (int i = 0; i < S[0].size(); i++)
	{
	cout << S[nbCon][i] << " ";
	}
	cout << endl;*/

	vector<double> cut(n_A, 0.0);
	cut[indVarFrac] = -1.0;
	for (int i = 0; i < n_A; i++)
	{
		A[m_A][i] = cut[i];
	}
	b[m_A] = -ceil(S[indVar][n_S - 1]);
	sign[m_A] = "<=";
	m_A++;
	//cout << "x" << indVarFrac << ">=" << (-b[b.size() - 1]) << endl;
}

bool initialPhase(double** A, int& max_n_A, int& max_m_A, int& n_A, int& m_A, double* b, string* sign, vector<vector<double> >& C, vector<double>& x, vector<vector<double> >& SND, vector<vector<double> >& Eff)
{
	//cout << "Initial Phase" << endl;
	int status(0);
	unsigned int level(0);
	//allLevels << 0 << endl;
	IloEnv env;
	//cout << "obj = " << endl;
	//afficher(obj);
	//vector<vector<double> > L = buildLamkeMatrix(A, b, M, c);
	double** S = buildSimplexMatrix(A, n_A, m_A, b, sign, C);
	// On suppose que tous les contraintes sont "<="
	int n_S = n_A + m_A + 1, m_S = m_A + C.size(), max_n_S = m_A * 2, max_m_S = (n_A + m_A) * 2;
	//cout << "S=" << endl;
	//afficher(S);
	vector<int> bases, xBases;
	for (int i = 1; i <= m_A; i++)
	{
		bases.push_back(n_A + i);
	}
	if (simplex(S, n_S, m_S, m_A, n_A, C.size(), bases, x))
	{
		int indNoTInt;
		//cout << "S after =" << endl;
		//afficher(S);
		if (entiere(x, indNoTInt))
		{
			// UpdateSND & Eff
			Eff.push_back(x);
			SND.push_back(Z(C, x));
			// Add efficiency cut if it exists
			/*vector<vector<double> > A_(A);
			vector<double> b_(b);
			vector<string> sign_(sign);*/
			vector<int> outBases, Hl;
			detectOutBasesSimplex(bases, outBases, m_A, x.size());
			//cout << "out bases = ";
			//afficher(outBases);
			detectHl(S, n_S, m_S, outBases, Hl, m_A);
			int_sol++;
			//cout << "Hl = "; 
			//afficher(Hl);
			if (Hl.empty())
			{
				return false;
			}
			else
			{
				cout << "init Phase" << endl;
				/*vector<vector<double> > A_2(A);
				vector<double> b_2(b);
				vector<string> sign_2(sign);*/
				int nbCon = m_A + 1, nbVar = n_A, nbObj = C.size();
				addEfficientCutSimplex(S, n_S, m_S, max_n_S, max_m_S, Hl, m_A, A, n_A, m_A, max_n_A, max_m_A, b, sign, outBases);
				bases.push_back(n_S - 1);
				// Create root
				//NoeudLemke noeud(0, A_, b_, sign_, xBases, false);
				//tree.push(NoeudLemke(noeud));
				NoeudQuadratic noeud(num_noeud, level, S, A, b, sign, bases, n_S, m_S, max_n_S, max_m_S, n_A, m_A, max_n_A, max_m_A, nbObj);

				tree.push(NoeudQuadratic(noeud));
				num_noeud++;
			}
		}
		else // Not integer solution
		{
			// Add cut inf
			double** A1(A);
			double* b1(b);
			string* sign1(sign);
			double** S1(S);
			double** S2(S);
			int n_S1 = n_S, m_S1 = m_S, max_n_S1 = max_n_S, max_m_S1 = max_m_S;
			int n_A1 = n_A, m_A1 = m_A, max_n_A1 = max_n_A, max_m_A1 = max_m_A;
			vector<int> bases1(bases), bases2(bases);
			addCutInfSimplex(S1, n_S1, m_S1, max_n_S1, max_m_S1, bases1, m_A1, A1, n_A1, m_A1, max_n_A1, max_m_A1, b1, sign1, indNoTInt);
			// Create root
			int nbObj = C.size();
			NoeudQuadratic noeud(num_noeud, level, S1, A1, b1, sign1, bases1, n_S1, m_S1, max_n_S1, max_m_S1, n_A1, m_A1, max_n_A1, max_m_A1, nbObj);
			//tree.push(NoeudQuadratic(noeud));
			num_noeud++;
			// Add cut sup
			double** A2(A);
			double* b2(b);
			string* sign2(sign);
			int n_S2 = n_S, m_S2 = m_S, max_n_S2 = max_n_S, max_m_S2 = max_m_S;
			int n_A2 = n_A, m_A2 = m_A, max_n_A2 = max_n_A, max_m_A2 = max_m_A;
			addCutSupSimplex(S2, n_S2, m_S2, max_n_S2, max_m_S2, bases2, m_A2, A2, n_A2, m_A2, max_n_A2, max_m_A2, b2, sign2, indNoTInt);
			// Create root
			nbObj = C.size();
			NoeudQuadratic noeud2(num_noeud, level, S1, A1, b1, sign1, bases1, n_S1, m_S1, max_n_S1, max_m_S1, n_A1, m_A1, max_n_A1, max_m_A1, nbObj);
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

bool dualSimplex(double** S, int& n_S, int& m_S, int nbCon, int nbVar, int nbObjL, vector<int>& bases, vector<double>& x)
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
			for (int i = 0; i < n_S - 1; i++)
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
				pivoting(S, n_S, m_S, indRow, indCol);
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
			for (int i = 0; i < (n_S - 1); i++)
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
			//cout << "Solution du dual = ";
			//afficher(x);
			//cout << "CLICK TO CONTINUE ... " << endl;
			////cin >> c;
			return true;
		}
	}
	//cout << "DUAL END" << endl;
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

void generalPhase(double** A, int& max_n_A, int& max_m_A, int& n_A, int& m_A, double* b, string* sign, vector<vector<double> >& C, vector<double>& x, vector<vector<double> >& SND, vector<vector<double> >& Eff)
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
		//cout << "num_noeud : " << num_noeud << endl;
		//cout << "CLICK TO CONTINUE ... " << endl;
		//cin >> c;
		vector<int> bases(tree.top().getBase()), xBases;
		double** S(tree.top().getS());
		double** A2(tree.top().getA());
		double* b2(tree.top().getB());
		string* sign2(tree.top().getSign());
		int n_S(tree.top().getn_S()), m_S(tree.top().getm_S()), max_m_S(tree.top().getmax_m_S()), max_n_S(tree.top().getmax_n_S());
		int n_A(tree.top().getn_A()), m_A(tree.top().getm_A()), max_m_A(tree.top().getmax_m_A()), max_n_A(tree.top().getmax_n_A());
		//allLevels << tree.top().getLevel() << endl;
		//cout << "S before dual =" << endl;
		//afficher(S);
		//cout << "dual" << endl;
		if (dualSimplex(S, m_S, n_S, m_A, n_A, C.size(), bases, x_))
		{
			//detectBase(L, bases, xBases, x, tree.top().getA());
			//extractSolution(tree.top().getS(), tree.top().getBase(), tree.top().getNbCon(), tree.top().getNbVar(), x);
			//cout << "x = " << endl;
			//afficher(x_);
			//cout << "S after dual =" << endl;
			//afficher(S);
			//cout << "actual f = " << z << endl;
			if (SND.empty())
			{
				int indNoTInt;
				if (entiere(x_, indNoTInt))
				{
					x = x_;
					SND.push_back(Z(C, x));
					Eff.push_back(x);
					//cout << "Solution non efficace" << endl;
					//cout << "x = " << endl;
					//afficher(x);
					//cout << " z = " << z << endl;
					// Add efficiency cut
					double** A_(A2);
					double* b_(b2);
					string* sign_(sign2);
					int n_S1 = n_S, m_S1 = m_S, max_n_S1 = max_n_S, max_m_S1 = max_m_S;
					int n_A1 = n_A, m_A1 = m_A, max_n_A1 = max_n_A, max_m_A1 = max_m_A;
					vector<int> outBases, Hl;
					//detectOutBase(bases, outBases, x.size(), A.size());
					detectOutBasesSimplex(bases, outBases, m_A, n_A);
					//addEfficientCut(A_, b_, sign_, outBases);
					detectHl(S, n_S, m_S, outBases, Hl, m_A);
					int_sol++;
					//cout << "Hl : ";
					//afficher(Hl);
					if (Hl.empty())
					{
						tree.pop();
						hl_delete++;
					}
					else
					{
						//cout << "general Phase Epsilon 1" << endl;
						//cout << "status = " << status << endl;
						//cout << "epsilon = " << epsilon << endl;
						double** A_2(A2);
						double* b_2(b2);
						string* sign_2(sign2);
						int n_S1_ = n_S1, m_S1_ = m_S1, max_n_S1_ = max_n_S1, max_m_S1_ = max_m_S1;
						int n_A1_ = n_A1, m_A1_ = m_A1, max_n_A1_ = max_n_A1, max_m_A1_ = max_m_A1;
						addEfficientCutSimplex(S, n_S1_, m_S1_, max_n_S1_, max_m_S1_, Hl, m_A1_, A_2, n_A1_, m_A1_, max_n_A1_, max_m_A1_, b_2, sign_2, outBases);
						bases.push_back(n_S - 1);
						// Create root
						NoeudQuadratic noeud(num_noeud, tree.top().getLevel(), S, A_2, b_2, sign_2, bases, n_S, m_S, max_n_S, max_m_S, n_A1_, m_A1_, max_n_A1_, max_m_A1_, tree.top().getNbObj());
						tree.pop();
						tree.push(NoeudQuadratic(noeud));
						num_noeud++;
					}
					//cout << "CLICK TO CONTINUE ... " << endl;
					//cin >> c;

				}
				else // Not integer solution
				{
					// Delete actual node
					tree.pop();
					// Add cut inf
					double** A1(A2);
					double* b1(b2);
					string* sign1(sign2);
					double** S1(S);
					double** S2(S);
					int n_S1 = n_S, m_S1 = m_S, max_n_S1 = max_n_S, max_m_S1 = max_m_S;
					int n_A1 = n_A, m_A1 = m_A, max_n_A1 = max_n_A, max_m_A1 = max_m_A;
					vector<int> bases1(bases), bases2(bases);
					addCutInfSimplex(S1, n_S1, m_S1, max_n_S1, max_m_S1, bases1, m_A1, A1, n_A1, m_A1, max_n_A1, max_m_A1, b1, sign1, indNoTInt);
					// Create root
					NoeudQuadratic noeud(num_noeud, tree.top().getLevel(), S1, A1, b1, sign1, bases1, n_S1, m_S1, max_n_S1, max_m_S1, n_A1, m_A1, max_n_A1, max_m_A1, tree.top().getNbObj());
					//tree.push(NoeudQuadratic(noeud));
					num_noeud++;
					// Add cut sup
					double** A3(A2);
					double* b3(b2);
					string* sign3(sign2);
					int n_S2 = n_S, m_S2 = m_S, max_n_S2 = max_n_S, max_m_S2 = max_m_S;
					int n_A2 = n_A, m_A2 = m_A, max_n_A2 = max_n_A, max_m_A2 = max_m_A;
					addCutSupSimplex(S2, n_S2, m_S2, max_n_S2, max_m_S2, bases2, m_A2, A3, n_A2, m_A2, max_n_A2, max_m_A2, b3, sign3, indNoTInt);
					// Create root
					NoeudQuadratic noeud2(num_noeud, tree.top().getLevel(), S2, A3, b3, sign3, bases2, n_S2, m_S2, max_n_S2, max_m_S2, n_A2, m_A2, max_n_A2, max_m_A2, tree.top().getNbObj());
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
				int indNoTInt;
				//cout << "x = ";
				//afficher(x_);
				if (entiere(x_, indNoTInt))
				{
					x = x_;
					vector<double> z(Z(C, x));
					updateSND(Eff, SND, z, x);
					//cout << "Solution non efficace" << endl;
					//cout << "x = " << endl;
					//afficher(x_);
					//cout << " z = " << z_ << endl;
					// Add efficiency cut
					double **A_(A2);
					double *b_(b2);
					string *sign_(sign2);
					int n_S1 = n_S, m_S1 = m_S, max_n_S1 = max_n_S, max_m_S1 = max_m_S;
					int n_A1 = n_A, m_A1 = m_A, max_n_A1 = max_n_A, max_m_A1 = max_m_A;
					vector<int> outBases, Hl;
					detectOutBasesSimplex(bases, outBases, m_A, n_A);
					//cout << "OutBases : ";
					//afficher(outBases);
					//addEfficientCut(A_, b_, sign_, outBases);
					detectHl(S, n_S, m_S, outBases, Hl, m_A);
					int_sol++;
					//cout << "Hl : ";
					//afficher(Hl);
					if (Hl.empty())
					{
						//tree.top().~NoeudQuadratic();
						tree.pop();
						hl_delete++;
					}
					else
					{
						//cout << "general Phase Epsilon 2" << endl;
						//cout << "status = " << status << endl;
						//cout << "epsilon = " << epsilon << endl;
						double **A_2(A2);
						double *b_2(b2);
						string *sign_2(sign2);
						int n_S1_ = n_S1, m_S1_ = m_S1, max_n_S1_ = max_n_S1, max_m_S1_ = max_m_S1;
						int n_A1_ = n_A1, m_A1_ = m_A1, max_n_A1_ = max_n_A1, max_m_A1_ = max_m_A1;
						addEfficientCutSimplex(S, n_S1_, m_S1_, max_n_S1_, max_m_S1_, Hl, m_A1_, A_2, n_A1_, m_A1_, max_n_A1_, max_m_A1_, b_2, sign_2, outBases);
						bases.push_back(n_S - 1);
						// Create root
						NoeudQuadratic noeud(num_noeud, tree.top().getLevel(), S, A_2, b_2, sign_2, bases, n_S, m_S, max_n_S, max_m_S, n_A1_, m_A1_, max_n_A1_, max_m_A1_, tree.top().getNbObj());
						tree.pop();
						tree.push(NoeudQuadratic(noeud));
						num_noeud++;
					}
				}
				else // Not integer solution
				{
					//cout << "NO ENTIRE SOLUTION" << endl;
					// Delete actual node
					tree.pop();
					// Add cut inf
					double **A1(A2);
					double *b1(b2);
					string *sign1(sign2);
					//addCutInf(A1, b1, sign1, x, indNoTInt);
					double **S1(S);
					double **S2(S);
					int n_S1 = n_S, m_S1 = m_S, max_n_S1 = max_n_S, max_m_S1 = max_m_S;
					int n_A1 = n_A, m_A1 = m_A, max_n_A1 = max_n_A, max_m_A1 = max_m_A;
					vector<int> bases1(bases), bases2(bases);
					addCutInfSimplex(S1, n_S1, m_S1, max_n_S1, max_m_S1, bases1, m_A1, A1, n_A1, m_A1, max_n_A1, max_m_A1, b1, sign1, indNoTInt);
					//cout << "S after adding inf cut" << endl;
					//afficher(S1);
					// Create root
					NoeudQuadratic noeud(num_noeud, tree.top().getLevel(), S1, A1, b1, sign1, bases1, n_S1, m_S1, max_n_S1, max_m_S1, n_A1, m_A1, max_n_A1, max_m_A1, tree.top().getNbObj());
					num_noeud++;
					// Add cut sup
					double **A3(A2);
					double *b3(b2);
					string *sign3(sign2);
					int n_S2 = n_S, m_S2 = m_S, max_n_S2 = max_n_S, max_m_S2 = max_m_S;
					int n_A2 = n_A, m_A2 = m_A, max_n_A2 = max_n_A, max_m_A2 = max_m_A;
					//addCutSup(A2, b2, sign2, x, indNoTInt);
					addCutSupSimplex(S2, n_S2, m_S2, max_n_S2, max_m_S2, bases2, m_A2, A3, n_A2, m_A2, max_n_A2, max_m_A2, b3, sign3, indNoTInt);
					//cout << "S after adding inf cut" << endl;
					//afficher(S2);
					// Create root
					NoeudQuadratic noeud2(num_noeud, tree.top().getLevel(), S2, A3, b3, sign3, bases2, n_S2, m_S2, max_n_S2, max_m_S2, n_A2, m_A2, max_n_A2, max_m_A2, tree.top().getNbObj());
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
		}
		else // No solution for this node
		{
			tree.pop();
			empty_delete++;
		}
		//cout << "CLICK TO CONTINUE ... " << endl;
		//cin >> c;
	}
}

void randomData(vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<double> >& C)
{
	srand(time(0));
	cout << "Donner le nombre de variables : " << endl;
	int n, m, p, maxC, maxA, maxb, minb;
	double r;
	cin >> n;
	output3 << n << endl;
	cout << "Donner le nombre de contraintes : " << endl;
	cin >> m;
	output3 << m << endl;
	cout << "Donner le nombre d'objectifs : " << endl;
	cin >> p;
	output3 << p << endl;
	cout << "Donner la valeur max des coefficients des fonctions objectifs : " << endl;
	cin >> maxC;
	cout << "Donner la valeur max des coefficients de la matrie A : " << endl;
	cin >> maxA;
	cout << "Donner le ratio (entre 0 et 1) correspondant aux valeurs max du becteur b par rapport à la somme des éléments des lignes de la matrice A : " << endl;
	cin >> r;
	output2 << "C = \n[";
	for (int i = 0; i < p; i++)
	{
		vector<double> c;
		for (int j = 0; j < n; j++)
		{
			c.push_back((rand() % maxC)*((rand() % 2) == 0 ? 1 : -1));
			output2 << c[j] << " ";
		}
		C.push_back(c);
		if (i == (p - 1))
		{
			output2 << "]\n";
		}
		else
		{
			output2 << "; ";
		}
	}
	output2 << "A = \n[";
	for (int i = 0; i < m; i++)
	{
		vector<double> c;
		double somme = 0;
		for (int j = 0; j < n; j++)
		{
			c.push_back(rand() % maxA);
			somme += c[j];
			output2 << c[j] << " ";
		}
		A.push_back(c);
		maxb = ceil(somme * r);
		minb = ceil(somme * r / 2);
		b.push_back((rand() % (maxb - minb + 1)) + minb);
		sign.push_back("<");
		if (i == (m - 1))
		{
			output2 << "]\n";
		}
		else
		{
			output2 << "; ";
		}
	}
	output2 << "b = \n[";
	for (int i = 0; i < m; i++)
	{
		if (i == (m - 1))
		{
			output2 << b[i] << "]\n";
		}
		else
		{
			output2 << b[i] << ";";
		}
	}
	for (int i = 0; i < m; i++)
	{
		output2 << sign[i];
	}
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			output3 << A[i][j] << " ";
		}
		output3 << sign[i] << " " << b[i] << endl;
	}
	for (int i = 0; i < p; i++)
	{
		for (int j = 0; j < n; j++)
		{
			output3 << C[i][j] << " ";
		}
		output3 << endl;
	}
	output2.close();
	output3.close();
}

int main()
{
	vector<vector<double> > A, C;
	vector<vector<double> > SND, Eff;
	vector<int> H, N;
	vector<double> b, x;
	vector<string> sign;
	clock_t beg, ed, tm;
	output.open("output.txt", ofstream::trunc);
	output2.open("data.txt", ofstream::trunc);
	//output3.open("input.txt", ofstream::trunc);
	//levels.open("levels.csv", ofstream::trunc);
	//allLevels.open("allLevels.txt", ofstream::trunc);
	//cout << numeric_limits<double>::epsilon()/2 << endl;
	//cout << numeric_limits<double>::lowest() << endl;
	//cout << numeric_limits<double>::min() << endl;
	beg = clock();
	readData(A, b, sign, C);
	//randomData(A, b, sign, C);

	double **A_ = new double*[A.size()*2];
	double *b_ = new double[A.size() * 2];
	string *sign_ = new string[A.size() * 2];
	for (int i = 0; i < A.size() * 2; i++)
	{
		A_[i] = new double[A[0].size() * 2];
	}
	for (int i = 0; i < A.size(); i++)
	{
		for (int j = 0; j < A[0].size(); j++)
		{
			A_[i][j] = A[i][j];
		}
		b_[i] = b[i];
		sign_[i] = sign[i];
	}
	int n_A = A[0].size(), m_A = A.size(), max_n_A = n_A*2, max_m_A = m_A*2;
	if (initialPhase(A_, max_n_A, max_m_A, n_A, m_A, b_, sign_, C, x, SND, Eff))
	{
		generalPhase(A_, max_n_A, max_m_A, n_A, m_A, b_, sign_, C, x, SND, Eff);
	}
	ed = clock();
	tm = ed - beg;

	cout << "Ensemble des solutions efficaces :";
	afficher(Eff);
	cout << "Ensemble des solutions non-dominées :";
	afficher(SND);

	cout << "Execution time : " << tm / 1000.0 << " sec" << endl;
	cout << "File time : " << fileTime / 1000.0 << " sec" << endl;
	char h;

	cout << "Number of nodes : " << num_noeud << endl;
	cout << "Number of nodes deleted because efficiency : \t\t\t" << eff_delete << endl;
	cout << "Number of nodes deleted because Hl empty : \t\t\t" << hl_delete << endl;
	cout << "Number of nodes deleted because dominanced sub region : \t" << dom_delete << endl;
	cout << "Number of nodes deleted because cost : \t\t\t\t" << cost_delete << endl;
	cout << "Number of nodes deleted because sub region empty : \t\t" << empty_delete << endl;
	cout << "Number of integer solutions found : \t\t\t\t" << int_sol << endl;
	cout << "Number of integer efficient solutions found : \t\t\t" << int_eff_sol << endl;
	cout << "Number of positive Epsilon : \t\t\t\t\t" << int_epsilon_positive << endl;
	cout << "Number of null Epsilon : \t\t\t\t\t" << int_epsilon_null << endl;
	cout << "Number of abs Epsilon : \t\t\t\t\t" << int_epsilon_abs << endl;
	cout << "Number of error Epsilon : \t\t\t\t\t" << int_epsilon_error << endl;
	cout << "ENTRER UN CARACTERE ET CLIQUER ENTRER POUR SORTIR ... " << endl;
	cin >> h;
	output.close();
	//levels.close();
	//allLevels.close();
	return 0;
}