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

void *receiver(void *attributes);

unsigned char mensaje[512];
unsigned char mensaje2 [512] ;

int main(){
	
	struct  sockaddr_in local, remota;
	
	int udp_socket;
	int lbind;
	int tam;

	int ptam;

	pthread_t thread0;

	udp_socket = socket(AF_INET,SOCK_DGRAM,0); //creamos el socket (descriptor de sockets)

	if (udp_socket == -1){

		perror("\nError al abrir el socket");//siempre llama a una variable errno	
		exit(1);//salida de error

	} else {

		perror("\nExito al abrir el socket");
		
		memset(&local,0x00,sizeof(local)); // para resetear la memoria de la estructura los pone en valor 0x00
		
		local.sin_family = AF_INET;  //establecemos la familia del socket
		local.sin_port = htons(8000); // establecemos puerto aleatorio para el socket
		local.sin_addr.s_addr = INADDR_ANY; /* inet_addr("0.0.0.0") */  /* Es para establecer la ip*/

		lbind = bind(udp_socket, (struct sockaddr *) &local,sizeof(local));	//Asocia la direccion ip al socket
																			// creamos bind mandando descriptor de socket 
																		    //la estrucutra casteada y el tamaño de la estructura

		if(lbind == -1){

			perror("\nError en bind");
			exit(1);

		} else {

			perror("\nExito en bind");
			
			memset(&remota,0x00,sizeof(remota)); // para resetear la memoria de la estructura los pone en valor 0x00
		
			remota.sin_family = AF_INET;  //establecemos la familia
			remota.sin_port = htons(8000); // establecemos puerto aleatorio
			remota.sin_addr.s_addr = inet_addr("10.0.0.8");//INADDR_ANY;inet_addr("8.8.8.8"); /* inet_addr("0.0.0.0") */  /* Es para establecer la ip*/

			ptam = sizeof(remota);

			for (int i = 0; i<4;i++){
				fflush(stdin);
				fgets(mensaje,sizeof(mensaje),stdin);
				
				tam = sendto(udp_socket,mensaje,sizeof(mensaje),0,(struct sockaddr *) &remota,sizeof(remota));//mandamos el mensaje a el destino "remota

				if (tam == -1){
					perror("\nError al enviar");
				} else {
					perror("\nExito al enviar");
				}


				tam = recvfrom(udp_socket,mensaje2,sizeof(mensaje2),0,(struct sockaddr *) &remota,&ptam);//recibimos el mensaje desde el destino "cliente"

				if (tam == -1){
					perror("\nError al recibir");
				} else {
					perror("\nExito al recibir mensaje: ");
					puts(mensaje2);
				}
			}
		}
	}

	close(udp_socket); //cerrar el socket
	return 1; 	
}

/* this function is run by the second thread */
void *receiver(void *attributes){

	/* increment x to 100 */
	int *x_ptr = (int *)x_void_ptr;
	while(++(*x_ptr) < 100){
		printf("x: %d\n",*x_ptr);
	}

	printf("x increment finished\n");

	pthread_exit(NULL);

	/* the function must return something - NULL will do */
	return NULL;

}
