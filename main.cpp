#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

int errorMsg(const char *msg)
{
	std::cerr << msg << std::endl;
	return -1;
}

int main(int ac, char **av)
{
	// Initialize port
	if (ac != 2)
		return (errorMsg("Invalid number of arguments"));
	int port = std::atoi(av[1]);

	// Create socket
	int listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == -1)
		return (errorMsg("Can't create socket"));

	// Bind socket ti IP / port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	hint.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (bind(listening, (sockaddr*)(&hint), sizeof(hint)) == -1)
		return (errorMsg("Can't bind socket"));

	// Mark the socket for listening in
	if (listen(listening, SOMAXCONN) == -1)
		return (errorMsg("Can't listen"));

	// Accept a call and close listening socket
	sockaddr_in client;
	socklen_t clientLen = sizeof(client);
	char host[NI_MAXHOST];
	char svc[NI_MAXSERV];
	int clientSocket = accept(listening, (sockaddr*)(&client), &clientLen);
	if (clientSocket == -1)
		return (errorMsg("Problem with client connecting"));
	close(listening);

	// Register host and service info
	memset(host, 0, NI_MAXHOST);
	memset(svc, 0, NI_MAXSERV);
	int result = getnameinfo((sockaddr*)(&client), sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
	if (result)
		std::cout << host << " connected on " << svc << std::endl;
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
	}

	// Display message and resend message
	char buf[4096];
	while (true)
	{
		// Clear buffer
		memset(buf, 0, 4096);
		// Wait for message
		int bytesRecv = recv(clientSocket, buf, 4096, 0);
		if (bytesRecv == -1) {
			errorMsg("Connection issue detected");
			break;
		}
		if (bytesRecv == 0) {
			errorMsg("Client is disconnected");
			break;
		}
		// Display and resend message
		std::cout << "Received: " << buf << std::endl;
		send(clientSocket, buf, bytesRecv + 1, 0);
	}

	// Close socket
	close(clientSocket);

	return 0;
}