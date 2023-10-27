#include "../includes/Channel.hpp"
#include "../includes/User.hpp"
#include "../includes/Message.hpp"
#include "../includes/IRCServer.hpp"
#include "../raph/TCP_IPv4"

using namespace TCP_IPv4;

int main(int ac, char **av)
{
	IRCServer server("test");

	// (void)ac;
	// (void)av;
	// server.fonctionTest();

	if (ac < 2)
		return -1;
	
	server.start(av[1]);
	while (1)
		server.runTest();

	return 0;
}