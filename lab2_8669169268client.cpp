#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <sstream>
#include <iostream>

using namespace std;

#define PORT "4268" 
#define MAXDATASIZE 200 




void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}







string int_to_string(int n){
	string str_result;
	ostringstream ss;
	ss<<n;
	str_result=ss.str();
	return str_result;	
}








int random_255(){
	int re;
	re=rand()%255;
	return re;
}




int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
		fprintf(stderr,"usage: client hostname\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
	s, sizeof(s));
	printf("client: connecting to %s\n", s);

	/*Discover phase*/
	string transition_id;
	transition_id="13";
	strcpy(buf, transition_id.c_str());	

	if ((numbytes= send(sockfd, buf, MAXDATASIZE-1, 0 )>0)){
		printf("Sending the following Transaction ID to server: %s\n", buf);

	}

	string received_addr;
	string received_id;
	
	if ((numbytes= recv(sockfd, buf, MAXDATASIZE-1, 0 )>0)){
		string received=string(buf);
		int split_index=received.find("$$");
		received_addr=received.substr(0, split_index);
		received_id=received.substr(split_index+2);
		cout<<"Received the following: "<<endl;
		cout<<"IP address: "<<received_addr<<endl;
		cout<<"Transaction ID: "<<received_id<<endl;
		//printf("Received the following:%s\n");
		//printf("IP address:%s\n", received_addrr):
		//printf("Transaction ID: %s\n", received_id);

	}

	//formal requiest
	

	int temp_trans_id=random_255();
	string temp_trans_id_string=int_to_string(temp_trans_id);
	
	strcpy(buf, temp_trans_id_string.c_str());
	if ((numbytes= send(sockfd, buf, MAXDATASIZE-1, 0 )>0)){
		//cout<<"Formally requesting the following server"<<endl;
		//out<<"IP address: "<<received_addr<<endl;
		//cout<<"Tranaction ID: "<<temp_trans_id_string<<endl;
		printf("Formally requesting the following server:\n");
		//printf("IP address:%s\n", received_addr);
		cout<<"IP address: "<<received_addr<<endl;
		//printf("Transaction ID:%s\n", temp_trans_id);
		cout<<"Transaction ID: "<<temp_trans_id_string<<endl;
		
	}

	//send echo IP
	strcpy(buf, received_addr.c_str());
	send(sockfd, buf, MAXDATASIZE-1, 0 );

	if ((numbytes= recv(sockfd, buf, MAXDATASIZE-1, 0 )>0)){
		cout<<"Officially connected to IP Address: "<<received_addr<<endl;
		cout<<"client received "<<buf<<endl;
	

	}

	

	
	
	

	//if (()>0)
	
/*
	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	buf[numbytes] = '\0';
	printf("client: received '%s'\n",buf);*/
	close(sockfd);

	return 0;
}
