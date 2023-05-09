cocos0 : cocos0.c winsuport.o winsuport.h
	gcc -Wall cocos0.c winsuport.o -o cocos0 -lcurses


cocos1 : cocos1.c winsuport.o winsuport.h
	gcc -Wall cocos1.c winsuport.o -o cocos1 -lcurses -lpthread


cocos2 : cocos2.c winsuport.o winsuport.h
	gcc -Wall cocos2.c winsuport.o -o cocos2 -lcurses -lpthread

cocos3 : cocos3.c memoria.o missatge.o winsuport2.o winsuport2.h fantasmes3.c
	gcc -w cocos3.c memoria.o missatge.o winsuport2.o -o cocos3 -lcurses -lpthread
	gcc -w fantasmes3.c memoria.o missatge.o winsuport2.o -o fantasmes3 -lcurses
	
cocos4: cocos4.c memoria.o missatge.o winsuport2.o winsuport2.h fantasmes4.c semafor.o
	gcc -w cocos4.c memoria.o missatge.o semafor.o  winsuport2.o -o cocos4 -lcurses -lpthread
	gcc -w fantasmes4.c memoria.o missatge.o semafor.o winsuport2.o -o fantasmes4 -lcurses -lpthread

memoria.o : memoria.c memoria.h
	gcc -c -Wall memoria.c -o memoria.o 
missatge.o : missatge.c missatge.h
	gcc -c -Wall missatge.c -o missatge.o
winsuport2.o : winsuport2.c winsuport2.h
	gcc -c -Wall winsuport2.c -o winsuport2.o

winsuport.o : winsuport.c winsuport.h
	gcc -c -Wall winsuport.c -o winsuport.o 

semafor.o : semafor.c semafor.h
	gcc -c -Wall semafor.c -o semafor.o 

clean:
	rm -f cocos0 cocos1 cocos2 cocos3 winsuport.o
