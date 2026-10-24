/*********************************************
 * OPL 12.6.0.0 Model
 * Author: HP
 * Creation Date: 31 déc. 2016 at 19:00:57
 *********************************************/
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
int M[var][var] = ...;

dvar int x[var];

minimize
  sum(j1, j2 in var) M[j1][j2]*x[j1]*x[j2];
  
subject to {
	forall(i in con)
	  const:
	  	sum(j in var) A[i][j]*x[j] <= b[i];
	forall(i in cone)
	  consteg:
	  	sum(j in var) Ae[i][j]*x[j] == be[i];
	forall(j in var)
	  positive:
	    x[j] >= 0;
}