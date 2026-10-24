int n = ...;
int m = ...;
int me = ...;
int p = ...;
range vare = 1..n;
range con = 1..m;
range cone = 0..me;
range obj = 1..p;
int A[con][vare] = ...;
int b[con] = ...;
int Ae[cone][vare] = ...;
int be[cone] = ...;
int Q1[vare][vare] = ...;
int cm1[vare] = ...;
int c1[vare] = ...;
int x0[vare] = ...;
dvar int x[vare];
dvar float t[obj];
maximize
  sum(k in obj) t[k];
subject to {
	forall(i in con)
	  const:
		sum(j in vare) A[i][j]*x[j] <= b[i];
	forall(i in cone)
	  consteg:
		sum(j in vare) Ae[i][j]*x[j] == be[i];
	  objqcons1:
		t[1] + sum(j, k in vare) Q1[k][j]*x[j]*x[k] + sum(j in vare) cm1[j]*x[j] == sum(j, k in vare) Q1[k][j]*x0[j]*x0[k] + sum(j in vare) cm1[j]*x0[j];
	  objlcons1:
		t[2] + sum(j in vare) c1[j]*x[j] == sum(j in vare) c1[j]*x0[j];
	forall(j in vare)
	  positive:
		x[j] >= 0;
	forall(j in obj)
	  positiveT:
		t[j] >= 0;
}