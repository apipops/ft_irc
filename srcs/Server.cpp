#include "../includes/Server.hpp"

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

/***************** PARSING ****************/

// Check if nickname "nick" already exists
void Server::checkUserDup(std::string nick)
{
	if (m_users.find(nick) != m_users.end())
		throw User::UserError("Nickname \'" + nick + "\' already exists");
}

// Check if channel "channel" already exists
void Server::checkChanDup(std::string channel)
{
	if (m_channels.find(channel) != m_channels.end())
		throw Channel::ChannelError("Channel \'" + channel + "\' already exists");
}

/************ USERS & CHANNELS ************/

void Server::addUser(std::string nick, std::string username, ASocket* socket)
{
	try {
		checkUserDup(nick);
		User::checkFormat("Nickname", nick);
		User::checkFormat("Username", username);
		User user(nick, username, socket);
		m_users.insert(std::pair<std::string, User>(nick, user));
	} 
	catch (User::UserError & e) {
		std::cout << e.what() << std::endl;
	}
}

void Server::addChannel(std::string name, std::string topic)
{
	try {
		checkChanDup(name);
		Channel::checkChanFormat(name);
		Channel::checkTopicFormat(topic);
		Channel channel(name, topic);
		m_channels.insert(std::pair<std::string, Channel>(name, channel));
	}
	catch (Channel::ChannelError & e) {
		std::cout << e.what() << std::endl;
	}
}

void Server::addChannel(std::string name, std::string topic, std::string pwd)
{
	try {
		checkChanDup(name);
		Channel::checkChanFormat(name);
		Channel::checkTopicFormat(topic);
		Channel::checkPwdFormat(pwd);
		Channel channel(name, topic, pwd);
		m_channels.insert(std::pair<std::string, Channel>(name, channel));
	}
	catch (Channel::ChannelError & e) {
		std::cout << e.what() << std::endl;
	}
}

/************ UTILS FOR TESTS ************/

mapChannel Server::getChannels() const
{
	return m_channels;
}

mapUser Server::getUsers() const
{
	return m_users;
}


void Server::showUsers() const
{
	mapUser::const_iterator it = m_users.begin();
	for (; it != m_users.end(); ++it)
		std::cout << "User: " << it->first << std::endl;
}



void Server::showChannels() const
{
	mapChannel::const_iterator it = m_channels.begin();
	for (; it != m_channels.end(); it++)
		std::cout << "Channel: " << it->first << std::endl;
}