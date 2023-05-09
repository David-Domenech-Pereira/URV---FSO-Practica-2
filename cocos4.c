/**
* @file cocos4.c
* @brief Codi que controla el moviment d'un joc menjacocos sense sincronització 
* @author Assmaa Ouladali i David Domènech
*/
/*****************************************************************************/
/*									                                         */
/*				     cocos4.c				                                 */
/*									                                         */
/*     Programa inicial d'exemple per a les practiques 2.1 i 2.2 de FSO.     */
/*     Es tracta del joc del menjacocos: es dibuixa un laberint amb una      */
/*     serie de punts (cocos), els quals han de ser "menjats" pel menja-     */
/*     cocos. Aquest menjacocos es representara amb el caracter '0', i el    */
/*     moura l'usuari amb les tecles 'w' (adalt), 's' (abaix), 'd' (dreta)   */
/*     i 'a' (esquerra). Simultaniament hi haura un conjunt de fantasmes,    */
/*     representats per numeros de l'1 al 9, que intentaran capturar al      */
/*     menjacocos. En la primera versio del programa, nomes hi ha un fan-    */
/*     tasma.								                                 */
/*     Evidentment, es tracta de menjar tots els punts abans que algun fan-  */
/*     tasma atrapi al menjacocos.					                         */
/*									                                         */
/*  Arguments del programa:						                             */
/*     per controlar la posicio de tots els elements del joc, cal indicar    */
/*     el nom d'un fitxer de text que contindra la seguent informacio:	     */
/*		n_fil1 n_col fit_tauler creq				                         */
/*		mc_f mc_c mc_d mc_r						                             */
/*		f1_f f1_c f1_d f1_r						                             */
/*									                                         */
/*     on 'n_fil1', 'n_col' son les dimensions del taulell de joc, mes una   */
/*     fila pels missatges de text a l'ultima linia. "fit_tauler" es el nom  */
/*     d'un fitxer de text que contindra el dibuix del laberint, amb num. de */
/*     files igual a 'n_fil1'-1 i num. de columnes igual a 'n_col'. Dins     */
/*     d'aquest fitxer, hi hauran caracter ASCCII que es representaran en    */
/*     pantalla tal qual, excepte el caracters iguals a 'creq', que es visua-*/
/*     litzaran invertits per representar la paret.			                 */
/*     Els parametres 'mc_f', 'mc_c' indiquen la posicio inicial de fila i   */
/*     columna del menjacocos, aixi com la direccio inicial de moviment      */
/*     (0 -> amunt, 1-> esquerra, 2-> avall, 3-> dreta). Els parametres	     */
/*     'f1_f', 'f1_c' i 'f1_d' corresponen a la mateixa informacio per al    */
/*     fantasma 1. El programa verifica que la primera posicio del menja-    */
/*     cocos o del fantasma no coincideixi amb un bloc de paret del laberint.*/
/*	   'mc_r' 'f1_r' son dos reals que multipliquen el retard del moviment.  */ 
/*     A mes, es podra afegir un segon argument opcional per indicar el      */
/*     retard de moviment del menjacocos i dels fantasmes (en ms);           */
/*     el valor per defecte d'aquest parametre es 100 (1 decima de segon).   */
/*									                                         */
/*  Compilar i executar:					  	                             */
/*     El programa invoca les funcions definides a 'winsuport.h', les        */
/*     quals proporcionen una interficie senzilla per crear una finestra     */
/*     de text on es poden escriure caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':				                     */
/*									                                         */
/*	   $ gcc -Wall cocos0.c winsuport.o -o cocos0 -lcurses		             */
/*	   $ ./cocos0 fit_param [retard]				                         */
/*									                                         */
/*  Codis de retorn:						  	                             */
/*     El programa retorna algun dels seguents codis al SO:		             */
/*	0  ==>  funcionament normal					                             */
/*	1  ==>  numero d'arguments incorrecte 				                     */
/*	2  ==>  fitxer de configuracio no accessible			                 */
/*	3  ==>  dimensions del taulell incorrectes			                     */
/*	4  ==>  parametres del menjacocos incorrectes			                 */
/*	5  ==>  parametres d'algun fantasma incorrectes			                 */
/*	6  ==>  no s'ha pogut crear el camp de joc			                     */
/*	7  ==>  no s'ha pogut inicialitzar el joc			                     */
/*****************************************************************************/



