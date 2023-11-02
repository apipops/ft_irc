#include "../includes/IRCServer.hpp"

/********************** BASIC COMMANDS **********************/

// [PASS] Check if password is correct
void	IRCServer::passCmd(User* user, Message& msg)
{
	if (!msg.m_args.size() || msg.m_args[0] != m_pwd) {
 		writeToClient(user, m_name, buildReply(user, ERR_PASSWDMISMATCH));
		user->m_socket->send();
		closeConnection(user->m_socket);
		removeUser(user);
		throw IRCServer::UserRemoved();
	}
	user->m_authentified = true;
}

// [NICK] Add/replace nickname of user
void	IRCServer::nickCmd(User* user, Message& msg)
{
	// Parsing - throw exception
	if (!(user->m_authentified)) {
		Message emptyMsg;
		passCmd(user, emptyMsg);
	}
	if (!msg.m_args.size())
		throw CmdError(ERR_NONICKNAMEGIVEN, user);
	checkNickFormat(msg.m_args[0], user);
	checkNickDup(msg.m_args[0], user);

	// Suppress from map, update in vector and add again in map
	if (user->m_nick.empty())
		writeWelcome(user, msg.m_args[0]);
	else {
		writeToClient(user, user->getPrefix(), "NICK :" + msg.m_args[0] + CRLF);
		writeToRelations(user, "NICK :" + msg.m_args[0] + CRLF);
	}
	m_mapUser.erase(user->m_nick);
	user->m_nick = msg.m_args[0];
	m_mapUser[msg.m_args[0]] = user;
}


// [USER] Add/replace username and realname of user
void	IRCServer::userCmd(User* user, Message& msg)
{
	if (msg.m_args.size() != 4)
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	user->m_user = msg.m_args[0];
	user->m_real = msg.m_args[3];
}

// [PING] Notify the client the server is up
void	IRCServer::pingCmd(User* user, Message& msg)
{
	(void)msg;
	std::string reply = "PONG " + m_name + " :" + user->m_nick + CRLF;
	writeToClient(user, m_name, reply);
}

// [WHOIS] Give information on user and server
void	IRCServer::whoisCmd(User* user, Message& msg)
{
	std::string reply;
	if (!msg.m_args.size())
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	reply = buildReply(user, RPL_WHOISUSER, user->m_user + " * :" + user->m_real);
	writeToClient(user, m_name, reply);
	reply = buildReply(user, RPL_WHOISERVER, m_name + " * " + ":This server is our own IRC Server");
	writeToClient(user, m_name, reply);
	writeToClient(user, m_name, buildReply(user, RPL_ENDOFWHOIS));
}

// [JOIN] Make user join/create a channel
void	IRCServer::joinCmd(User* user, Message& msg)
{
	// Checking if command is "JOIN 0"
	if (!msg.m_args.size())
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	if (msg.m_args[0] == "0") {
		// Removing user from all channels
		mapChannel::const_iterator	it = user->m_allChan.begin();
		for (; it != user->m_allChan.end(); it++) {
			it->second->removeUser(user->m_nick);
			it->second->removeOps(user->m_nick);
			user->m_allChan.erase(it->first);
			user->m_opsChan.erase(it->first);
			writeToClient(user, user->getPrefix(), "PART " + it->first + CRLF);
			if (msg.m_args.size() > 1)
				writeToRelations(user, "PART :" + msg.m_args[1] + CRLF);
			else
				writeToRelations(user, "PART\r\n");

			// Check if users are remaining
			if (m_mapChan[it->first]->m_users.empty())
				removeChannel(it->first);
		}
		return ;
	}
	
	// Getting channels and passwords
	vecStr 	chans = parseMsgArgs(msg.m_args[0]);
	vecStr	pwds;
	if (msg.m_args.size() > 1)
		pwds = parseMsgArgs(msg.m_args[1]);
	
	// Creating new channels or Joining existing channels
	std::string reply;
	for (size_t i = 0; i < chans.size(); i++) {
		mapChannel::const_iterator it = m_mapChan.find(chans[i]);
		if (it == m_mapChan.end()) // channel does not exist
		{
			if (pwds.size() >= i + 1)
				addChannel(chans[i], pwds[i], user);
			else
				addChannel(chans[i], user);
			m_mapChan[chans[i]]->addOps(user);
			m_mapChan[chans[i]]->addUser(user);
			user->m_allChan[chans[i]] = m_mapChan[chans[i]];
			user->m_opsChan[chans[i]] = m_mapChan[chans[i]];
			writeToClient(user, user->getPrefix(), "JOIN :" + chans[i] + CRLF);
		}
		else // channel already exists
		{
			Channel *channel = it->second;
			if (pwds.size() < i + 1 && !channel->m_pwd.empty())
				throw CmdError(ERR_BADCHANNELKEY, user, channel->m_name);
			if ((pwds.size() >= i + 1) && pwds[i] != channel->m_pwd)
				throw CmdError(ERR_BADCHANNELKEY, user, channel->m_name);
			if (channel->m_invitMode && !channel->checkInvit(user->m_nick))
				throw CmdError(ERR_INVITEONLYCHAN, user, channel->m_name);
			if (channel->m_maxUsers == static_cast<int>(channel->m_users.size()))
				throw CmdError(ERR_CHANNELISFULL, user, channel->m_name);
			channel->addUser(user);
			user->m_allChan[chans[i]] = channel;
			writeToChannel(user, channel, true, "JOIN :" + channel->m_name + CRLF);
		}
		
		// topic cmd
		Message namesMsg("", "NAMES", chans[i]);
		namesCmd(user, namesMsg);
	}
}

