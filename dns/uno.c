#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

unsigned char dns[5000];

unsigned char dns1[5000];
unsigned char Lnegra [10][50] = {"www.google.com","www.pornohub.com"};


int formatoName (unsigned char *nombre, int aux, unsigned char *dest){
    int j=0, veces=0;
    while(aux < strlen(nombre)){
            dest[j]= nombre[j];
            if(j==aux){
                if(veces<2){
                    j++;
                    aux+=nombre[j]+1;
                    dest[j]=46;
                    veces++;
                }
        }
        j++;
    }
}

int isListaNegra(unsigned char *ip){
    int i;
    for(i=0;i<strlen(ip);i++){
        printf("tam ln %d tamip %d \n",strlen(Lnegra[i]),strlen(ip));
    }
    for(i=0;i<10;i++){
        //printf("lista negra en %d => %s tam %lu : otro %s tam %lu \n",i,Lnegra[i],strlen(Lnegra[i]),ip,strlen(ip));
        if(strcmp(Lnegra[i],ip)==0){
            printf("Entre ALV \n");
            return 1;
        }
    }
    return 0;
}
int getINT(unsigned char* dns, int indice){
    short int entero;
    entero =  dns[indice];
    entero <<= 8;
    entero += dns[indice+1];
    return entero;
}
void formatoINT(int indice, int numero, unsigned char* fin){
    fin[indice] = (numero&0xff00)>>8; ;
    fin[indice+1] = (numero&0x00ff);
}
void formatoRespuestaDNS(unsigned char* fin){
    fin[2] = 129;
    fin[3] = 128;
}
int main(){
    struct sockaddr_in cliente;
	struct sockaddr_in servidor, Gserv;
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    unsigned char buffer[1024];
    if(udp_socket == -1){
        perror("Error al abrir el socket\n");
    }else{
        memset(&servidor, 0x00, sizeof(servidor)); //Setea la estructura
        printf("Exito al abrir el socket\n");

        servidor.sin_family = AF_INET; //Asigna la familia
        servidor.sin_port = htons(53); //Se asigna un puerto al azar por htons(0)
        servidor.sin_addr.s_addr = INADDR_ANY; //Se asigna la direccion ip actual de la tarjeta de red por el INADDR_ANY
        int lbind = bind(udp_socket, (struct sockaddr*)&servidor, sizeof(servidor)); //Llamada a bind()
        cliente.sin_family = AF_INET;
        cliente.sin_port = htons(0);
        int i = 12;
       if(lbind==-1){
           printf("error abriendo el puerto \n");
       }else{
           printf("exito en el bind \n");
            while(1){
                unsigned int tam, len = (unsigned int)sizeof(cliente);
        		
            	//inet_aton("127.0.0.1", &cliente.sin_addr);
            	if((tam = recvfrom(udp_socket, dns, sizeof(dns), 0, (struct sockaddr*)&cliente, &len)) == -1){
            		perror("Error al recibir\n");
                }else{
                    char data[1024], nombre[1014];
                    int j = 0;
                    while(dns[i] != '\0'){
                        data[j++] = dns[i++];
                        };
                    formatoName(data, dns[12], nombre);
                    printf("despues %s",nombre);
                    printf("\n");
                    if(!isListaNegra(data)){
                        printf("IP bloqueada mandando respuesta \n");
                        //opcode
                        int id = getINT(dns,0);
                        printf("\n Transaccion %d", id);
                        formatoRespuestaDNS(dns);
                        //el indice en este punto esta en 4
                        formatoINT(4,1,dns);
                        //answers
                        formatoINT(6,1,dns);
                        //authority
                        formatoINT(8,0,dns);
                        //aditional
                        formatoINT(10,0,dns);
                        //int tam = 12;
                        //Aqui se supone que empieza la respuesta, se suponeeee
                        i=j;
                        i+=16;
                        printf("j -> %d ",i);
                        //PTR
                        dns[++i]=192;
                        dns[++i] = 12;
                        //type
                        formatoINT(++i,1,dns);
                        i++;
                        //clase
                        formatoINT(++i,1,dns);
                        i++;
                        //TTL response 
                        formatoINT(++i,0,dns);
                        i++;
                        formatoINT(++i,12,dns);
                        i++;
                        //tam de datos
                        formatoINT(++i,4,dns);
                        i++;
                        i++;
                        //IP regresada
                        dns[i++] = 127;
                        dns[i++] = 0;
                        dns[i++] = 0;
                        dns[i++] = 1;
                        printf("\n indice %d \n",i);
                        /*i++;
                        formatoINT(++i,1,dns);*/
                        socklen_t tcli = sizeof(cliente);
                        if(sendto(udp_socket,(char *) dns,i,0,(struct sockaddr *) &cliente,tcli)==-1){
                            printf("error el enviar el mensaje");
                        }else{
                             printf("Se envio correctamente el mensaje al cliente \n");
                        }
                    }else{
                        printf("IP permitida \n");
                        Gserv.sin_family = AF_INET;
                        Gserv.sin_addr.s_addr = inet_addr("148.204.103.2");
                        Gserv.sin_port = htons(53);
                        
                        socklen_t lonm = sizeof(Gserv);
                        if(sendto(udp_socket,(char *) dns,tam,0,(struct sockaddr *) &Gserv,lonm)==-1){
                            printf("Error al enviar \n");
                        }else{
                            printf("si pude enviarlo \n");
                            int len_res = recvfrom(udp_socket, (char *) dns1, sizeof(dns1), 0, (struct sockaddr *)&Gserv, lonm);
                            if(len_res!=-1){
                                printf("error recibiendo del dns de google \n");
                            }else{
                                printf("enviando al cliente origen \n");
                                socklen_t tcli = sizeof(cliente);
                                if(sendto(udp_socket,(char *) dns1,sizeof(dns1),0,(struct sockaddr *) &cliente,tcli)==-1){
                                    printf("error el enviar el mensaje");
                                }else{
                                    printf("Se envio correctamente el mensaje al cliente \n");
                                }
                                printf("\n \n que verga");
                            }
                        }
                    }
                }
            }
       }
    }
    
    return 0;
}