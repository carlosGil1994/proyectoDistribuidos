#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <signal.h>
#include <mysql.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

MYSQL *con;
MYSQL *con2;
MYSQL *con3;
MYSQL_RES *res;
MYSQL_RES *res2;
MYSQL_ROW row;

char *server = "localhost";
char *user = "root";
char *pass = "pikakyu";
char *database = "distribuidosdb";

	void error(char* msg){
		perror(msg);
		exit(1);
	}
void manejador(int signum){
		int n=0;
		char id_cli[12];
		char cons[256] = "";
		char cons2[256] = "";		
		char buffer[256] = "";
		bzero(buffer,256);

		//conexion
		con2 = mysql_init(NULL);
		if (!mysql_real_connect(con2, server, user, pass, database, 0, NULL,0)){
			fprintf(stderr, "%s\n", mysql_error(con2));
		}

		con3 = mysql_init(NULL);
		if (!mysql_real_connect(con3, server, user, pass, database, 0, NULL,0)){
			fprintf(stderr, "%s\n", mysql_error(con3));
		}

		snprintf(id_cli, 12, "%d",getpid());
		strcat(cons, "SELECT clave FROM cliente where id_cli='"); 
		strcat(cons,id_cli);
		strcat(cons,"'");

		if(mysql_query(con2, cons)){	//Clave del cliente para el socket
			fprintf(stderr, "%s\n", mysql_error(con2));
			exit(1);
		}
		bzero(cons,256);

		res2 = mysql_use_result(con2);
		row = mysql_fetch_row(res2);

		char *clave= row[0];
		int j=0;
		j=atoi(clave);


		mysql_free_result(res2);
		strcat(cons, "SELECT DISTINCT id_col FROM col_cli WHERE id_cli='");
		strcat(cons,id_cli);
		strcat(cons,"'");

		//Encuentra los id de las colas asociados a ese cliente para buscar en esas colas msj
		if(mysql_query(con2, cons)){
			fprintf(stderr, "%s\n", mysql_error(con2));
			exit(1);
		}
		bzero(cons,256);
		res2 = mysql_store_result(con2);
		int num_rows= mysql_num_rows(res2);
		int num_fields = mysql_num_fields(res2);

		char id_cols[num_rows][12];
		int l=0;
		while ((row = mysql_fetch_row(res2))) { 
			for(int i = 0; i < num_fields; i++) { 
				strcpy(id_cols[l],row[i]);			//Guardo los id de las colas en el arreglo	    
			} l++;
		}

		strcat(cons, "SELECT id_msj, cont_msj FROM msj WHERE id_col='");
		strcat(cons,id_cols[0]);
		for(int i=1; i<num_rows;i++){
			strcat(cons,"' OR id_col='");
			strcat(cons,id_cols[i]);
		}
		strcat(cons,"'");
		

		strcat(cons2, "DELETE FROM msj WHERE id_col='");
		strcat(cons2,id_cols[0]);
		for(int i=1; i<num_rows;i++){
			strcat(cons2,"' OR id_col='");
			strcat(cons2,id_cols[i]);
		}
		strcat(cons2,"'");

		if(mysql_query(con2, cons)){
			fprintf(stderr, "%s\n", mysql_error(con2));
			exit(1);
		}

		res2 = mysql_store_result(con2);
		num_rows= mysql_num_rows(res2);
		
		if(mysql_query(con3, cons2)){
			fprintf(stderr, "%s\n", mysql_error(con3));
			exit(1);
		}

		bzero(cons,256);		
		bzero(cons2,256);
		mysql_close(con3);

		bzero(buffer,256);

		if(num_rows>0){
				row = mysql_fetch_row(res2);

			for(int i=0; i<num_rows;i++){				
				strcpy(buffer,row[1]);
				n=write(j,buffer,strlen(buffer));
				bzero(buffer,256);
				row = mysql_fetch_row(res2);
			}	
		} else {
			strcpy(buffer,"No hay mensajes");
			n=write(j,buffer,strlen(buffer));
		}
	

mysql_free_result(res2);
mysql_close(con2);
			
}

