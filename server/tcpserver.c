/*
gcc tcpserver.c -o tcpserver -pthread
*/

//headers
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

FILE *serverlogs;
int sockfd, clientfd;
struct sockaddr_in client;

void clientip(struct sockaddr_in addr, FILE *filepointer){
      fprintf(filepointer, "%d.%d.%d.%d :",
          addr.sin_addr.s_addr & 0xff,
          (addr.sin_addr.s_addr & 0xff00) >> 8,
          (addr.sin_addr.s_addr & 0xff0000) >> 16,
          (addr.sin_addr.s_addr & 0xff000000) >> 24);
  }

//threadmain func
void *threadMain1(void *arg){

	while(1){
		char buff[512] = {0}; 
		int retval, slen;
		fgets(buff, sizeof(buff), stdin);
		retval = send(*((int *)arg), buff, 512, 0);
		if(retval == -1){
			fprintf(stdout,"Send error: %s\n",strerror(errno));
			exit(1);
		}



	}
}

void *threadMain2(void *arg){

        while(1){

                char buff[512] = {0};
                int retval;
                retval = recv(*((int *)arg), &buff, 512, 0);
                if(retval == -1){
                        fprintf(stdout,"Recv error: %s\n",strerror(errno));
                        exit(1);
                }else{

			if(buff[0] == '\0'){
				clientip(client, stdout);	
				fprintf(stdout, "Client ayrıldı!\n");
				exit(1);
			}

			fprintf(stdout,"%s", buff);
//			serverlogs = fopen("./serverlogs" , "ab");
//			fprintf(serverlogs,"%s", buff);
//			fclose(serverlogs);
			serverlogs = fopen("./serverlogs" , "ab");
			if (serverlogs == NULL){
				fprintf(stdout, "Send error: %s\n", strerror(errno));
				exit(1);
			}
			clientip(client, serverlogs);
			fprintf(serverlogs,"%s", buff);
			fclose(serverlogs);
		}
        }
}




//main func
int main(){
	
	struct sockaddr_in server;
	int retval, pretval1, pretval2;
        int port = 54321;
	int yes = 1;
	socklen_t slen = sizeof(server);
	pthread_t tid1, tid2;

//open socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		fprintf(stdout,"Socket error: %s\n",strerror(errno));
		exit(1);

	}
//build socket struct
	server.sin_family      = AF_INET;
	server.sin_port        = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

//setsockopt
	retval = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(&yes));
	if(retval == -1){
                fprintf(stdout,"Setsockopt error: %s\n",strerror(errno));
                exit(1);

        }

//bind socket
	retval = bind(sockfd, (struct sockaddr *)&server, slen);
	if(retval == -1){
                fprintf(stdout,"Bind error: %s\n",strerror(errno));
                exit(1);

        }

//listen socket
	retval = listen(sockfd, 0);
	if(retval == -1){
                fprintf(stdout,"Listen error: %s\n",strerror(errno));
                exit(1);

        }

//accept socket

	socklen_t clen = sizeof(client);
	clientfd = accept(sockfd, (struct sockaddr *)&client, &clen);
	if(clientfd == -1){
                fprintf(stdout,"Accept error: %s\n",strerror(errno));
                exit(1);

	}
	
	clientip(client, stdout);	
	fprintf(stdout, "Client Bağlandı!\n");

//create recv & send threads
	pretval1 = pthread_create(&tid1, NULL, &threadMain1, &clientfd);
	if(pretval1 != 0){
                fprintf(stdout,"pthread_create error: %s\n",strerror(errno));
                exit(1);

        }

	pretval2 = pthread_create(&tid2, NULL, &threadMain2, &clientfd);
        if(pretval2 != 0){
                fprintf(stdout,"pthread_create error: %s\n",strerror(errno));
                exit(1);

        }

//recv & send
		pthread_join(tid1, NULL);
		pthread_join(tid2, NULL);

		return 0;
}
