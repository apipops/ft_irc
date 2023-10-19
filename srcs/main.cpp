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

	server.createUser("mark", "mark", socket);
	server.createUser("john", "john", socket);
	server.createUser("bella", "bella", socket);

	server.showUsers();
	return 0;


}