#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>		/* per exit() */
#include <unistd.h>		/* per getpid() */
#include "winsuport2.h"		/* incloure definicions de funcions propies */
#include <pthread.h>
#include "memoria.h"
#include <sys/wait.h>

#define MIN_FIL 7		/* definir limits de variables globals */
#define MAX_FIL 25
#define MIN_COL 10
#define MAX_COL 80
#define MAX_PROCS 10
#define MAX_FANTASMES 100
				/* definir estructures d'informacio */
typedef struct {		/* per un objecte (menjacocos o fantasma) */
	int f;				/* posicio actual: fila */
	int c;				/* posicio actual: columna */
	int d;				/* direccio actual: [0..3] */
    float r;            /* per indicar un retard relati */
	char a;				/* caracter anterior en pos. actual */
} objecte;

int df[] = {-1, 0, 1, 0};	/* moviments de les 4 direccions possibles */
int dc[] = {0, -1, 0, 1};	/* dalt, esquerra, baix, dreta */
/* variables globals */
int n_fil1, n_col;		/* dimensions del camp de joc */
char tauler[70];		/* nom del fitxer amb el laberint de joc 70*/
char c_req;			    /* caracter de pared del laberint */


pid_t tpid[MAX_PROCS];
pthread_t coco;
objecte mc;      		/* informacio del menjacocos */
objecte fantasmes[10];			    /* informacio del fantasma 1 */
int n_fantasmes = 0;



int cocos;			/* numero restant de cocos per menjar */
int retard;		    /* valor del retard de moviment, en mil.lisegons */
int fi1=0, fi2=0;
int xocs=0;
char xocs[MAX_F][MAX_C];

