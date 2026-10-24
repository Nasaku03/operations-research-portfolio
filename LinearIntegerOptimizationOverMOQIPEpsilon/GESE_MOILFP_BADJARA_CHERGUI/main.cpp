// =====================================================================================
// GESE-MOILFP : g�n�ration de l'ensemble efficace d'un MOILFP via le co�t r�duit
// fractionnaire gamma_j^k = dhat_k * ahat_j^k - bhat_k * chat_j^k (Section MOILFP du
// papier GESE-MOIP), SANS transformation de Charnes-Cooper sur l'algorithme principal.
//
// Contrairement � main_MOILFP_CC.cpp, on travaille directement en x du d�but � la fin :
// branchement, coupes, programme (3)/(4) sont RIGOUREUSEMENT IDENTIQUES au cas MOILP.
// Charnes-Cooper n'est utilis� que ponctuellement, dans approxIdealFrac(), pour la
// relaxation CONTINUE du point id�al local (aucune int�grit� � pr�server � cet endroit,
// donc aucune des r�serves th�oriques de la version CC compl�te ne s'applique ici).
//
// Les fonctions marqu�es "REPRIS TEL QUEL" sont copi�es sans AUCUNE modification depuis
// main.cpp.
// =====================================================================================

#include <ilopl/iloopl.h>

#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <map>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <string>
#include <limits>
#include <queue>
#include <utility>

#include "NoeudLemke.h"
#include "NoeudQuadratic.h"
#include "CMatrix.h"

using namespace std;

class mycomparison2
{
	bool reverse;
public:
	mycomparison2(const bool& revparam = false) { reverse = revparam; }
	bool operator() (const NoeudQuadratic& lhs, const NoeudQuadratic& rhs) const
	{
		if (reverse) return (lhs.getNum() >= rhs.getNum());
		else return (lhs.getNum() < rhs.getNum());
	}
};

ofstream output, output2;
unsigned int num_noeud = 0, dom_delete = 0, hl_delete = 0, empty_delete = 0, nadir_delete = 0,
int_sol = 0, int_eff_sol = 0,
int_epsilon_positive = 0, int_epsilon_null = 0, int_epsilon_abs = 0, int_epsilon_error = 0;
priority_queue<NoeudQuadratic, vector<NoeudQuadratic>, mycomparison2> tree;
clock_t idealTime(0), fileTime(0), epsilonTime(0);
vector<double> epsilonsPositive;
vector<vector<double> > SND;      // valeurs des VRAIS ratios f_k(x), pas des lignes de C
set<vector<double> > Eff;

// nombre d'objectifs fractionnaires -- global pour simplifier les signatures
int P_OBJ = 0;

// =====================================================================================
// Affichage --- REPRIS TEL QUEL
// =====================================================================================
void afficher(vector<vector<double> > mat)
{
	cout << '[' << endl;
	for (unsigned int i = 0; i < mat.size(); i++)
	{
		cout << '[';
		for (unsigned int j = 0; j < mat[0].size(); j++) cout << mat[i][j] << ' ';
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
		for (unsigned int j = 0; j < (*i).size(); j++) cout << (*i)[j] << ' ';
		cout << "]\n";
	}
	cout << ']' << endl;
}
void afficher(vector<double> vec)
{
	cout << '[';
	for (unsigned int j = 0; j < vec.size(); j++) cout << vec[j] << ' ';
	cout << ']' << endl;
}

// =====================================================================================
// Lecture des donn�es MOILFP (format d�taill� dans le README)
//   n, m, p
//   m lignes  : A[i][1..n]  sign  b[i]
//   1 ligne   : trajectoire w[1..n]
//   p blocs   : a_k[1..n] b_k   puis   c_k[1..n] d_k
// =====================================================================================
void readDataFrac(vector<vector<double> >& A, vector<double>& b, vector<string>& sign,
	vector<double>& w,
	vector<vector<double> >& NUM, vector<double>& NUMc,
	vector<vector<double> >& DEN, vector<double>& DENc)
{
	unsigned int n, m, p; char c;
	ifstream fichier("inputMOILFP_gamma.txt");
	if (!fichier) { cout << "inputMOILFP_gamma.txt introuvable" << endl; return; }

	double mot;
	fichier >> n; fichier >> m; fichier >> p;
	P_OBJ = (int)p;

	for (unsigned int i = 0; i < m; i++)
	{
		vector<double> inter;
		for (unsigned int j = 0; j < n; j++) { fichier >> mot; inter.push_back(mot); }
		A.push_back(inter);
		fichier >> c; fichier >> mot;
		b.push_back(mot);
		sign.push_back((c == '<' ? "<=" : ">="));
	}

	for (unsigned int j = 0; j < n; j++) { fichier >> mot; w.push_back(mot); }

	for (unsigned int k = 0; k < p; k++)
	{
		vector<double> a; for (unsigned int j = 0; j < n; j++) { fichier >> mot; a.push_back(mot); }
		NUM.push_back(a); fichier >> mot; NUMc.push_back(mot);

		vector<double> cc; for (unsigned int j = 0; j < n; j++) { fichier >> mot; cc.push_back(mot); }
		DEN.push_back(cc); fichier >> mot; DENc.push_back(mot);
	}
}

