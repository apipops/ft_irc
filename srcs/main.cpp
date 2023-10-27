#include "../includes/Channel.hpp"
#include "../includes/User.hpp"
//#include "../includes/Message.hpp"
#include "../includes/IRCServer.hpp"
#include "../raph/TCP_IPv4"

// class Server;
// class User;
using namespace TCP_IPv4;

int main(int ac, char **av)
{
	(void)ac;
	TCP_IPv4::Server server("test");

	server.start(av[1]);
	while (1)
		server.runTest();

	return 0;
}