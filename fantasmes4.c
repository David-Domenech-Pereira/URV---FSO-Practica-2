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
#include <pthread.h>
#include <unistd.h>		/* per getpid() */
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
    pthread_t bustia;
    int mode_normal=1;
    int numero_fantasmes;

void enviar_missatge(int valor, int id_bustia){
    char missatge[20];
    sprintf(missatge,"%d",valor);
    fprintf(stderr,"SEND => Mode normal: %d\n",mode_normal);
    for(int i = 0; i < (numero_fantasmes); i++){ //tenim en compte que el fantasma actual ja ho sap
        fprintf("Bustia %d x%d",id_bustia,i);
        sendM(id_bustia, missatge, sizeof(missatge)); //enviem el missatge amb el mode a tots els fantasmes
    }
    
 }
 

/*
* funció que llegeix la bustia
*/
void * read_bustia(void *index)
{
   
    char missatge[20];
    int id_bustia = (intptr_t) index;
     fprintf(stderr,"Thread bustia creat, id = %d",id_bustia);
    do{
        receiveM(id_bustia, missatge);
        mode_normal=atoi(missatge);
        fprintf(stderr,"RECIEVE => Mode normal: %d\n",mode_normal);
    }while(1);
   

}


 int main(int n_args, char *ll_args[]){
    int id_sem=atoi(ll_args[12]);
    int i = atoi(ll_args[3]);
    waitS(id_sem);
    fprintf(stderr,"Fantasma %d inici - Semafor %d VERD\n",i,id_sem);
    int fantasma_trobat = -1; //per defecte l'iniciem a -1
    void *p_win;
    //printeamos todos los argumentos
    for(int i = 0; i < n_args; i++){
        fprintf(stderr,"Argument %d: %s\n",i,ll_args[i]);
    }
    int id_fi1 = atoi(ll_args[1]);
    int id_fi2 = atoi(ll_args[2]);
    //int id_df = atoi(ll_args[3]);
    //int id_dc = atoi(ll_args[4]);
    fi1_p = map_mem(id_fi1);
    
    fi2_p = map_mem(id_fi2); 
   
    fprintf(stderr,"CAracteristiques exec fantasma:\t fi1: %d fi2: %d \n", fi1, fi2);
    
    //df = map_mem(id_df);
    //dc = map_mem(id_dc);
    
   
    int id_f = atoi(ll_args[4]); //posicio memoria compartida fantasmes
    int id_mc = atoi(ll_args[5]); //la variable mc
    int id_retard = atoi(ll_args[6]);
    //int id_win = atoi(ll_args[9]);
    //int fil = atoi(ll_args[10]);
    //int col = atoi(ll_args[11]);
   /* int id_fantasma_trobat=atoi(ll_args[10]);   //index del fantasma que ha trobat el mensjacocos
    int id_mode_normal=atoi(ll_args[11]); 
    int *p_fantasma_trobat=map_mem(id_fantasma_trobat);
    int mode_normal=map_mem(id_mode_normal)*/
    

    mc = map_mem(id_mc);
    retard = map_mem(id_retard);
    objecte *fantasmes = map_mem(id_f); //punter a la memoria compartida fantasmes

    objecte f1 = fantasmes[i];
    //fprintf(stderr,"CAracteristiques del fantasma:\t fila: %d col: %d dir: %d car: %c\n", f1.f, f1.c, f1.d, f1.a);

    if(pthread_create(&bustia,NULL,read_bustia,NULL)!=0){
              fprintf(stderr,"Error al crear el thread del menjacocos\n");
              exit(1);
          }
          

    if(i < 0){
            fprintf(stderr,"Error: index incorrecte\n");
            exit(6);
    }else{
        fprintf(stderr,"Fantasma %d creat\n",i);
    }
    
    int id_win = atoi(ll_args[7]);
    p_win = map_mem(id_win);
    if (p_win == (int *)-1)
    {   fprintf(stderr,"proces (%d): error en identificador de finestra\n",(int)getpid());
        exit(0);
    }
    int n_fil = atoi(ll_args[8]);		/* obtenir dimensions del camp de joc */
    int n_col = atoi(ll_args[9]);
    int id_bustia=atoi(ll_args[10]);
    numero_fantasmes = atoi(ll_args[11]);
 
     if(pthread_create(&bustia,NULL,read_bustia,(void *) id_bustia)!=0){
              fprintf(stderr,"Error al crear el thread de la bustia del procés %d\n",i);
              exit(1);
          }

    //win_set((void *)id_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */
    win_set(p_win,n_fil,n_col);

    objecte seg;
    int ret;
    int k, vk, nd, vd[3];
    int distancia[3];
    ret = 0; 
    
    do{
    
    	//win_update();
    	
        fprintf(stderr, "Posició menjacocos: %d %d\n", mc->f, mc->c);
        nd = 0; //numero de direccions disponibles
        if(mode_normal){
        for (k=-1; k<=1; k++)		/* provar direccio actual i dir. veines */
        {
            vk = (f1.d + k) % 4;		/* direccio veina */
            if (vk < 0) vk += 4;		/* corregeix negatius */

            seg.f = f1.f + df[vk]; /* calcular posicio en la nova dir.*/
            seg.c = f1.c + dc[vk];
            seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
            if (mode_normal == 0 ||(seg.a==' ') || (seg.a=='.') || (seg.a=='0') ) //si está disponible o no estem en mode normal
            { 
            
                vd[nd] = vk;			/* memoritza com a direccio possible */
                if(mode_normal==0){
                    //treiem distancia entre el fantasma i el comecocos, la guardem en una taula
                    fprintf(stderr,"Moviment fila = %d, columna = %d a les posicions (%d,%d)\n",df[vk],dc[vk],seg.f,seg.c);
                    fprintf(stderr, "Posició menjacocos: %d %d\n", mc->f, mc->c);
                    if(seg.f == mc->f || seg.c == mc->c){
                        distancia[nd] = 1;
                    }else{
                        distancia[nd] = 0;
                    }
                    fprintf(stderr,"CACERA -> Direcció %d, distancia %d\n", vd[nd], distancia[nd]);
                }else{
                    fprintf(stderr,"NORMAL = %d - Direcció %d\n", mode_normal ,vd[nd]);
                }
                nd++;
            }else{
                fprintf(stderr,"NORMAL = %d\n", mode_normal);
            }
        }
        }
        if(mode_normal==0){
            	for (k=-1; k<=1; k++)		/* provar direccio actual i dir. veines */
        	{
            	  vk = (f1.d + k) % 4;		/* direccio veina */
               	  if (vk < 0) vk += 4;		/* corregeix negatius */

            	  seg.f = f1.f + df[vk]; /* calcular posicio en la nova dir.*/
            	  seg.c = f1.c + dc[vk];
            	  seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
            		
            	    if ((seg.f==n_fil || seg.c==n_col || seg.f== 0|| seg.c== 0)){ //si es la paret i ens en sortim
            	    
            	    }
            	      else{
            	         vd[nd] = vk;
            		 }
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
            else if(mode_normal){				/* altrament */
                f1.d = vd[rand() % nd];		/* segueix una dir. aleatoria */
            }/*else{
                //hem de mirar totes les direccions quina está més a prop del fantasma
              
               
                for(int j = 0; j < nd; j++){
                    if(distancia[j] == 1){
                        
                        f1.d = vd[j]; // es un moviment possible
                        break;
                    }
                }
               fprintf(stderr,"direccio triada: %d\n", f1.d);
            }*/
            seg.f = f1.f + df[f1.d];  /* calcular seguent posicio final */
            seg.c = f1.c + dc[f1.d];
            seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
            win_escricar(f1.f,f1.c,f1.a,NO_INV);	/* esborra posicio anterior */
            fprintf(stderr,"CAracteristiques del fantasma:\t fila: %d col: %d dir: %d car: %c\n", f1.f, f1.c, f1.d, f1.a);
            f1.f = seg.f; 
            f1.c = seg.c; 
            f1.a = seg.a;	/* actualitza posicio */
     	    win_escricar(f1.f,f1.c,'1'+i,NO_INV);
            fprintf(stderr,"CAracteristiques del fantasma:\t fila: %d col: %d dir: %d car: %c\n", f1.f, f1.c, f1.d, f1.a);
            
            
                    
            
            
            
            if (f1.a == '0'){
                ret = 1;		/* ha capturat menjacocos */
                fprintf(stderr,"Fantasma %d capturat\n",i);
            }else if(mode_normal||fantasma_trobat == i){
                //fantasma_trobat és -1 si no es sap qui l'ha trobat, només ho sap el que l'ha trobat
                //recorrem tota la direcció mentre sigui '.' o ' '
                int fila_actual = f1.f + df[f1.d];
                int col_actual = f1.c + dc[f1.d];
                while(win_quincar(fila_actual, col_actual) == '.' || win_quincar(fila_actual, col_actual) == ' '){
                    fila_actual += df[f1.d]; //avancem en la direcció
                    col_actual += dc[f1.d]; //avancem en la direcció
                }
                if(win_quincar(fila_actual, col_actual) == '0'){
                    fprintf(stderr,"Activant mode cacera, menjacocos a la pos %d, %d\n", fila_actual, col_actual);
                    mode_normal=0;    //activar mode cacera
                    enviar_missatge(mode_normal, id_bustia); //enviem el mode normal
                    fantasma_trobat=i;  //indicar fantasma que l'ha trobat
                    
                }else if(fantasma_trobat == i){ //si arriba no l'ha trobat, és a dir s'ha trobat una paret
                //si és el que l'havia trobat al principi el deixa de veure
                    fprintf(stderr,"Desactivant mode cacera, menjacocos a la pos %d, %d\n", fila_actual, col_actual);
                    fantasma_trobat = -1;
                    //TODO
                    mode_normal=1;    //desactivar mode cacera
                    enviar_missatge(mode_normal, id_bustia); //enviem el mode normal
                    
                }
            }
            if(fantasma_trobat == i){
                win_escricar(f1.f,f1.c,(char) ('1'+i),INVERS);	//dibuixar fantasma invers
            }else{
                win_escricar(f1.f,f1.c,(char) ('1'+i),NO_INV);	//dibuixar fantasmes normal
            }
           
            
        }
        
        *fi2_p=ret;
        fprintf(stderr,"CAracteristiques exec fantasma:\t fi1: %d fi2: %d \n", (*fi1_p), (*fi2_p));
        signalS(id_sem); //señalizamos que ya hemos acabado
        fprintf(stderr,"Fantasma %d - Esperem al semafor %d\n",i,id_sem);
        win_retard((mc->r)* (int) *retard);
        waitS(id_sem); //bloqueja
        fprintf(stderr,"Fantasma %d - Semafor %d VERD\n",i,id_sem);
    }while (!(*fi1_p) && !(*fi2_p));//!fi1 && !fi2)
    fprintf(stderr,"FI Fantasma %d",i);
    signalS(id_sem);
    return (fi2); 
 }
 
