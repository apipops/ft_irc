#include "../includes/IRCServer.hpp"


/************** CONSTRUCTORS & DESTRUCTORS **************/

IRCServer::IRCServer(std::string name):Server(name)
{
}

IRCServer::IRCServer(std::string name, std::string pwd):Server(name)
{
	m_pwd = pwd;
}

IRCServer::IRCServer(const IRCServer & src):Server(src.m_name)
{ 
	*this = src;
}

IRCServer & IRCServer::operator=(const IRCServer & src)
{ 
	// Copy basic servor members and pwd
	Server::operator=(src);
	m_pwd = src.m_pwd;

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

/************************ EXECUTION *********************/

// Wait for events and parse commands
void	IRCServer::checkCommands()
{
	m_socEvent.wait();
	if (this->pendingConnection()) {
		TCP_IPv4::ASocket *newASocket = this->newConnection();
		addUser(newASocket);
	}
	for (size_t i = 0; i < m_users.size(); ++i) {
		//TCP_IPv4::ASocket	*m_users[i]->m_socket = m_users[i]->m_socket;
		if (m_users[i]->m_socket->isReadable()) {
			m_users[i]->m_socket->receive();
			while (!this->isdown() && !m_users.empty() && m_users[i]->m_socket->pendingData()) {
				std::string buf;
				if (m_users[i]->m_socket->extractData(buf, CRLF)) {
					this->log()	<< "command from " << "[" << m_users[i]->m_socket->host()
								<< ":" << m_users[i]->m_socket->serv() << "]:" << std::endl
								<< buf << std::endl;
					executeCommand(m_users[i], buf);
				}
			}
			if (!m_users.empty())
				m_users[i]->m_socket->send();
		}
	}
}

// Send welcome message at the first connection
void	IRCServer::writeWelcome(User *user, std::string nick)
{
	user->m_socket->write(":" + m_name + " 001 " + nick + " :Welcome to the Internet Relay Network!\n");
	user->m_socket->write(":" + m_name + " 002 " + nick + " :Your host is " + m_name + ".\n");
	user->m_socket->write(":" + m_name + " 003 " + nick + " :This server was created " + m_creationTime);
	// user->m_socket->write(":" + m_name + " 004 " + nick + " " + m_name + " OurIRC-1.0 abBcCFiIoqrRswx ov\n");
	// user->m_socket->write(":" + m_name + " 005 " + nick + " RFC2812 CASEMAPPING=ascii PREFIX=(ohv)@+ CHANTYPES=# CHANMODES=itkol :are supported on this server.\n");
	// user->m_socket->write(":" + m_name + " 005 " + nick + " CHANNELLEN=50 NICKLEN=9 TOPICLEN=1000 :are supported on this server.\n");
}

// Execute commands
void	IRCServer::executeCommand(User *user, std::string cmd)
{
	try {
		Message msg(cmd);
		mapCmd	cmds;

		cmds["NICK"] = &IRCServer::nickCmd;
		cmds["USER"] = &IRCServer::userCmd;
		cmds["PING"] = &IRCServer::pingCmd;
		cmds["WHOIS"] = &IRCServer::whoisCmd;
		cmds["JOIN"] = &IRCServer::joinCmd; // in progress
		cmds["PART"] = &IRCServer::partCmd;
		cmds["QUIT"] = &IRCServer::quitCmd;

		mapCmd::const_iterator it = cmds.find(msg.m_cmd);
		if (it != cmds.end()) {
			(this->*(cmds[msg.m_cmd]))(user, msg);
		}
	}
	catch (CmdError &e) {
		writeReply(user, SERVER_PFX, e.what());
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}

// Write reply with prefix in buffer and logfile
void	IRCServer::writeReply(User *user, int prefix, const std::string reply)
{
	std::string fullReply;

	// Adding prefix
	if (prefix == SERVER_PFX)
		fullReply = ":" + m_name + " " + reply;
	else if (prefix == USER_PFX)
		fullReply = ":" + user->m_nick + "!" + user->m_user + " " + reply;
	else
		fullReply = reply;

	// Writing in logfile and buffer
	this->log() << "reply from server " << m_name << " to " << user->m_nick << ": " << std::endl << fullReply << std::endl;
	user->m_socket->write(fullReply);
}


/******************* SERVER SETTER (users, channels, memory) *****************/

// Add a user to IRC server - only socket
void IRCServer::addUser(ASocket* socket)
{
	// Adding to vector and map
	User *user = new User(socket);
	m_users.push_back(user);
}

// Add a channel to IRC server - without password
void IRCServer::addChannel(std::string name, User *user)
{
	// Parsing
	checkChanFormat(name, user);

	// Adding to vector and map
	Channel *channel = new Channel(name);
	m_channels.push_back(channel);
	m_mapChan[name] = m_channels.back();
}

// Add a channel to IRC server - with a password
void IRCServer::addChannel(std::string name, std::string pwd, User *user)
{
	// Parsing
	checkChanFormat(name, user);
	checkPwdFormat(pwd, user);

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


/**************************** UTILS **************************/

// Build a reply based on the IRC norm - without inserting argument
std::string IRCServer::buildReply(User *user, std::string what)
{
	std::string reply;

	// Adding commands and arguments
	size_t mid = what.find(':');
	if (mid != std::string::npos)
		reply += what.substr(0, mid) + user->m_nick + " " + what.substr(mid, what.length() - mid);
	else {
		mid = what.find('\r');
		reply += what.substr(0, mid) + user->m_nick + what.substr(mid, what.length() - mid);
	} 
	return reply;
}

// Build a reply based on the IRC norm - inserting 1 argument
std::string IRCServer::buildReply(User *user, std::string what, std::string arg)
{
	std::string reply;

	// Adding commands and arguments
	size_t mid = what.find(':');
	if (mid != std::string::npos)
		reply = what.substr(0, mid) + user->m_nick + " " + arg + " " + what.substr(mid, what.length() - mid);
	else {
		mid = what.find('\r');
		reply = what.substr(0, mid) + user->m_nick + " " + arg + what.substr(mid, what.length() - mid);
	} 
	return reply;
}

// Build a reply based on the IRC norm - inserting 2 arguments
std::string IRCServer::buildReply(User *user, std::string what, std::string arg1, std::string arg2)
{
	std::string reply;

	// Adding commands and arguments
	size_t mid = what.find(':');
	if (mid != std::string::npos)
		reply = what.substr(0, mid) + user->m_nick + " " + arg1 + " " + arg2 + " " + what.substr(mid, what.length() - mid);
	else {
		mid = what.find('\r');
		reply = what.substr(0, mid) + user->m_nick + " " + arg1 + " " + arg2 + what.substr(mid, what.length() - mid);
	} 
	return reply;
}


/**************************** PARSING *************************/

// Check if nickname "nick" already exists
void IRCServer::checkNickDup(std::string nick, User *user)
{
	if (m_mapUser.find(nick) != m_mapUser.end())
		throw CmdError(ERR_NICKNAMEINUSE, user, nick);
}

// Check if nickanme "name" matches policy name
void IRCServer::checkNickFormat(std::string nick, User *user)
{
	if (nick.length() < USER_MINCHAR)
		throw CmdError(ERR_ERRONEOUSNICKNAME, user, nick);
	if (nick.length() > USER_MAXCHAR)
		throw CmdError(ERR_ERRONEOUSNICKNAME, user, nick);
	for(int i = 0; nick[i]; i++) {
		if (!isalnum(nick[i]) && nick[i] != '-' && nick[i] != '_')
			throw CmdError(ERR_ERRONEOUSNICKNAME, user, nick);
	}
	if (!isalpha(nick[0]))
		throw CmdError(ERR_ERRONEOUSNICKNAME, user, nick);
}

// Check if channel "name" matches policy
void IRCServer::checkChanFormat(std::string name, User *user)
{
	if (name.length() < CHAN_MINCHAR)
		throw CmdError(ERR_INVALIDCHANNELNAME, user, name);
	if (name.length() > CHAN_MAXCHAR)
		throw CmdError(ERR_INVALIDCHANNELNAME, user, name);
	if (name[0] != '#')
		throw CmdError(ERR_INVALIDCHANNELNAME, user, name);
	for(int i = 0; name[i]; i++) {
		if (!isprint(name[i]) || name[i] == ' ' || name[i] == ',' || name[i] == ':')
			throw CmdError(ERR_INVALIDCHANNELNAME, user, name);
	}
}

// Check if topic description format matches the policy
void IRCServer::checkTopicFormat(std::string topic, User *user)
{
	if (topic.length() > TOPIC_MAXCHAR)
		throw CmdError("Topic description is too long", user);
	for(int i = 0; topic[i]; i++) {
		if (!isprint(topic[i]))
			throw CmdError("Topic description has invalid character(s)", user);
	}
 }

// Check if password format matches the policy
void IRCServer::checkPwdFormat(std::string pwd, User *user)
{
	if (pwd.length() < PWD_MINCHAR)
		throw CmdError(ERR_INVALIDKEYFORMAT, user);
	if (pwd.length() > PWD_MAXCHAR)
		throw CmdError(ERR_INVALIDKEYFORMAT, user);
}

/********************* UTILS FOR TESTS ******************/

// void IRCServer::fonctionTest()
// {
// 	try {

// 		ASocket *socket = NULL;

// 		addUser("mark", "mark", socket);
// 		addUser("sam", "sam", socket);
// 		addUser("john", "john", socket);
// 		addUser("bella", "bella", socket);
		
// 		showMapUsers();
// 		showVecUsers();

// 		removeUser("sam");
// 		nickCmd(m_mapUser["bella"], "anne");
		
// 		showMapUsers();
// 		showVecUsers();

// 		joinCmd(m_mapUser["mark"], "#1", "pwd");
// 		joinCmd(m_mapUser["mark"], "#2");
// 		joinCmd(m_mapUser["mark"], "#3");
// 		joinCmd(m_mapUser["john"], "#1", "pwd");

// 		showMapChannels();
// 		showvecChans();

// 		showChannelsOfUser("mark");
// 		showUsersOfChannel("#1");

// 		partCmd(m_mapUser["mark"], "#1");
// 		showChannelsOfUser("mark");
// 		showUsersOfChannel("#2");

// 	}
// 	catch (CmdError & e) {
// 		std::cerr << e.what() << std::endl;
// 	}
// }

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
		std::cout << " Nick: " << it->first << ", User: " << it->second->m_user << ", Real: " << it->second->m_real << std::endl;
}


void IRCServer::showMapChannels() const
{
	std::cout << "[CHANNELS - MAP]:" << std::endl;
	mapChannel::const_iterator it = m_mapChan.begin();
	for (; it != m_mapChan.end(); it++)
		std::cout << " Channel: " << it->first << std::endl;
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

/*************************** EXCEPTIONS **************************/

// Instanciate an error message according to IRC norm - without argument
IRCServer::CmdError::CmdError(std::string what, User *user)
{
	m_what = buildReply(user, what);
}

// Instanciate an error message according to IRC norm - with an argument
IRCServer::CmdError::CmdError(std::string what, User *user, std::string arg)
{
	m_what = buildReply(user, what, arg);
}


IRCServer::CmdError::~CmdError() _NOEXCEPT
{
}

const char *IRCServer::CmdError::what() const _NOEXCEPT
{
	return m_what.c_str();
}