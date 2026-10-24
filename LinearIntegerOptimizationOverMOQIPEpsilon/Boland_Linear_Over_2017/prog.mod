int n = ...;
int m = ...;
int q = ...;
int p = ...;
range vare = 1..n;
range con = 1..m;
range objQ = 1..q;
range objL = 1..p;
range obj = 1..(p+q);
int A[con][vare] = ...;
int b[con] = ...;
int Q1[objQ][vare][vare] = ...;
int cm1[objQ][vare] = ...;
int c1[objL][vare] = ...;
int y0[obj] = ...;
int y1[obj] = ...;
dvar int x[vare];
dvar float z[obj];

maximize
  sum(k in objQ) z[k] + sum(k in objL) z[k+q];
subject to {
	forall(i in con)
	  const:
		sum(j in vare) A[i][j]*x[j] <= b[i];
	forall(i in objQ)
	  objqcons1:
		z[i] == sum(j, k in vare) Q1[i][k][j]*x[j]*x[k] + sum(j in vare) cm1[i][j]*x[j] ;
	  objlcons1:
	forall(i in objL)
		z[i+q] == sum(j in vare) c1[i][j]*x[j];
	forall(k in obj)
		z[k] <= y0[k];
	forall(k in obj)
		z[k] >= y1[k];
	forall(j in vare)
	  positive:
		x[j] >= 0;
}