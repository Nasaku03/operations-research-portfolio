set vare;
set con;
set objL;
param A{i in con, j in vare};
param b{i in con};
param c{i in vare};
param y2; 
param y3;
var x {i in vare}, >=0, integer;

maximize obj: sum{j in vare} c[j]*x[j];

subject to const {i in con}: sum{j in vare} A[i,j]*x[j] <= b[i];
subject to borne_supL: sum{j in vare} c[j]*x[j] <= y2;
subject to borne_infL: sum{j in vare} c[j]*x[j] >= y3;
