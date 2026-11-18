#include <stdio.h>
#include <math.h>
#include "../src/phys.h"
static void show(const char*n,const char*e,St*s,St*a){ if(e){printf("%s: ERR %s\n",n,e);return;}
 printf("%-10s P=%.5g V=%.5g T=%.5g W=%.5g Q=%.5g  dU-(Q+W)=%.3g (rel %.2g)\n",n,s->P,s->V,s->T,s->W,s->Q,dU(a,s)-(s->Q+s->W),(dU(a,s)-(s->Q+s->W))/(fabs(s->Q)+fabs(s->W)+1e-9));}
int main(void){
 gas_set(8.31448f,1.4f,0.029f);
 St e0={1e5f,1e-3f,300.f,0,0,0,0}; G_M=e0.P*e0.V/(G_R*e0.T);
 printf("R=%g cv=%g cp=%g m=%g\n",G_R,G_CV,G_CP,G_M);
 St o; 
 show("isoch P",calc_isochore(&e0,0,2e5f,&o),&o,&e0);
 show("isoch Q",calc_isochore(&e0,2,50.f,&o),&o,&e0);
 show("isob V",calc_isobare(&e0,0,2e-3f,&o),&o,&e0);
 show("isob W",calc_isobare(&e0,3,-30.f,&o),&o,&e0);
 show("isot P",calc_isotherme(&e0,0,2e5f,&o),&o,&e0);
 show("isot Q",calc_isotherme(&e0,3,20.f,&o),&o,&e0);
 show("adiab V",calc_poly(&e0,1,5e-4f,1.4f,3,&o),&o,&e0);
 show("poly T",calc_poly(&e0,2,350.f,1.2f,4,&o),&o,&e0);
 show("poly W",calc_poly(&e0,3,10.f,1.2f,4,&o),&o,&e0);
 show("isochV",calc_isochore(&e0,1,-5.f,&o),&o,&e0);
}