// =====================================================================================
// Construction de C : ligne 0 = trajectoire ; puis, pour k=1..p, DEUX lignes
// (numerateur a_k, denominateur c_k). Les constantes b_k,d_k ne figurent PAS dans le
// tableau -- elles sont evaluees a la volee dans detectHlFrac()/fracObjectives().
// =====================================================================================
void buildCFrac(vector<double>& w, vector<vector<double> >& NUM, vector<vector<double> >& DEN,
	vector<vector<double> >& C)
{
	C.push_back(w);
	for (unsigned int k = 0; k < NUM.size(); k++)
	{
		C.push_back(NUM[k]);
		C.push_back(DEN[k]);
	}
}

// =====================================================================================
// Simplexe / tableau --- REPRIS TEL QUEL (identique � main.cpp)
// =====================================================================================
vector<vector<double> > buildSimplexMatrix(vector<vector<double> >& A, vector<double>& b, vector<string>& sign, vector<vector<double> >& C)
{
	vector<vector<double> > S;
	for (unsigned int i = 0; i < A.size(); i++) S.push_back(A[i]);
	for (unsigned int i = 0; i < A.size(); i++)
	{
		for (unsigned int j = 0; j < A.size(); j++) S[i].push_back(i == j ? 1.0 : 0.0);
		S[i].push_back(b[i]);
	}
	for (unsigned int i = 0; i < C.size(); i++) S.push_back(C[i]);
	for (unsigned int i = 0; i < C.size(); i++)
		for (unsigned int j = 0; j <= A.size(); j++) S[A.size() + i].push_back(0.0);
	return S;
}

void pivoting(vector<vector<double> >& M, int const& line, int const& column)
{
	double pivot = M[line][column];
	for (unsigned int j = 0; j < M[line].size(); j++)
	{
		if (abs((M[line][j] / pivot) - round(M[line][j] / pivot)) > (numeric_limits<double>::epsilon() * 10000000))
			M[line][j] /= pivot;
		else
			M[line][j] = round(M[line][j] / pivot);
	}
	for (unsigned int i = 0; i < M.size(); i++)
	{
		if ((int)i != line)
		{
			double alpha = M[i][column];
			for (unsigned int j = 0; j < M[i].size(); j++)
			{
				if (abs((M[i][j] - M[line][j] * alpha) - round(M[i][j] - M[line][j] * alpha)) > (numeric_limits<double>::epsilon() * 10000000))
					M[i][j] -= M[line][j] * alpha;
				else
					M[i][j] = round(M[i][j] - M[line][j] * alpha);
			}
		}
	}
}

void initialSolution(vector<double>& x, int nbVar)
{
	x.clear();
	for (int i = 0; i < nbVar; i++) x.push_back(0.0);
}

void extractSolution(vector<vector<double> >& S, vector<int>& bases, int nbCon, int nbVar, vector<double>& x)
{
	initialSolution(x, nbVar);
	for (unsigned int i = 0; i < bases.size(); i++)
		if (bases[i] <= nbVar) x[bases[i] - 1] = S[i][nbCon + nbVar];
}

bool simplex(vector<vector<double> >& S, int nbCon, int nbVar, int nbObjL, vector<int>& bases, vector<double>& x)
{
	while (true)
	{
		double minCol = numeric_limits<double>::infinity(), minRow = numeric_limits<double>::infinity();
		int indCol = -1, indRow = -1;
		for (unsigned int i = 0; i < S[0].size(); i++)
			if (((int)i != (int)(S[0].size() - 1)) && (S[nbCon][i] < 0) && (S[nbCon][i] < minCol))
			{
				minCol = S[nbCon][i]; indCol = i;
			}
		if (indCol != -1)
		{
			for (int i = 0; i < nbCon; i++)
				if ((S[i][indCol] > 0) && ((S[i][nbCon + nbVar] / S[i][indCol]) < minRow))
				{
					minRow = S[i][nbCon + nbVar] / S[i][indCol]; indRow = i;
				}
			if (indRow != -1) { pivoting(S, indRow, indCol); bases[indRow] = indCol + 1; }
			else return false;
		}
		else
		{
			for (int i = 0; i < nbCon; i++) if (S[i][nbCon + nbVar] < 0) return false;
			extractSolution(S, bases, nbCon, nbVar, x);
			return true;
		}
	}
}

bool dualSimplex(vector<vector<double> >& S, int nbCon, int nbVar, int nbObjL, vector<int>& bases, vector<double>& x)
{
	vector<pair<pair<int, int>, double> > memory;
	while (true)
	{
		double maxCol = -numeric_limits<double>::infinity(); int indCol = -1, indRow = -1;
		vector<int> listRows;
		for (int i = 0; i < nbCon; i++) if (S[i][nbCon + nbVar] < 0) listRows.push_back(i);
		if (!listRows.empty())
		{
			indRow = listRows[0];
			for (unsigned int i = 0; i < S[0].size() - 1; i++)
			{
				pair<int, int> in(indRow, i); pair<pair<int, int>, double> out(in, S[indRow][i]);
				if ((S[indRow][i] < 0) && (find(bases.begin(), bases.end(), i + 1) == bases.end())
					&& (maxCol <= (S[nbCon][i] / S[indRow][i])) && (find(memory.begin(), memory.end(), out) == memory.end()))
				{
					maxCol = S[nbCon][i] / S[indRow][i]; indCol = i; memory.push_back(out);
				}
				else if ((S[indRow][i] < 0) && (find(bases.begin(), bases.end(), i + 1) == bases.end())
					&& (maxCol < (S[nbCon][i] / S[indRow][i])) && (find(memory.begin(), memory.end(), out) != memory.end()))
				{
					maxCol = S[nbCon][i] / S[indRow][i];
				}
			}
			if (indCol != -1) { pivoting(S, indRow, indCol); bases[indRow] = indCol + 1; }
			else return false;
		}
		else
		{
			for (unsigned int i = 0; i < S[0].size() - 1; i++) if (S[nbCon][i] < 0) return false;
			extractSolution(S, bases, nbCon, nbVar, x);
			return true;
		}
	}
}

