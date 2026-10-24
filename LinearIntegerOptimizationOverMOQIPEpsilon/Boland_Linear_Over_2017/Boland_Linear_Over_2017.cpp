#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "gams.h"
#include "Noeud.h"

#define _PATH_EFFICIENT_BOLAND_ "C:\\Users\\m-ela.NINO\\Documents\\efficient_boland.gms"
//#define _PATH_OPT_BOLAND_ "C:\\Users\\CLIENT\\Documents\\Visual Studio 2015\\Projects\\LinearIntegerOptimizationOverMOQIPEpsilon\\x64\\Release\\optimal_boland.gms"
#define _PATH_BEST_OPT_ "C:\\Users\\m-ela.NINO\\Documents\\best_optimal_boland.gms"

using namespace std;
using namespace gams;

double f_u = numeric_limits<double>::infinity(), f_l = -numeric_limits<double>::infinity(), eps1 = 0.00000001, eps2 = 0.000000001;
vector<vector<double> > A, C;
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
	ofstream model(_PATH_EFFICIENT_BOLAND_);
	ofstream model_best(_PATH_BEST_OPT_);
	if (fichier)
	{
		double mot; //Une variable pour stocker les lignes lues
		fichier >> n;
		fichier >> m;
		fichier >> p;
		model << "Set j 'set of variables of indices' /1*" << n << "/;\nAlias (j,k);\nSet i 'set of constraints' /1*" << m << "/;\n";
		model_opt = model_opt + "Set j 'set of variables of indices' /1*" + to_string(n) + "/;\nAlias (j,k);\nSet i 'set of constraints' /1*" + to_string(m) + "/;\n";
		model_best << "Set j 'set of variables of indices' /1*" << n << "/;\nAlias (j,k);\nSet i 'set of constraints' /1*" << m << "/;\n";
		model << "Set l 'set of lin objectives' / 1 * " << p << " / ; \n";
		model_opt = model_opt + "Set l 'set of lin objectives' /1*" + to_string(p) + "/;\n";
		model_best << "Set l 'set of lin objectives' /1*" << p << "/;\n";
		model << "Parameters\n\ty2(l) 'Upper Lin Bound'\n\ty3(l) 'Lower lin Bound'\n";
		model_opt = model_opt + "Parameter\n\ty3(l) 'Lower lin Bound on y_L'\n";
		model_opt = model_opt + "\t\ty5(l) 'upper bound lin on y'\n";
		model_best << "Parameter\n\ty2(l) 'Upper Lin Bound on Y_Tild'\n";
		model << "\tb(i) 'Second member vector'\n\tc(l,j) 'Lin obj vector'\n";
		model_opt = model_opt + "\tb(i) 'Second member vector'\n\tc(l,j) 'Lin obj vector'\n\ttarget(j) 'Objective Opt'\n\tMM(l) 'Big Ml';\n";
		model_opt = model_opt + "Scalar epsilon /0.1/;\n\n";
		model_best << "\tb(i) 'Second member vector'\n\tc(l,j) 'Lin obj vector'\n\ttarget(j) 'Objective Opt';\n";
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
		model << "Table A(i,j) 'Lin constraints matrix'\n\t";
		model_opt = model_opt + "Table A(i,j) 'Lin constraints matrix'\n\t";
		model_best << "Table A(i,j) 'Lin constraints matrix'\n\t";
		for (unsigned int i = 1; i <= n; i++)
		{
			model << i << "\t";
			model_opt = model_opt + to_string(i) + "\t";
			model_best << i << "\t";
		}
		for (unsigned int i = 0; i < m; i++)
		{
			model << "\n" << (i + 1) << "\t";
			model_opt = model_opt + "\n" + to_string(i + 1) + "\t";
			model_best << "\n" << (i + 1) << "\t";
			for (unsigned int j = 0; j < n; j++)
			{
				model << (int)A[i][j] << "\t";
				model_opt = model_opt + to_string((int)A[i][j]) + "\t";
				model_best << (int)A[i][j] << "\t";
			}
		}
		model << ";\nTable LL(j,l) 'Matrice des objectifs lin de la partie lin'\n\t";
		model_opt = model_opt + ";\nTable LL(j,l) 'Matrice des objectifs lin de la partie lin'\n\t";
		model_best << ";\nTable LL(j,l) 'Matrice des objectifs lin de la partie lin'\n\t";
		for (unsigned int i = 1; i <= p; i++)
		{
			model << i << "\t";
			model_opt = model_opt + to_string(i) + "\t";
			model_best << i << "\t";
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
			model << "\n" << (j + 1) << "\t";
			model_opt = model_opt + "\n" + to_string(j + 1) + "\t";
			model_best << "\n" << (j + 1) << "\t";
			for (unsigned int k = 0; k < p; k++)
			{
				model << (int)C[k][j] << "\t";
				model_opt = model_opt + to_string((int)C[k][j]) + "\t";
				model_best << (int)C[k][j] << "\t";
			}
		}
		model << ";\n";
		model_opt = model_opt + ";\n";
		model_best << ";\n";
		for (unsigned int h = 1; h <= p; h++)
		{
			model << "c('" << h << "', j) = LL(j, '" << h << "');\n";
			model_opt = model_opt + "c('" + to_string(h) + "', j) = LL(j, '" + to_string(h) + "');\n";
			model_best << "c('" << h << "', j) = LL(j, '" << h << "');\n";
		}
		for (unsigned int i = 0; i < b.size(); i++)
		{
			model << "b('" << (i + 1) << "') = " << b[i] << ";\n";
			model_opt = model_opt + "b('" + to_string(i + 1) + "') = " + to_string(b[i]) + ";\n";
			model_best << "b('" << (i + 1) << "') = " << b[i] << ";\n";
		}
		for (unsigned int i = 1; i <= p; i++)
		{
			model_opt = model_opt + "MM('" + to_string(i) + "') = 10000000;\n";
		}
		for (unsigned int i = 1; i <= n; i++)
		{
			model_opt = model_opt + "target('" + to_string(i) + "') = " + to_string(obj[i - 1]) + ";\n";
			model_best << "target('" << i << "') = " << obj[i - 1] << ";\n";
		}
		model << "Variables\n\tx(j) 'variables'\n\tsol(l) 'sub obj l'\n\tobj 'objectif';\nInteger Variable x;\n";
		model_opt = model_opt + "Variable\n\tx(j) 'variables'\n\tsol(l) 'sub obj l'\n\tobj 'objectif'\n";
		model_opt = model_opt + "\t\tbc(l) 'choice2 on y';\nInteger Variable x;\nBinary Variables b3, bc;\n";
		model_best << "Variable\n\tx(j) 'variables'\n\tsol(l) 'sub obj l'\n\tobj 'objectif';\n";
		model_best << "Integer Variable x;\n";
		model << "Equations\n\tf\t\t'Fonction Objectif'\n\tsubl(l)\t'Function Lineaires'\n\tconst(i)'1er groupe'\n\t";
		model_opt = model_opt + "Equation\n\tf\t\t'Fonction Objectif'\n\tsubl(l)\t'Function Lineaires'\n\tconst(i)'1er groupe'\n\t";
		model_best << "Equation\n\tf\t\t'Fonction Objectif'\n\tsubl(l)\t'Function Lineaires'\n\tconst(i)'1er groupe'\n\t";
		model << "bsl(l)\t'borne sup lin'\n\tbil(l)\t'born inf lin';\n";
		model_opt = model_opt + "bil(l)\t'born inf lin on y_L'\n";
		model_opt = model_opt + "\tbsl2(l)'borne sup lin on y'\n\tfixb2\t'fix b to 1 _ 2';\n";
		model_best << "bsl(l)'borne sup lin on Y_Tild';\n";
		model << "subl(l)..\tsol(l) =e= sum(j, c(l,j)*x(j));\n";
		model_opt = model_opt + "subl(l)..\tsol(l) =e= sum(j, c(l,j)*x(j));\n";
		model_best << "subl(l)..\tsol(l) =e= sum(j, c(l,j)*x(j));\n";
		model << "f..\t\t\tobj =e= sum(l, sol(l));\n";
		model_opt = model_opt + "f..\t\t\tobj =e= sum(j, target(j)*x(j));\n";
		model_best << "f..\t\t\tobj =e= sum(j, target(j)*x(j));\n";
		model << "const(i)..\tsum(j, A(i, j) * x(j)) =l= b(i);\n";
		model_opt = model_opt + "const(i)..\tsum(j, A(i, j) * x(j)) =l= b(i);\n";
		model_best << "const(i)..\tsum(j, A(i, j) * x(j)) =l= b(i);\n";
		model << "bsl(l)..\tsol(l) =l= y2(l);\n";
		model_best << "bsl(l)..\tsol(l) =l= y2(l);\n";
		model << "bil(l)..\tsol(l) =g= y3(l);\n";
		model_opt = model_opt + "bil(l)..\tsol(l) =g= y3(l);\n";
		model_opt = model_opt + "bsl2(l)..\tsol(l) =l= (y5(l) - epsilon - MM(l))*bc(l) + MM(l);\n";
		model_opt = model_opt + "fixb2..\t\tsum(l, bc(l)) =e= 1;\n";
		model << "Model boland /all/;\noption MIP = cplex;\n";
		model_best << "Model boland /all/;\noption MIP = cplex;\n";
	}
	else
	{
		//cout << "We are out" << endl;
	}
	model.close();
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

