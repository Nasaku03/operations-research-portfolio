int n = ...;
int m = ...;
range vare = 1..n;
range con = 1..m;
int A[con][vare] = ...;
int b[con] = ...;
float hl[vare] = ...;
float scb = ...;
dvar int x[vare];
dvar float epsilon;
minimize
  epsilon;
subject to {
	forall(i in con)
	  const:
		sum(j in vare) A[i][j]*x[j] <= b[i];
	  borneMax:
		epsilon + sum(j in vare) hl[j]*x[j] == scb;
	  epsilonMin:
		epsilon >= 0;
	forall(j in vare)
	  positive:
		x[j] >= 0;
}