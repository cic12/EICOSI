#ifndef MPC_H_
#define MPC_H_
#include "libgrampc.h"
#define NX    	4
#define NU  	1

// Exo
//#define A 		0.066f
//#define B       1.926f
//#define J_      0.066f
//#define tau_g   0.415f // from old

// Lumped
//#define A 	  0.7129f
//#define B       2.2936f
//#define J_      0.1744f
//#define tau_g   6.0796f

// Mini rig 
#define A 		1.5f
#define B       0.8f
#define J_      1.0f
#define tau_g   0.0f

//#define w_theta 1500000 // EICOSI
#define w_theta 10000 //1000
#define w_tau	1

void openFile(FILE **file, const char *name);
void printNumVector2File(FILE *file, ctypeRNum *const val, ctypeInt size);
void printIntVector2File(FILE *file, ctypeInt *const val, ctypeInt size);
void mpcInit(typeGRAMPC **grampc, typeUSERPARAM *userparam, const ctypeRNum *x0, const ctypeRNum *xdes, const ctypeRNum *u0, const ctypeRNum *udes, const ctypeRNum *umax, const ctypeRNum *umin, const ctypeRNum *Thor, const ctypeRNum *dt, const ctypeRNum *t, const typeChar *TerminalCost, const typeChar *IntegralCost, const typeChar *ScaleProblem);

#endif