vector<double> Z(vector<vector<double> >& C, vector<double> x)
{
	vector<double> vec;
	for (int i = 0; i < C.size(); i++)
	{
		vec.push_back(ZLinear(C[i], x));
	}
	return vec;
}

bool initialPhase(vector<double>& x, vector<Noeud>& queue)
{
	GAMSWorkspaceInfo wsi(GAMSEnum::ShowLog);
	GAMSWorkspace ws1(wsi);
	string str = model_opt + "\nScalar ms 'model status', ss 'solve status';\n";
	for (unsigned int i = 1; i <= C.size(); i++)
	{
		str = str + "y3('" + to_string(i) + "') = -10000000;\n";
		str = str + "y5('" + to_string(i) + "') = 10000000;\n";
	}
	str = str + "Model boland /all/;\noption MIP = cplex;\n";
	//cout << "str = " << str << endl;

	GAMSJob t = ws1.addJobFromString(str + "solve boland max obj use mip; ms = boland.modelstat; ss = boland.solvestat;");
	GAMSOptions op = ws1.addOptions();
	op.setOptCR(0);
	t.run(op);
	if (t.outDB().getParameter("ms").findRecord().value() == 1)
	{
		f_u = t.outDB().getVariable("obj").firstRecord().level();
		cout << "f_u = " << f_u << endl;
	}
	GAMSWorkspaceInfo wsi2(GAMSEnum::ShowLog);
	GAMSWorkspace ws2(wsi2);
	GAMSJob t2 = ws2.addJobFromString(str + "solve boland min obj use mip; ms = boland.modelstat; ss = boland.solvestat;");
	op = ws2.addOptions();
	op.setOptCR(0);
	t2.run(op);
	if (t2.outDB().getParameter("ms").findRecord().value() != 1)
	{
		return false;
	}
	else
	{
		f_l = t2.outDB().getVariable("obj").firstRecord().level();
		cout << "f_l = " << f_l << endl;
		for (GAMSVariableRecord rec : t2.outDB().getVariable("x"))
			x[stoi(rec.key(0)) - 1] = rec.level();
		cout << "x : ";
		afficher(x);
		queue.push_back(Noeud(C.size(), f_l, vector<double>(x), vector<vector<double> >(), vector<double>(), vector<double>()));
		return true;
	}
}