int main(int argc, char** argv){

	int sockfd, newsockfd, portno, clilen, n; //Identificador de socket //Variable para albergar el nuevo socket TCP  //Num de puerto //longitu del cliente // Cant de bytes que hay en el string
	char buffer[256]; //Escribiremos lo que vaya y venga
	struct sockaddr_in serv_addr, cli_addr; //referncia la dirr del servidor y referencia la dirr del cliente //Se guarda la direccion el puert, la familia, toda la config del serv

	if (argc < 2) {
		fprintf(stderr, "No port\n");
		exit(1);
	}

	sockfd = socket(AF_INET,SOCK_STREAM, 0); //La familia //Tipo de socket  //Tipo de protocolo (El 0 del dice al SO que elija el mejor procolo)

	if (sockfd < 0){
		error("Error opening socket\n");
	}

	//Inicializar
	bzero((char*) &serv_addr, sizeof(serv_addr));  //Direccion //Tama;o de la estrutura
	portno = atoi(argv[1]);							//Puerto del servidor
	serv_addr.sin_family = AF_INET;						//Asigne familia al servidor
	serv_addr.sin_port = htons(portno); 	//Pasa lo que le pases a big endian
	serv_addr.sin_addr.s_addr = INADDR_ANY;	//aceptar solicitudes de cualquier IP disponible, de localhost o relaciones remotas

	if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)	{ 	//HAce que mi sistema de diga al so que quiere pegarse al socket
		error("Error on binding\n");
	}	

	listen (sockfd, 5); //socket //maximo de conexiones
	//El cliente debe saber a que puerto le debe hablar, se lo manda mediante el sockfd
	int banderaCliente=0;
	char cliente[5]="";
	int pid;
	while(1){

			clilen=sizeof(cli_addr);
			newsockfd=accept(sockfd,(struct sockaddr*)&cli_addr,&clilen);

			if(newsockfd<0){
				error("error on accept");
			}

			
			if (( pid = fork()) == -1)
			{
				close(newsockfd);
				continue;
			}
			else if(pid > 0)		//Codigo padre
			{
				close(newsockfd);
				continue;
			}
			else if(pid == 0)		//Codigo hijo
			{
			
			///////logear cliente/////////
				bzero(buffer,256);
				n=read(newsockfd,buffer,255);
				n=write(newsockfd,"Introduce cliente",50);
			///////////////////////////////

				char id_cli[12];
				char clavSock[12];
				snprintf(id_cli, 12, "%d",getpid());
				snprintf(clavSock, 12, "%d",newsockfd);

				//conexion
				con = mysql_init(NULL);
				if (!mysql_real_connect(con, server, user, pass, database, 0, NULL,0)){
					fprintf(stderr, "%s\n", mysql_error(con));
				}
			
				char cons[256] = "";
				char *last= "')";
				strcat(cons, "INSERT INTO cliente(id_cli,clave) VALUES('");
				strcat(cons,id_cli);
				strcat(cons, "', '");
				strcat(cons,clavSock);
				strcat(cons,last);

				if(mysql_query(con, cons)){
					fprintf(stderr, "%s\n", mysql_error(con));
					exit(1);
				}
				bzero(cons,256);

//					//Se cierra la conexion
					mysql_close(con);

signal(SIGUSR2,manejador);

			while(1) {
					//conexion
					con = mysql_init(NULL);
					if (!mysql_real_connect(con, server, user, pass, database, 0, NULL,0)){
						fprintf(stderr, "%s\n", mysql_error(con));
					}

					bzero(buffer,256);
					if(read(newsockfd,buffer,255)<-1){
					}


//-------------->Crear buzon
					if(buffer[0]=='c'){
						strcat(cons, "INSERT INTO buzon(tip_buz, nom_buz) VALUES(");
						int t=0;
						char nom_buz[256];
						bzero(nom_buz,256);

						if(buffer[1]=='1'){
							for(int i=2;i<=strlen(buffer);i++){
								nom_buz[t]=buffer[i];
								t++;
							}
							strcat(cons, "1, '");
							strcat(cons,nom_buz);
							strcat(cons,last);

							//--------------------->Crear buzon en BD
							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
							bzero(cons,256);

							n=write(newsockfd,"Crear buzon tipo 3",50);
						}

						if(buffer[1]=='2'){
							for(int i=2;i<=strlen(buffer);i++){
								nom_buz[t]=buffer[i];
								t++;
							}

							strcat(cons, "2, '");
							strcat(cons,nom_buz);
							strcat(cons,last);		

							//--------------------->Crear buzon
							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
							bzero(cons,256);
							
							printf("%s\n",nom_buz);
							n=write(newsockfd,"Crear buzon tipo 2",50);
						}				
						if(buffer[1]=='3'){
							for(int i=2;i<=strlen(buffer);i++){
								nom_buz[t]=buffer[i];
								t++;
							}

							strcat(cons, "3, '");
							strcat(cons,nom_buz);
							strcat(cons,last);		

							//--------------------->Crear buzon
							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
							bzero(cons,256);
														
							printf("%s\n",nom_buz);
							n=write(newsockfd,"Crear buzon tipo 3",50);
						}		
					}

//-------------->Crear cola						
					if(buffer[0]=='H'){					
						int t=0;
						char nom_buz[256];
						char nom_col[256];
						bzero(nom_buz,256);
						bzero(nom_col,256);
						int i=0;
						for(i=1;buffer[i]!='+';i++){ //Obtiene el nombre del buzon
							nom_buz[t]=buffer[i];
							t++;
						}
						//Encontrar el id de buzon al cual se le asociara la cola
						strcat(cons, "SELECT id_buz INTO @buzonn FROM buzon where nom_buz='");
						strcat(cons,nom_buz);
						strcat(cons,"'");

							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
						bzero(cons,256);						

						//Encontrar el tipo de buzon al cual se le asociara la cola
						strcat(cons, "SELECT tip_buz FROM buzon where id_buz=@buzonn");

						if(mysql_query(con, cons)){
							fprintf(stderr, "%s\n", mysql_error(con));
							exit(1);
						}
						bzero(cons,256);
						res = mysql_use_result(con);
						row = mysql_fetch_row(res);

						char *tipo= row[0];
						if(!(strcmp(tipo,"1"))){
							//Se liberan los resultados
							mysql_free_result(res);
							//Cuento la cantidad de veces que ese buzon tiene colas asociadas
							if(mysql_query(con, "SELECT distinct COUNT(id_buz) FROM buz_col WHERE id_buz= @buzonn")){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
							res = mysql_use_result(con);
							row = mysql_fetch_row(res);

							//Compara si ya tiene cola asignada
							if(!(strcmp(row[0],"1"))){
								fprintf(stderr,"La cola no puede ser creada ya que el buzon tiene cola asociada\n");
								exit(1);	
							}
						}
						mysql_free_result(res);

						t=0;
						i=i+1;
						for(i;i<strlen(buffer);i++){ //Obtiene el nombre de la cola
							nom_col[t]=buffer[i];
							t++;
						}
						//--------------->Inserta la cola en la tabla cola
						strcat(cons, "INSERT INTO cola(nom_col) VALUES('");
						strcat(cons,nom_col);
						strcat(cons,last);

						if(mysql_query(con, cons)){
							fprintf(stderr, "%s\n", mysql_error(con));
							exit(1);
						}
					bzero(cons,256);						
						//----------------->Encontrar el id de la cola
						strcat(cons,"SELECT id_col INTO @colaa FROM cola where nom_col='");
						strcat(cons,nom_col);
						strcat(cons, "'");

							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
						bzero(cons,256);		

						//Asociar la cola al buzon
						if(mysql_query(con, "INSERT INTO buz_col VALUES(@buzonn, @colaa)")){
							fprintf(stderr, "%s\n", mysql_error(con));
							exit(1);
						}		
						n=write(newsockfd,"Crear cola",50);
					}

//-------------->No se para que es					
					if(buffer[0]=='P'){
						int t=0;
						int i=0;						
						char nom_col[256];
						char nom_cli[256];
						bzero(nom_col,256);
						bzero(nom_cli,256);
						for(i=1;i<strlen(buffer);i++){ //Obtiene nombre de cola
							nom_col[t]=buffer[i];
							t++;
						}

						//Encontrar el id de la cola
						strcat(cons,"SELECT id_col INTO @colaa FROM cola where nom_col='");
						strcat(cons,nom_col);
						strcat(cons, "'");
							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
						bzero(cons,256);


						snprintf(id_cli, 12, "%d",getpid());
						strcat(cons,"INSERT INTO col_cli VALUES(@colaa,'");
						strcat(cons,id_cli);
						strcat(cons,"')");
						//Asociar la cola al buzon
						if(mysql_query(con, cons)){
							fprintf(stderr, "%s\n", mysql_error(con));
							exit(1);
						}

						bzero(cons,256);
						n=write(newsockfd,"Asigno cola a cliente",50);	
/*
						int j=0;
						j=getpid();
						kill(j,SIGUSR2);
*/						
					}

//-------------->Asociar cola a buzon

					if(buffer[0]=='A'){
						int t=0;
						char nom_buz[256];
						char nom_col[256];

						bzero(nom_buz,256);
						bzero(nom_col,256);
						int i=0;
						for(i=1;buffer[i]!='+';i++){ //Obtiene el nombre del buzon
							nom_buz[t]=buffer[i];
							t++;
						}
						//Encontrar el id de buzon al cual se le asociara la cola
						strcat(cons, "SELECT id_buz INTO @buzonn FROM buzon where nom_buz='");
						strcat(cons,nom_buz);
						strcat(cons,"'");

							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
						bzero(cons,256);

						//Encontrar el tipo de buzon al cual se le asociara la cola
						strcat(cons, "SELECT tip_buz FROM buzon where id_buz=@buzonn");

						if(mysql_query(con, cons)){
							fprintf(stderr, "%s\n", mysql_error(con));
							exit(1);
						}
						bzero(cons,256);
						res = mysql_use_result(con);
						row = mysql_fetch_row(res);

						char *tipo= row[0];
						if(!(strcmp(tipo,"1"))){
							//Se liberan los resultados
							mysql_free_result(res);
							//Cuento la cantidad de veces que ese buzon tiene colas asociadas
							if(mysql_query(con, "SELECT distinct COUNT(id_buz) FROM buz_col WHERE id_buz= @buzonn")){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
							res = mysql_use_result(con);
							row = mysql_fetch_row(res);

							//Compara si ya tiene cola asignada
							if(!(strcmp(row[0],"1"))){
								fprintf(stderr,"El buzon ya tiene cola asociada\n");
								exit(1);	
							}
						}
						mysql_free_result(res);

						t=0;
						i=i+1;
						for(i;i<strlen(buffer);i++){ //Obtiene el nombre de la cola
							nom_col[t]=buffer[i];
							t++;
						}
						//Encontrar el id de la cola
						strcat(cons,"SELECT id_col INTO @colaa FROM cola where nom_col='");
						strcat(cons,nom_col);
						strcat(cons, "'");

							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
						bzero(cons,256);		

						//Asociar la cola al buzon
						if(mysql_query(con, "INSERT INTO buz_col VALUES(@buzonn, @colaa)")){
							fprintf(stderr, "%s\n", mysql_error(con));
							exit(1);
						}		

						n=write(newsockfd,"Asociar cola a buzon",50);
					}
//-------------->Escribir mensaje
					if(buffer[0]=='W'){

						int t=0;
						char nom_buz[256];
						char msj[256];
						bzero(nom_buz,256);
						bzero(msj,256);

						int i=0;
						for(i=1;buffer[i]!='+';i++){ //Obtiene el nombre del buzon
							nom_buz[t]=buffer[i];
							t++;
						}

						//Encontrar el id de buzon al cual se le asociara la cola
						strcat(cons, "SELECT id_buz INTO @buzonn FROM buzon where nom_buz='");
						strcat(cons,nom_buz);
						strcat(cons,"'");

						if(mysql_query(con, cons)){
							fprintf(stderr, "%s\n", mysql_error(con));
							exit(1);
						}
						bzero(cons,256);

						//Encuentra los id de las colas asociados a ese buzon
						if(mysql_query(con, "SELECT DISTINCT id_col FROM buz_col WHERE id_buz= @buzonn")){
							fprintf(stderr, "%s\n", mysql_error(con));
							exit(1);
						}
						res = mysql_store_result(con);
						int cant_cols= mysql_num_rows(res);
						int num_fields = mysql_num_fields(res);


						char id_cols[cant_cols][12];
						int j=0;
						while ((row = mysql_fetch_row(res))) { 
							for(int i = 0; i < num_fields; i++) { 
								strcpy(id_cols[j],row[i]);
							} j++;
						}

						t=0;
						i=i+1;
						for(i;i<strlen(buffer);i++){ //Lee el mensaje
							msj[t]=buffer[i];
							t++;
						}

						for (int i=0; i<cant_cols; i++){		//Introduce el mensaje en BD
							strcat(cons,"INSERT INTO msj(id_col, cont_msj) VALUES('");	
							strcat(cons,id_cols[i]);
							strcat(cons,"','");
							strcat(cons,msj);
							strcat(cons,"')");
							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
							bzero(cons,256);
						}	

							strcat(cons,"SELECT DISTINCT id_cli FROM col_cli WHERE id_col='");
							strcat(cons,id_cols[0]);
								for(int i=1; i<cant_cols;i++){
									strcat(cons,"' OR id_col='");
									strcat(cons,id_cols[i]);
								}	
							strcat(cons,"'");
							//Encuentra los id de los clientes asociados a esa cola
							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
							bzero(cons,256);
							res = mysql_store_result(con);

							int num_rows2 = 0;
							num_rows2= mysql_num_rows(res);

							j=0;

							n=write(newsockfd,"Escribio mensaje",50);
							while ((row = mysql_fetch_row(res))) { 
								j=atoi(row[0]);
								kill(j,SIGUSR2);	//Mando se;al a los clientes asociados a la cola
							}

					}
					//-------------->Enviar mensaje a buzones de tipo topico					
					if(buffer[0]=='T'){

						int t=0;
						char nom_buz[256];
						char topico[256];
						char msj[256];
						bzero(nom_buz,256);
						bzero(topico,256);
						bzero(msj,256);

						int i=0;
						for(i=1;buffer[i]!='+';i++){ //Obtiene el nombre del buzon
							nom_buz[t]=buffer[i];
							t++;
						}

						//Encontrar el id de buzon al cual se le asociara la cola
						strcat(cons, "SELECT id_buz INTO @buzonn FROM buzon where nom_buz='");
						strcat(cons,nom_buz);
						strcat(cons,"'");

						if(mysql_query(con, cons)){
							fprintf(stderr, "%s\n", mysql_error(con));
							exit(1);
						}
						bzero(cons,256);	

						t=0;
						i=i+1;
						for(i;buffer[i]!='+';i++){ //Encontrar el topico de la cola
							topico[t]=buffer[i];
							t++;
						}	

						strcat(cons, "SELECT DISTINCT c.id_col FROM cola c, buz_col bz WHERE bz.id_buz=@buzonn AND c.nom_col='");
						strcat(cons,topico);
						strcat(cons,"'");																
						if(mysql_query(con, cons)){
							fprintf(stderr, "%s\n", mysql_error(con));
							exit(1);
						}
						bzero(cons,256);	
						res = mysql_store_result(con);
						int cant_cols= mysql_num_rows(res);
						int num_fields = mysql_num_fields(res);


						char id_cols[cant_cols][12];
						int j=0;
						while ((row = mysql_fetch_row(res))) { 
							for(int i = 0; i < num_fields; i++) { 
								strcpy(id_cols[j],row[i]);
							} j++;
						}

						t=0;
						i=i+1;
						for(i;i<strlen(buffer);i++){ //Lee el mensaje
							msj[t]=buffer[i];
							t++;
						}

						for (int i=0; i<cant_cols; i++){		//Introduce el mensaje en BD
							strcat(cons,"INSERT INTO msj(id_col, cont_msj) VALUES('");	
							strcat(cons,id_cols[i]);
							strcat(cons,"','");
							strcat(cons,msj);
							strcat(cons,"')");
							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}
							bzero(cons,256);
						}	

							strcat(cons,"SELECT DISTINCT id_cli FROM col_cli WHERE id_col='");
							strcat(cons,id_cols[0]);
								for(int i=1; i<cant_cols;i++){
									strcat(cons,"' OR id_col='");
									strcat(cons,id_cols[i]);
								}	
							strcat(cons,"'");

							if(mysql_query(con, cons)){
								fprintf(stderr, "%s\n", mysql_error(con));
								exit(1);
							}

							bzero(cons,256);
							res = mysql_store_result(con);

							int num_rows2 = 0;
							num_rows2= mysql_num_rows(res);

							j=0;

							n=write(newsockfd,"Escribio mensaje",50);
							while ((row = mysql_fetch_row(res))) { 
											j=atoi(row[0]);
											kill(j,SIGUSR2);	//Mando se;al a los clientes asociados a la cola
							}
					}

//Se cierra la conexion
mysql_close(con);		
			}

				
		}
	}
}
