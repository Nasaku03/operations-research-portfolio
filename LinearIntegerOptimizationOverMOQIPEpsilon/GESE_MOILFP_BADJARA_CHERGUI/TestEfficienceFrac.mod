// Test d'efficacite de Benson, reformule lineairement pour des objectifs fractionnaires
// (voir README_MOILFP_Gamma.md, section "Test de Benson sans division").
//
// f_k(x0) est calcule une fois pour toutes via un dexpr (simple donnee numerique, x0
// etant fixe) -- aucune variable ne multiplie une autre variable nulle part, tout reste
// lineaire. g_k(x) >= 0 <=> t_k(x) >= 0 puisque le denominateur est toujours > 0 ;
// max sum(g_k) = 0 <=> x0 efficace, exactement comme dans le test original.
//
// Code en dur pour p=2 objectifs, comme TestEfficience.mod original (meme limitation
// heritee, voir README_MOILFP_Gamma.md).
int n = ...;
int m = ...;
range vare = 1..n;
range con = 1..m;
float A[con][vare] = ...;
float b[con] = ...;

float a1[vare] = ...; float b1 = ...; float c1[vare] = ...; float d1 = ...;
float a2[vare] = ...; float b2 = ...; float c2[vare] = ...; float d2 = ...;
float x0[vare] = ...;

dexpr float f1l = (sum(j in vare) a1[j]*x0[j] + b1) / (sum(j in vare) c1[j]*x0[j] + d1);
dexpr float f2l = (sum(j in vare) a2[j]*x0[j] + b2) / (sum(j in vare) c2[j]*x0[j] + d2);

dvar int x[vare];
dvar float g1;
dvar float g2;

maximize
  g1 + g2;
subject to {
	forall(i in con)
	  const:
		sum(j in vare) A[i][j]*x[j] <= b[i];
	  gdef1:
		g1 == f1l*(sum(j in vare) c1[j]*x[j] + d1) - (sum(j in vare) a1[j]*x[j] + b1);
	  gdef2:
		g2 == f2l*(sum(j in vare) c2[j]*x[j] + d2) - (sum(j in vare) a2[j]*x[j] + b2);
	  positiveG1:
		g1 >= 0;
	  positiveG2:
		g2 >= 0;
	forall(j in vare)
	  positive:
		x[j] >= 0;
}