bool yEqualInfinityMinus(vector<double>& y)
{
	for (int i = 0; i < y.size(); i++)
	{
		if (y[i] != -numeric_limits<double>::infinity())
		{
			return false;
		}
	}
	return true;
}

void principalPhase(vector<double>& x, vector<Noeud>& queue, GAMSWorkspace& ws, GAMSCheckpoint& cp, GAMSWorkspace& ws3, GAMSCheckpoint& cp3)
{
	bool endSearch = false;
	while (!queue.empty() and !endSearch)
	{
		//char cc;
		//cin >> cc;
		Noeud noeud = queue[0];
		queue.erase(queue.begin());
		f_l = noeud.getF_l();
		cout << "f_l actuel = " << f_l << endl;
		cout << "f_u_actuel = " << f_u << endl;
		cout << "ratio = " << (abs((f_u - f_l)) / abs((f_u + eps1))) << endl;
		cout << "eps = " << eps2 << endl;
		vector<double> x_f;
		if ((abs(f_u - f_l) / abs(f_u + eps1)) > eps2)
		{
			cout << "IN" << endl;
			vector<double> z = Z(C, noeud.getX_0());
			cout << "x IN : ";
			afficher(noeud.getX_0());
			cout << "z : ";
			afficher(z);
			string str = "\nScalar ms 'model status', ss 'solve status';\n";
			for (int i = 1; i <= noeud.getL(); i++)
			{
				str = str + "y2('" + to_string(i) + "') = " + to_string(z[i - 1]) + ";\n";
				str = str + "y3('" + to_string(i) + "') = -10000000;\n";
			}
			cout << "str = " << endl << str;
			GAMSJob t = ws.addJobFromString(str + "solve boland min obj use mip; ms = boland.modelstat; ss = boland.solvestat;", cp);
			t.run(cp);
			nb_non_eff++;
			GAMSModelInstance mi = cp.addModelInstance();
			cout << "status : " << t.outDB().getParameter("ms").findRecord().value() << endl;
			cout << "solve : " << t.outDB().getParameter("ss").findRecord().value() << endl;
			if (t.outDB().getParameter("ms").findRecord().value() != 1 && t.outDB().getParameter("ms").findRecord().value() != 8)
			{
				cout << "in1" << endl;
				continue;
			}
			else
			{
				cout << "in2" << endl;
				vector<double> x_n(x);
				for (GAMSVariableRecord rec : t.outDB().getVariable("x"))
					x_n[stoi(rec.key(0)) - 1] = rec.level();
				cout << "x_n : ";
				afficher(x_n);
				vector<double> zz(Z(C, x_n));
				nb_eff += 2;
				if (Z(C, noeud.getX_0()) == zz)
				{
					cout << "in3" << endl;
					x_f = vector<double>(noeud.getX_0());
				}
				else
				{
					cout << "in4" << endl;
					str = "\nScalar ms 'model status', ss 'solve status';\n";
					for (int i = 1; i <= noeud.getL(); i++)
					{
						str = str + "y2('" + to_string(i) + "') = " + to_string(zz[i - 1]) + ";\n";
					}
					cout << "str : " << endl << str;
					GAMSJob t2 = ws3.addJobFromString(str + "solve boland min obj use mip; ms = boland.modelstat; ss = boland.solvestat;", cp3);
					t2.run(cp3);
					GAMSModelInstance mi2 = cp3.addModelInstance();
					if (t2.outDB().getParameter("ms").findRecord().value() != 1 && t2.outDB().getParameter("ms").findRecord().value() != 8)
					{
						cout << "in5" << endl;
						//continue;
					}
					else
					{
						cout << "in6" << endl;
						x_f = vector<double>(x_n);
						for (GAMSVariableRecord rec : t2.outDB().getVariable("x"))
							x_f[stoi(rec.key(0)) - 1] = rec.level();
						cout << "x_f : ";
						afficher(x_f);
					}
				}
				if (f_u > ZLinear(obj, x_f))
				{
					cout << "in7" << endl;
					f_u = ZLinear(obj, x_f);
					cout << "f_u = " << f_u << endl;
					x = vector<double>(x_f);
				}
				if (f_l < f_u)
				{
					cout << "in8" << endl;
					vector<double> y, z_xn(Z(C, x_n));
					if (noeud.getY().empty())
					{
						cout << "in9" << endl;
						for (unsigned int i = 0; i < C.size(); i++)
						{
							y.push_back(z_xn[i]);
						}
					}
					else if (noeud.getY_L().empty())
					{
						cout << "in9__" << endl;
						for (unsigned int i = 0; i < C.size(); i++)
						{
							y.push_back(min(noeud.getY()[i], z_xn[i]));
						}
					}
					else
					{
						cout << "in10" << endl;
						for (unsigned int i = 0; i < C.size(); i++)
						{
							y.push_back(max(min(noeud.getY()[i], z_xn[i]), noeud.getY_L()[i]));
						}
					}
					cout << "y = ";
					afficher(y);
					if (!noeud.getY().empty())
					{
						cout << "in10__" << endl;
						vector<double> yy(y.size(), -10000000), yyy(y.size(), -10000000);
						for (unsigned int i = 0; i < C.size(); i++)
						{
							if (z_xn[i] > noeud.getY()[i] && z_xn[i] > y[i])
							{
								yy[i] = z_xn[i];
							}
							if (z_xn[i] < noeud.getY()[i] && noeud.getY()[i] > y[i])
							{
								yyy[i] = noeud.getY()[i];
							}
						}
						cout << "yy = ";
						afficher(yy);
						cout << "yyy = ";
						afficher(yyy);
						if (!yEqualInfinityMinus(yy) && !yEqualInfinityMinus(yyy))
						{
							cout << "in11" << endl;
							string str = model_opt + "\nScalar ms 'model status', ss 'solve status';\n";
							str = str + "Set h 'Set of YTilds' /1*" + to_string(noeud.getYTild().size() + 2) + "/;\n";
							str = str + "Parameter y2(l,h) 'Upper Lin Bound on Y_Tild';\n";
							str = str + "Binary Variable \n\tbb(l,h) 'choice2 on Y_Tild';\n";
							str = str + "Equation\n\tbsl(l,h) 'borne sup lin on Y_Tild'\n\tfixb(h) 'fix b to 1';";
							str = str + "bsl(l,h)..\tsol(l) =l= (y2(l,h) - epsilon - MM(l))*bb(l,h) + MM(l);\n";
							str = str + "fixb(h)..\tsum(l, bb(l,h)) =e= 1;\n";
							str = str + "Model boland /all/;\noption MIP = cplex;\n";
							for (int i = 1; i <= noeud.getL(); i++)
							{
								for (unsigned int j = 1; j <= noeud.getYTild().size(); j++)
								{
									if (noeud.getYTild()[j - 1][i - 1] == -numeric_limits<double>::infinity() || noeud.getYTild()[j - 1][i - 1] == -10000000)
									{
										str = str + "y2('" + to_string(i) + "','" + to_string(j) + "') = -10000000; \n";
										str = str + "bb.up('" + to_string(i) + "','" + to_string(j) + "') = 0; \n";
									}
									else
									{
										str = str + "y2('" + to_string(i) + "','" + to_string(j) + "') = " + to_string(noeud.getYTild()[j - 1][i - 1]) + "; \n";
									}
								}
								if (yy[i - 1] == -numeric_limits<double>::infinity() || yy[i - 1] == -10000000)
								{
									str = str + "y2('" + to_string(i) + "','" + to_string(noeud.getYTild().size() + 1) + "') = -10000000; \n";
									str = str + "bb.up('" + to_string(i) + "','" + to_string(noeud.getYTild().size() + 1) + "') = 0; \n";
								}
								else
								{
									str = str + "y2('" + to_string(i) + "','" + to_string(noeud.getYTild().size() + 1) + "') = " + to_string(yy[i - 1]) + "; \n";
								}
								if (yyy[i - 1] == -numeric_limits<double>::infinity() || yyy[i - 1] == -10000000)
								{
									str = str + "y2('" + to_string(i) + "','" + to_string(noeud.getYTild().size() + 2) + "') = -10000000; \n";
									str = str + "bb.up('" + to_string(i) + "','" + to_string(noeud.getYTild().size() + 2) + "') = 0; \n";
								}
								else
								{
									str = str + "y2('" + to_string(i) + "','" + to_string(noeud.getYTild().size() + 2) + "') = " + to_string(yyy[i - 1]) + "; \n";
								}
								str = str + "y3('" + to_string(i) + "') = " + to_string(y[i - 1]) + "; \n";
								str = str + "y5('" + to_string(i) + "') = 10000000;\n";
							}
							GAMSWorkspaceInfo wsi(GAMSEnum::ShowLog);
							GAMSWorkspace ws2(wsi);
							GAMSJob t2 = ws2.addJobFromString(str + "\nsolve boland min obj use mip; ms = boland.modelstat; ss = boland.solvestat;");
							t2.run();
							//cout << "str : " << endl << str;
							if (t2.outDB().getParameter("ms").findRecord().value() != 1 && t2.outDB().getParameter("ms").findRecord().value() != 8)
							{
								cout << "in12" << endl;
								nb_non_eff++;
							}
							else
							{
								cout << "in13" << endl;
								vector<double> x_o(noeud.getX_0());
								for (GAMSVariableRecord rec : t2.outDB().getVariable("x"))
									x_o[stoi(rec.key(0)) - 1] = rec.level();
								cout << "x_o : ";
								afficher(x_o);
								vector<double> z_x_o = Z(C, x_o);
								cout << "z_x_o = ";
								afficher(z_x_o);
								double f_x_o = ZLinear(obj, x_o);
								if (ZLinear(obj, x_o) < f_u)
								{
									cout << "in14" << endl;
									vector<vector<double> > YTild(noeud.getYTild());
									YTild.push_back(yy);
									YTild.push_back(yyy);
									queue.push_back(Noeud(noeud.getL(), f_x_o, x_o, YTild, vector<double>(), y));
								}
							}
						}
					}
					// Compute ^y
					vector<double> yy;
					if (noeud.getY_L().empty())
					{
						for (int i = 0; i < (noeud.getL()); i++)
						{
							yy.push_back(y[i]);
						}
					}
					else
					{
						for (int i = 0; i < (noeud.getL()); i++)
						{
							if (y[i] > noeud.getY_L()[i])
							{
								yy.push_back(y[i]);
							}
							else
							{
								yy.push_back(-numeric_limits<double>::infinity());
							}
						}
					}
					cout << "yy^^ : ";
					afficher(yy);
					if (!yEqualInfinityMinus(yy))
					{
						cout << "in15" << endl;
						string str = model_opt + "\nScalar ms 'model status', ss 'solve status';\n";
						if (!noeud.getYTild().empty())
						{
							str = str + "Set h 'Set of YTilds' /1*" + to_string(noeud.getYTild().size()) + "/;\n";
							str = str + "Parameter y2(l,h) 'Upper Lin Bound on Y_Tild';\n";
							str = str + "Binary Variable \n\tbb(l,h) 'choice2 on Y_Tild';\n";
							str = str + "Equation\n\tbsl(l,h) 'borne sup lin on Y_Tild'\n\tfixb(h) 'fix b to 1';\n";
							str = str + "bsl(l,h)..\tsol(l) =l= (y2(l,h) - epsilon - MM(l))*bb(l,h) + MM(l);\n";
							str = str + "fixb(h)..\tsum(l, bb(l,h)) =e= 1;\n";
						}
						str = str + "Model boland /all/;\noption MIP = cplex;\n";
						for (int i = 1; i <= noeud.getL(); i++)
						{
							for (unsigned int j = 1; j <= noeud.getYTild().size(); j++)
							{
								if (noeud.getYTild()[j - 1][i - 1] == -numeric_limits<double>::infinity() || noeud.getYTild()[j - 1][i - 1] == -10000000)
								{
									str = str + "y2('" + to_string(i) + "','" + to_string(j) + "') = -10000000; \n";
									str = str + "bb.up('" + to_string(i) + "','" + to_string(j) + "') = 0; \n";
								}
								else
								{
									str = str + "y2('" + to_string(i) + "','" + to_string(j) + "') = " + to_string(noeud.getYTild()[j - 1][i - 1]) + "; \n";
								}
							}
							if (noeud.getY_L().empty())
							{
								str = str + "y3('" + to_string(i) + "') = -10000000;\n";
							}
							else if (noeud.getY_L()[i - 1] == -numeric_limits<double>::infinity())
							{
								str = str + "y3('" + to_string(i) + "') = -10000000;\n";
							}
							else
							{
								str = str + "y3('" + to_string(i) + "') = " + to_string(noeud.getY_L()[i - 1]) + ";\n";
							}
							if (yy[i - 1] == -numeric_limits<double>::infinity())
							{
								str = str + "y5('" + to_string(i) + "') = 10000000;\n";
							}
							else
							{
								str = str + "y5('" + to_string(i) + "') = " + to_string(yy[i - 1]) + ";\n";
							}
						}
						//cout << "str : " << endl << str;
						GAMSWorkspaceInfo wsi(GAMSEnum::ShowLog);
						GAMSWorkspace ws2(wsi);
						GAMSJob t2 = ws2.addJobFromString(str + "\nsolve boland min obj use mip; ms = boland.modelstat; ss = boland.solvestat;");
						t2.run();
						if (t2.outDB().getParameter("ms").findRecord().value() != 1 && t2.outDB().getParameter("ms").findRecord().value() != 8)
						{
							cout << "in16" << endl;
							nb_non_eff++;
						}
						else
						{
							cout << "in17" << endl;
							vector<double> x_o(noeud.getX_0());
							for (GAMSVariableRecord rec : t2.outDB().getVariable("x"))
								x_o[stoi(rec.key(0)) - 1] = rec.level();
							cout << "x_o : ";
							afficher(x_o);
							vector<double> z_x_o = Z(C, x_o);
							cout << "z_x_o = ";
							afficher(z_x_o);
							double f_x_o = ZLinear(obj, x_o);
							if (f_x_o < f_u)
							{
								cout << "in18" << endl;
								queue.push_back(Noeud(noeud.getL(), f_x_o, x_o, noeud.getYTild(), yy, noeud.getY_L()));
							}
						}
					}
				}
			}
		}
		else
		{
			cout << "in19" << endl;
			endSearch = true;
		}
	}
}

