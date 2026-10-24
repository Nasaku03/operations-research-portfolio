int n = ...;
int m = ...;
int p = ...;
range vare = 1..n;
range con = 1..m;
range obj = 1..p;
int A[con][vare] = ...;
int b[con] = ...;
int c1[vare] = ...;
int c2[vare] = ...;
int x0[vare] = ...;
dvar int x[vare];
dvar float t[obj];
maximize
  sum(k in obj) t[k];
subject to {
	forall(i in con)
	  const:
		sum(j in vare) A[i][j]*x[j] <= b[i];
	  objlcons1:
		t[1] + sum(j in vare) c1[j]*x[j] == sum(j in vare) c1[j]*x0[j];
	  objlcons2:
		t[2] + sum(j in vare) c2[j]*x[j] == sum(j in vare) c2[j]*x0[j];
	forall(j in vare)
	  positive:
		x[j] >= 0;
	forall(j in obj)
	  positiveT:
		t[j] >= 0;
}