#include "../includes/Channel.hpp"
#include "../includes/User.hpp"
#include "../includes/IRCServer.hpp"
#include "../raph/TCP_IPv4"

class Server;
class User;
using namespace TCP_IPv4;

int main(void)
{
	IRCServer server;

	server.fonctionTest(); // tester les commandes basiques

	return 0;
}