int main(int argc, char* argv[])
{
	vector<vector<double> > SND;
	vector<int> H, N;
	clock_t beg, ed, tm;
	double z = numeric_limits<double>::infinity();
	vector<Noeud> queue;

	
	cout << "0" << endl;
	readData();
	vector<double> x(C[0].size(), 0);
	cout << "1" << endl;
	try {
		GAMSWorkspaceInfo wsInfo(GAMSEnum::ShowLog);
		if (argc > 1)
			wsInfo.setSystemDirectory(argv[1]);
		cout << "2" << endl;
		GAMSWorkspace ws(wsInfo);
		GAMSCheckpoint cp = ws.addCheckpoint();

		cout << "3" << endl;
		cout << "4" << endl;
		cout << "5" << endl;

		GAMSWorkspaceInfo wsInfo3(GAMSEnum::ShowLog);
		if (argc > 1)
			wsInfo3.setSystemDirectory(argv[1]);

		GAMSWorkspace ws3(wsInfo3);
		GAMSCheckpoint cp3 = ws3.addCheckpoint();

		ws3.addJobFromFile(_PATH_BEST_OPT_).run(cp3);
		cout << "6" << endl;

		ws.addJobFromFile(_PATH_EFFICIENT_BOLAND_).run(cp);
		cout << "7" << endl;
		beg = clock();
		if (initialPhase(x, queue))
		{
			cout << "8" << endl;
			principalPhase(x, queue, ws, cp, ws3, cp3);
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
	z = ZLinear(obj, x);
	vector<double> zz = Z(C, x);
	cout << endl << "Cout = " << z << endl;
	cout << "Z : ";
	afficher(zz);
	cout << "Temps : " << (tm / 1000.0) << " seconds" << endl;
	cout << "Nombre de sol. eff. = " << nb_eff << endl;
	cout << "Nombre de sol. non eff. = " << nb_non_eff << endl;
	char h;
	cin >> h;
	return 0;
}