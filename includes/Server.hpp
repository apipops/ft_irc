#ifndef SERVER_HPP
# define SERVER_HPP

# include "Channel.hpp"
# include "User.hpp"
# include "../raph/TCP_IPv4"
# include <map>
# include <algorithm>
# include <iostream>
# include <string>

using namespace TCP_IPv4;

class Server 
{
public:
	Server();
	Server(const Server & src);
	Server & operator=(const Server & src);
	virtual ~Server();

	// PARSING
	// void checkUserFormat(std::string type, std::string str);
	// void checkUserDup(std::string nick, std::string user);

	// USERS & CHANNELS
	void addUser(std::string nick, std::string user, ASocket* socket);
	void addChannel(std::string name, std::string topic);

	// TEST UTILS
	std::map<std::string, Channel> getChannels();
	std::map<std::string, User> getUsers();
	void showUsers();
	void showChannels();

private:
	std::map<std::string, Channel> m_channels;
	std::map<std::string, User> m_users;
};

#endif