void	IRCServer::namesCmd(User* user, Message& msg)
{
	// Getting channels names
	if (!msg.m_args.size())
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	vecStr 	chans = parseMsgArgs(msg.m_args[0]);

	// List users
	for (size_t i = 0; i < chans.size(); i++)
	{
		std::string userlist = ":";
		Channel *channel;
		vecUser users;
		if (m_mapChan.find(chans[i]) != m_mapChan.end()) {
			channel = m_mapChan[chans[i]];
			users = m_mapChan[chans[i]]->m_users;
			for (size_t i = 0; i < users.size(); i++) {
				if (channel->checkOps(users[i]->m_nick))
					userlist += "@";
				userlist += users[i]->m_nick;
				if (i < users.size() - 1)
					userlist += " ";
			}
			writeToClient(user, m_name, buildReply(user, RPL_NAMREPLY, "= " + chans[i] + " " + userlist));
			writeToClient(user, m_name, buildReply(user, RPL_ENDOFNAMES, chans[i]));
		}
	}
}

// [PART] Make user leave a channel
void	IRCServer::partCmd(User* user, Message& msg)
{
	// Getting channels
	if (!msg.m_args.size())
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	vecStr 	chans = parseMsgArgs(msg.m_args[0]);

	// Removing user from channels
	Channel		*channel;
	std::string reply;
	for (size_t i = 0; i < chans.size(); i++)
	{
		if (m_mapChan.find(chans[i]) == m_mapChan.end())
			throw CmdError(ERR_NOSUCHCHANNEL, user, chans[i]);
		if (user->m_allChan.find(chans[i]) == user->m_allChan.end())
			throw CmdError(ERR_NOTONCHANNEL, user, chans[i]);
		channel = m_mapChan[chans[i]];
		if (msg.m_args.size() > 1)
			reply = "PART " + chans[i] + " :" + msg.m_args[1] + CRLF;
		else
			reply = "PART " + chans[i] + CRLF;
		writeToChannel(user, channel, true, reply);
		channel->removeUser(user->m_nick);
		channel->removeOps(user->m_nick);
		user->m_allChan.erase(chans[i]);
		user->m_opsChan.erase(chans[i]);

		// Check if users are remaining
		if (m_mapChan[chans[i]]->m_users.empty())
			removeChannel(chans[i]);
	}
}

// [QUIT] Delete a user from IRC server
void	IRCServer::quitCmd(User* user, Message& msg)
{
	(void)msg;
	if (msg.m_args.size() > 1)
		writeToRelations(user, "QUIT :Quit: " + msg.m_args[1] + CRLF);
	else
		writeToRelations(user, "QUIT :Quit: leaving\r\n");
	std::string nick = user->m_nick;
	closeConnection(user->m_socket);
	removeUser(nick);
	throw IRCServer::UserRemoved();
}

