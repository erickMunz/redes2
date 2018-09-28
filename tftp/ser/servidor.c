 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netinet/ip.h> /* superset of previous */
 #include <stdlib.h>
 #include <unistd.h>
 #include <stdio.h>
 #include <arpa/inet.h>
 #include <sys/time.h>
 #include <string.h>

int operacion(unsigned char*trama){
	unsigned short opt_code = trama[0];
	opt_code <<= 8;
	opt_code += trama[1];	
	return opt_code;

}

void getNombreArchivo(unsigned char *trama, unsigned char* destino){
  strcpy( destino, trama+2);
}

int secuenciaDATA(unsigned char* trama){
	unsigned short secuencia = trama[2];
	secuencia <<=8;
	secuencia +=trama[3];
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

/*void formatoError(unsigned char* trama){
  trama[0] = 0;
  trama[1] = 5;
  trama[2] = 3;
  strcpy(trama+3,"error prro");
  trama[3+strlen("error prro")]= 0;
}*/


int obtenerCodigoError(unsigned char *trama){
  short codigo;
	codigo = trama[2];
	codigo <<=8;
	codigo += trama[3];
	return codigo;

}

int formatoError(unsigned char *trama, int codigo, unsigned char *mensaje){	
	int tam = strlen(mensaje);
	trama[0] = 0;
	trama[1] = 5;
	trama[2] = (codigo & 0xf0) >> 8;
	trama[3] = codigo& 0x0f;
	strcpy(trama+4, mensaje);
	return 4+tam;
}


 int main()
 {
 struct sockaddr_in local, remota;    
 int udp_socket,lbind,tam,lrecv, bandera = 0;
 struct timeval start, end;
 long mtime, seconds, useconds;    
 char nombre[50];
 FILE *archivo;
 int tamSock = sizeof(remota);
 gettimeofday(&start, NULL);
 unsigned char mensaje[1014];
 udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
 if(udp_socket==-1)
   {
    perror("\nError al abrir el socket");
    exit(1);
   }
 else
   {

    perror("\nExito al abrir el socket");
    
    local.sin_family=AF_INET;
    local.sin_port=htons(69);
    local.sin_addr.s_addr=INADDR_ANY;
    lbind=bind(udp_socket,(struct sockaddr *)&local,sizeof(local));
    if(lbind==-1)
      {
      printf("\nError en bind");
      exit(1);
      }
    else
      {
      while(1){
        remota.sin_family = AF_INET;
        remota.sin_port = htons(0);
        FILE *archivo;
        size_t tamSock = sizeof(remota);
       lrecv=sizeof(remota);
      unsigned char mensaje[1014];
          tam=recvfrom(udp_socket,mensaje,512,MSG_DONTWAIT,(struct sockaddr*)&remota,&tamSock);
           if(tam!=-1)
            {
            int accion = operacion(mensaje);
            printf("operacion %d \n", accion);
            switch(accion){
              case 2:
                  getNombreArchivo(mensaje,nombre);
                  printf("Peticion escritura \n \n");
                  printf("nombre del archivo %s \n", nombre);
                  formatoACK(mensaje,0);
                  //int tam = peticionLectura(mensaje, nombre);
                  if(sendto(udp_socket,mensaje,tam,0,(struct sockaddr*)&remota,sizeof(remota)) != -1){
                    int aux = recvfrom(udp_socket, mensaje, 516, 0, (struct sockaddr*)&remota, &tamSock);
                    if( aux == -1){
                      printf("Error al recibir \n");
                    }else{
                      printf("Exito al recibir \n");
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
                            if(aux-4<512){
                              printf("numero de paquetes %d \n",secuenciaDATA(mensaje));
                              printf("bytes leidos %d",sum);
                              break;
                            }
                            if(sendto(udp_socket, mensaje, aux, 0, (struct sockaddr*)&remota, sizeof(remota))==-1){
                              printf("Error al enviar ACK %hu\n", secuenciaDATA(mensaje));
                            }

                            aux = recvfrom(udp_socket, mensaje, 516, 0, (struct sockaddr*)&remota, &tamSock); 
                            //seguir con el flujo del tftp	
                            
                          
                            
                            }
                            
                            fclose(archivo);
                          }
                          else{
                            printf("Error al abrir el archivo");
                          }
                      
                    }
                    
                  }else{
                    printf("error al enviar la peticion \n ");
                  }
                  
                  break;
              case 1:
            
                  getNombreArchivo(mensaje,nombre);
                  printf("Peticion lectura \n \n");
                  printf("nombre del archivo %s \n", nombre);
                  archivo = fopen(nombre,"rb");
                  if(archivo!=NULL){
                    
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
                            else{
                              if(secuenciaDATA(mensaje)!=veces){
                                printf("error en la secuencia \n");
                              }
                            }
                            bytes = fread(datos,1,512,archivo);
                            veces++;
                            if(bytes==0){
                              formatoDATA(mensaje,veces,datos,bytes);	
                              if(sendto(udp_socket,mensaje,bytes+4,0,(struct sockaddr*)&remota, tamSock)==-1){
                                printf("Error al enviar DATA\n");
                              }
                            }
                            printf("bytes %d\n", bytes);
                            printf("veces %d\n", veces);
                          }
                          printf("Se envio el archivo con %d numero de secuencias \n",veces);
                  }
                  else{
                    formatoError(mensaje,1,"archivo no existe");
                     if(sendto(udp_socket,mensaje,tam,0,(struct sockaddr*)&remota,sizeof(remota)) != -1){
                       printf("se envio error \n ");
                     }
                     else{
                       printf("error enviando el error XD");
                     }
                  }
                  break;
              case 3:
                  printf("DATA");
                  break;
              default: 
                  printf("no haz puesto una operacion valida %d",accion);
                  break;
            }
            
            }
    }
    
   }
 }
 
 close(udp_socket);
 return 0;
 }
