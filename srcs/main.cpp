#include "../includes/Channel.hpp"
#include "../includes/User.hpp"
#include "../includes/Message.hpp"
#include "../includes/IRCServer.hpp"
#include "../raph/TCP_IPv4"

using namespace TCP_IPv4;

int main(int ac, char **av)
{

	try {
		if (ac != 3) {
			std::cout << "Usage: ./irc <port> <password>" << std::endl;
			return -1;
		}
		for (size_t i = 0; av[1][i]; i++) {
			if (!std::isdigit(av[1][i])) {
				std::cerr << "Error: parse: wrong port number" << std::endl;
				return -1;
			}
		}
		if (std::atoll(av[1]) > INT_MAX) {
			std::cerr << "Error: parse: port number" << std::endl;
			return -1;
		}

		IRCServer server("MiniRC", av[2]);
		server.start(av[1]);
		while (!server.isdown()) {
			server.checkEvents();
			// server.showMapUsers();
			// server.showMapChannels();
		}

	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}