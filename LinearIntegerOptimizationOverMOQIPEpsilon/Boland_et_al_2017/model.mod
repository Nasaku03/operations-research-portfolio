set vare;
set con;
set objQ;
set objL;
param A{i in con, j in vare};
param b{i in con};
param Q{j in vare, k in vare};
param cm{i in vare};
param c{i in vare};
param y0;
param y1;
param y2; 
param y3;
var x {i in vare}, >=0, integer;

maximize obj: sum{j in vare, k in vare} Q[j,k]*x[j]*x[k] + sum{j in vare} cm[j]*x[j] + sum{j in vare} c[j]*x[j];

subject to const {i in con}: sum{j in vare} A[i,j]*x[j] <= b[i];
subject to borne_supQ: sum{j in vare, k in vare} Q[k,j]*x[j]*x[k] + sum{j in vare} cm[j]*x[j] <= y0;
subject to borne_infQ: sum{j in vare, k in vare} Q[k,j]*x[j]*x[k] + sum{j in vare} cm[j]*x[j] >= y1;
subject to borne_supL: sum{j in vare} c[j]*x[j] <= y2;
subject to borne_infL: sum{j in vare} c[j]*x[j] >= y3;
