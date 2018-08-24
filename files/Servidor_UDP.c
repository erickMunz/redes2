#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>


unsigned char mensaje[512];
unsigned char mensaje2[512];

int main(){

	struct  sockaddr_in servidor, cliente;

	int udp_socket;
	int lbind;
	int tam;

	int ptam;

	udp_socket = socket(AF_INET,SOCK_DGRAM,0); //creamos el socket (descriptor de socket)

	if (udp_socket == -1){

		perror("\nError al abrir el socket");//siempre llama a una variable errno
		exit(1); //salida de error

	} else {

		perror("\nExito al abrir el socket");

		memset(&servidor,0x00,sizeof(servidor)); // para resetear la memoria de la estructura los pone en valor 0x00

		servidor.sin_family = AF_INET;  //establecemos la familia
		servidor.sin_port = htons(8000); // establecemos puerto aleatorio
		servidor.sin_addr.s_addr = INADDR_ANY; /* inet_addr("0.0.0.0") */  /* Es para establecer la ip*/

		lbind = bind(udp_socket, (struct sockaddr *) &servidor,sizeof(servidor));	//Asocia la direccion ip al socket
																			// creamos bind mandando descriptor de socket
																		    //la estrucutra casteada y el tamaño de la estructura

		if(lbind == -1){

			perror("\nError en bind");
			exit(1);

		} else {

			perror("\nExito en bind");

			memset(&cliente,0x00,sizeof(cliente)); // para resetear la memoria de la estructura los pone en valor 0x00

			cliente.sin_family = AF_INET;  //establecemos la familia
			cliente.sin_port = htons(8001); // establecemos puerto aleatorio
			cliente.sin_addr.s_addr = inet_addr("10.0.0.15");//INADDR_ANY;inet_addr("8.8.8.8"); /* inet_addr("0.0.0.0") */  /* Es para establecer la ip*/

			for(int i=0; i<4;i++){
				ptam = sizeof(cliente);
				tam = recvfrom(udp_socket,mensaje,sizeof(mensaje),0,(struct sockaddr *) &cliente,&ptam);//recibimos el mensaje desde el destino "cliente"

				if (tam == -1){
					perror("\nError al recibir el mensaje");
				} else {
					perror("\nExito al recibir mensaje: ");
					printf("Mensaje recibido: ");
					puts(mensaje); // imprime mensaje recibido
					//printf("Direccion ip: %d ",cliente.sin_addr.s_addr);//imprime direccion ip del cliente
				}
				fflush(stdin);
				printf("Mensaje a enviar;") ;
				fgets(mensaje2,sizeof(mensaje2),stdin);
				tam = sendto(udp_socket,mensaje2,sizeof(mensaje2),0,(struct sockaddr *) &cliente,sizeof(cliente));//mandamos el mensaje a el destino "remota

				if(tam == -1){
					perror("\n Error al enviar");
				} else {
					perror("\nExito al enviar");
				}
			}


		}
		close(udp_socket); //cerrar el socket
		return 1;
	}
}