/* funcio per realitzar la carrega dels parametres de joc emmagatzemats */
/* dins d'un fitxer de text, el nom del qual es passa per referencia a  */
/* 'nom_fit'; si es detecta algun problema, la funcio avorta l'execucio */
/* enviant un missatge per la sortida d'error i retornant el codi per-	*/
/* tinent al SO (segons comentaris al principi del programa).		    */
void carrega_parametres(const char *nom_fit)
{
  FILE *fit;

  fit = fopen(nom_fit,"rt");		/* intenta obrir fitxer */
  if (fit == NULL)
  {	fprintf(stderr,"No s'ha pogut obrir el fitxer \'%s\'\n",nom_fit);
  	exit(2);
  }

  if (!feof(fit)){
   fscanf(fit,"%d %d %s %c\n",&n_fil1,&n_col,tauler,&c_req);
   fprintf(stderr,"\nHa carregat el tauler\n'\n");
   }
  else {
	fprintf(stderr,"Falten parametres al fitxer \'%s\'\n",nom_fit);
	fclose(fit);
	exit(2);
	}
  if ((n_fil1 < MIN_FIL) || (n_fil1 > MAX_FIL) ||
	(n_col < MIN_COL) || (n_col > MAX_COL))
  {
	fprintf(stderr,"Error: dimensions del camp de joc incorrectes:\n");
	fprintf(stderr,"\t%d =< n_fil1 (%d) =< %d\n",MIN_FIL,n_fil1,MAX_FIL);
	fprintf(stderr,"\t%d =< n_col (%d) =< %d\n",MIN_COL,n_col,MAX_COL);
	fclose(fit);
	exit(3);
  }

  if (!feof(fit)) fscanf(fit,"%d %d %d %f\n",&mc.f,&mc.c,&mc.d,&mc.r);
  else {
	fprintf(stderr,"Falten parametres al fitxer \'%s\'\n",nom_fit);
	fclose(fit);
	exit(2);
	}
  if ((mc.f < 1) || (mc.f > n_fil1-3) ||
	(mc.c < 1) || (mc.c > n_col-2) ||
	(mc.d < 0) || (mc.d > 3))
  {
	fprintf(stderr,"Error: parametres menjacocos incorrectes:\n");
	fprintf(stderr,"\t1 =< mc.f (%d) =< n_fil1-3 (%d)\n",mc.f,(n_fil1-3));
	fprintf(stderr,"\t1 =< mc.c (%d) =< n_col-2 (%d)\n",mc.c,(n_col-2));
	fprintf(stderr,"\t0 =< mc.d (%d) =< 3\n",mc.d);
	fclose(fit);
	exit(4);
  }
  int i = 0;
  while(!feof(fit)){
  	fscanf(fit,"%d %d %d %f\n",&fantasmes[i].f,&fantasmes[i].c,&fantasmes[i].d,&fantasmes[i].r);
  	
  	
	  if ((fantasmes[i].f < 1) || (fantasmes[i].f > n_fil1-3) ||
		(fantasmes[i].c < 1) || (fantasmes[i].c > n_col-2) ||
		(fantasmes[i].d < 0) || (fantasmes[i].d > 3))
	    {
		fprintf(stderr,"Error: parametres fantasma 1 incorrectes:\n");
		fprintf(stderr,"\t1 =< f1.f (%d) =< n_fil1-3 (%d)\n",fantasmes[i].f,(n_fil1-3));
		fprintf(stderr,"\t1 =< f1.c (%d) =< n_col-2 (%d)\n",fantasmes[i].c,(n_col-2));
		fprintf(stderr,"\t0 =< f1.d (%d) =< 3\n",fantasmes[i].d);
		fclose(fit);
		exit(5);
	    }else{
	    	fprintf(stderr,"Fantasma %d: Fila %d i Columna %d\n",i,fantasmes[i].f,fantasmes[i].c);
	    }
	    
	    i++;
	}
  	
  
  n_fantasmes = i;
  fprintf(stderr,"El valor del numero de fantasmes és %d\n",i);
  
  fclose(fit);			/* fitxer carregat: tot OK! */
  printf("Joc del MenjaCocos\n\tTecles: \'%c\', \'%c\', \'%c\', \'%c\', RETURN-> sortir\n",
		TEC_AMUNT, TEC_AVALL, TEC_DRETA, TEC_ESQUER);
  printf("prem una tecla per continuar:\n");
  getchar();
}






/* funcio per moure el menjacocos una posicio, en funcio de la direccio de   */
/* moviment actual; retorna -1 si s'ha premut RETURN, 1 si s'ha menjat tots  */
/* els cocos, i 0 altrament */
//int mou_menjacocos(void)
void * mou_menjacocos(void * null)
{
  char strin[12];
  objecte seg;
  int tec, ret;
  
  ret = 0;
  do{
  
    tec = win_gettec();
    //win_update();
  if (tec != 0)
   switch (tec)		/* modificar direccio menjacocos segons tecla */
   {
    case TEC_AMUNT:	  mc.d = 0; break;
    case TEC_ESQUER:  mc.d = 1; break;
    case TEC_AVALL:	  mc.d = 2; break;
    case TEC_DRETA:	  mc.d = 3; break;
    case TEC_RETURN:  ret = -1; break;
   }
 
    seg.f = mc.f + df[mc.d];	/* calcular seguent posicio */
    seg.c = mc.c + dc[mc.d];
  
    seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
    

    if ((seg.a == ' ') || (seg.a == '.')) //si la següent posició és possible
    {    
        win_escricar(mc.f,mc.c,' ',NO_INV);		/* esborra posicio anterior */
        mc.f = seg.f; mc.c = seg.c;			/* actualitza posicio */
        win_escricar(mc.f,mc.c,'0',NO_INV);		/* redibuixa menjacocos */
        if (seg.a == '.')
        {
          cocos--;
          fprintf(stderr,"cocos = %d\n",cocos);
          sprintf(strin,"Cocos: %d", cocos); win_escristr(strin);
          //sprintf(strin,"Fanta: %d",n_fantasmes); win_escristr(strin);
          if (cocos == 0) ret = 1;
        }
      
    }else if(xocs<2){
      //la següent posició és una pared
      if(parets[seg.f][seg.c]==0){
        //si no hem xocat abans conta com un xoc
        parets[seg.f][seg.c]=1;
        xocs++;
      }
    }
  
    fi1=ret;
    
  
    win_retard(mc.r*retard);
  }while(!fi1 && !fi2);

  return ((void*) (intptr_t) fi1);
}



