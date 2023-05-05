 #include <stdio.h>
 #include "memoria.h"
 #include "winsuport.h"		/* incloure definicions de funcions propies */

 typedef struct {		/* per un objecte (menjacocos o fantasma) */
	int f;				/* posicio actual: fila */
	int c;				/* posicio actual: columna */
	int d;				/* direccio actual: [0..3] */
    float r;            /* per indicar un retard relati */
	char a;				/* caracter anterior en pos. actual */
} objecte;

    int *df;	/* moviments de les 4 direccions possibles */
    int *dc;	/* dalt, esquerra, baix, dreta */
    int *fi1, *fi2;
    objecte *mc;      		/* informacio del menjacocos */
    int *retard;
 int main(int n_args, char *ll_args[]){
    
    int id_fi1 = atoi(ll_args[1]);
    int id_fi2 = atoi(ll_args[2]);
    int id_df = atoi(ll_args[3]);
    int id_dc = atoi(ll_args[4]);
    fi1 = map_mem(id_fi1);
    fi2 = map_mem(id_fi2); 
    df = map_mem(id_df);
    dc = map_mem(id_dc);
    
    int i = atoi(ll_args[5]);
    int id_f = atoi(ll_args[6]); //posicio memoria compartida fantasmes
    int id_mc = atoi(ll_args[7]); //la variable mc
    int id_retard = atoi(ll_args[8]);
    mc = map_mem(id_mc);
    retard = map_mem(id_retard);
    objecte *fantasmes = map_mem(id_f); //punter a la memoria compartida fantasmes

    objecte f1 = fantasmes[i];
    if(i < 0){
            fprintf(stderr,"Error: index incorrecte\n");
            exit(6);
    }
    
    objecte seg;
    int ret;
    int k, vk, nd, vd[3];
    
    ret = 0; 
    
    do{
        nd = 0; //numero de direccions disponibles
        for (k=-1; k<=1; k++)		/* provar direccio actual i dir. veines */
        {
        
        vk = (f1.d + k) % 4;		/* direccio veina */
        if (vk < 0) vk += 4;		/* corregeix negatius */

        seg.f = f1.f + df[vk]; /* calcular posicio en la nova dir.*/
        seg.c = f1.c + dc[vk];
        seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
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
        f1.f = seg.f; 
        f1.c = seg.c; 
        f1.a = seg.a;	/* actualitza posicio */
        win_escricar(f1.f,f1.c,'1'+i,NO_INV);		/* redibuixa fantasma */
        
        if (f1.a == '0') ret = 1;		/* ha capturat menjacocos */
        
        }
        fi2=ret;
        
        win_retard((mc->r)*(int) retard);
        
    }while(!fi1 && !fi2);
    
    return (fi2); 
 }