#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

unsigned char mensaje[] = "Hola red Santiago\0";

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
			remota.sin_port = htons(8000);
			remota.sin_addr.s_addr = inet_addr("8.12.0.212");
			
			tam = sendto(udp_socket,mensaje,20,0,(struct sockaddr*)&remota,sizeof(remota));
			if (tam == -1){
				perror("\nError al enviar");
				exit(1);
			}
			else{
				perror("\nExito al enviar");
			}

		}
		close(udp_socket);
		return 1;
	}

//xhost +si:localuser:root
}
