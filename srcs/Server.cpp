#include "../includes/Server.hpp"
#include <algorithm>

/*********** CONSTRUCTORS & DESTRUCTORS ************/

Server::Server()
{
}

Server::Server(const Server & src)
{ 
	*this = src;
}

Server & Server::operator=(const Server & src)
{ 
	this->m_channels = src.m_channels;
	this->m_users = src.m_users;

	return *this;
}

Server::~Server()
{
}

/*********** USERS & CHANNELS ************/

void Server::addUser(std::string nick, std::string username, ASocket* socket)
{
	User user(nick, username, socket);
	m_users.insert(std::pair<std::string, User>(nick, user));
}

void Server::addChannel(std::string name, std::string topic)
{
	Channel channel(name, topic);
	m_channels.insert(std::pair<std::string, Channel>(name, channel));
}

/*********** TEST UTILS ************/

MapChannel Server::getChannels() const
{
	return m_channels;
}

MapUser Server::getUsers() const
{
	return m_users;
}


void Server::showUsers() const
{
	MapUser::const_iterator it = m_users.begin();
	for (; it != m_users.end(); ++it)
		std::cout << "User: " << it->first << std::endl;
}



void Server::showChannels() const
{
	MapChannel::const_iterator it = m_channels.begin();
	for (; it != m_channels.end(); it++)
		std::cout << "Channel: " << it->first << std::endl;
}