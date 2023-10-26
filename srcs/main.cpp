#include "../includes/Channel.hpp"
#include "../includes/User.hpp"
#include "../includes/Message.hpp"
#include "../includes/IRCServer.hpp"
#include "../raph/TCP_IPv4"

class Server;
class User;
using namespace TCP_IPv4;

int main(int ac, char **av)
{
	(void)ac;
	(void)av;
	IRCServer server("test");

	server.fonctionTest(); // tester les commandes basiques

	// try {
	// (void)ac;
	// Message message(av[1]);
	// std::cout << "Full message:" << message.getMessage() << std::endl;
	// message.showMessage();
	// }
	// catch (std::exception & e) {
	// 	std::cout << e.what() << std::endl;
	// }

	return 0;
}