void detectOutBasesSimplex(vector<int>& bases, vector<int>& outBases, int nbCon, int nbVar)
{
	outBases.clear();
	for (int i = 1; i <= (nbCon + nbVar); i++)
		if (find(bases.begin(), bases.end(), i) == bases.end()) outBases.push_back(i);
}

double ZLinear(vector<double>& c, vector<double>& x) { double z(0.0); for (unsigned int i = 0; i < x.size(); i++) z += c[i] * x[i]; return z; }
bool domine(vector<double> v1, vector<double> v2) { for (unsigned int i = 0; i < v1.size(); i++) if (v1[i] > v2[i]) return false; return true; }
bool exist(set<vector<double> >& xEff, vector<double>& x) { return xEff.find(x) != xEff.end(); }

// entiere : REPRIS TEL QUEL -- x est manipul� directement, pas de x=y/t a recuperer.
bool entiere(vector<double>& x, int& ind)
{
	for (unsigned int i = 0; i < x.size(); i++)
		if (abs(x[i] - round(x[i])) > 1e-6) { ind = i; return false; }
	return true;
}

// =====================================================================================
// fracObjectives : evalue les VRAIS ratios f_k(x) = (a_k^T x + b_k)/(c_k^T x + d_k)
// --- NOUVEAU (remplace Z() pour le reporting dans SND)
// =====================================================================================
vector<double> fracObjectives(vector<vector<double> >& NUM, vector<double>& NUMc,
	vector<vector<double> >& DEN, vector<double>& DENc, vector<double>& x)
{
	vector<double> f;
	for (unsigned int k = 0; k < NUM.size(); k++)
	{
		double num = ZLinear(NUM[k], x) + NUMc[k];
		double den = ZLinear(DEN[k], x) + DENc[k];
		f.push_back(num / den);
	}
	return f;
}

// =====================================================================================
// detectHlFrac : remplace detectHl(). CORRIGE par rapport a la premiere version :
// implemente maintenant la definition COMPLETE du papier,
//   Hl = {j : il existe k, gamma_j^k<0} UNION {j : gamma_j^k=0 pour tout k}
// La seconde branche etait absente de la version precedente (et l'est deja dans
// detectHl() du MOILP original) ; elle est explicitement confirmee necessaire par
// Ait Mehdi, Chergui & Abbas (2014), "An Improved Method for Solving MOILFP" (leur H_l
// utilise exactement la meme union, avec gamma^i = beta_i*c^i - alpha_i*d^i -- la meme
// formule que gamma_j^k ici, a une correspondance de notation pres).
// =====================================================================================
void detectHlFrac(vector<vector<double> >& S, vector<int>& outBases, vector<int>& Hl, int nbCon,
	vector<vector<double> >& NUM, vector<double>& NUMc,
	vector<vector<double> >& DEN, vector<double>& DENc, vector<double>& x)
{
	int p = (int)NUM.size();
	vector<double> bhat(p), dhat(p);
	for (int k = 0; k < p; k++)
	{
		bhat[k] = ZLinear(NUM[k], x) + NUMc[k];
		dhat[k] = ZLinear(DEN[k], x) + DENc[k];
	}

	for (unsigned int j = 0; j < outBases.size(); j++)
	{
		bool existsNegative = false;
		bool allZero = true;
		for (int k = 0; k < p; k++)
		{
			int rowA = nbCon + 1 + 2 * k;   // ligne numerateur de l'objectif k (index dans S)
			int rowC = nbCon + 2 + 2 * k;   // ligne denominateur de l'objectif k
			double ahat = S[rowA][outBases[j] - 1];
			double chat = S[rowC][outBases[j] - 1];
			double gamma = dhat[k] * ahat - bhat[k] * chat;
			if (gamma < -1e-9) existsNegative = true;
			if (abs(gamma) > 1e-9) allZero = false;
		}
		if (existsNegative || allZero) Hl.push_back(outBases[j] - 1);
	}
}

