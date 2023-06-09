 /**
* @file fantasmes.c
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
typedef struct{
    int mode_normal;
    int fila;
    int col;
} missatge_t;

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
    int fila_mc, columna_mc;
    int numero_fantasmes;

void enviar_missatge(missatge_t contingut, int id_bustia){
    char missatge[20];
    sprintf(missatge,"%d %d %d",contingut.mode_normal, contingut.fila, contingut.col);
  
    for(int i = 0; i < (numero_fantasmes); i++){ //tenim en compte que el fantasma actual ja ho sap
     
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

    do{
        receiveM(id_bustia, missatge);
        //hem de separar els 3 enters de "%d %d %d" en 3 variables
        sscanf(missatge, "%d %d %d", &mode_normal, &fila_mc, &columna_mc);
       
      
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
    int k, vk, nd, vd[3], vd_sec[3]; //vd_sec es el vector de direccions secundaries
    int distancia[3];
    ret = 0; 
    
    do{
    
    	//win_update();
    	
        fprintf(stderr, "Posició menjacocos: %d %d\n", fila_mc, columna_mc);
        nd = 0; //numero de direccions disponibles
        int nd_secundari = 0;
        float distancia[3]; //distancia entre el fantasma i el comecocos
        for (k=-1; k<=1; k++)		/* provar direccio actual i dir. veines */
        {
            vk = (f1.d + k) % 4;		/* direccio veina */
            if (vk < 0) vk += 4;		/* corregeix negatius */

            seg.f = f1.f + df[vk]; /* calcular posicio en la nova dir.*/
            seg.c = f1.c + dc[vk];
            seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
            if (mode_normal == 0 ||(seg.a==' ') || (seg.a=='.') || (seg.a=='0') ) //si está disponible o no estem en mode normal
            { 
            
               			
                if(mode_normal==0){
                    //treiem distancia entre el fantasma i el comecocos, la guardem en una taula
                    
                    fprintf(stderr, "Posició menjacocos: %d %d\n", fila_mc, columna_mc);
                    if(seg.f == fila_mc || seg.c == columna_mc){
                        vd[nd] = vk;/* memoritza com a direccio possible */
                         nd++;
                         fprintf(stderr,"%i-CACERA=>Moviment fila = %d, columna = %d a les posicions (%d,%d)\n",i,df[vk],dc[vk],seg.f,seg.c);
                         
                    }else{
                        //en el cas de que no sigui una directa la guardem com a segona opció
                        if (!(seg.f>=n_fil || seg.c>=n_col || seg.f<= 0|| seg.c<= 0)){ //si no es la paret i no ens en sortim
                            vd_sec[nd_secundari] = vk;
                            distancia[nd_secundari] = (seg.f - fila_mc)*(seg.f - fila_mc) + (seg.c - columna_mc)*(seg.c - columna_mc); //distancia euclidiana
                            
                            fprintf(stderr,"CERCA DEL COMOCOS -> Direcció %d, distancia %f, moviment a (%d,%d)\n", vd_sec[nd_secundari], distancia[nd_secundari], seg.f, seg.c);
                            nd_secundari++;
                        }else{
                            fprintf(stderr,"PARET -> Direcció %d, posicion (%d,%d)\n", vk, seg.f, seg.c);
                        }
                    }
                    
                }else{
                    vd[nd] = vk; /* memoritza com a direccio possible */
                     nd++;
                    fprintf(stderr,"NORMAL = %d - Direcció %d, nd=%d\n", mode_normal ,vd[nd],nd);
                }
               
            }else{
                fprintf(stderr,"NORMAL = %d\n", mode_normal);
            }
        }
        
     
        
        
        if (nd == 0){			/* si no pot continuar, */
            if(mode_normal==0){
                
                //estem en mode cacera, però no alineats amb el comecocos
                //hem de mirar totes les direccions quina está més a prop del fantasma
                
                //mirem totes les distancies i escollim el camí més curt entre les direccions disponibles
                fprintf(stderr,"nd_secundari = %d\n", nd_secundari);
                //agafem la distancia més petita
                float min = distancia[0];
                f1.d = vd_sec[0];
                for(int j = 1; j < nd_secundari; j++){
                    if(distancia[j] < min){
                        min = distancia[j];
                        f1.d = vd_sec[j];
                    }
                }
                fprintf(stderr,"Fantasma %d desbloquejat, direcció %d\n",i,f1.d);
            }else{
                 fprintf(stderr,"Fantasma %d bloquejat, donem la volta\n",i);
                f1.d = (f1.d + 2) % 4;		/* canvia totalment de sentit */
            }
           
        }else
        { 
            
            if (nd == 1){			/* si nomes pot en una direccio */
                f1.d = vd[0];			/* li assigna aquesta */
            }
            else{				/* altrament */
                f1.d = vd[rand() % nd];		/* segueix una dir. aleatoria */
            }
        }
        seg.f = f1.f + df[f1.d];  /* calcular seguent posicio final */
        seg.c = f1.c + dc[f1.d];
        seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
        if(seg.f < n_fil&&seg.c<n_col&&seg.f>=0&&seg.c>=0){ //comprovem que estiguem al rang
            fprintf("moviment a (%d,%d), limits n_files = %d, n_cols = %d\n",seg.f,seg.c,n_fil,n_col);
            //TODO posicio anterior si hi ha una pared deixar-la invertida
            if(f1.a=='+') win_escricar(f1.f,f1.c,f1.a,INVERS);	/* esborra posicio anterior */
            else win_escricar(f1.f,f1.c,f1.a,NO_INV);

            fprintf(stderr,"Fantasma %d abans=>\t fila: %d col: %d dir: %d car: %c\n",i, f1.f, f1.c, f1.d, f1.a);
            f1.f = seg.f; 
            f1.c = seg.c; 
            f1.a = seg.a;	/* actualitza posicio */
            win_escricar(f1.f,f1.c,'1'+i,NO_INV);
            fprintf(stderr,"Fantasma %d després=>\t fila: %d col: %d dir: %d car: %c\n", i, f1.f, f1.c, f1.d, f1.a);
        }else{
            //posició fora de rang
            fprintf(stderr,"Fantasma %d fora de rang\n",i);
            f1.d = (f1.d + 2) % 4;		/* canvia totalment de sentit */
        }
        
        
        
                
        
        
        
        if (f1.a == '0'){
            ret = 1;		/* ha capturat menjacocos */
            fprintf(stderr,"Fantasma %d capturat\n",i);
        }else if(mode_normal||fantasma_trobat == i){
            //fantasma_trobat és -1 si no es sap qui l'ha trobat, només ho sap el que l'ha trobat
            //recorrem tota la direcció mentre sigui '.' o ' '
            int fila_actual = f1.f + df[f1.d];
            int col_actual = f1.c + dc[f1.d];
            while(win_quincar(fila_actual, col_actual)==' '||win_quincar(fila_actual, col_actual)=='.'){
                fila_actual += df[f1.d]; //avancem en la direcció
                col_actual += dc[f1.d]; //avancem en la direcció
            }
            if(mode_normal==1&&win_quincar(fila_actual, col_actual)=='0'){
                fprintf(stderr,"Activant %i mode cacera, menjacocos a la pos %d, %d\n",i, fila_actual, col_actual);
                missatge_t msg;
                msg.mode_normal=0;
                msg.fila=fila_actual;
                msg.col=col_actual;
                signalS(id_sem); //deixem moure a la resta perquè rebin el missatge
                enviar_missatge(msg,id_bustia);
                waitS(id_sem); //esperem
                mode_normal=0;    //activar mode cacera
                
                fantasma_trobat=i;  //indicar fantasma que l'ha trobat
                
            }else if(win_quincar(fila_actual, col_actual)!='0'&&fantasma_trobat == i){ //si arriba no l'ha trobat, és a dir s'ha trobat una paret
            //si és el que l'havia trobat al principi el deixa de veure
                fprintf(stderr,"Desactivant %i mode cacera, fantasma a la pos (%d, %d) que hi ha %c. Menjacocos a la pos (%d,%d)\n",i, fila_actual, col_actual, win_quincar(fila_actual, col_actual), fila_mc, columna_mc);
                fantasma_trobat = -1;
                missatge_t msg;
                msg.mode_normal=1;
                msg.fila=fila_actual;
                msg.col=col_actual;
                signalS(id_sem); //deixem moure a la resta perquè rebin el missatge
                enviar_missatge(msg,id_bustia);
                waitS(id_sem); //esperem
                mode_normal=1;    //desactivar mode cacera
                
                
            }
        }
        if(fantasma_trobat == i){
            win_escricar(f1.f,f1.c,(char) ('1'+i),INVERS);	//dibuixar fantasma invers
        }else{
            win_escricar(f1.f,f1.c,(char) ('1'+i),NO_INV);	//dibuixar fantasmes normal
        }
           
            
        
        
        *fi2_p=ret;
        
        signalS(id_sem); //señalizamos que ya hemos acabado
       
     
        win_retard((mc->r)* (int) *retard);
        waitS(id_sem); //bloqueja
    }while (!(*fi1_p) && !(*fi2_p));//!fi1 && !fi2)

    fprintf(stderr,"FI Fantasma %d",i);
    signalS(id_sem);
    return (fi2); 
 }
 
