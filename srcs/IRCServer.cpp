#include "../includes/IRCServer.hpp"


/************** CONSTRUCTORS & DESTRUCTORS **************/

IRCServer::IRCServer(std::string name):Server(name)
{
}

IRCServer::IRCServer(const IRCServer & src):Server(src.m_name)
{ 
	*this = src;
}

IRCServer & IRCServer::operator=(const IRCServer & src)
{ 
	// Copy basic servor members
	Server::operator=(src);

	// Free memory
	this->freeMemory();

	// Copy users
	User *user;
	vecUser::const_iterator itu = src.m_users.begin();
	for (; itu != m_users.end(); itu++) {
 		user = new User (**(itu));
		this->m_users.push_back(user);
	}
	this->m_mapUser = src.m_mapUser;

	// Copy channels
	Channel *channel;
	vecChan::const_iterator itc = src.m_channels.begin();
	for (; itc != m_channels.end(); itc++) {
		channel = new Channel (**(itc));
		this->m_channels.push_back(channel);
	}
	this->m_mapChan = src.m_mapChan;

	return *this;
}

IRCServer::~IRCServer()
{
	freeMemory();
}

/************************* PARSING **********************/

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

/******************* SERVER SETTER (users, channels, memory) *****************/

// Add a user to IRC server
void IRCServer::addUser(std::string nick, std::string username, ASocket* socket)
{
	// Parsing
	checkUserDup(nick);
	checkUserFormat("Nickname", nick);
	checkUserFormat("Username", username);

	// Adding to vector and map
	User *user = new User(nick, username, socket);
	m_users.push_back(user);
	m_mapUser[nick] = m_users.back();
}

// Add a channel to IRC server - without password
void IRCServer::addChannel(std::string name)
{
	// Parsing
	checkChanDup(name);
	checkChanFormat(name);

	// Adding to vector and map
	Channel *channel = new Channel(name);
	m_channels.push_back(channel);
	m_mapChan[name] = m_channels.back();
}

// Add a channel to IRC server - with a password
void IRCServer::addChannel(std::string name, std::string pwd)
{
	// Parsing
	checkChanDup(name);
	checkChanFormat(name);
	checkPwdFormat(pwd);

	// Adding to vector and map
	Channel *channel = new Channel(name, pwd);
	m_channels.push_back(channel);
	m_mapChan[name] = m_channels.back();
}

// Remove a user from their nickname
void IRCServer::removeUser(std::string nick)
{
	// From channels
	User *user = m_mapUser[nick];
	mapChannel::const_iterator it_chan = user->m_allChan.begin();
	for (; it_chan != user->m_allChan.end(); it_chan++) {
		it_chan->second->removeUser(nick);
		it_chan->second->removeOps(nick);
	}

	// From IRC server
	vecUser::iterator it = m_users.begin();
	while (it != m_users.end() && (*it)->m_nick != nick)
		it++;
	if (it != m_users.end())
	{
		delete user;
		m_mapUser.erase(nick);
		m_users.erase(it);
	}
}

// Remove a channel from its name
void IRCServer::removeChannel(std::string name)
{
	// From users
	Channel *channel = m_mapChan[name];
	vecUser::const_iterator it_user = channel->m_users.begin();
	for (; it_user != channel->m_users.end(); it_user++) {
		(*it_user)->m_allChan.erase(name);
		(*it_user)->m_opsChan.erase(name);
	}

	// From IRC server
	vecChan::iterator it = m_channels.begin();
	while (it != m_channels.end() && (*it)->m_name != name)
		it++;
	if (it != m_channels.end())
	{
		delete channel;
		m_mapChan.erase(name);
		m_channels.erase(it);
	}
}

void	IRCServer::freeMemory(void)
{
	// Free users
	vecUser::iterator itu = m_users.begin();
	itu = m_users.begin();
	for (; itu != m_users.end(); itu++) {
		(*itu)->m_allChan.clear();
		(*itu)->m_opsChan.clear();
		delete *(itu);
	}
	m_mapUser.clear();
	m_users.clear();

	// Free channels
	vecChan::iterator itc = m_channels.begin();
	itc = m_channels.begin();
	for (; itc != m_channels.end(); itc++) {
		(*itc)->m_users.clear();
		(*itc)->m_ops.clear();
		delete *(itc);
	}
	m_mapChan.clear();
	m_channels.clear();
}

/********************** BASIC COMMANDS **********************/

// Replace nickname of user by 'newNick'
void	IRCServer::nickCmd(User* user, std::string newNick)
{
	// Parsing - throw exception
	checkUserFormat("Nickname", newNick);
	checkUserDup(newNick);

	// Suppress from map, update in vector and add again in map
	m_mapUser.erase(user->m_nick);
	user->m_nick = newNick;
	m_mapUser[newNick] = user;
}

// Replace username by 'newUser'
void	IRCServer::userCmd(User* user, std::string newUser)
{
	checkUserFormat("Username", newUser);
	user->m_user = newUser;

}


// Make a user join a channel named 'name' - without password
void	IRCServer::joinCmd(User* user, std::string name)
{
	mapChannel::const_iterator it = m_mapChan.find(name);
	// Creating or joing existing channel
	if (it == m_mapChan.end()) {
		addChannel(name);
		Channel *channel = m_mapChan[name];
		channel->addOps(user);
		channel->addUser(user);
		user->m_allChan[name];
		user->m_opsChan[name] = channel;
	}
	else {
		Channel *channel = it->second;
		if (!(channel->m_pwd.empty()))
			throw CmdError("Access denied to channel '" + name + "'. Password required.");
		if (channel->m_invitMode && !channel->checkInvit(user->m_nick))
			throw CmdError("Access denied to channel \'" + channel->m_name + "\'. Invitation only.");
		if (channel->m_maxUsers == static_cast<int>(channel->m_users.size()))
			throw CmdError("Access denied to channel \'" + channel->m_name + "\'. Maximum numbers of users reached.");
		channel->addUser(user);
		user->m_allChan[name] = channel;
	}
}