// =====================================================================================
// Branchement --- REPRIS TEL QUEL (identique � main.cpp, travaille directement sur x)
// =====================================================================================
void addCutInfSimplex(vector<vector<double> >& S, vector<int>& bases, int nbCon,
	vector<vector<double> >& A, vector<double>& b, vector<string>& sign,
	int indVarFrac, double xFracValue)
{
	double q = floor(xFracValue);
	for (unsigned int i = 0; i < S.size(); i++) S[i].insert(S[i].end() - 1, 0.0);
	int indVar = -1;
	for (int i = 0; i < nbCon; i++) if ((bases[i] - 1) == indVarFrac) { indVar = i; break; }
	vector<double> inter(S[0].size(), 0.0);
	for (unsigned int i = 0; i + 1 < inter.size(); i++)
		if (indVarFrac != (int)i && indVar != -1) inter[i] = -S[indVar][i];
	inter[S[0].size() - 2] = 1.0;
	inter[S[0].size() - 1] = (indVar != -1) ? (q - S[indVar][S[0].size() - 1]) : 0.0;
	S.insert(S.begin() + nbCon, inter);
	bases.push_back(S[0].size() - 1);

	vector<double> cut(A[0].size(), 0.0);
	cut[indVarFrac] = 1.0;
	A.push_back(cut);
	b.push_back(q);
	sign.push_back("<=");
}

void addCutSupSimplex(vector<vector<double> >& S, vector<int>& bases, int nbCon,
	vector<vector<double> >& A, vector<double>& b, vector<string>& sign,
	int indVarFrac, double xFracValue)
{
	double q = ceil(xFracValue);
	for (unsigned int i = 0; i < S.size(); i++) S[i].insert(S[i].end() - 1, 0.0);
	int indVar = -1;
	for (int i = 0; i < nbCon; i++) if ((bases[i] - 1) == indVarFrac) { indVar = i; break; }
	vector<double> inter(S[0].size(), 0.0);
	for (unsigned int i = 0; i + 1 < inter.size(); i++)
		if (indVarFrac != (int)i && indVar != -1) inter[i] = S[indVar][i];
	inter[S[0].size() - 2] = 1.0;
	inter[S[0].size() - 1] = (indVar != -1) ? (S[indVar][S[0].size() - 1] - q) : 0.0;
	S.insert(S.begin() + nbCon, inter);
	bases.push_back(S[0].size() - 1);

	vector<double> cut(A[0].size(), 0.0);
	cut[indVarFrac] = -1.0;
	A.push_back(cut);
	b.push_back(-q);
	sign.push_back("<=");
}

// =====================================================================================
// Coupe efficace (generalisee) --- REPRIS TEL QUEL (purement en x, programme (3)/(4)
// du papier, sans aucun rapport avec la nature fractionnaire des objectifs)
// =====================================================================================
void addEpsilonEfficientCutSimplex(vector<vector<double> >& S, vector<int>& Hl, int nbCon,
	vector<vector<double> >& A, vector<double>& b, vector<string>& sign,
	double& epsilon)
{
	for (unsigned int i = 0; i < S.size(); i++) S[i].insert(S[i].end() - 1, 0.0);
	vector<double> inter(S[0].size(), 0.0);
	for (unsigned int i = 0; i < Hl.size(); i++) inter[Hl[i]] = -1.0;
	inter[S[0].size() - 2] = 1.0;
	inter[S[0].size() - 1] = -1.0;
	S.insert(S.begin() + nbCon, inter);

	vector<double> cut(A[0].size(), 0.0);
	for (unsigned int i = 0; i < Hl.size(); i++) cut[Hl[i]] = -1.0;
	double scdB = -1.0 - epsilon;
	A.push_back(cut);
	b.push_back(scdB);
	sign.push_back("<=");
}

// =====================================================================================
// getEpsilon.mod --- REPRIS TEL QUEL. Le programme (3)/(4) porte uniquement sur x
// entier, aucune fraction n'entre ici : c'est exactement le meme fichier getEpsilon.mod
// que pour le cas MOILP.
// =====================================================================================
void writeDatFileEpsilon(vector<vector<double> >& A, vector<double>& b, vector<string>& sign,
	vector<double>& hl, double& b1)
{
	ofstream data;
	data.open("getEpsilon.dat", ofstream::trunc);
	data << "n = " << A[0].size() << ";\nm = " << A.size() << ";\nA = [";
	for (unsigned int i = 0; i < A.size(); i++)
	{
		data << "[";
		for (unsigned int j = 0; j < A[0].size(); j++) data << A[i][j] << " ";
		data << (i + 1 < A.size() ? "],\n" : "]];\n");
	}
	data << "b = [ ";
	for (unsigned int i = 0; i < b.size(); i++) data << b[i] << " ";
	data << "];\nhl = [" << endl;
	for (unsigned int i = 0; i < hl.size(); i++) data << hl[i] << " ";
	data << "];\nscb = " << b1 << ";" << endl;
	data.close();
}

bool getEpsilon(IloEnv env, vector<vector<double> >& A, vector<double>& b, vector<string>& sign,
	vector<double>& cut, double& b1, double& epsilon, int& status)
{
	status = 6;
	try
	{
		writeDatFileEpsilon(A, b, sign, cut, b1);
		IloOplRunConfiguration rc1(env, "getEpsilon.mod", "getEpsilon.dat");
		rc1.getCplex().setParam(IloCplex::NodeSel, CPX_NODESEL_DFS);
		rc1.getCplex().setOut(env.getNullStream());
		rc1.getOplModel().generate();
		if (rc1.getCplex().solve())
		{
			double val = rc1.getCplex().getObjValue();
			if (abs(val) < (numeric_limits<double>::epsilon() * 10000000)) { epsilon = 0; int_epsilon_null++; }
			else { epsilon = abs(val); int_epsilon_positive++; epsilonsPositive.push_back(epsilon); }
			status = 4;
			return true;
		}
		else { int_epsilon_abs++; status = 5; }
	}
	catch (IloOplException& e) { status = 1; int_epsilon_error++; }
	catch (IloException& e) { e.print(cout); status = 2; int_epsilon_error++; }
	catch (...) { status = 3; int_epsilon_error++; }
	return false;
}

