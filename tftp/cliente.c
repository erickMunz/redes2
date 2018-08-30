#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


int main(int argc, char *argv[]){
	if(argc <2){
		printf("no hay suficientes argumentos \n");
		return 0;
	}
	struct sockaddr_in local,remota, server;
	int udp_socket, lbind, tam;
	FILE *archivo;
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
			printf("ip %s", argv[1]);
			inet_pton(AF_INET, argv[1], &(remota.sin_addr));
			remota.sin_addr.s_addr = inet_addr(argv[1]);
			unsigned char * mensaje [520];
			char nombre[20];
			//char *buffer[10000];
			int opc;
			printf("\n\n+++++++ Menu TFTP ++++++\n 1 --> peticion de lectura \n 2 --> peticion de lectura \n ");
			scanf("%d",&opc);
			switch(opc){
				case 1:
						printf("Escribe el nombre del archivo \n");
						scanf("%s",nombre);
						int tam = peticionLectura(mensaje, nombre);
						if(sendto(udp_socket,mensaje,tam,0,(struct sockaddr*)&remota,sizeof(remota)) != -1){
							if(recvfrom(udp_socket, mensaje, 516, 0, (struct sockaddr*)&remota, sizeof(remota) == -1)){
								printf("Error al recibir \n");
							}else{
								printf("Exito al recibir \n");
								if(operacion(mensaje)==3){
									int sec = secuenciaACK(mensaje);
									archivo = fopen(nombre,"wb+");
									if(archivo!=NULL){
										fwrite(datosACK(mensaje),sizeof(char),sizeof(datosACK(mensaje)),archivo);
										//seguir con el flujo del tftp
										while(i==veces){

										}
									}
									else{
										printf("Error en el ack");
									}
								}else{

								}
							}
							
						}else{
							printf("error al enviar la peticion");
						}
						
						break;
				case 2:
						printf("Escribe el nombre del archivo \n");
						scanf("%s",nombre);
						archivo = fopen(nombre,"rb");
						if(archivo!=NULL){
							int t = tamArchivo(archivo);
							char* buffer = (char*)malloc(t);
							fread(buffer,tamArchivo(archivo),1,archivo);
							printf("tamaño de archivo %d",t);
							int veces = t/512;
							int i = 0;
							while(i==veces){

							}
						}
						else{

							printf("no se pudo abrir el archiv especificado");
							return 0;
						}
						break;
				case 3:
						printf("DATA");
						break;
				default: 
						printf("no haz puesto una operacion valida");
						break;
			}
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
//RRQ

int operacion(unsigned char*trama){
	unsigned short opt_code = trama[0];
	opt_code <<= 8;
	opt_code += trama[1];	
	return opt_code;
}

int secuenciaACK(unsigned char* trama){
	unsigned char secuencia = trama[0];
	secuencia <<=8;
	secuencia +=trama[1];
	return secuencia;
}

unsigned char datosACK(unsigned char* trama){
	unsigned char *origen;
	for(int  i = 0; i<sizeof(trama)-4; i++){
		//printf("%hu ", buffer[i+4]);
		origen = trama[i+4];
	}
	return origen;
}
int peticionLectura(unsigned char* trama, char * nombreArchivo){
	trama[0] = 0;
	trama[1] = 1;
	strcpy(trama+2, nombreArchivo);
	strcpy(trama+3+strlen(nombreArchivo),"octet");
	trama[strlen(nombreArchivo) + strlen("octet") + 3] = 0;
	return strlen(nombreArchivo)+strlen("octet") +4;
}

//WRQ
void peticionEscritura(unsigned char* trama, char* nombreArchivo){
	trama[0] = 0;
	trama[1] = 2;
	strcpy(trama+2, nombreArchivo);
	
}
//ACK
void formatoACK(unsigned char* trama, int bloque){
	trama[0] = 0;
	trama[1] = 4;
	trama[3] = (bloque & 0xff00) >> 8;
	trama[4] = (bloque & 0x00ff);
}

void formatoDATA(unsigned char* trama, int indice, FILE *archivo){
	trama[0] = 0;
	trama[1] = 3;
	strcpy(trama+2,archivo);
}
int tamArchivo(FILE *arch){
	fseek(arch, 0, SEEK_END); 
	int tam = ftell(arch); 
	fseek(arch, 0, SEEK_SET); 
	return tam;
}
void escribirArchivo(FILE *fp, int offset){
	fp=fopen("c:\\test.txt", "w");
	if(fp==NULL){
		printf("Error abriendo el archivo \n");
	}else{
		fprintf(fp, "Testing...\n");
	}
}

