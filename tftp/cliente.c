	#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

//RRQ

int operacion(unsigned char*trama){
	unsigned short opt_code = trama[0];
	opt_code <<= 8;
	opt_code += trama[1];	
	return opt_code;

}

int secuenciaDATA(unsigned char* trama){
	short secuencia = trama[2];
	secuencia <<=8;
	secuencia +=trama[3] ;
	return secuencia;
}
void datosDATA(unsigned char* trama, int tam, unsigned char* dest){
	for(int  i = 0; i<tam; i++){
		dest[i] = trama[i+4];
	}

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
int  peticionEscritura(unsigned char* trama, char* nombreArchivo){
	trama[0] = 0;
	trama[1] = 2;
	strcpy(trama+2, nombreArchivo);
	strcpy(trama+3+strlen(nombreArchivo),"octet");
	trama[strlen(nombreArchivo) + strlen("octet") + 3] = 0;
	return strlen(nombreArchivo)+strlen("octet") +4;
	
}
//ACK
void formatoACK(unsigned char* trama, int bloque){
	trama[0] = 0;
	trama[1] = 4;
	trama[2] = (bloque & 0xff00) >> 8;
	trama[3] = (bloque & 0x00ff);
}

void formatoDATA(unsigned char* trama, int indice, unsigned char* datos, int tam){
	trama[0] = 0;
	trama[1] = 3;
	trama[2] = (indice  >> 8) & 0x00ff;
	trama[3] = indice & 0x00ff;

	for (int i= 0; i<tam;i++){
		trama[i+4] = datos[i];
	}
}
int tamArchivo(FILE *arch){
	fseek(arch, 0, SEEK_END); 
	int tam = ftell(arch); 
	fseek(arch, 0, SEEK_SET); 
	return tam;
}





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
			size_t tamSock = sizeof(remota);
			printf("ip %s", argv[1]);
			inet_pton(AF_INET, argv[1], &(remota.sin_addr));
			remota.sin_addr.s_addr = inet_addr(argv[1]);
			unsigned char * mensaje [1024];
			char nombre[20];
			//char *buffer[10000];
			int accion = 1;
			while(accion){
			printf("\n\n+++++++ Menu TFTP ++++++\n 1 --> peticion de lectura \n 2 --> peticion de escritura \n ");
			scanf("%d",&accion);
			switch(accion){
				case 1:
						printf("Escribe el nombre del archivo \n");
						scanf("%s",nombre);
						int tam = peticionLectura(mensaje, nombre);
						if(sendto(udp_socket,mensaje,tam,0,(struct sockaddr*)&remota,sizeof(remota)) != -1){
							int aux = recvfrom(udp_socket, mensaje, 516, 0, (struct sockaddr*)&remota, &tamSock);
							if( aux == -1){
								printf("Error al recibir \n");
							}else{
								printf("Exito al recibir \n");
								if(operacion(mensaje)==3){
									int sum,t;
									archivo = fopen(nombre,"wb+");
									if(archivo!=NULL){
										while(aux-4<=512){
											t= aux -4;
											sum+=t;
											unsigned char datos[t];
											datosDATA(mensaje, t,datos);
											fwrite(datos,1,t,archivo);
											formatoACK(mensaje,secuenciaDATA(mensaje));
											
											if(sendto(udp_socket, mensaje, aux, 0, (struct sockaddr*)&remota, sizeof(remota))==-1){
												printf("Error al enviar ACK %hu\n", secuenciaDATA(mensaje));
											}
											if(aux-4<512){
												
												printf("numero de paquetes %d \n",secuenciaDATA(mensaje));
												printf("bytes leidos %d",sum);
												break;
											}
											aux = recvfrom(udp_socket, mensaje, 516, 0, (struct sockaddr*)&remota, &tamSock); 
											//seguir con el flujo del tftp	
											
										
											
											}
											
											fclose(archivo);
										}
										else{
											printf("Error en el archivo");
										}
								}else{
									printf("Error no se encuentró el archivo %d",operacion(mensaje));
								}
							}
							
						}else{
							printf("error al enviar la peticion \n ");
						}
						
						break;
				case 2:
						printf("Escribe el nombre del archivo \n");
						scanf("%s",nombre);
						archivo = fopen(nombre,"rb");
						if(archivo!=NULL){
							int tam = peticionEscritura(mensaje,nombre);
							if(sendto(udp_socket, mensaje,tam,0,(struct sockaddr*)&remota, sizeof(remota))!=-1){
								int t = recvfrom(udp_socket,mensaje,516,0,(struct sockaddr*)&remota, &tamSock);
								if(t==-1){
									printf("Error con la conexión al servidor d\n ");
								}else{
									if(operacion(mensaje)==4){
										int datos[512];
										int bytes= fread(datos,1,512,archivo);
										int veces=1;
										int tam=0;
										printf("numeros %d",bytes);
										while(bytes>0){	
											formatoDATA(mensaje,veces,datos,bytes);	
											if(sendto(udp_socket,mensaje,bytes+4,0,(struct sockaddr*)&remota, tamSock)==-1){
												printf("Error al enviar DATA\n");
											}
											tam = recvfrom(udp_socket,mensaje,516,0,(struct sockaddr*)&remota,&tamSock);
											if(tam==-1){
												
												printf("error al recibir ACK \n");
											}
											bytes = fread(datos,1,512,archivo);
											if(bytes==0){
											formatoDATA(mensaje,veces,datos,bytes);	
											if(sendto(udp_socket,mensaje,bytes+4,0,(struct sockaddr*)&remota, tamSock)==-1){
												printf("Error al enviar DATA\n");
											}
											}
											veces++;
										}
										printf("Se envio el archivo con %d numero de secuencias \n",veces);
									}else{
										printf("ERROR!! opc %d",operacion(mensaje));
									}
								}
									
							}else{
								printf("Error en la conexión con el servidor");
							}
							
						}
						else{
							printf("El archivo no existe!!");
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
			}

		}
		close(udp_socket);
		return 1;
	}

//xhost +si:localuser:root
}