// =====================================================================================
// Test de Benson lin�aris� --- ADAPT� (voir README : g_k(x) au lieu de t_k directement)
// =====================================================================================
void writeDatFileEfficienceFrac(vector<vector<double> >& A, vector<double>& b, vector<string>& sign,
	vector<vector<double> >& NUM, vector<double>& NUMc,
	vector<vector<double> >& DEN, vector<double>& DENc, vector<double>& x0)
{
	ofstream data;
	data.open("TestEfficienceFrac.dat", ofstream::trunc);
	data << "n = " << A[0].size() << ";\nm = " << A.size() << ";\nA = [";
	for (unsigned int i = 0; i < A.size(); i++)
	{
		data << "[";
		for (unsigned int j = 0; j < A[0].size(); j++) data << A[i][j] << " ";
		data << (i + 1 < A.size() ? "],\n" : "]];\n");
	}
	data << "b = [ ";
	for (unsigned int i = 0; i < b.size(); i++) data << b[i] << " ";
	data << "];\n";
	for (unsigned int k = 0; k < NUM.size(); k++)
	{
		data << "a" << (k + 1) << " = [ "; for (unsigned int j = 0; j < NUM[k].size(); j++) data << NUM[k][j] << " "; data << "];\n";
		data << "b" << (k + 1) << " = " << NUMc[k] << ";\n";
		data << "c" << (k + 1) << " = [ "; for (unsigned int j = 0; j < DEN[k].size(); j++) data << DEN[k][j] << " "; data << "];\n";
		data << "d" << (k + 1) << " = " << DENc[k] << ";\n";
	}
	data << "x0 = [ ";
	for (unsigned int i = 0; i < x0.size(); i++) data << x0[i] << " ";
	data << "];" << endl;
	data.close();
}

// Renvoie true si x0 est efficace (T*=0). Sinon, remplit x0 avec le point dominant
// trouve par le test (meme convention que testEfficiency() original).
bool testEfficiencyFrac(IloEnv env, vector<vector<double> >& A, vector<double>& b, vector<string>& sign,
	vector<vector<double> >& NUM, vector<double>& NUMc,
	vector<vector<double> >& DEN, vector<double>& DENc,
	vector<double>& x0, int& status)
{
	status = 6;
	try
	{
		writeDatFileEfficienceFrac(A, b, sign, NUM, NUMc, DEN, DENc, x0);
		IloOplRunConfiguration rc1(env, "TestEfficienceFrac.mod", "TestEfficienceFrac.dat");
		rc1.getCplex().setParam(IloCplex::NodeSel, CPX_NODESEL_DFS);
		rc1.getCplex().setOut(env.getNullStream());
		rc1.getOplModel().generate();
		if (rc1.getCplex().solve())
		{
			if (abs(rc1.getCplex().getObjValue()) < (numeric_limits<double>::epsilon() * 10000000))
				return true;
			IloNumMap t = rc1.getOplModel().getElement("x").asNumMap();
			x0.clear();
			for (IloInt i = 1; i <= (IloInt)t.getSize(); i++) x0.push_back(t.get(i));
			return false;
		}
	}
	catch (IloOplException& e) { status = 1; }
	catch (IloException& e) { e.print(cout); status = 2; }
	catch (...) { status = 3; }
	return false;
}

// =====================================================================================
// approxIdealFrac : point ideal local. SEUL endroit ou l'on utilise Charnes-Cooper --
// mais ici sur une relaxation CONTINUE, un objectif a la fois, chacun avec SON PROPRE
// denominateur (pas besoin de denominateur commun ici, contrairement au coeur de
// l'algorithme). Aucune integrite a preserver -> aucune des reserves theoriques
// signalees pour la version CC complete ne s'applique.
// =====================================================================================
vector<double> approxIdealFrac(NoeudQuadratic n, vector<vector<double> >& NUM, vector<double>& NUMc,
	vector<vector<double> >& DEN, vector<double>& DENc)
{
	vector<double> idealPoint;
	IloEnv env3;
	int nvar = (int)NUM[0].size();
	for (unsigned int k = 0; k < NUM.size(); k++)
	{
		IloModel model(env3);
		IloNumVarArray y(env3);
		IloNumVar t(env3, 0.0, IloInfinity, "t");
		for (int j = 0; j < nvar; j++)
		{
			string result = "y" + to_string(j);
			y.add(IloNumVar(env3, 0.0, IloInfinity, result.data()));
		}

		IloNumExpr obj(env3);
		for (int j = 0; j < nvar; j++) obj += NUM[k][j] * y[j];
		obj += NUMc[k] * t;
		model.add(IloMinimize(env3, obj));

		IloRangeArray ct(env3);
		for (unsigned int i = 0; i < n.getA().size(); i++)
		{
			IloNumExpr c(env3);
			for (int j = 0; j < nvar; j++) c += n.getA()[i][j] * y[j];
			c -= n.getB()[i] * t;    // ramene tout a gauche : Ay - b*t {sign} 0
			if (n.getSign()[i] == "<=") ct.add(c <= 0);
			else if (n.getSign()[i] == ">=") ct.add(c >= 0);
			else ct.add(c == 0);
		}
		IloNumExpr den(env3);
		for (int j = 0; j < nvar; j++) den += DEN[k][j] * y[j];
		den += DENc[k] * t;
		ct.add(den == 1);
		model.add(ct);

		IloCplex cpx(model);
		cpx.setOut(cpx.getEnv().getNullStream());
		if (cpx.solve()) idealPoint.push_back(cpx.getObjValue());
		else cout << "No solution (approxIdealFrac, objectif " << k << ")" << endl;
	}
	env3.end();
	return idealPoint;
}

