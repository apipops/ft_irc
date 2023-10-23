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
	ASocket *socket = NULL;

	server.addUser("mark", "mark", socket);
	server.addUser("john", "john", socket);
	server.addUser("bella", "bella", socket);
	
	server.showMapUsers();
	server.showVecUsers();

 	server.addChannel("#1", "random");
	server.addChannel("#2", "random");
	server.addChannel("#3", "random", "pwd");

	server.showMapChannels();
	server.showVecChannels(); 

	std::cout << std::endl;
	server.fonctionTest();
	server.showMapUsers();
	//server.showVecUsers();
	server.showChannelsOfUser("mark");
	// server.showUsersOfChannel("#1");


	return 0;


}