/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
void inicialitza_joc(void)
{
  int r,i,j;
  char strin[12];
  int ex_code = 0;
  fprintf(stderr,"n_files = %d, n_cols = %d",n_fil1, n_col);
  for(int i=0;i<4;i++){
  	fprintf(stderr,"%c", tauler[i]);
  }
    	
  r = win_carregatauler(tauler,n_fil1-1,n_col,c_req);
  
  if (r == 0)
  {
  
	    mc.a = win_quincar(mc.f,mc.c);
	    
	    if (mc.a == c_req) r = -6;		/* error: menjacocos sobre pared */
	    
    
  }
  if(r ==0){
    cocos = 0;			/* compta el numero total de cocos */
    for (i=0; i<n_fil1-1; i++)
      for (j=0; j<n_col; j++){
      
        if (win_quincar(i,j)=='.') cocos++;
        
      }
      fprintf(stderr,"\nAL peincipi hi han cocos = %d\n",cocos);

    
    win_escricar(mc.f,mc.c,'0',NO_INV); 
    

  
    if (mc.a == '.') cocos--;	/* menja primer coco */
    fprintf(stderr,"Ara passen a cocos = %d\n",cocos);
    

    sprintf(strin,"Cocos: %d", cocos); 
    
    win_escristr(strin);
   
  }
  if (r < 0)
  {	win_fi();
	fprintf(stderr,"Error: no s'ha pogut inicialitzar el joc:\n");
	switch (r)
	{ case -1: fprintf(stderr,"  nom de fitxer erroni\n"); break;
	  case -2: fprintf(stderr,"  numero de columnes d'alguna fila no coincideix amb l'amplada del tauler de joc\n"); break;
	  case -3: fprintf(stderr,"  numero de columnes del laberint incorrecte\n"); break;
	  case -4: fprintf(stderr,"  numero de files del laberint incorrecte\n"); break;
	  case -5: fprintf(stderr,"  finestra de camp de joc no oberta\n"); break;
	  case -6: fprintf(stderr,"  posicio inicial del menjacocos damunt la pared del laberint\n"); break;
	  case -7: fprintf(stderr,"  posicio inicial del fantasma %d damunt la pared del laberint\n",ex_code); break;
	}
	exit(7);
  }

}


