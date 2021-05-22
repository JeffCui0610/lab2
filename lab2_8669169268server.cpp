#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
	
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <cstring>
#include <sstream>
#include <iostream>
#include <math.h>
#include <cstdlib>

using namespace std;

#define PORT "4268" 
#define BACKLOG 10 
#define MAXDATASIZE 200





void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}
// get sockaddr, IPv4 or IPv6:





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


string rand_ip(string host_id){
	string result="";
	int counter=0;
	while (counter<3){
		counter++;
		int temp;
		temp=random_255();
		result=result+int_to_string(temp);
		result=result+".";
	}
	result=result+host_id;
	return result;
}






int main(void)
{
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	char buf[MAXDATASIZE];
	int rv;
	int numbytes;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
		}
	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
		p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
			}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
		sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
			}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
			}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) { // main accept() loop
	sin_size = sizeof their_addr;
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if (new_fd == -1) {
		perror("accept");
		continue;
	}

	inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
	printf("server: got connection from %s\n", s);

	string address;

	string id;

	if ((numbytes=recv(new_fd, buf,MAXDATASIZE-1,0))>0){
		printf("Received the following Transactionn ID from client: %s\n", buf);
		//cout<<"Received the following TRANsactionn ID from client: "<<buf<<endl;
	}


	//offer ip to client
	string receive_id=string(buf);
	//cout<<to_string(123);
	string ip_address;
	ip_address=rand_ip(receive_id);
	int temp_tran_id=random_255();
	string random_tran_id=int_to_string(temp_tran_id);
	string reply="";
	reply=reply+ip_address;
	reply=reply+"$$";
	reply=reply+random_tran_id;
	strcpy(buf, reply.c_str());
	if ((numbytes=send(new_fd, buf, MAXDATASIZE-1,0))>0){
		printf("Sending the following to Client:\n");

		//printf("IP address:%s\n", ip_address);
		cout<<"IP address: "<<ip_address<<endl;
		//printf("Transaction ID:%s\n", random_tran_id);
		cout<<"Transaction ID: "<<random_tran_id<<endl;}

	//acknowledge
	string new_id;

	if ((numbytes=recv(new_fd, buf,MAXDATASIZE-1,0))>0){
		printf("Received the following requsst:\n");
		printf("Transaction ID: %s\n",buf);
		//cout<<"Received the following request:"<<endl;
		//cout<<"Transaction ID: "<<buf<<endl;
	}

	//receive echo IP
	string echo_ip;
	if ((numbytes=recv(new_fd, buf,MAXDATASIZE-1,0))>0){
		echo_ip=string(buf);
	}
	
	
	int new_id_int=random_255();
	string new_tran_id=int_to_string(new_id_int);
	strcpy(buf, new_tran_id.c_str());
	if ((numbytes=send(new_fd, buf, MAXDATASIZE-1,0))>0){
		cout<<"Sending fellowing acknowledgement:"<<endl;
		cout<<"IP address: "<<ip_address<<endl;
		cout<<"Transaction ID: "<<new_tran_id<<endl;
	}
	
	
	
	
	





		/*if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
				if (send(new_fd, "Hello, world!", 13, 0) == -1)
					perror("send");
					close(new_fd);
					exit(0);
				}*/
		close(new_fd); // parent doesn't need this
		}
	return 0;
}