// =====================================================================================
// computeNadirPoint : point nadir GLOBAL, calcule UNE SEULE FOIS a la racine (table de
// gains, cf. Ait Mehdi, Chergui & Abbas 2014). Pour chaque objectif k, on cherche
// l'optimum ENTIER de f_k sur X TOUT ENTIER -- ici via une petite boucle de Dinkelbach
// sur des PL entiers ordinaires (aucun Charnes-Cooper : c'est justement l'alternative
// dont on a discute, appliquee la ou elle est la plus naturelle -- un seul objectif,
// optimise une seule fois, sans avoir besoin de rester dans un tableau de simplexe).
// Nd_i = pire valeur (la plus grande, puisqu'on minimise) de l'objectif i observee
// parmi les p points de la table de gains -- comme le signale le papier, c'est une
// ESTIMATION, fiable surtout en biobjectif (p=2).
// =====================================================================================
vector<double> computeNadirPoint(vector<vector<double> >& A, vector<double>& b, vector<string>& sign,
	vector<vector<double> >& NUM, vector<double>& NUMc,
	vector<vector<double> >& DEN, vector<double>& DENc)
{
	int p = (int)NUM.size();
	int nvar = (int)NUM[0].size();
	const double BIGM_INT = 1000000.0;
	vector<vector<double> > payoff; // p lignes (une par objectif optimise), p colonnes
	IloEnv env;

	for (int k = 0; k < p; k++)
	{
		double q = 0.0;
		vector<double> xk(nvar, 0.0);
		bool ok = false;

		for (int iter = 0; iter < 50; iter++)
		{
			IloModel model(env);
			IloNumVarArray x(env);
			for (int j = 0; j < nvar; j++)
			{
				string nm = "x" + to_string(j);
				x.add(IloIntVar(env, 0, (IloInt)BIGM_INT, nm.data()));
			}
			// programme de Dinkelbach : min (a_k - q*c_k)^T x + (b_k - q*d_k)
			IloNumExpr obj(env);
			for (int j = 0; j < nvar; j++) obj += (NUM[k][j] - q * DEN[k][j]) * x[j];
			obj += (NUMc[k] - q * DENc[k]);
			model.add(IloMinimize(env, obj));

			IloRangeArray ct(env);
			for (unsigned int i = 0; i < A.size(); i++)
			{
				IloNumExpr c(env);
				for (int j = 0; j < nvar; j++) c += A[i][j] * x[j];
				if (sign[i] == "<=") ct.add(c <= b[i]);
				else if (sign[i] == ">=") ct.add(c >= b[i]);
				else ct.add(c == b[i]);
			}
			model.add(ct);

			IloCplex cpx(model);
			cpx.setOut(cpx.getEnv().getNullStream());
			bool solved = cpx.solve();
			if (solved) { for (int j = 0; j < nvar; j++) xk[j] = cpx.getValue(x[j]); ok = true; }
			model.end();
			if (!solved) break;

			double num = ZLinear(NUM[k], xk) + NUMc[k];
			double den = ZLinear(DEN[k], xk) + DENc[k];
			double newQ = num / den;
			if (abs(newQ - q) < 1e-7) { q = newQ; break; }
			q = newQ;
		}
		if (ok) payoff.push_back(fracObjectives(NUM, NUMc, DEN, DENc, xk));
	}
	env.end();

	vector<double> nadir(p, -numeric_limits<double>::infinity());
	for (unsigned int row = 0; row < payoff.size(); row++)
		for (int col = 0; col < p; col++)
			if (payoff[row][col] > nadir[col]) nadir[col] = payoff[row][col];
	return nadir;
}

