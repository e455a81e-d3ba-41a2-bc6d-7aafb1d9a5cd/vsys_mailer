#include "client.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <iostream>



Client::Client() :
	m_sockfd(-1)
{

}

Client::~Client()
{
	close(m_sockfd);
}

int Client::Connect (char* const server_hostname, const char* server_service)
{
	struct addrinfo* ai,* ai_sel = NULL;
	struct addrinfo hints;
	int err;

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;

	//resolve ipaddr
	if((err = getaddrinfo(server_hostname, server_service, &hints, &ai)) != 0){
		(void) fprintf(stderr, "ERROR:  %s\n", gai_strerror(err));
		return -1;
	}

	if(ai == NULL){
		(void) fprintf(stderr, "Could not resolve host %s.\n", server_hostname);
		freeaddrinfo(ai);
		return -1;
	}

	ai_sel = ai;

	//creat socket
	if((m_sockfd = socket(ai_sel->ai_family, ai_sel->ai_socktype, ai->ai_protocol)) < 0){
		(void) fprintf(stderr, "Socket creation failed\n");
		freeaddrinfo(ai);
		return -1;
	}

	//connect to server
	if(connect(m_sockfd, ai_sel->ai_addr, ai_sel->ai_addrlen) < 0){
		(void) close(m_sockfd);
		freeaddrinfo(ai);
		(void) fprintf(stderr, "Connection failed.\n");
		return -1;
	}

	freeaddrinfo(ai);
	//freeaddrinfo(ai_sel);

	return m_sockfd;
}

int Client::SendMessage(std::string message)
{
	long bytes_sent = send(m_sockfd, message.c_str(), message.length(), 0);
		if(bytes_sent < 0){
			printf("Send Error.");
		return -1;
		}

	return 0;
}


int Client::SendMessage(std::string message, std::string fileName)
{
	std::string fileContent;
	if(ReadFile(fileName, fileContent) == -1) {
		printf("ERROR: File %s not found.\n", fileName.c_str());
		return -1;
	}
	message += attachmentDelim + fileContent;
	std::cout << message << std::endl;
	long bytes_sent = send(m_sockfd, message.c_str(), message.length(), 0);
		if(bytes_sent < 0){
			printf("Send Error.");
		return -1;
		}

	return 0;
}

int Client::ReadFile(std::string fileName, std::string& out)
{
	std::fstream file;
	file.open(fileName,std::ios::in|std::ios::binary|std::ios::ate);

	if(!file.is_open())
	{
		file.close();
		return -1;
	} else {
		auto size = file.tellg();
		char *content = new char[size];
		file.seekg(0,std::ios::beg);
		file.read(content, size);
		out = std::string(content);
		delete[] content;
	}
	file.close();
	return 0;
}

int Client::Login(const char *user, const char *pw)
{

}