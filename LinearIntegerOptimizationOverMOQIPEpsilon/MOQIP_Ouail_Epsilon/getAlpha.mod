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
int hl[vare] = ...;
dvar int x[vare];
dvar int alpha;
maximize
  alpha;
subject to {
	forall(i in con)
	  const:
		sum(j in vare) A[i][j]*x[j] <= b[i];
	forall(i in cone)
	  consteg:
		sum(j in vare) Ae[i][j]*x[j] == be[i];
	  borneMin:
		sum(j in vare) hl[j]*x[j] >= 1;
	  borneMax:
		sum(j in vare) hl[j]*x[j] <= alpha;
	  alphaMin:
		alpha >= 1;
	forall(j in vare)
	  positive:
		x[j] >= 0;
}