#include "../includes/Channel.hpp"
#include "../includes/User.hpp"
#include "../includes/Server.hpp"
#include "../raph/TCP_IPv4"

class Server;
class User;
using namespace TCP_IPv4;

int main(void)
{
	Server server;
	ASocket* socket = NULL;

	server.addUser("mark", "mark", socket);
	server.addUser("john", "john", socket);
	server.addUser("bella", "bella", socket);
	server.addUser("bella", "bella", socket);
	
	server.showUsers();

	server.addChannel("#1", "random");
	server.addChannel("#2", "random");
	server.addChannel("#3", "random");
	server.addChannel("#4", "random", "pwd");
	server.addChannel("#4", "random");
	server.addChannel("yoo", "random");

	server.showChannels();

	return 0;


}