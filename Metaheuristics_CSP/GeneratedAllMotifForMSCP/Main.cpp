#include <ilopl/iloopl.h>

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

using namespace std;

void readData(unsigned int& n, unsigned int&m, vector<vector<int> >& A)
{
	//cout << "read data" << endl;
	ifstream fichier("input.txt");
	vector<int> in;
	in.push_back(1);
	in.push_back(2);
	in.push_back(3);
	in.push_back(4);
	in.push_back(5);
	in.push_back(6);
	in.push_back(7);
	in.push_back(8);
	in.push_back(9);
	in.push_back(10);
	in.push_back(11);
	in.push_back(12);
	in.push_back(13);
	in.push_back(14);
	in.push_back(15);
	in.push_back(16);
	in.push_back(17);
	in.push_back(18);
	in.push_back(19);
	in.push_back(20);
	in.push_back(21);
	in.push_back(22);
	in.push_back(23);
	in.push_back(24);
	in.push_back(25);
	in.push_back(26);
	in.push_back(27);
	in.push_back(28);
	in.push_back(29);
	in.push_back(30);
	if (fichier)
	{
		int mot; //Une variable pour stocker les lignes lues
		fichier >> n;
		fichier >> m;
		vector<int> inter;
		
		for (unsigned int i = 0; i<m; i++)
		{
			cout << " i = " << (i+1) << endl;
			int e1 = -1, e2 = -1;
			for (unsigned int j = 0; j<n; j++)
			{
				cout << "j = " << (j + 1) << endl;
				if (find(in.begin(), in.end(), (j+1)) != in.end())
				{
					fichier >> mot;				
					inter.push_back(mot);
					if (mot == 1 && e1 == -1)
					{
						e1 = j;
						cout << "e1 = " << e1 << endl;
					}
					else if(mot == 1 && e2 == -1)
					{
						e2 = j;
						cout << "e2 = " << e2 << endl;
					}
				}
				else
				{
					fichier >> mot;
				}
			}
			if (e1 != -1 && e2 != -1)
			{
				A.push_back(inter);
				cout << "in" << endl;
			}
			else
			{
				cout << "out" << endl;
			}
			inter.clear();
		}
		fichier.close();
		n = A[0].size();
		m = A.size();
	}
	else
	{
		//cout << "We are out" << endl;
	}
}

void lectureDimacs(unsigned int& n, unsigned int& m, vector<vector<int> > & A)
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
	vector<unsigned int> somme(n, 0);
	for (unsigned int j = 0; j<n; j++)
	{
		for (unsigned int i = 0; i<A.size(); i++)
		{
			somme[j] += A[i][j];
		}
	}
}

void writeDatFile(unsigned int& n, unsigned int& m, int delta, vector<vector<int> >& A, vector<vector<int> >& B, vector<int>& motif)
{
	//cout << "WriteFileAlpha" << endl;	
	ofstream data;
	data.open("getSolution.dat", ofstream::trunc);
	data << "n = " << n << ";\nm = " << m << ";\ndelta = " << delta << ";\nA = [";
	for (unsigned int i = 0; i < m; i++)
	{
		data << "[";
		for (unsigned int j = 0; j < n; j++)
		{
			data << A[i][j] << " ";
		}
		if (i < m - 1)
		{
			data << "],\n";
		}
		else
		{
			data << "]];\nP = [";
		}
	}
	/*for (unsigned int i = 0; i < n; i++)
	{
		data << "[";
		for (unsigned int j = 0; j < n; j++)
		{
			data << B[i][j] << " ";
		}
		if (i < n - 1)
		{
			data << "],\n";
		}
		else
		{
			data << "]];\nP = [";
		}
	}*/
	for (int k = 0; k < delta; k++)
	{
		data << motif[k] << " ";
	}
	data << "];" << endl;
	data.close();
}

int getCout(vector<int> sol) {
	int cout = 0;
	for (int i = 0; i<sol.size(); i++) {
		cout += sol[i]*(i + 1);
	}
	return cout;
}

map<int, vector<int> > gToListAdj(vector<vector<int> >& A)
{
	map<int, vector<int> > out;
	for (int j = 0; j<A[0].size(); j++)
	{
		vector<int> inter;
		for (int i = 0; i<A.size(); i++)
		{
			if (A[i][j] == 1)
			{
				for (int k = 0; k<A[0].size(); k++)
				{
					if (j != k && A[i][k] == 1 && find(inter.begin(), inter.end(), k+1) == inter.end())
					{
						inter.push_back(k + 1);
						break;
					}
				}
			}
		}
		out[j + 1] = inter;
	}
	return out;
}

vector<vector<int> > gtoAdj(map<int, vector<int> >& G)
{
	vector<vector<int> > A;
	for (map<int, vector<int> >::iterator it = G.begin(); it != G.end(); it++)
	{
		vector<int> inter(G.size(), 0);
		for (vector<int>::iterator it2 = G[it->first].begin(); it2 != G[it->first].end(); it2++)
		{
			inter[(*it2) - 1] = 1;
		}
		if (!G[it->first].empty())
		{
			inter[(it->first) - 1] = 1;
		}
		A.push_back(inter);
	}
	return A;
}