// =====================================================================================
// Phase initiale et phase generale --- structure IDENTIQUE a main.cpp ; seuls les
// appels a detectHl/testEfficiency/approxIdeal sont rediriges vers les versions Frac,
// et Z(C,x) est remplace par fracObjectives(NUM,NUMc,DEN,DENc,x).
// =====================================================================================
bool initialPhase(vector<vector<double> >& A, vector<double>& b, vector<string>& sign,
	vector<vector<double> >& C,
	vector<vector<double> >& NUM, vector<double>& NUMc,
	vector<vector<double> >& DEN, vector<double>& DENc)
{
	int status(0);
	IloEnv env;
	vector<vector<double> > S = buildSimplexMatrix(A, b, sign, C);
	vector<int> bases; vector<double> x;
	for (unsigned int i = 1; i <= A.size(); i++) bases.push_back((int)(A[0].size() + i));

	if (!simplex(S, (int)A.size(), (int)A[0].size(), (int)C.size(), bases, x)) return false;

	int indFrac;
	if (entiere(x, indFrac))
	{
		int_sol++;
		vector<double> xCopy(x);
		if (testEfficiencyFrac(env, A, b, sign, NUM, NUMc, DEN, DENc, xCopy, status))
		{
			SND.push_back(fracObjectives(NUM, NUMc, DEN, DENc, x));
			Eff.insert(x);
			int_eff_sol++;
		}
		vector<vector<double> > A_(A); vector<double> b_(b); vector<string> sign_(sign);
		vector<int> outBases, Hl;
		detectOutBasesSimplex(bases, outBases, (int)A.size(), (int)x.size());
		detectHlFrac(S, outBases, Hl, (int)A.size(), NUM, NUMc, DEN, DENc, x);
		if (Hl.empty()) return false;

		double epsilon(0);
		vector<double> cut(A[0].size(), 0.0); double scdB = -1.0;
		for (unsigned int i = 0; i < Hl.size(); i++)
		{
			if (Hl[i] < (int)A[0].size()) cut[Hl[i]] -= 1.0;
			else { for (unsigned int j = 0; j < A[0].size(); j++) cut[j] += A_[Hl[i] - A[0].size()][j]; scdB += b_[Hl[i] - A[0].size()]; }
		}
		if (getEpsilon(env, A_, b_, sign_, cut, scdB, epsilon, status))
		{
			vector<vector<double> > A_2(A); vector<double> b_2(b); vector<string> sign_2(sign);
			int nbCon = (int)A.size() + 1, nbVar = (int)A[0].size(), nbObj = (int)C.size();
			addEpsilonEfficientCutSimplex(S, Hl, (int)A.size(), A_2, b_2, sign_2, epsilon);
			bases.push_back((int)S[0].size() - 1);
			NoeudQuadratic noeud(num_noeud, 0, S, A_2, b_2, sign_2, bases, nbCon, nbVar, nbObj);
			tree.push(NoeudQuadratic(noeud));
			num_noeud++;
		}
	}
	else
	{
		vector<vector<double> > A1(A); vector<double> b1(b); vector<string> sign1(sign);
		vector<vector<double> > S1(S), S2(S);
		vector<int> bases1(bases), bases2(bases);
		addCutInfSimplex(S1, bases1, (int)A.size(), A1, b1, sign1, indFrac, x[indFrac]);
		int nbCon = (int)A.size() + 1, nbVar = (int)A[0].size(), nbObj = (int)C.size();
		NoeudQuadratic noeud(num_noeud, 0, S1, A1, b1, sign1, bases1, nbCon, nbVar, nbObj);
		num_noeud++;

		vector<vector<double> > A2(A); vector<double> b2(b); vector<string> sign2(sign);
		addCutSupSimplex(S2, bases2, (int)A.size(), A2, b2, sign2, indFrac, x[indFrac]);
		NoeudQuadratic noeud2(num_noeud, 0, S2, A2, b2, sign2, bases2, nbCon, nbVar, nbObj);
		tree.push(NoeudQuadratic(noeud2));
		tree.push(NoeudQuadratic(noeud));
		num_noeud++;
	}
	env.end();
	return true;
}

