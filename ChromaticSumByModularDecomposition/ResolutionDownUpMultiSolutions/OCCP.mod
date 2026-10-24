/*********************************************
 * OPL 12.6.0.0 Model
 * Author: HP
 * Creation Date: 10 nov. 2016 at 08:56:12
 *********************************************/
int n = ...;
int m = ...;
int delta = ...;
range vertices = 1..n;
range cliques = 1..m;
range colours = 1..delta;
int A[cliques][vertices] = ...;
int p[vertices] = ...;

dvar int x[vertices][colours] in 0..1;

minimize
  sum(j in vertices, l in colours) l*p[j]*x[j][l];
  
subject to {
	forall(i in cliques, l in colours)
	  stable:    	
	  	sum(j in vertices) A[i][j] * x[j][l] <= 1;
	forall(j in vertices)
	  colour:
	  	sum(l in colours) x[j][l] == 1;
}