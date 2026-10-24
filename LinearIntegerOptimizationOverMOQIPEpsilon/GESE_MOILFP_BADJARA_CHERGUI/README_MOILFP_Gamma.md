# GESE-MOILFP via le coût réduit fractionnaire γ (sans Charnes-Cooper)

Contrairement à la version Charnes-Cooper livrée précédemment, cette variante travaille
**directement en x**, du début à la fin de la branche-and-cut. Elle réutilise donc
beaucoup plus de code de `main.cpp` sans aucune modification.

## Principe

Chaque objectif fractionnaire $f_k(x)=\dfrac{a_k^Tx+b_k}{c_k^Tx+d_k}$ est représenté par
**deux lignes** dans le tableau du simplexe (au lieu d'une seule ligne comme pour un
objectif linéaire) : une ligne pour $a_k$ (numérateur), une pour $c_k$ (dénominateur).
Ces deux lignes se mettent à jour par `pivoting()` **exactement comme n'importe quelle
autre ligne** — aucune modification du simplexe.

À chaque nœud, pour un $x^l$ courant :
- $\hat a_j^k,\hat c_j^k$ = coûts réduits lus directement dans le tableau (déjà à jour)
- $\hat b_k=a_k^Tx^l+b_k$, $\hat d_k=c_k^Tx^l+d_k$ = évalués directement (pas de suivi
  dans le tableau nécessaire)
- $\hat\gamma_j^k=\hat d_k\hat a_j^k-\hat b_k\hat c_j^k$

## Ce qui est repris à l'identique de `main.cpp` (sans AUCUNE modification)

`pivoting`, `simplex`, `dualSimplex`, `buildSimplexMatrix`, `extractSolution`,
`initialSolution`, `detectOutBasesSimplex`, `entiere` (test d'intégrité sur $x$
directement, plus besoin de $x=y/t$), `addCutInfSimplex`, `addCutSupSimplex`
(branchement), `addEfficientCutSimplex`, `addEpsilonEfficientCutSimplex`,
`writeDatFileEpsilon`, `getEpsilon`, **`getEpsilon.mod` tel quel** (le programme (3)/(4)
reste un programme en $x$ entier, sans aucune fraction — c'est le même fichier que pour
le MOILP, vous pouvez littéralement le recopier sans le toucher).

## Ce qui est adapté

| Fonction | Rôle |
|---|---|
| `readDataFrac` | Nouveau format : pour chaque objectif $k$, lit $(a_k,b_k)$ et $(c_k,d_k)$ séparément |
| `buildCFrac` | Construit `C` avec $1+2p$ lignes : trajectoire, puis paires (numérateur, dénominateur) par objectif |
| `detectHlFrac` | Remplace la lecture directe du coût réduit par le calcul de $\gamma_j^k$ (voir ci-dessus). Reproduit fidèlement la même simplification que `detectHl()` original : seule la branche "$\exists k,\gamma_j^k<0$" est testée, pas la branche "$\forall k,\gamma_j^k=0$" de la définition formelle du papier — c'est déjà comme ça dans votre code MOILP existant, je n'ajoute rien de plus strict |
| `fracObjectives` | Évalue les vrais ratios $f_k(x)$ (remplace `Z()` pour le reporting dans `NDS`) |
| `TestEfficienceFrac.mod` + `testEfficiencyFrac` | Test de Benson reformulé linéairement (voir plus bas) |
| `approxIdealFrac` | Point idéal local — **seul endroit où Charnes-Cooper reste utile**, voir plus bas |

## Test de Benson sans division

Le test original (7) s'écrit $f_k(x)=f_k(x^l)-t_k$ — mais $f_k$ est un ratio, donc cette
égalité est non linéaire en $x$ si on la garde telle quelle. Astuce : $f_k(x^l)$ est une
**constante connue** (on vient de la calculer). On pose

$$g_k(x) := f_k(x^l)\,(c_k^Tx+d_k) - (a_k^Tx+b_k)$$

qui est **linéaire en $x$**, et comme $c_k^Tx+d_k>0$ toujours, on a $g_k(x)\ge0
\Leftrightarrow t_k\ge0$ et $g_k(x)=0\Leftrightarrow t_k=0$ — donc tester $\max\sum_k
g_k(x)$ au lieu de $\max\sum_k t_k$ donne exactement la même conclusion (T\*=0 ⟺
efficace), avec exactement la même région réalisable. `TestEfficienceFrac.mod` calcule
$f_k(x^l)$ via un `dexpr` (une simple donnée numérique, pas une variable), donc tout
reste linéaire — pas besoin de CPLEX non linéaire.

