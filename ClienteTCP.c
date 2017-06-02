#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

	void error(char* msg){
		perror(msg);
		exit(1);
	}

	int crearBuzon(char* tipo,char* nombre,int sockfd){
		char buffer[256];
		bzero(buffer,256);
		buffer[0]='c';
		strcat(buffer,tipo);
		strcat(buffer,nombre);
		printf("%s\n",buffer);
		int n = write(sockfd, buffer, strlen (buffer));
		bzero(buffer,256);
			n = read(sockfd, buffer,255);	
				printf("Msg: %s\n", buffer);

		if (n < 0){
		error("Error on read\n");
		}

		return 0;
	}

	int crearCola(char* cola,char* buzon,int sockfd){
		char buffer[256];
		bzero(buffer,256);

		strcat(buffer,"H");
		strcat(buffer,buzon);
		strcat(buffer,"+");
		strcat(buffer,cola);
		printf("%s\n",buffer);
		int n = write(sockfd, buffer, strlen (buffer));
		bzero(buffer,256);
			n = read(sockfd, buffer,255);	
				printf("Msg: %s\n", buffer);

		if (n < 0){
		error("Error on read\n");
		}

		return 0;
	}

	int logear(char* clave,int sockfd){
		int n = write(sockfd,clave, strlen (clave));
		char buffer[256];
		bzero(buffer,256);
		n = read(sockfd, buffer,255);	
				printf("Msg: %s\n", buffer);
        printf("%s\n",buffer);
		if (n < 0){
		error("Error on read\n");
		}
		return 0;
	}

	int asignarCola(char* buzon,char* cola,int sockfd){
		char buffer[256];
		bzero(buffer,256);
		strcat(buffer,"A");
		strcat(buffer,buzon);
		strcat(buffer,"+");
		strcat(buffer,cola);
		printf("%s\n",buffer);
		int n = write(sockfd,buffer, strlen (buffer));
		bzero(buffer,256);
		n = read(sockfd, buffer,255);	
		return 0;
	}

	int asignarColCli(char* cola,int sockfd){
		char buffer[256];
		bzero(buffer,256);
		strcat(buffer,"P");
		strcat(buffer,cola);
	//	strcat(buffer,"+");
	//	strcat(buffer,cli);
		printf("%s\n",buffer);
		int n = write(sockfd,buffer, strlen (buffer));
		bzero(buffer,256);
		n = read(sockfd, buffer,255);	
		        printf("%s\n",buffer);
		return 0;
	}

	int escribirMensaje(char* buzon,char* mensaje,int sockfd){
        char buffer[256];
        bzero(buffer,256);
        strcat(buffer,"W");
        strcat(buffer,buzon);
        strcat(buffer,"+");
        strcat(buffer,mensaje);
        printf("%s\n",buffer);
        int n = write(sockfd,buffer, strlen (buffer));
        bzero(buffer,256);
        n = read(sockfd, buffer,255);    
                printf("%s\n",buffer);
        return 0;
    }

	int escribirMensajeT(char* buzon, char* topico, char* mensaje,int sockfd){
        char buffer[256];
        bzero(buffer,256);
        strcat(buffer,"T");
        strcat(buffer,buzon);
        strcat(buffer,"+");
        strcat(buffer,topico);
        strcat(buffer,"+");
        strcat(buffer,mensaje);
        printf("%s\n",buffer);
        int n = write(sockfd,buffer, strlen (buffer));
        bzero(buffer,256);
        n = read(sockfd, buffer,255);    
        return 0;
    }	

int main(int argc, char** argv){
	int sockfd, portno,n;
	struct sockaddr_in serv_addr;
	struct hostent *server; 
	char buffer[256];


	if (argc < 3) { //2do ip 3ero puerto
		fprintf(stderr, "IP or Port missing\n");
		exit(1);
	}

	portno = atoi(argv[2]);			
	sockfd = socket(AF_INET,SOCK_STREAM, 0); //La familia //Tipo de socket  //Tipo de protocolo (El 0 del dice al SO que elija el mejor procolo)
	if (sockfd < 0){
		error("Error opening socket\n");
	}

	server = gethostbyname(argv[1]);
	if (server == NULL){
		fprintf(stderr, "Error Domain%s\n" );
		exit(1);
	}

	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;					
	serv_addr.sin_port = htons(portno); 

	bcopy( (char*)server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length); //Dir del servidor //El lugar donde deberia estar la direccion //Tama;o de la dir

	if (connect (sockfd, &serv_addr, sizeof(serv_addr)) < 0){
		error ("Error on connect\n");
	}
	logear("123",sockfd);
//	crearBuzon("2","tipo3",sockfd);
//	crearBuzon("2","tipo2",sockfd);
//	crearBuzon("3","tipo3",sockfd);
//	asignarCola("tipo2","Colita",sockfd);
//	crearBuzon("3","tipo3",sockfd);
//	asignarCola("tipo1","Colita3",sockfd);	
//	crearCola("Colita6","tipo3",sockfd);
//	escribirMensaje("tipo2","Dormir",sockfd);

//	crearCola("Colita6","tipo3",sockfd);
//	asignarColCli("Colita6",sockfd);
//	escribirMensaje("tipo3","tres",sockfd);
//	escribirMensaje("tipo3","Colita6",sockfd);

//	asignarColCli("cl3",sockfd);
//	escribirMensaje("tipo2","La tristeza",sockfd);	
	//asignarColCli("cl2",sockfd);

//	crearBuzon("3","TRES",sockfd);
//	crearCola("futbol","TRES",sockfd);	
	asignarColCli("futbol",sockfd);	
	escribirMensajeT("TRES","futbol","ME",sockfd);		

	while(1){
		bzero (buffer,256);
			n = read(sockfd, buffer,256); 
			printf("Msg: %s\n", buffer);
			printf("%d\n",n);
			bzero (buffer,256);
			bzero (buffer,256);
	}
	return 0;
}