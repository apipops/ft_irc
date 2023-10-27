#include "../includes/Channel.hpp"
#include "../includes/User.hpp"
#include "../includes/Message.hpp"
#include "../includes/IRCServer.hpp"
#include "../raph/TCP_IPv4"

using namespace TCP_IPv4;

int main()
{
	IRCServer server("test");

	server.fonctionTest();
}