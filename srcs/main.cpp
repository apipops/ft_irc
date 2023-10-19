#include "../includes/Channel.hpp"
#include "../includes/User.hpp"
#include "../includes/Server.hpp"
#include "../raph/TCP_IPv4"

class Server;
using namespace TCP_IPv4;

int main(void)
{
	Server server;
	ASocket* socket = NULL;

	server.addUser("mark", "mark", socket);
	server.addUser("john", "john", socket);
	server.addUser("bella", "bella", socket);

	server.showUsers();
	return 0;


}