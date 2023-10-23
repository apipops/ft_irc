#include "../includes/IRCServer.hpp"


/************** CONSTRUCTORS & DESTRUCTORS **************/

IRCServer::IRCServer()
{
}

IRCServer::IRCServer(const IRCServer & src)
{ 
	*this = src;
}

IRCServer & IRCServer::operator=(const IRCServer & src)
{ 
	this->m_channels = src.m_channels;
	this->m_users = src.m_users;
	this->m_mapChan = src.m_mapChan;
	this->m_mapUser = src.m_mapUser;

	return *this;
}

IRCServer::~IRCServer()
{
}


/************************* PARSING *********************/

// Check if nickname "nick" already exists
void IRCServer::checkUserDup(std::string nick)
{
	if (m_mapUser.find(nick) != m_mapUser.end())
		throw CmdError("Nickname \'" + nick + "\' already exists");
}

// Check if channel "channel" already exists
void IRCServer::checkChanDup(std::string channel)
{
	if (m_mapChan.find(channel) != m_mapChan.end())
		throw CmdError("Channel \'" + channel + "\' already exists");
}

// Check if "Nickname"/"Username" (defined by type) "name" matches policy name
void IRCServer::checkUserFormat(std::string type, std::string name)
{
	if (name.length() < USER_MINCHAR)
		throw CmdError(type + "\'" + name +"\' is too short");
	if (name.length() > USER_MAXCHAR)
		throw CmdError(type + "\'" + name +"\' is too long");
	for(int i = 0; name[i]; i++) {
		if (!isalnum(name[i]) && name[i] != '-' && name[i] != '_')
			throw CmdError(type + "\'" + name +"\' has invalid character(s)");
	}
	if (!isalpha(name[0]))
		throw CmdError(type + " must start with a letter");
}

// Check if channel "name" matches policy
void IRCServer::checkChanFormat(std::string name)
{
	if (name.length() < CHAN_MINCHAR)
		throw CmdError("Channel name \'" + name +"\' is too short");
	if (name.length() > CHAN_MAXCHAR)
		throw CmdError("Channel name \'" + name +"\' is too long");
	if (name[0] != '#')
		throw CmdError("Channel name must start with character \'#\'");
	for(int i = 0; name[i]; i++) {
		if (!isprint(name[i]) || name[i] == ' ' || name[i] == ',' || name[i] == ':')
			throw CmdError("Chanel name \'" + name +"\' has invalid character(s)");
	}
}

// Check if topic description format matches the policy
void IRCServer::checkTopicFormat(std::string topic)
{
	if (topic.length() > TOPIC_MAXCHAR)
		throw CmdError("Topic description is too long");
	for(int i = 0; topic[i]; i++) {
		if (!isprint(topic[i]))
			throw CmdError("Topic description has invalid character(s)");
	}
 }

// Check if password format matches the policy
void IRCServer::checkPwdFormat(std::string pwd)
{
	if (pwd.length() < PWD_MINCHAR)
		throw CmdError("Password is too short");
	if (pwd.length() > PWD_MAXCHAR)
		throw CmdError("Password is too long");
}


/******************* USERS & CHANNELS *****************/

// Add a user to IRC server
void IRCServer::addUser(std::string nick, std::string username, ASocket* socket)
{
	// Parsing
	checkUserDup(nick);
	checkUserFormat("Nickname", nick);
	checkUserFormat("Username", username);

	// Adding to vector and map
	User user(nick, username, socket);
	m_users.push_back(user);
	m_mapUser[nick] = &(m_users.back());
}

// Add a channel to IRC server - without password
void IRCServer::addChannel(std::string name, std::string topic)
{
	// Parsing
	checkChanDup(name);
	Channel::checkChanFormat(name);
	Channel::checkTopicFormat(topic);

	// Adding to vector and map
	Channel channel(name, topic);
	m_channels.push_back(channel);
	m_mapChan[name] = &(m_channels.back());
}

// Add a channel to IRC server - with a password
void IRCServer::addChannel(std::string name, std::string topic, std::string pwd)
{
	// Parsing
	checkChanDup(name);
	Channel::checkChanFormat(name);
	Channel::checkTopicFormat(topic);
	Channel::checkPwdFormat(pwd);

	// Adding to vector and map
	Channel channel(name, topic, pwd);
	m_channels.push_back(channel);
	m_mapChan[name] = &(m_channels.back());
}


/******************** BASIC COMMANDS ********************/

// Replace nickname of user by 'newNick'
void	IRCServer::nickCmd(User* user, std::string newNick)
{
	// Parsing
	checkUserFormat("Nickname", newNick);
	checkUserDup(newNick);

	// Suppress from map, update in vector and add again in map
	m_mapUser.erase(user->m_nick);
	user->m_nick = newNick;
	m_mapUser[newNick] = user;
}

	
	// void	userCmd(std::string newUser);
	// Channel createCmd(std::string name, std::string topic); // sans pwd
	// Channel createCmd(std::string name, std::string topic, std::string pwd); // avec pwd
	// void	joinCmd(Channel & channel); // sans pwd
	// void	joinCmd(Channel & channel, std::string pwd); // avec pwd
	// void	partCmd(Channel & channel);

	// // Operator commands
	// void 	kickCmd();
	// void 	inviteCmd();
	// void 	topicCmd();
	// void 	modeCmd();


void IRCServer::fonctionTest()
{
	try {
		nickCmd(m_mapUser["bella"], "karl");
	}
	catch (CmdError & e) {
		std::cerr << e.what() << std::endl;
	}
}

/************** OPERATOR COMMANDS ***************/


/*************** UTILS FOR TESTS ***************/

mapChannel IRCServer::getChannels() const
{
	return m_mapChan;
}

mapUser IRCServer::getUsers() const
{
	return m_mapUser;
}

void IRCServer::showMapUsers() const
{
	std::cout << "[MAP] All users:" << std::endl;
	mapUser::const_iterator it = m_mapUser.begin();
	for (; it != m_mapUser.end(); ++it)
		std::cout << " User: " << it->first << std::endl;
}


void IRCServer::showMapChannels() const
{
	std::cout << "[MAP] All channels:" << std::endl;
	mapChannel::const_iterator it = m_mapChan.begin();
	for (; it != m_mapChan.end(); it++)
		std::cout << " Channel: " << it->first << std::endl;
}

void IRCServer::showVecUsers() const
{
	std::cout << "[VEC] All users:" << std::endl;
	std::vector<User>::const_iterator	it = m_users.begin();
	for (; it != m_users.end(); ++it)
		std::cout << " User: " << it->m_nick << std::endl;
}


void IRCServer::showVecChannels() const
{
	std::cout << "[VEC] All channels:" << std::endl;
	std::vector<Channel>::const_iterator it = m_channels.begin();
	for (; it != m_channels.end(); ++it)
		std::cout << " Channel: " << it->m_name << std::endl;
}

/************** EXCEPTION ***************/

IRCServer::CmdError::CmdError(std::string what): Error(what)
{
}