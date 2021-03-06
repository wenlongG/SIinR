#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#define nzvals(i) nzvals[(i)-1]
#define rowind(i) rowind[(i)-1]
#define colptr(i) colptr[(i)-1]
#define xsol(i)   xsol[(i)-1]
#define rhs(i)    rhs[(i)-1]
#define diag(i)   diag[(i)-1]
#define diag2(i)  diag2[(i)-1]

extern int ldlt_preprocess__(int *, int *, int *, int *, int *, int *, int *);
extern int ldlt_fact__(int *, int *, int *, double *);
extern int ldlt_solve__(int *, double *, double *);
extern int ldlt_free__(int *);
extern int ldlt_selinv__(int *, double *, int*,double*,int*,int*);

extern int readmatrixheader_(char *, int *, int *);
extern int readmatrix_(char *, int *, int *, double *);
#ifdef TIMING
extern double getime(void);
#endif

double * selinv2julia(int nnodes, int nnz, int *colptr, int *rowind, 
		      double* nzvals, int* LnnzOutput)
{
   int i, j, nedges, ia, ibeg, iend ;
   int nx= -1, ny = -1, count, node;
   int *perm;
   //int *rowind, *colptr;
   double *rhs, *xsol, *diag2;
   int token, order=-1;
   int Lnnz;
   double t0,t1, errmax; 
   long myclktck;
   double dval;
   double errabs;
   int dumpL=0;
   int ierr = 0;
   double* diagOutput;

   //nnodes = 0;
   //nnz = 0;

   //   char filename[120]="/home/grad/wgong/Downloads/newSelInv/SelInv/UTILITIES/bcsstk14.ccf";
   

   //   readmatrixheader_(filename, &nnodes, &nnz);

   // colptr = (int*)malloc((nnodes+1)*sizeof(int));
   //rowind = (int*)malloc(nnz*sizeof(int));
   //nzvals = (double*)malloc(nnz*sizeof(double));

   //readmatrix_(filename, colptr, rowind, nzvals);
   token = 0;
  
   perm = NULL;   /* perm is not used here*/
   ldlt_preprocess__(&token, &nnodes, colptr, rowind, &Lnnz, &order, perm);   
   //printf(" NUMBER OF ROWS IN A = %d\n", nnodes);
   //printf(" NUMBER OF NONZERO ELEMENTS IN A = %d\n" , nnz);
   //printf(" NUMBER OF NONZERO ELEMENTS IN L = %d\n\n", Lnnz); 

   ldlt_fact__(&token, colptr, rowind, nzvals);

   double * LNZXX;
   int * RowXX,* ColXX;
   double * V;

    RowXX =(int *) malloc(Lnnz*sizeof(int));
    ColXX =(int *) malloc(Lnnz*sizeof(int));
    LNZXX = (double *)malloc(Lnnz*sizeof(double));

   V=(double*)calloc(3*Lnnz,sizeof(double));
   /* selected inversion */
   diag2 = (double*)calloc(nnodes,sizeof(double));
   ldlt_selinv__(&token, diag2, &dumpL,LNZXX,RowXX,ColXX);

   ///
   for(i=0;i<Lnnz;i++)
     {
       //printf("%d %d %f\n",RowXX[i], ColXX[i], LNZXX[i]);
       V[i]=(double) RowXX[i];
       V[i+Lnnz]=ColXX[i];
       V[i+2*Lnnz]=LNZXX[i];
     }

   ///
   //for(i=0;i<nnodes;i++)
   //    diagOutput[i] = diag2[i];
   LnnzOutput[0] = Lnnz;

   ldlt_free__(&token); 

   if (order == 0) free(perm);
   //free(colptr); 
   //free(rowind); 
   //free(nzvals); 
   //free(rhs); 
   //free(xsol); 
   free(diag2); 

   return V;
}
