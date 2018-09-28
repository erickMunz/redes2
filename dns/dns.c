#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#define puerto 53


unsigned char dns[5000];

int setID(unsigned char *dns){
    unsigned short int id = ((rand()%(65535)) + 1);
      // dns[0]; id <<= 8;id += dns[1];
      dns[0] = (id&0xff00)>>8;
      dns[1] = (id&0x00ff);
      return id;
}

struct sockaddr_in inicializa(int p, char* direccion){
    struct sockaddr_in est;
    est.sin_family = AF_INET;
    est.sin_port = htons(p);
    if (strcmp(direccion,"0")==0){
        est.sin_addr.s_addr = INADDR_ANY;
    }else{
        printf("direccion %s", direccion);
        est.sin_addr.s_addr = inet_addr(direccion);
    }
    return est;
}
void formatoINT(int indice, int numero, unsigned char* fin){
    fin[indice] = (numero&0xff00)>>8; ;
    fin[indice+1] = (numero&0x00ff);
}
int getINT(unsigned char* dns, int indice){
    short int entero;
    entero =  dns[indice];
    entero <<= 8;
    entero += dns[indice+1];
    return entero;
}

int formatoPreguntas(int indice, unsigned char*trama, unsigned char *origen){
        char *tsub = strtok(origen,".");
        int i = 0, aux= 0;
        while(tsub!=NULL){
            trama[indice++] = strlen(tsub);
            for (i=0;i<strlen(tsub);i++){
                trama[indice++]=tsub[i];
                aux++;
            }
            tsub = strtok (NULL, ".");
    }
    trama[indice++] = '\0';
    return indice;
}

int main(int argc, char const *argv[]){
    //srand(time(NULL));
     
    int s, s2;
   socklen_t clilen;
   struct sockaddr_in servidor, aCliente, aServidor, cliente;
   
   s  = socket(AF_INET, SOCK_DGRAM, 0);
   
   s2 = socket(AF_INET, SOCK_DGRAM, 0);
   if(s==-1||s2==-1){
       printf("uno de los sockets valio verga \n");
   }
   bzero((char *)&aServidor, sizeof(aServidor));
   //aServidor= inicializa(puerto,argv[1]);
   aServidor.sin_family = AF_INET;
   aServidor.sin_addr.s_addr = inet_addr(argv[1]);
   aServidor.sin_port = htons(53);
   bzero((char *)&cliente, sizeof(cliente));
   //cliente = inicializa(0,"0");
   cliente.sin_family = AF_INET;
   cliente.sin_addr.s_addr = INADDR_ANY;
   /*cuando se utiliza por numero de puerto el 0, el sistema se encarga de asignarle uno */
   cliente.sin_port = htons(0);
   if(bind(s2, (struct sockaddr *)&cliente,sizeof(cliente))==-1){
       printf("error en bind cliente");
   }
   /* se asigna una direccion al socket del servidor*/
   /*bzero((char *)&servidor, sizeof(servidor));
   servidor=inicializa(puerto,"0");
   if(bind(s, (struct sockaddr *)&servidor, sizeof(servidor))==-1){
       printf("error en el bind servidor");
   }*/
   clilen = sizeof(aCliente);
   while(1){
       short int preg, rr, codigo,i;
       printf("Transaccion %d \n", setID(dns));
       printf("Escribe el codigo de operacion \n ");
       scanf("%hu", &codigo);
       formatoINT(2,codigo, dns);
       printf("Escribe el numero de preguntas que haras al servidor \n");
       scanf("%hu", &preg); 
       formatoINT(4,preg,dns);
       //answers
       formatoINT(6,0,dns);
       //authority
       formatoINT(8,0,dns);
       //aditional
       formatoINT(10,0,dns);
       int tam = 12;
       for(i=0;i<preg;i++){
           char name[1024];
           fflush( stdout );
           printf("Nombre = \n");
           scanf("%s", name);
           fflush( stdout );
           tam = formatoPreguntas(tam,dns,name);
           short int type;
           printf("Tipo: \n");
           scanf("%hu", &type);
           dns[tam++] = 0;
           dns[tam++] = type;
           short int class;
           printf("Clase :\n");
           scanf("%hu", &class);
           dns[tam++] = 0;
           dns[tam++] = class;
           
        }
        socklen_t lonm = sizeof(aServidor);
        if(sendto(s2,(char *) dns,tam,0,(struct sockaddr *) &aServidor,lonm)==-1){
            printf("Error al enviar \n");
        }
        else{
            int len_res = recvfrom(s2, (char *) dns, sizeof(dns), 0, (struct sockaddr *)&aServidor, lonm);
            if(len_res==-1){
                printf("\n\n\n\n\n\n\n*********Respuesta del servidor *************");
                printf("\n* Transaccion       : %hu *", getINT(dns,0));
                printf("\n* Codigo Operacion  : %hu *", getINT(dns,2));
                printf("\n* Preguntas         : %hu *", getINT(dns,4));
                printf("\n* Respuestas RR     : %hu *", getINT(dns,6));
                printf("\n* Autoridad         : %hu *", getINT(dns,8));
                printf("\n* Adicional         : %hu *", getINT(dns,10));
                int i=0,v=12, j= 0;
                for(i = 0; i<getINT(dns,4);i++){
                    printf("tam %d", dns[v]);
                    while(v > 0){
                        printf("%c", dns[v+j]);
                        j++;
                        if(j==dns[v]){
                            v=dns[v+j];
                        }
                    }
                }
            }
            
        }
    }
}