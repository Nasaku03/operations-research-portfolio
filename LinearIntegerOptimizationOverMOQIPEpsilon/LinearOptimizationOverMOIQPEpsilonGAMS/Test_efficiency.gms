Set j 'Variables' /1*2/;
Set i 'Constraints' /1*3/;
Set q 'Quadratic Objectives' /1*1/;
Set l 'linear Objectives' /1*1/;
Alias(j,k);
Parameters
        A(i, j) 'matrix of constraints'
        b(i) 'second Member vector'
        QQ(q,j,j) 'Quadratic objectives matrix'
        LQ(q,j) 'Linear part of quadratic objectives'
        LL(l,j) 'Linear objectives'
        y(j) 'Given solution to test';
Variable
        x(j) 'principle variables'
        obj 'Objective';
Integer Variable x;
Positive Variables
        t(q) 'added variables for quadratic'
        t2(l)'added variables for linear';
Equations
        ff 'Objective function'
        const(i)'Principle constraints'
        up(q) 'Upper bounds of quadratic objectives'
        lo(l) 'Lower bounds of linear objectives';
ff..    obj =e= sum(q, t(q)) + sum(l, t2(l));
const(i)..sum(j, A(i, j) * x(j)) =l= b(i);
up(q)..    sum((j,k), QQ(q,j,k)*x(j)*x(k)) + sum(j, LQ(q,j)*x(j)) =e= sum((j,k), QQ(q,j,k)*y(j)*y(k)) + sum(j, LQ(q,j)*y(j)) - t(q);
lo(l)..    sum(j, LL(l,j)*x(j)) =e= sum(j, LL(l,j)*y(j)) - t2(l);
A('1','1') = 1;
A('1','2') = 1;
A('2','1') = 2;
A('2','2') = -1;
A('3','1') = -1;
A('3','2') = 3;
b('1') = 10.000000;
b('2') = 7.000000;
b('3') = 12.000000;
QQ('1','1','1') = 2;
QQ('1','1','2') = 0;
QQ('1','2','1') = -1;
QQ('1','2','2') = 3;
LQ('1','1') = 1;
LQ('1','2') = -10;
LL('1','1') = 1;
LL('1','2') = 3;
model principle /all/;

Scalar ms 'model status', ss 'solve status';
y('1') = 5.000000;
y('2') = 5.000000;

option miqcp = lindoglobal;
solve principle using miqcp max obj;
ms = principle.modelstat; ss = principle.solvestat;
display ms;
