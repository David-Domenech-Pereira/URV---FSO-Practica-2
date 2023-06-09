 /**
* @file fantasmes3.c
* @brief Programa que implementa el comportament dels fantasmes, sent fils del programa principal
* @author Assmaa Ouladali i David Domènech
*
 */
 
 #include <stdio.h>
 #include "memoria.h"
 #include "winsuport2.h"		/* incloure definicions de funcions propies */
#include <stdlib.h>		/* per exit() */
 typedef struct {		/* per un objecte (menjacocos o fantasma) */
	int f;				/* posicio actual: fila */
	int c;				/* posicio actual: columna */
	int d;				/* direccio actual: [0..3] */
    float r;            /* per indicar un retard relati */
	char a;				/* caracter anterior en pos. actual */
} objecte;

    //int *df;	/* moviments de les 4 direccions possibles */
    //int *dc;	/* dalt, esquerra, baix, dreta */
    int df[] = {-1, 0, 1, 0};	/* moviments de les 4 direccions possibles */
    int dc[] = {0, -1, 0, 1};	/* dalt, esquerra, baix, dreta */
    int *fi1_p, *fi2_p;
    int fi1, fi2;
    objecte *mc;      		/* informacio del menjacocos */
    int *retard;
 int main(int n_args, char *ll_args[]){

    void *p_win;
    int id_fi1 = atoi(ll_args[1]);
    int id_fi2 = atoi(ll_args[2]);
    //int id_df = atoi(ll_args[3]);
    //int id_dc = atoi(ll_args[4]);
    fi1_p = map_mem(id_fi1);
    
    fi2_p = map_mem(id_fi2); 
   
    fprintf(stderr,"CAracteristiques exec fantasma:\t fi1: %d fi2: %d \n", fi1, fi2);
    
    //df = map_mem(id_df);
    //dc = map_mem(id_dc);
    
    int i = atoi(ll_args[3]);
    int id_f = atoi(ll_args[4]); //posicio memoria compartida fantasmes
    int id_mc = atoi(ll_args[5]); //la variable mc
    int id_retard = atoi(ll_args[6]);
    //int id_win = atoi(ll_args[9]);
    //int fil = atoi(ll_args[10]);
    //int col = atoi(ll_args[11]);
    
    

    mc = map_mem(id_mc);
    retard = map_mem(id_retard);
    objecte *fantasmes = map_mem(id_f); //punter a la memoria compartida fantasmes

    objecte f1 = fantasmes[i];
    fprintf(stderr,"CAracteristiques del fantasma:\t fila: %d col: %d dir: %d car: %c\n", f1.f, f1.c, f1.d, f1.a);


    if(i < 0){
            fprintf(stderr,"Error: index incorrecte\n");
            exit(6);
    }else{
        fprintf(stderr,"Fantasma %d creat",i);
    }
    
    int id_win = atoi(ll_args[7]);
    p_win = map_mem(id_win);
    if (p_win == (int *)-1)
    {   fprintf(stderr,"proces (%d): error en identificador de finestra\n",(int)getpid());
        exit(0);
    }
    int n_fil = atoi(ll_args[8]);		/* obtenir dimensions del camp de joc */
    int n_col = atoi(ll_args[9]);

    //win_set((void *)id_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */
    win_set(p_win,n_fil,n_col);

    objecte seg;
    int ret;
    int k, vk, nd, vd[3];
    
    ret = 0; 
    //int z=0;
    do{
    //z++;
     //fprintf(stderr,"CAracteristiques exec fantasma:\t fi1: %d fi2: %d \n", fi1, fi2);
    	//win_update();
    	
        //fprintf(stderr,"entra a fantasma per %d cop\n", z);
        nd = 0; //numero de direccions disponibles
        for (k=-1; k<=1; k++)		/* provar direccio actual i dir. veines */
        {
            vk = (f1.d + k) % 4;		/* direccio veina */
            if (vk < 0) vk += 4;		/* corregeix negatius */

            seg.f = f1.f + df[vk]; /* calcular posicio en la nova dir.*/
            fprintf(stderr,"CAracteristiques exec fantasma:\t seg.f: %d f1.f: %d df[vk]: %d vk: %d\n", seg.f, f1.f, df[vk], vk);
            seg.c = f1.c + dc[vk];
            fprintf(stderr,"CAracteristiques exec fantasma:\t seg.c: %d f1.c: %d dc[vk]: %d vk: %d\n", seg.c, f1.c, dc[vk], vk);
            seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
            fprintf(stderr,"CAracteristiques exec fantasma:\t seg.a: %c \n", seg.a);
            if ((seg.a==' ') || (seg.a=='.') || (seg.a=='0'))
            { 
                vd[nd] = vk;			/* memoritza com a direccio possible */
                nd++;
            }
        }
        if (nd == 0){			/* si no pot continuar, */
            f1.d = (f1.d + 2) % 4;		/* canvia totalment de sentit */
        }else
        { 
            if (nd == 1){			/* si nomes pot en una direccio */
                f1.d = vd[0];			/* li assigna aquesta */
            }
            else{				/* altrament */
                f1.d = vd[rand() % nd];		/* segueix una dir. aleatoria */
            }
            seg.f = f1.f + df[f1.d];  /* calcular seguent posicio final */
            seg.c = f1.c + dc[f1.d];
            seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
            win_escricar(f1.f,f1.c,f1.a,NO_INV);	/* esborra posicio anterior */
            fprintf(stderr,"CAracteristiques del fantasma:\t fila: %d col: %d dir: %d car: %c\n", f1.f, f1.c, f1.d, f1.a);
            f1.f = seg.f; 
            f1.c = seg.c; 
            f1.a = seg.a;	/* actualitza posicio */
            win_escricar(f1.f,f1.c,'1'+i,NO_INV);		/* redibuixa fantasma */
            fprintf(stderr,"CAracteristiques del fantasma:\t fila: %d col: %d dir: %d car: %c\n", f1.f, f1.c, f1.d, f1.a);
            
            if (f1.a == '0'){
                ret = 1;		/* ha capturat menjacocos */
                fprintf(stderr,"Fantasma %d capturat\n",i);
            } 
           
            
        }
        
        *fi2_p=ret;
        fprintf(stderr,"CAracteristiques exec fantasma:\t fi1: %d fi2: %d \n", (*fi1_p), (*fi2_p));
        
        win_retard((mc->r)* (int) *retard);
          
    }while (!(*fi1_p) && !(*fi2_p));//!fi1 && !fi2)
   
    return (fi2); 
 }