## Le seul endroit où Charnes-Cooper reste le bon outil : `approxIdealFrac`

Le point idéal local $\widehat{Id}_l=(f_1(\bar x^1),\dots,f_p(\bar x^p))$ demande de
minimiser **chaque** $f_k$ séparément sur la relaxation continue du nœud — un LP
fractionnaire à résoudre, pas un coût réduit à lire. Ici, contrairement au cœur de
l'algorithme :
- c'est une relaxation **continue** (aucune intégrité à préserver, donc le problème
  d'intégrité de $\epsilon$ que j'ai signalé pour la version CC complète ne se pose pas)
- chaque objectif peut avoir **son propre dénominateur** $c_k,d_k$ (pas besoin d'un
  dénominateur commun, puisqu'on résout $p$ petits LP indépendants, pas un seul LP
  combiné)

Donc `approxIdealFrac` fait, pour chaque $k$, un Charnes-Cooper *local* et *jetable* via
l'API Concert de CPLEX (comme `approxIdeal` original le fait déjà pour le cas linéaire,
juste avec la transformation en plus) : c'est un usage de CC exactement dans le
périmètre où il est irréprochable.

## Mise à jour : élagage par point nadir + branche manquante de $H_l$ (Ait Mehdi, Chergui & Abbas, 2014)

Deux ajouts inspirés de « An Improved Method for Solving MOILFP » (Ait Mehdi, Chergui &
Abbas, *Advances in Decision Sciences*, 2014), qui améliore Chergui & Moulaï (2008) —
la méthode de base dont GESE-MOIP est la généralisation.

**1. `detectHlFrac` corrigée.** La version précédente ne testait que la branche
"$\exists k,\gamma_j^k<0$" de la définition de $H_l$ (comme `detectHl()` original pour
le MOILP). La définition complète du papier —
$$H_l=\{j:\exists k,\gamma_j^k<0\}\cup\{j:\gamma_j^k=0,\forall k\}$$
— est maintenant implémentée intégralement (avec une tolérance numérique `1e-9`).

**2. `computeNadirPoint` + règle d'élagage supplémentaire.** Calculé **une seule fois**
à la racine, via une table de gains : pour chaque objectif $k$, on cherche
$\min_{x\in X\text{ entier}}f_k(x)$ **sur le domaine complet** (pas une relaxation), au
moyen d'une petite boucle de **Dinkelbach** sur des PL entiers ordinaires — c'est
justement l'alternative à Charnes-Cooper évoquée plus tôt, appliquée là où elle est la
plus naturelle. $Nd_i$ = la pire (plus grande, puisqu'on minimise) valeur de l'objectif
$i$ observée parmi les $p$ points de la table de gains.

Dans `generalPhase`, en plus du test de dominance existant (point idéal local dominé par
une solution non dominée **déjà trouvée**), on ajoute : si $\exists i,\ Id_i^l>Nd_i$
(même le meilleur cas atteignable sur cette branche est pire que le pire cas déjà
accepté globalement), le nœud est sondé — **sans même calculer $H_l$**. Les deux règles
sont complémentaires, pas redondantes : l'une compare à des points trouvés en cours de
route, l'autre à une borne globale fixée dès le départ.

Comme le signale le papier, cette estimation du nadir via table de gains est fiable
surtout en **biobjectif** ($p=2$) — au-delà, elle peut sur- ou sous-estimer le vrai
nadir. Le compteur `nadir_delete` dans la sortie du programme permet de voir combien de
nœuds cette règle élimine sur vos instances.



```
n
m
p
<m lignes: A[i][1..n] sign b[i]>
<1 ligne : trajectoire w[1..n]>
<p blocs, chacun :
   a_k[1..n] b_k
   c_k[1..n] d_k>
```

Le fichier `inputMOILFP_gamma_example.txt` fourni reprend l'exemple résolu à la main
dans la conversation : $f_1=\dfrac{2x_1+x_2+1}{x_1+x_2+5}$,
$f_2=\dfrac{-2x_1+3x_2+2}{2x_1+x_2+4}$, avec les mêmes contraintes que l'exemple du
papier GESE-MOIP. Vous pouvez vérifier que le premier $H_l$ trouvé (au nœud
$x_1\le0$, $X^1=(0,2)$) redonne bien $H_l=\{x_1,x_4\}$ et $\epsilon_l=4$ comme calculé
à la main.