void afficher(vector<vector<int> >& P)
{
	cout << '{';
	for (int i = 0; i<P.size(); i++)
	{
		cout << '{';
		for (int j = 0; j<P[i].size(); j++)
		{
			cout << P[i][j] << ' ';
		}
		cout << "} ";
	}
	cout << '}' << endl;
}

void afficher(map<int, vector<int> >& P)
{
	cout << '{' << endl;
	for (map<int, vector<int> >::iterator it = P.begin(); it != P.end(); it++)
	{
		cout << it->first << " : ";
		for (int j = 0; j<(it->second).size(); j++)
		{
			cout << (it->second)[j] << ' ';
		}
		cout << endl;
	}
	cout << '}' << endl;
}

int main()
{
	// TODO code application logic here
	unsigned int n, m;
	vector<vector<int> > A;
	//readData(n, m, A);
	lectureDimacs(n, m, A);
	map<int, vector<int> > G = gToListAdj(A);
	cout << "G : " << endl;
	afficher(G);
	vector<int> nTab, kTab, coutTab, difTab;
	vector<vector<int> > motifTab;
	vector<double> difkTab;
	for (int i = n; i <= n; i++) {
		vector<vector<int> > motifList, stockMotifList;
		for (int k = i; k>0; k--) {
			vector<int> sous;
			for (int l = 0; l<k; l++) {
				sous.push_back(0);
			}
			//System.out.println("n = "+n+"\nk = "+k);
			if (k == i) {
				for (int l = 0; l<k; l++) {
					sous[l] = 1;
				}
				motifTab.push_back(sous);
				coutTab.push_back(getCout(sous));
				motifList.push_back(sous);
				nTab.push_back(i);
				kTab.push_back(k);
			}
			else if (k == 1) {
				sous[0] = i;
				motifTab.push_back(sous);
				coutTab.push_back(getCout(sous));
				motifList.push_back(sous);
				nTab.push_back(i);
				kTab.push_back(k);
			}
			else {
				for (int j = 0; j<motifList.size(); j++) {
					for (int l = motifList[j].size() - 1; l>0; l--) {
						if (l == 1) {
							vector<int> copie(sous);
							sous[l - 1] = motifList[j][l] + motifList[j][l - 1];
							for (int t = l - 2; t >= 0; t--) {
								sous[t] = motifList[j][t];
							}
							if (find(motifTab.begin(), motifTab.end(), sous) == motifTab.end())
							{
								motifTab.push_back(sous);
								nTab.push_back(i);
								kTab.push_back(k);
								stockMotifList.push_back(sous);
								coutTab.push_back(getCout(sous));
							}
							sous = copie;
							sous[l - 1] = motifList[j][l];
						}
						else if ((motifList[j][l] + motifList[j][l - 1]) <= motifList[j][l - 2]) {
							vector<int> copie(sous);
							sous[l - 1] = motifList[j][l] + motifList[j][l - 1];
							for (int t = l - 2; t >= 0; t--) {
								sous[t] = motifList[j][t];
							}
							if (find(motifTab.begin(), motifTab.end(), sous) == motifTab.end())
							{
								motifTab.push_back(sous);
								nTab.push_back(i);
								kTab.push_back(k);
								stockMotifList.push_back(sous);
								coutTab.push_back(getCout(sous));
							}
							sous = copie;
							sous[l - 1] = motifList[j][l];
						}
						else {
							sous[l - 1] =  motifList[j][l];
						}
					}
				}
				motifList = stockMotifList;
				stockMotifList.clear();
			}
		}
	}
	vector<int> nTab_, kTab_, coutTab_;
	vector<vector<int> > motifTab_;
	for (int i = nTab.size() - 1; i>-1; i--) {
		nTab_.push_back(nTab[i]);
		kTab_.push_back(kTab[i]);
		motifTab_.push_back(motifTab[i]);
		coutTab_.push_back(coutTab[i]);
		int sec = motifTab[i][0] - motifTab[i][motifTab[i].size() - 1];
		difTab.push_back(sec);
		difkTab.push_back((double)sec / (double)kTab[i]);
	}
	ofstream data, sol;
	data.open("Solutions.csv", ofstream::trunc);
	sol.open("Sol.txt");
	int status;
	vector<vector<int> > B = gtoAdj(G);
	for (int i = 0; i<nTab.size(); i++) {
		try
		{
			clock_t db2 = clock();
			writeDatFile(n, m, motifTab_[i].size(), A, B, motifTab_[i]);
			IloEnv env;
			IloOplRunConfiguration rc1(env, "getSolution.mod", "getSolution.dat");
			rc1.getCplex().setParam(IloCplex::NodeSel, CPX_NODESEL_DFS);
			rc1.getCplex().setParam(IloCplex::Param::MIP::Strategy::MIQCPStrat, 1);
			rc1.getCplex().setOut(env.getNullStream());
			rc1.getOplModel().generate();
			IloRangeArray c(rc1.getOplModel().getEnv());
			IloIntVarArray x = rc1.getOplModel().getElement("x").asIntVarMap().asNewIntVarArray();
			IloInt delta = rc1.getOplModel().getElement("delta").asInt();
			IloInt n = G.size();
			//x.add(IloIntVar(rc1.getOplModel().getEnv(), 0, 1, "x1"));
			//x.add(IloIntVar(rc1.getOplModel().getEnv(), 0, 1, "x2"));
			//x.add(IloIntVar(rc1.getOplModel().getEnv(), 0, 1, "x3"));
			/*for (int i = 0; i < n; i++)
			{
				for (int j = 1; j < delta; j++)
				{
					for (int k = 0; k < j; k++)
					{
						IloNumExpr cons(rc1.getOplModel().getEnv());
						for (int l = 0; l < n; l++)
						{
							cons += B[i][l] * x[l*delta+k];
						}
						cons += x[i*delta + j];
						c.add(cons >= 1);
					}
				}
			}
			rc1.getOplModel().getModel().add(c);
			rc1.getCplex().extract(rc1.getOplModel().getModel());*/
			rc1.getCplex().exportModel("test.lp");
			if (rc1.getCplex().solve())
			{
				cout << nTab_[i] << "\t" << kTab_[i] << "\t[";
				data << nTab_[i] << ";" << kTab_[i] << ";[";
				for (int j = 0; j < motifTab_[i].size(); j++)
				{
					if (j < motifTab_[i].size() - 1)
					{
						cout << motifTab_[i][j] << ", ";
						data << motifTab_[i][j] << ", ";
					}
					else
					{
						cout << motifTab_[i][j];
						data << motifTab_[i][j];
					}
				}
				cout << "]\t" << coutTab_[i] << "\t" << difTab[i] << "\t" << difkTab[i] << "\t" << motifTab_[i][0] << endl;
				data << "];" << coutTab_[i] << ";" << 1;
				IloNumArray vals(env);
				IloIntMap t = rc1.getOplModel().getElement("x").asIntMap();
				vector<vector<int> > x, S;
				for (IloInt i = 1; i <= t.getSize(); i++)
				{
					IloIntMap t_sub(t.getSub(i));
					vector<int> t_;
					S.push_back(vector<int>());
					for (IloInt j = 1; j <= t_sub.getSize(); j++)
					{
						//cout << t_sub.get(j) << " ";
						t_.push_back(t_sub.get(j));
					}
					//cout << '\n';
					x.push_back(t_);
				}
				for (int i = 0; i < x.size(); i++)
				{
					for (int j = 0; j < x[i].size(); j++)
					{
						if (x[i][j] == 1)
						{
							S[j].push_back(i);
						}
					}
				}
				cout << "S : " << endl;
				afficher(S);
				bool oneMove = false;
				for (int i = S.size() - 1; i > 0; i--)
				{					
					if (!S[i].empty())
					{
						for (int j = 0; j < S[i].size(); j++)
						{							
							for (int k = 0; k < i; k++)
							{
								bool oneAdj = false;
								for (int l = 0; l < S[k].size(); l++)
								{
									if (find(G[S[i][j]+1].begin(), G[S[i][j]+1].end(), S[k][l]+1) != G[S[i][j]+1].end())
									{
										oneAdj = true;
										break;
									}
								}
								if (!oneAdj)
								{
									oneMove = true;
									cout << "Sij = " << (S[i][j] + 1) << endl;
									cout << "k = " << k << endl;
									break;
								}
							}
							if (oneMove)
							{
								break;
							}
						}
					}
					if (oneMove)
					{
						break;
					}
				}
				if (!oneMove)
				{
					data << ";" << 1 << endl;
				}
				else
				{
					data << ";" << 0 << endl;
				}
			}
			else
			{
				cout << nTab_[i] << "\t" << kTab_[i] << "\t[";
				data << nTab_[i] << ";" << kTab_[i] << ";[";
				for (int j = 0; j < motifTab_[i].size(); j++)
				{
					if (j < motifTab_[i].size() - 1)
					{
						cout << motifTab_[i][j] << ", ";
						data << motifTab_[i][j] << ", ";
					}
					else
					{
						cout << motifTab_[i][j];
						data << motifTab_[i][j];
					}
				}
				cout << "]\t" << coutTab_[i] << "\t" << difTab[i] << "\t" << difkTab[i] << "\t" << motifTab_[i][0] << endl;
				data << "];" << coutTab_[i] << ";" << 0 << ";" << 0 << endl;
			}
		}
		catch (IloOplException & e) {
			//cout << "### OPL exception: " << e.getMessage() << endl;
			status = 1;
		}
		catch (IloException & e) {
			//cout << "### CONCERT exception: ";
			e.print(cout);
			status = 2;
		}
		catch (...) {
			//cout << "### UNEXPECTED ERROR ..." << endl;
			status = 3;
		}
	}
	cout << nTab.size() << endl;

	data.close();
	system("pause");

	return 0;
}