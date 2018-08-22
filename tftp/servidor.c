 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netinet/ip.h> /* superset of previous */
 #include <stdlib.h>
 #include <unistd.h>
 #include <stdio.h>
 #include <arpa/inet.h>
 #include <sys/time.h>


 int main()
 {
 struct sockaddr_in local, remota;    
 int udp_socket,lbind,tam,lrecv, bandera = 0;
 struct timeval start, end;
 long mtime, seconds, useconds;    
 gettimeofday(&start, NULL);
 unsigned char mensaje[]="Hola red como estas";
 udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
 if(udp_socket==-1)
   {
    perror("\nError al abrir el socket");
    exit(1);
   }
 else
   {

    perror("\nExito al abrir el socket");
    while(1){
    local.sin_family=AF_INET;
    local.sin_port=htons(8000);
    local.sin_addr.s_addr=INADDR_ANY;
    lbind=bind(udp_socket,(struct sockaddr *)&local,sizeof(local));
    if(lbind==-1)
      {
      printf("\nError en bind");
      exit(1);
      }
    else
      {
       printf(" Exito en bind \n");
       lrecv=sizeof(remota);
       while(mtime<100000){

          tam=recvfrom(udp_socket,mensaje,512,MSG_DONTWAIT,(struct sockaddr*)&remota,&lrecv);
           if(tam!=-1)
            {

             //printf("\nExito al enviar");   
             printf("Mensaje recibido \n \n %s\n",mensaje );
             printf("ip recibida %s \n \n ",inet_ntoa(remota.sin_addr));
             if(sendto(udp_socket,"hola prro",20,0,(struct sockaddr*)&remota,sizeof(remota))!=-1)
             {
              printf("Envie un mensaje ALV \n");
             }
             bandera= 1 ;
            
            }
            gettimeofday(&end, NULL);
            seconds  = end.tv_sec  - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;
            mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
            
            if(bandera){
              break;
            }
         }
         printf(" %ld millisegundos transcurridos\n", mtime); 
         
      }

    }
    
   }
 
 close(udp_socket);
 return 0;
 }
