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

typedef std::map<std::string, User> mapUser;
typedef std::map<std::string, Channel> mapChannel;

class Channel;
class User;

class Server 
{
public:
	// Constructors and destructors
	Server();
	Server(const Server & src);
	Server & operator=(const Server & src);
	virtual ~Server();

	// Parsing
	void checkUserDup(std::string nick);
	void checkChanDup(std::string channel);

	// Users & Channels
	void addUser(std::string nick, std::string user, ASocket* socket);
	void addChannel(std::string name, std::string topic);
	void addChannel(std::string name, std::string topic, std::string pwd);

	// Utils for test
	mapChannel getChannels() const;
	mapUser getUsers() const;
	void showUsers() const;
	void showChannels() const;

private:
	mapChannel	m_channels;
	mapUser 	m_users;
};

#endif