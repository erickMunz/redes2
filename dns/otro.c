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
unsigned char dns[5000];


int main(int argc, char const *argv[]){
    //srand(time(NULL));
    printf("hola");
    int s, s2;
   socklen_t clilen;
   struct sockaddr_in servidor, aCliente, aServidor, cliente;
   s  = socket(AF_INET, SOCK_DGRAM, 0);
   s2 = socket(AF_INET, SOCK_DGRAM, 0);
   if(s==-1||s2==-1){
       printf("uno de los sockets valio verga \n");
   }
   memset(&servidor, 0x00, sizeof(servidor));
   //bzero((char *)&servidor, sizeof(servidor));
   servidor.sin_addr.s_addr = INADDR_ANY;
   servidor.sin_family = AF_INET;
   servidor.sin_port = htons(53);
   if(bind(s2, (struct sockaddr *)&servidor,sizeof(servidor))==-1){
        printf("error en bind servidor");
    }
   clilen = sizeof(aCliente);
   while(1){
       printf("hola");
       short int preg, rr, codigo,i;
       socklen_t lonm = sizeof(cliente);
        int len_res = recvfrom(s2, (char *) dns, sizeof(dns), 0, (struct sockaddr *)&cliente, lonm);
        if(len_res==-1){
            printf("Llego algo ALV");
            int v = 12;
            char datos[1024];
            int j=0;
            while(dns[v] != '\0'){
                        datos[j++] = dns[v++];
                        printf("%d \n ", datos[j]);
            }
        }else{
            printf("no llego ni vergas");
        }
   }
}

void pptr(int ptr){
   int sz = dns[ptr];
   int i;
   for(i = 1; i<=sz;i++)
      printf("%c", dns[ptr+i]);
   if(dns[ptr + sz + 1] == 0x00) return;
   else if(dns[ptr + sz + 1] == 0xc0){
     printf(".");
     pptr(dns[ptr + sz + 2]);
   }else{
      printf(".");
      pptr(ptr + sz + 1);
   }
}
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
