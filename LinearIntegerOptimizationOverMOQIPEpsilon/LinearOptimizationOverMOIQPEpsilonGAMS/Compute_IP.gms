Set j 'Variables' /1*2/;
Set i 'Constraints' /1*4/;
Set q 'Quadratic Objectives' /1*1/;
Set l 'linear Objectives' /1*1/;
Alias(j,k);
Parameters
    A(i, j) 'matrix of constraints'
    b(i) 'second Member vector'
    QQ(q,j,j) 'Quadratic objectives matrix'
    LQ(q,j) 'Linear part of quadratic objectives'
    LL(l,j) 'Linear objectives'
    uppers(j) 'upper bounds of variables'
    lowers(j) 'lower bounds of variables';
Variable
    x(j) 'principle variables'
    obj 'Objective';
Integer Variable x;
Equations
    const(i)'Principle constraints'
    up(j) 'Upper bounds'
    lo(j) 'Lower bounds';
const(i)..sum(j, A(i, j) * x(j)) =l= b(i);
up(j)..x(j) =l= uppers(j);
lo(j)..x(j) =g= lowers(j);
A('1','1') = 2;
A('1','2') = 1;
A('2','1') = -1;
A('2','2') = 4;
A('3','1') = 2;
A('3','2') = -1;
A('4','1') = -2;
A('4','2') = 3;
b('1') = 12.000000;
b('2') = 29.000000;
b('3') = 8.000000;
b('4') = 20.000000;
QQ('1','1','1') = 4;
QQ('1','1','2') = 1;
QQ('1','2','1') = 1;
QQ('1','2','2') = 3;
LQ('1','1') = -12;
LQ('1','2') = -6;
LL('1','1') = -14;
LL('1','2') = -9;;
uppers('1') = 2.000000; 
lowers('1') = 0.000000;
uppers('2') = 7.000000; 
lowers('2') = 0.000000;
Scalar ms 'model status', ss 'solve status';
Equation
    newObj 'New objective';
Variable
    nobj 'new objective variable';
newObj..    nobj =e= sum((j,k), QQ('1',j, k)* x(j)* x(k)) + sum(j, LQ('1',j)* x(j)); 
Model ip /all/;
option miqcp = cplex;
solve ip using miqcp min nobj; ms = ip.modelstat; ss = ip.solvestat;

display ms;