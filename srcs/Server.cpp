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

std::map<std::string, Channel> Server::getChannels()
{
	return m_channels;
}

std::map<std::string, User> Server::getUsers()
{
	return m_users;
}


void Server::showUsers()
{
	std::map<std::string, User>::iterator it = m_users.begin();
	for (; it != m_users.end(); ++it)
		std::cout << "User: " << it->first << std::endl;
}



void Server::showChannels()
{
	std::map<std::string, Channel>::iterator it = m_channels.begin();
	for (; it != m_channels.end(); it++)
		std::cout << "Channel: " << it->first << std::endl;
}