int ultim_fantasma_creat = 0;
void crear_fantasma(){
 
  
        int i = ultim_fantasma_creat; //creem el ultim fantasma
        
        if(i < n_fantasmes){ //si no hem creat tots els fantasmes
          fantasmes[i].a = win_quincar(fantasmes[i].f,fantasmes[i].c);
          
          if (fantasmes[i].a == c_req){ //comprovem posició code ok
            win_fi();
            r = -7;	/* error: fantasma sobre pared */
          
            fprintf(stderr,"ERROR: posicio inicial del fantasma %d damunt la pared del laberint\n",i); 
            exit(7);
          }
          
          win_escricar(fantasmes[i].f,fantasmes[i].c,(char) ('1'+i),NO_INV);
          //TODO revisar paràmetres
          char str_fi1[20];
          int id_fi1 = ini_mem(sizeof(int)); //creem la zona de memoria compartida
          int *p_fi1 = map_mem(id_fi1); //fem el mapeig de la zona de memoria compartida
          *p_fi1 = fi1; //inicialitzem la zona de memoria compartida
          sprintf(str_fi1,"%i",id_fi1); //passem l'identificador de la zona de memoria compartida a un string
      //fem el mateix per fi2
          char str_fi2[20];
          int id_fi2 = ini_mem(sizeof(int)); //creem la zona de memoria compartida
          int *p_fi2 = map_mem(id_fi2); //fem el mapeig de la zona de memoria compartida
          //*p_fi2 = fi2; //inicialitzem la zona de memoria compartida
          sprintf(str_fi2,"%i",id_fi2); //passem l'identificador de la zona de memoria compartida a un string
          // fem el mateix per df
          char str_df[20];
          int id_df = ini_mem(sizeof(int)*4); //creem la zona de memoria compartida
          int *p_df = map_mem(id_df); //fem el mapeig de la zona de memoria compartida
          p_df[0] = df[0]; //inicialitzem la zona de memoria compartida
          p_df[1] = df[1]; //inicialitzem la zona de memoria compartida
          p_df[2] = df[2]; //inicialitzem la zona de memoria compartida
          p_df[3] = df[3]; //inicialitzem la zona de memoria compartida
          sprintf(str_df,"%i",id_df); //passem l'identificador de la zona de memoria compartida a un string
          //fem el mateix per dc
          char str_dc[20];
          int id_dc = ini_mem(sizeof(int)*4); //creem la zona de memoria compartida
          int *p_dc = map_mem(id_dc); //fem el mapeig de la zona de memoria compartida
          p_dc[0] = dc[0]; //inicialitzem la zona de memoria compartida
          p_dc[1] = dc[1]; //inicialitzem la zona de memoria compartida
          p_dc[2] = dc[2]; //inicialitzem la zona de memoria compartida
          p_dc[3] = dc[3]; //inicialitzem la zona de memoria compartida
          sprintf(str_dc,"%i",id_dc); //passem l'identificador de la zona de memoria compartida a un string
          //creem zona de memoria compartida per fantsmes
          char str_fantasmes[20];
          int id_fantasmes = ini_mem(sizeof(objecte)*n_fantasmes); //creem la zona de memoria compartida
          objecte *p_fantasmes = map_mem(id_fantasmes); //fem el mapeig de la zona de memoria compartida
          for(i = 0; i < n_fantasmes; i++){
              p_fantasmes[i] = fantasmes[i]; //inicialitzem la zona de memoria compartida
          }
          sprintf(str_fantasmes,"%i",id_fantasmes); //passem l'identificador de la zona de memoria compartida a un string
          //fem el mateix amb mc
          char str_mc[20];
          int id_mc = ini_mem(sizeof(objecte)); //creem la zona de memoria compartida
          objecte *p_mc = map_mem(id_mc); //fem el mapeig de la zona de memoria compartida
          *p_mc = mc; //inicialitzem la zona de memoria compartida
          sprintf(str_mc,"%i",id_mc); //passem l'identificador de la zona de memoria compartida a un string
          //fem el mateix d'abans amb la variable retard
          char str_retard[20];
          int id_retard = ini_mem(sizeof(int)); //creem la zona de memoria compartida
          int *p_retard = map_mem(id_retard); //fem el mapeig de la zona de memoria compartida
          *p_retard = retard; //inicialitzem la zona de memoria compartida
          sprintf(str_retard,"%i",id_retard); //passem l'identificador de la zona de memoria compartida a un string
          //fem el mateix d'abans amb la variable retard
          if(pthread_create(&coco,NULL,mou_menjacocos,NULL)!=0){
              fprintf(stderr,"Error al crear el thread del menjacocos\n");
              exit(1);
          }
          int n = 0;
          char id_proces[20];
          //inicialitza_joc();
          fprintf(stderr,"\nhi han %d fantasmes\n",n_fantasmes);
          for(i = 0; i < n_fantasmes; i++){
              tpid[n] = fork();
              if(tpid[n] == (pid_t) 0){
                  sprintf(id_proces,"%i",i); //passem l'identificador del thread a un string
                  if(execlp("./fantasmes3","fantasmes3",str_fi1,str_fi2,id_proces,str_fantasmes,str_mc,str_retard,str_win,str_n_fil,str_n_col,(char *)0)==-1){
                      fprintf(stderr,"Error al crear el proces del fantasma %d\n",i);
                      exit(0);
                  }
              }else if(tpid[n] >0){
                  n++; //incrementem el numero de threads creats si és el pare
              }else{
                  fprintf(stderr,"Error al crear el proces del fantasma %d\n",i);
                  exit(0);
              }
          }
          ultim_fantasma_creat++;
        }
}
/* programa principal				    */
int main(int n_args, const char *ll_args[])
{
    int  rc;		/* variables locals */
    
     void *p_win;
       char str_win[20],str_n_fil[10],str_n_col[10];
       //int n_fil, n_col;
    srand(getpid());		/* inicialitza numeros aleatoris */

    

    if ((n_args != 2) && (n_args !=3))
    {	fprintf(stderr,"Comanda: cocos0 fit_param [retard]\n");
        exit(1);
    }
    carrega_parametres(ll_args[1]);
    
    //fprintf(stderr,"Fantasma %d: Fila %d i Columna %d\n",1,fantasmes[1].f,fantasmes[1].c);
    if (n_args == 3) retard = atoi(ll_args[2]);
    else retard = 100;
    //inicialitza_joc();
    fprintf(stderr,"n_files = %d, n_cols = %d",n_fil1, n_col); 
     rc = win_ini(&n_fil1,&n_col,'+',INVERS);	/* intenta crear taulell */
     
    if (rc >= 0)		/* si aconsegueix accedir a l'entorn CURSES */
    {
        int id_win = ini_mem(rc);	/* crear zona mem. compartida */
        p_win = map_mem(id_win);	/* obtenir adres. de mem. compartida */
        
        sprintf(str_win,"%i",id_win);
        sprintf(str_n_fil,"%i",n_fil1);	/* convertir mides camp en string */
        sprintf(str_n_col,"%i",n_col);

        win_set(p_win,n_fil1,n_col);		/* crea acces a finestra oberta */
         fprintf(stderr,"n_files = %d, n_cols = %d",n_fil1, n_col);
        inicialitza_joc();
          //win_set(p_win,n_fil1,n_col);	
        
         
        do{
          //Si ha xocat 2 cops hem de crear un nou fantasma
          if(xocs == 2){
            xocs = 0;
            crear_fantasma();
          }

          win_update();
          win_retard(100);
          fi2 = *p_fi2;
          *p_fi1 = fi1; //el controlem desde aquí
          
        }while(!fi1&&!fi2);
       
         pthread_join(coco, (void **)&fi1);
        for(int th=0; th<n_fantasmes;th++){
            waitpid(tpid[th],&fi2,NULL); //esperem que el fill acabi
        }
        
       
        
        elim_mem(id_fi1); //eliminem la zona de memoria compartida
        elim_mem(id_fi2); //eliminem la zona de memoria compartida
        elim_mem(id_df); //eliminem la zona de memoria compartida
        elim_mem(id_dc); //eliminem la zona de memoria compartida
        elim_mem(id_win);

        win_fi();
        if (fi1 == -1) printf("S'ha aturat el joc amb tecla RETURN!\n");
        else { if (fi1) printf("Ha guanyat l'usuari!\n");
            else printf("Ha guanyat l'ordinador!\n"); }
    }
    else
    {	
      fprintf(stderr,"Error: no s'ha pogut crear el taulell:\n");
        switch (rc)
        { case -1: fprintf(stderr,"camp de joc ja creat!\n");
            break;
        case -2: fprintf(stderr,"no s'ha pogut inicialitzar l'entorn de curses!\n");
            break;
        case -3: fprintf(stderr,"les mides del camp demanades son massa grans!\n");
            break;
        case -4: fprintf(stderr,"no s'ha pogut crear la finestra!\n");
            break;
        default: fprintf(stderr,"error desconegut, codi %d!\n",rc);
        }
        exit(6);
    }
    return(0);
}
