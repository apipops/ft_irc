#include "../includes/User.hpp"

/************** CONSTRUCTORS & DESTRUCTORS ****************/

User::User()
{
}

User::User(std::string nick, std::string user, ASocket* socket)
{

	this->m_nick = nick;
	this->m_user = user;
	this->m_socket = socket;
	this->m_servOps = 0;
}

User::User(const User & src)
{
	*this = src;
}

User & User::operator=(const User & src)
{
	this->m_nick = src.m_nick;
	this->m_user = src.m_user;
	this->m_socket = src.m_socket;
	this->m_servOps = src.m_servOps;
	this->m_allChan = src.m_allChan;
	this->m_opsChan = src.m_opsChan;
	
	return *this;
}

User::~User()
{
}

/************************ PARSING ************************/

// Check if "Nickname"/"Username" (defined by type) "name" matches policy name
void User::checkFormat(std::string type, std::string name)
{
	if (name.length() < USER_MINCHAR)
		throw UserError(type + "\'" + name +"\' is too short");
	if (name.length() > USER_MAXCHAR)
		throw UserError(type + "\'" + name +"\' is too long");
	for(int i = 0; name[i]; i++) {
		if (!isalnum(name[i]) && name[i] != '-' && name[i] != '_')
			throw User::UserError(type + "\'" + name +"\' has invalid character(s)");
	}
	if (!isalpha(name[0]))
		throw User::UserError(type + " must start with a letter");
}

/******************* GETTERS AND SETTERS *******************/

const std::string & User::getNick() const
{
	return m_nick;
}

/********************* BASIC COMMANDS *********************/

/* // Replace nickname by 'newNick'
void	User::nickCmd(std::string newNick)
{
	if (newNick == m_nick)
		throw UserError("Nickname is already \'" + m_nick + "\'");
	checkFormat("Nickname", newNick);
	m_nick = newNick;
}

// Replace username by 'newUser'
void	User::userCmd(std::string newUser)
{
	if (newUser == m_user)
		throw UserError("Username is already \'" + m_user + "\'");
	checkFormat("Username", newUser);
	m_user = newUser;
}

// Create a new channel and return it - without password
Channel User::createCmd(std::string name, std::string topic)
{
	Channel::checkChanFormat(name);
	Channel::checkTopicFormat(topic);
	Channel channel(name, topic);
	channel.addUser(*this);
	channel.addOps(*this);
	m_allChan[name] = channel;
	m_opsChan[name] = channel;
	return channel;
}

// Create a new channel and return it - with a password
Channel User::createCmd(std::string name, std::string topic, std::string pwd)
{
	Channel::checkChanFormat(name);
	Channel::checkTopicFormat(topic);
	Channel::checkPwdFormat(pwd);
	Channel channel(name, topic, pwd);
	channel.addUser(*this);
	channel.addOps(*this);
	m_allChan[name] = channel;
	m_opsChan[name] = channel;
	return channel;
}

// Join a channel - without providing a password
void	User::joinCmd(Channel & channel)
{
	if (!channel.m_pwd.empty())
		throw UserError("Access denied to channel \'" + channel.m_name + "\'. Password required.");
	if (channel.m_invitMode)
		throw UserError("Access denied to channel \'" + channel.m_name + "\'. Invitation only.");
	if (channel.m_maxUsers == static_cast<int>(channel.m_users.size()))
		throw UserError("Access denied to channel \'" + channel.m_name + "\'. Maximum numbers of users reached.");
	channel.addUser(*this);
	m_allChan[channel.m_name] = channel;
}

// Join a channel - with providing a password
void	User::joinCmd(Channel & channel, std::string pwd)
{
	if (pwd != channel.m_pwd)
		throw UserError("Access denied to channel \'" + channel.m_name + "\'. Incorrect password.");
	if (channel.m_invitMode)
		throw UserError("Access denied to channel \'" + channel.m_name + "\'. Invitation only.");	
	if (channel.m_maxUsers == static_cast<int>(channel.m_users.size()))
		throw UserError("Access denied to channel \'" + channel.m_name + "\'. Maximum numbers of users reached.");	
	channel.addUser(*this);
	m_allChan[channel.m_name] = channel;
}

// Part from a channel
void	User::partCmd(Channel & channel)
{
	channel.removeUser(m_nick);
	channel.removeOps(m_nick);
	m_allChan.erase(channel.m_name);
	m_opsChan.erase(channel.m_name);
	// si on est le dernier : renvoyer un valeur pour supprimer channel ?
} */


/********************** EXCEPTIONS ***********************/

User::UserError::UserError(std::string what): Error(what)
{
}
