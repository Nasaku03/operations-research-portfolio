int n = ...;
int m = ...;
int me = ...;
range var = 1..n;
range con = 1..m;
range cone = 0..me;
int A[con][var] = ...;
int b[con] = ...;
int Ae[cone][var] = ...;
int be[cone] = ...;
int C[var] = ...;

dvar float x[var];

minimize
  sum(j in var) C[j]*x[j];
  
subject to {
	forall(i in con)
	  const:
	  	sum(j in var) A[i][j]*x[j] >= b[i];
	forall(i in cone)
	  consteg:
	  	sum(j in var) Ae[i][j]*x[j] == be[i];
	forall(j in var)
	  positive:
	    x[j] >= 0;
}