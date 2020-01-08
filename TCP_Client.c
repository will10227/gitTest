#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define Server_PortNumber 0x8888
#define Server_Address "127.0.0.1"

void *recvFunc(void *arg){

	int byte_recv;
	char buffer[50]="";

	
	while(1){
		byte_recv = recv((int)(intptr_t) arg, buffer, sizeof(buffer),0);
		if (byte_recv < 0){
			printf("Error recving packet\n");
		}
		else if(byte_recv == 0){
			break;
		}
		printf("%s\n",buffer);
	}	

	pthread_exit(0);
}


void *sendFunc(void *arg){

	int byte_sent;
	char buffer[50]="";

	
	while(1){

		scanf("%s",buffer);
		if((strcmp(buffer,"quit") == 0) || (strcmp(buffer,"q") == 0)|| (strcmp(buffer,"Q") == 0)){
			break;
		}
		byte_sent = send((int)(intptr_t) arg, buffer, sizeof(buffer),0);
		if (byte_sent < 0){
			printf("Error sending packet!\n");
		}
	}	

	pthread_exit(0);
}

int main(int argc, char *argv[]) {
	struct sockaddr_in server_addr;
	
	int sock, server_addr_length = sizeof(server_addr);

	pthread_t sendThread,recvThread;
	int st,rt;
	
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0)    printf("Error creating socket!\n");

	bzero(&server_addr, server_addr_length);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(Server_PortNumber);
	server_addr.sin_addr.s_addr = inet_addr(Server_Address);

	if (connect(sock, (struct sockaddr *)&server_addr,server_addr_length)==-1) {
		printf("connect failed!");
		close(sock);
	}


	rt = pthread_create(&recvThread, NULL, recvFunc, (void *)(intptr_t) sock);
	if(rt){
		printf("Error; return code from pthread_create() is %d\n", rt);
		exit(-1);
	}
	st = pthread_create(&sendThread, NULL, sendFunc, (void *)(intptr_t) sock);
	if(st){
		printf("Error; return code from pthread_create() is %d\n", st);
		exit(-1);
	}


	rt = pthread_detach(recvThread);
	if(rt){
		printf("Error; return code from pthread_join() is %d\n",rt);
		exit(-1);
	}	
	st = pthread_join(sendThread,NULL);
	if(st){
		printf("Error; return code from pthread_join() is %d\n",st);
		exit(-1);
	}

	
	close(sock);
	
	
	return 0;
}
