Set j 'Variables' /1*2/;
Set i 'Constraints' /1*3/;
Parameters
        f(j) 'function to optimize'
        A(i, j) 'matrix of constraints'
        b(i) 'second Member vector'
        uppers(j) 'upper bounds of variables'
        lowers(j) 'lower bounds of variables';
Positive Variables
        x(j) 'principle variables';
Variable
        obj 'Objective';
Equations
        ff 'Objective function'
        const(i)'Principle constraints'
        up(j) 'Upper bounds'
        lo(j) 'Lower bounds';
ff..    obj =e= sum(j, f(j)*x(j));
const(i)..sum(j, A(i, j) * x(j)) =l= b(i);
up(j)..x(j) =l= uppers(j);
lo(j)..x(j) =g= lowers(j);
f('1') = -3.000000;
f('2') = -2.000000;
A('1','1') = 1;
A('1','2') = 1;
A('2','1') = 2;
A('2','2') = -1;
A('3','1') = -1;
A('3','2') = 3;
b('1') = 10.000000;
b('2') = 7.000000;
b('3') = 12.000000;

Scalar ms 'model status', ss 'solve status';
uppers('1') = 5.000000;
lowers('1') = 0.000000;
uppers('2') = 1000.000000;
lowers('2') = 0.000000;

model principle /all/;
solve principle using lp min obj;