// Make a user join a channel named 'name' - with a password
void	IRCServer::joinCmd(User* user, std::string name, std::string pwd)
{
	mapChannel::const_iterator it = m_mapChan.find(name);
	// Creating or joing existing channel
	if (it == m_mapChan.end()) {
		addChannel(name, pwd);
		Channel *channel = m_mapChan[name];
		channel->addOps(user);
		channel->addUser(user);
		user->m_allChan[name];
		user->m_opsChan[name] = channel;
	}
	else {
		Channel *channel = it->second;
		if (pwd != channel->m_pwd)
		{
			std::cout << "pwd: " << channel->m_pwd << " vs. given pwd: " << pwd << std::endl;
			throw CmdError("Access denied to channel '" + name + "'. Incorrect password.");
		}
		if (channel->m_invitMode && !channel->checkInvit(user->m_nick))
			throw CmdError("Access denied to channel \'" + channel->m_name + "\'. Invitation only.");
		if (channel->m_maxUsers == static_cast<int>(channel->m_users.size()))
			throw CmdError("Access denied to channel \'" + channel->m_name + "\'. Maximum numbers of users reached.");
		channel->addUser(user);
		user->m_allChan[name] = channel;
	}
}

void	IRCServer::partCmd(User* user, std::string name)
{
	if (m_mapChan.find(name) == m_mapChan.end())
		throw CmdError("Channel '" + name + "' does not exist.");

	// Remove user from channel
	m_mapChan[name]->removeUser(user->m_nick);
	m_mapChan[name]->removeOps(user->m_nick);
	user->m_allChan.erase(name);
	user->m_opsChan.erase(name);

	// Check if users are remaining
	if (m_mapChan[name]->m_users.empty())
		removeChannel(name);
}

/******************** OPERATOR COMMANDS ******************/

	// void 	kickCmd();
	// void 	inviteCmd();
	// void 	topicCmd();
	// void 	modeCmd();


/********************* UTILS FOR TESTS ******************/

void IRCServer::fonctionTest()
{
	try {

		ASocket *socket = NULL;

		addUser("mark", "mark", socket);
		addUser("sam", "sam", socket);
		addUser("john", "john", socket);
		addUser("bella", "bella", socket);
		
		showMapUsers();
		showVecUsers();

		removeUser("sam");
		nickCmd(m_mapUser["bella"], "anne");
		
		showMapUsers();
		showVecUsers();

		joinCmd(m_mapUser["mark"], "#1", "pwd");
		joinCmd(m_mapUser["mark"], "#2");
		joinCmd(m_mapUser["mark"], "#3");
		joinCmd(m_mapUser["john"], "#1", "pwd");

		showMapChannels();
		showvecChans();

		showChannelsOfUser("mark");
		showUsersOfChannel("#1");

		partCmd(m_mapUser["mark"], "#1");
		showChannelsOfUser("mark");
		showUsersOfChannel("#2");

	}
	catch (CmdError & e) {
		std::cerr << e.what() << std::endl;
	}
}

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
	std::cout << "[USERS - MAP]:" << std::endl;
	mapUser::const_iterator it = m_mapUser.begin();
	for (; it != m_mapUser.end(); ++it)
		std::cout << " User: " << it->first << ", addr:" << it->second << std::endl;
}


void IRCServer::showMapChannels() const
{
	std::cout << "[CHANNELS - MAP]:" << std::endl;
	mapChannel::const_iterator it = m_mapChan.begin();
	for (; it != m_mapChan.end(); it++)
		std::cout << " Channel: " << it->first << ", addr:" << it->second <<std::endl;
}

void IRCServer::showVecUsers() const
{
	std::cout << "[USERS - VECTOR]:" << std::endl;
	vecUser::const_iterator	it = m_users.begin();
	for (; it != m_users.end(); ++it)
		std::cout << " User: " << (*it)->m_nick << ", addr: " << (*it) << std::endl;
}


void IRCServer::showvecChans() const
{
	std::cout << "[CHANNELS - VECTOR]:" << std::endl;
	vecChan::const_iterator it = m_channels.begin();
	for (; it != m_channels.end(); ++it)
		std::cout << " Channel: " << (*it)->m_name << ", addr: " << (*it)  << std::endl;
}

void IRCServer::showChannelsOfUser(std::string nick) const
{
	User *user = m_mapUser.at(nick);
	
	mapChannel::const_iterator it = user->m_allChan.begin();
	std::cout << "[" << nick << "'s CHANNELS]: " << std::endl;
	for (; it != user->m_allChan.end(); ++it)
		std::cout << " " <<it->first << std::endl;

}

void IRCServer::showUsersOfChannel(std::string channel) const
{
	Channel *chan = m_mapChan.at(channel);
	
	vecUser::const_iterator it = chan->m_users.begin();
	std::cout << "[" << channel << " USERS]:" << std::endl;
	for (; it != chan->m_users.end(); ++it)
		std::cout << " " << (*it)->m_nick << std::endl;

}

/********************** EXCEPTION ********************/

IRCServer::CmdError::CmdError(std::string what): Error(what)
{
}