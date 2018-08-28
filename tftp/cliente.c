#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


int main(){
	struct sockaddr_in local,remota;
	int udp_socket, lbind, tam;
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if(udp_socket == -1)
	{
		perror("\nError al abrir el socket");
		exit(1);
	}else
	{
		perror("\nExito al abrir el socket");
		memset(&local,0x00,sizeof(local));
		local.sin_family = AF_INET;
		local.sin_port = htons(0);
		local.sin_addr.s_addr = INADDR_ANY;
		lbind = bind(udp_socket, (struct sockaddr *)&local,sizeof(local));
		if(lbind == -1)
		{
			perror("\nError en bind");
			exit(1);
		}
		else{
			perror("\nExito en bind");
			memset(&remota,0x00,sizeof(remota));
			remota.sin_family = AF_INET;
			remota.sin_port = htons(69);
			remota.sin_addr.s_addr = inet_addr("8.8.0.14");
			unsigned char * mensaje [720];
			funcionLectura(mensaje,"nombreArchivo");
			tam = sendto(udp_socket,mensaje,20,0,(struct sockaddr*)&remota,sizeof(remota));
			if (tam == -1){
				perror("\nError al enviar");
				exit(1);
			}
			else{
				printf(" esperando respuesta\n");
				if(recvfrom(udp_socket,mensaje,512,MSG_DONTWAIT,(struct sockaddr*)&remota,sizeof(remota))!=-1){
					printf("mensaje recibido \n \n");
					printf(" mensaje \n");
				}
			}

		}
		close(udp_socket);
		return 1;
	}

//xhost +si:localuser:root
}
void funcionLectura(unsigned char* trama, char * nombreArchivo){
	trama[0] = 0;
	trama[1] = 1;
	strcpy(trama+2, nombreArchivo);
	strcpy(trama+strlen(nombreArchivo),0x00);
}

