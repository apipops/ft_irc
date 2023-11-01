#include "../includes/Channel.hpp"
#include "../includes/User.hpp"
#include "../includes/Message.hpp"
#include "../includes/IRCServer.hpp"
#include "../raph/TCP_IPv4"

using namespace TCP_IPv4;

int main(int ac, char **av)
{

	// // (void)ac;
	// // (void)av;
	// // server.fonctionTest();

	if (ac != 3)
		return -1;
	
	IRCServer server("test", av[1]);
	server.start(av[1]);
	while (!server.isdown()) {
		server.checkCommands();
		// server.showMapUsers();
		// server.showMapChannels();
	}

	// try {

	// Message msg(av[1]);
	// std::cout << msg.getMessage() << std::endl;
	// msg.showMessage();
	// }
	// catch (Message::MsgError & e) {
	// 	std::cout << e.what() << std::endl;
	// }


	return 0;
}