void	IRCServer::privmsgCmd(User *user, Message& msg)
{
	// Parsing errors
	if (!msg.m_args.size())
		throw CmdError(ERR_NORECIPIENT, user);
	if (msg.m_args.size() == 1)
		throw CmdError(ERR_NOTEXTTOSEND, user);

	// Getting recipients
	vecStr		recip = parseMsgArgs(msg.m_args[0]);
	std::string reply;

	// Writing message in recipients' buffers
	for (size_t i = 0; i < recip.size(); i++)
	{
		if (recip[i][0] == '#') {
			if (m_mapChan.find(recip[i]) == m_mapChan.end())
				throw CmdError(ERR_NOSUCHCHANNEL, user, recip[i]);
			reply = "PRIVMSG " + m_mapChan[recip[i]]->m_name + " :" + msg.m_args[1] + CRLF;
			writeToChannel(user, m_mapChan[recip[i]], false, reply);
		}
		else {
			if (m_mapUser.find(recip[i]) == m_mapUser.end())
				throw CmdError(ERR_NOSUCHNICK, user, recip[i]);
			User *recipUser = m_mapUser[recip[i]];
			reply = "PRIVMSG " + recipUser->m_nick + " :" + msg.m_args[1] + CRLF;
			writeToClient(recipUser, user->getPrefix(), reply);
		}
	}
}


/******************** OPERATOR COMMANDS ******************/


// [KICK] Kicking user(s) from channel(s)
void 	IRCServer::kickCmd(User* user, Message& msg)
{
	// Getting parameters
	if (msg.m_args.size() < 2)
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	vecStr 	chans = parseMsgArgs(msg.m_args[0]);
	vecStr 	users = parseMsgArgs(msg.m_args[1]);

	// Kick each user from each channel
	Channel		*channel;
	User		*target;
	std::string	reply;
	for (size_t i = 0; i < chans.size(); i++) {
		// Check if user can kick
		if (m_mapChan.find(chans[i]) == m_mapChan.end())
			throw CmdError(ERR_NOSUCHCHANNEL, user, chans[i]);
		channel = m_mapChan[chans[i]];
		if (!channel->checkUser(user->m_nick))
			throw CmdError(ERR_NOTONCHANNEL, user, chans[i]);
		if (!channel->checkOps(user->m_nick))
			throw CmdError(ERR_CHANOPRIVSNEEDED, user, chans[i]);
		for (size_t j = 0; j < users.size(); j++) {
			if (m_mapUser.find(users[j]) == m_mapUser.end() || !(channel->checkUser(users[j])))
				throw CmdError(ERR_USERNOTINCHANNEL, user, users[j] + " " + chans[i]);
			// Kicking user and sending reply
			reply = "KICK " + chans[i] + " " + users[j] + " :" + msg.m_args[2] + CRLF;
			writeToChannel(user, channel, true, reply);
			target = m_mapUser[users[j]];
			channel->removeUser(users[j]);
			channel->removeOps(users[j]);
			target->m_allChan.erase(chans[i]);
			target->m_opsChan.erase(chans[i]);
		}
		// Check if users are remaining
		if (channel->m_users.empty())
			removeChannel(chans[i]);
	}
}

void 	IRCServer::inviteCmd(User* user, Message& msg)
{
	// Getting parameters
	if (msg.m_args.size() < 2)
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	if (m_mapUser.find(msg.m_args[0]) == m_mapUser.end())
		throw CmdError(ERR_NOSUCHNICK, user, msg.m_args[0]);
	if (m_mapChan.find(msg.m_args[1]) == m_mapChan.end())
		throw CmdError(ERR_NOSUCHCHANNEL, user, msg.m_args[1]); // nick ?
	User	*target = m_mapUser[msg.m_args[0]];
	Channel *channel = m_mapChan[msg.m_args[1]];
	if (channel->checkUser(target->m_nick))
		throw CmdError(ERR_USERONCHANNEL, user, target->m_nick + " " + channel->m_name);
	if (!(channel->checkOps(user->m_nick)))
		throw CmdError(ERR_CHANOPRIVSNEEDED, user, channel->m_name);

	// Add to invit list
	if (!channel->checkInvit(target->m_nick))
		channel->addInvit(target);

	// Send confirmation to user, target and operators
	writeToClient(user, m_name, buildReply(user, RPL_INVITING, target->m_nick + " " + channel->m_name));
	writeToClient(target, user->getPrefix(), "INVITE " + target->m_nick + " " + channel->m_name + CRLF);
	std::string opsNotice = " :" + user->m_nick + " invited " + target->m_nick + " into channel " + channel->m_name + CRLF;
	writeToOps(user, channel, "NOTICE @" + channel->m_name + opsNotice);
}


	// void 	topicCmd(User* user, Message& msg);
	// void 	modeCmd(User* user, Message& msg);
