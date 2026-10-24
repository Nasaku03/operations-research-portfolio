Set j 'Variables' /1*8/;
Set i 'Constraints' /1*8/;
Alias (i,ip), (j,jp);
Parameter
        a(i, j) 'matrix to be inverted'
        b(j, i) 'inverse of a'
        bp(i, j)'permuted and transposed inverse of a'
        pair(i, j) 'pivoting sequence and permutation'
        rank 'rank of matrix a'
        adet 'absolute value of determinant of matrix a'
        piv
        big
        tol;
Set
        r(i) 'pivot row candidates'
        npr(i) 'non pivot rows'
        s(j) 'pivot column candidates'
        nps(j) 'non pivot columns';
a('1','1') = 1.000000;
a('1','2') = 1.000000;
a('1','3') = 1.000000;
a('1','4') = 0.000000;
a('1','5') = 0.000000;
a('1','6') = 0.000000;
a('1','7') = 0.000000;
a('2','1') = 2.000000;
a('2','2') = -1.000000;
a('2','3') = 0.000000;
a('2','4') = 1.000000;
a('2','5') = 0.000000;
a('2','6') = 0.000000;
a('2','7') = 0.000000;
a('3','1') = -1.000000;
a('3','2') = 3.000000;
a('3','3') = 0.000000;
a('3','4') = 0.000000;
a('3','5') = 1.000000;
a('3','6') = 0.000000;
a('3','7') = 0.000000;
a('4','1') = 1.000000;
a('4','2') = 0.000000;
a('4','3') = 0.000000;
a('4','4') = 0.000000;
a('4','5') = 0.000000;
a('4','6') = 0.000000;
a('4','7') = 0.000000;
a('5','1') = 0.000000;
a('5','2') = 1.000000;
a('5','3') = 0.000000;
a('5','4') = 0.000000;
a('5','5') = 0.000000;
a('5','6') = 0.000000;
a('5','7') = 0.000000;
a('6','1') = -1.000000;
a('6','2') = 0.000000;
a('6','3') = 0.000000;
a('6','4') = 0.000000;
a('6','5') = 0.000000;
a('6','6') = 1.000000;
a('6','7') = 0.000000;
a('7','1') = 0.000000;
a('7','2') = -1.000000;
a('7','3') = 0.000000;
a('7','4') = 0.000000;
a('7','5') = 0.000000;
a('7','6') = 0.000000;
a('7','7') = 1.000000;
a('1','8') = 0.000000;
a('2','8') = 0.000000;
a('3','8') = 0.000000;
a('4','8') = 0.000000;
a('5','8') = 0.000000;
a('6','8') = 0.000000;
a('7','8') = 0.000000;
a('8','1') = 2.000000;
a('8','2') = 1.000000;
a('8','3') = 0.000000;
a('8','4') = 0.000000;
a('8','5') = 0.000000;
a('8','6') = 0.000000;
a('8','7') = 0.000000;
a('8','8') = 1.000000;
r(i) = yes;
s(j) = yes;
bp(i, j) = a(i, j);
rank = 0;
adet = 1;
tol = 1e-5;
loop(j,
        big = smax((r, s), abs(bp(r, s)));
        big$(big < tol) = 0;
        npr(i) = yes;
        nps(jp) = yes;
        loop((r, s)$(big and big = abs(bp(r, s))),
                rank = rank + 1;
                pair(r, s) = rank;
                piv = 1 / bp(r, s);
                big = 0;
                adet = adet / piv;
                npr(r) = no;
                nps(s) = no;
                bp(r, nps) = bp(r, nps) * piv;
                bp(npr, nps) = bp(npr, nps) - bp(r, nps) * bp(npr, s);
                bp(npr, s) = -bp(npr, s) * piv;
                bp(r, s) = piv;
        );
        r(r) = npr(r);
        s(s) = nps(s);
);
b(j, i) = sum((ip, jp)$(pair(i, jp) and pair(ip, j)), bp(ip, jp));
adet = abs(adet);
loop((i,j)$(abs(b(j,i)) < tol), b(j,i) = 0);

display a, rank, adet, pair, bp, b;