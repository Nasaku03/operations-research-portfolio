int n = ...;
int m = ...;
int delta = ...;
range vertices = 1..n;
range edges = 1..m;
range colours = 1..delta;
range coulours2 = 2..delta;
int A[edges][vertices] = ...;
int P[colours] = ...;

dvar int x[vertices][colours] in 0..1;

minimize
  sum(j in vertices, l in colours) l*x[j][l];
  
subject to {
	forall(i in edges, l in colours)
	  stable:    	
	  	sum(j in vertices) A[i][j] * x[j][l] <= 1;
	forall(j in vertices)
	  colour:
	  	sum(l in colours) x[j][l] == 1;
	forall(l in colours)
	  fix:
	    sum(j in vertices) x[j][l] == P[l];
}