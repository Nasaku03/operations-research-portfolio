int n = ...;
int m = ...;
int me = ...;
range vare = 1..n;
range con = 1..m;
range cone = 0..me;
int A[con][vare] = ...;
int b[con] = ...;
int Ae[cone][vare] = ...;
int be[cone] = ...;
int Q1[vare][vare] = ...;
int cm1[vare] = ...;
int c1[vare] = ...;
int x0[vare] = ...;
float cut[vare] = ...;
float cut2[vare] = ...;
float b1 = ...;
float b2 = ...;
dvar int x[vare];
dvar float epsilon;
minimize
  epsilon;
subject to {
	forall(i in con)
	  const:
		sum(j in vare) A[i][j]*x[j] <= b[i];
	forall(i in cone)
	  consteg:
		sum(j in vare) Ae[i][j]*x[j] == be[i];
	  minEpsilon:
		sum(j in vare) cut[j]*x[j] <= b1;
	  maxEpsilon:
		-epsilon + sum(j in vare) cut2[j]*x[j] <= b2;
	  objqcons1:
		sum(j, k in vare) Q1[k][j]*x[j]*x[k] + sum(j in vare) cm1[j]*x[j] <= sum(j, k in vare) Q1[k][j]*x0[j]*x0[k] + sum(j in vare) cm1[j]*x0[j];
	  objlcons1:
		sum(j in vare) c1[j]*x[j] <= sum(j in vare) c1[j]*x0[j];
	forall(j in vare)
	  positive:
		x[j] >= 0;
	  positiveEpsilon:
		epsilon >= 0;
}