void generalPhase(vector<vector<double> >& A, vector<double>& b, vector<string>& sign,
	vector<vector<double> >& C,
	vector<vector<double> >& NUM, vector<double>& NUMc,
	vector<vector<double> >& DEN, vector<double>& DENc,
	vector<double>& Nd)
{
	IloEnv env;
	num_noeud = (tree.size() == 1 ? 1 : 3);
	while (!tree.empty())
	{
		int status(0);
		vector<int> bases(tree.top().getBase());
		vector<vector<double> > S(tree.top().getS()), A2(tree.top().getA());
		vector<double> b2(tree.top().getB());
		vector<string> sign2(tree.top().getSign());
		int nbCon(tree.top().getNbCon()), nbVar(tree.top().getNbVar());
		vector<double> x;

		if (dualSimplex(S, nbCon, nbVar, (int)C.size(), bases, x))
		{
			int indFrac;
			if (entiere(x, indFrac))
			{
				int_sol++;
				vector<double> xCopy(x);
				bool eff = testEfficiencyFrac(env, A, b, sign, NUM, NUMc, DEN, DENc, xCopy, status);
				int sizeNow = (int)Eff.size();
				Eff.insert(x);
				if ((int)Eff.size() > sizeNow) { SND.push_back(fracObjectives(NUM, NUMc, DEN, DENc, x)); int_eff_sol++; }

				vector<double> IQ = approxIdealFrac(tree.top(), NUM, NUMc, DEN, DENc);
				bool domination(false);
				if (!IQ.empty())
				{
					for (unsigned int i = 0; i < SND.size(); i++) if (domine(SND[i], IQ)) { domination = true; break; }
				}
				else { tree.top().~NoeudQuadratic(); tree.pop(); empty_delete++; continue; }

				// Regle d'elagage par point nadir (Ait Mehdi, Chergui & Abbas 2014) :
				// si le MEILLEUR cas atteignable sur cette branche pour un objectif i
				// (IQ[i], le point ideal local) est deja PIRE que le pire cas deja connu
				// globalement (Nd[i]), la branche ne peut produire aucune solution
				// interessante -- on la sonde, sans meme avoir besoin de developper les
				// coupes H_l. Complementaire (pas redondante) avec le test de dominance
				// ci-dessus : celui-ci compare a des points DEJA TROUVES, le test nadir
				// compare a une borne GLOBALE calculee une seule fois a la racine.
				bool belowNadir(false);
				if (!domination && !Nd.empty())
				{
					for (unsigned int i = 0; i < Nd.size() && i < IQ.size(); i++)
						if (IQ[i] > Nd[i] + 1e-9) { belowNadir = true; break; }
				}

				if (belowNadir)
				{
					tree.top().~NoeudQuadratic(); tree.pop(); nadir_delete++;
				}
				else if (!domination)
				{
					vector<vector<double> > A_(A2); vector<double> b_(b2); vector<string> sign_(sign2);
					vector<int> outBases, Hl;
					detectOutBasesSimplex(bases, outBases, nbCon, nbVar);
					detectHlFrac(S, outBases, Hl, nbCon, NUM, NUMc, DEN, DENc, x);
					if (Hl.empty()) { tree.top().~NoeudQuadratic(); tree.pop(); hl_delete++; }
					else
					{
						double epsilon(0);
						vector<double> cut(A[0].size(), 0.0); double scdB = -1.0;
						for (unsigned int i = 0; i < Hl.size(); i++)
						{
							if (Hl[i] < (int)A[0].size()) cut[Hl[i]] -= 1.0;
							else { for (unsigned int j = 0; j < A[0].size(); j++) cut[j] += A_[Hl[i] - A[0].size()][j]; scdB += b_[Hl[i] - A[0].size()]; }
						}
						if (getEpsilon(env, A_, b_, sign_, cut, scdB, epsilon, status))
						{
							vector<vector<double> > A_2(A2); vector<double> b_2(b2); vector<string> sign_2(sign2);
							addEpsilonEfficientCutSimplex(S, Hl, nbCon, A_2, b_2, sign_2, epsilon);
							bases.push_back((int)S[0].size() - 1);
							NoeudQuadratic noeud(num_noeud, tree.top().getLevel(), S, A_2, b_2, sign_2, bases, nbCon + 2, nbVar, tree.top().getNbObj());
							tree.top().~NoeudQuadratic(); tree.pop();
							tree.push(NoeudQuadratic(noeud));
							num_noeud++;
						}
						else { tree.top().~NoeudQuadratic(); tree.pop(); }
					}
				}
				else { tree.top().~NoeudQuadratic(); tree.pop(); dom_delete++; }
			}
			else
			{
				double xFracVal = x[indFrac];
				tree.top().~NoeudQuadratic(); tree.pop();
				vector<vector<double> > A1(A2); vector<double> b1(b2); vector<string> sign1(sign2);
				vector<vector<double> > S1(S), S2(S);
				vector<int> bases1(bases), bases2(bases);
				addCutInfSimplex(S1, bases1, nbCon, A1, b1, sign1, indFrac, xFracVal);
				NoeudQuadratic noeud(num_noeud, tree.top().getLevel(), S1, A1, b1, sign1, bases1, nbCon + 1, nbVar, tree.top().getNbObj());
				num_noeud++;

				vector<vector<double> > A3(A2); vector<double> b3(b2); vector<string> sign3(sign2);
				addCutSupSimplex(S2, bases2, nbCon, A3, b3, sign3, indFrac, xFracVal);
				NoeudQuadratic noeud2(num_noeud, tree.top().getLevel(), S2, A3, b3, sign3, bases2, nbCon + 1, nbVar, tree.top().getNbObj());
				tree.push(NoeudQuadratic(noeud2));
				tree.push(NoeudQuadratic(noeud));
				num_noeud++;
			}
		}
		else { tree.top().~NoeudQuadratic(); tree.pop(); empty_delete++; }
	}
}

// =====================================================================================
int main()
{
	vector<vector<double> > A, NUM, DEN, C;
	vector<double> b, w, NUMc, DENc;
	vector<string> sign;
	clock_t beg, ed;

	output.open("outputGamma.txt", ofstream::trunc);

	readDataFrac(A, b, sign, w, NUM, NUMc, DEN, DENc);
	buildCFrac(w, NUM, DEN, C);

	beg = clock();
	vector<double> Nd = computeNadirPoint(A, b, sign, NUM, NUMc, DEN, DENc);
	cout << "Point nadir (calcule une fois a la racine) : "; afficher(Nd);

	if (initialPhase(A, b, sign, C, NUM, NUMc, DEN, DENc)) generalPhase(A, b, sign, C, NUM, NUMc, DEN, DENc, Nd);
	ed = clock();

	cout << "Ensemble des solutions efficaces (en x) :" << endl;
	afficher(Eff);
	cout << "Ensemble des solutions non-dominees (valeurs des ratios f_k(x)) :" << endl;
	afficher(SND);
	cout << "Nombre de solutions efficaces : " << int_eff_sol << endl;
	cout << "Temps d'execution : " << (ed - beg) / 1000.0 << " sec" << endl;
	cout << "Nombre de noeuds : " << num_noeud << endl;
	cout << "Noeuds elimines (Hl vide) : " << hl_delete << endl;
	cout << "Noeuds elimines (dominance) : " << dom_delete << endl;
	cout << "Noeuds elimines (nadir) : " << nadir_delete << endl;
	cout << "Noeuds elimines (domaine vide) : " << empty_delete << endl;

	output.close();
	return 0;
}
