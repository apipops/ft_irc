#include "../includes/IRCServer.hpp"

/********************** BASIC COMMANDS **********************/

// Add/replace nickname of user
void	IRCServer::nickCmd(User* user, Message &msg)
{
	// Parsing - throw exception
	if (!msg.m_args.size())
		throw CmdError(ERR_NONICKNAMEGIVEN);
	checkNickFormat(msg.m_args[0]);
	checkNickDup(msg.m_args[0]);

	// Suppress from map, update in vector and add again in map
	if (user->m_nick.empty())
		sendWelcome(user, msg.m_args[0]);
	m_mapUser.erase(user->m_nick);
	user->m_nick = msg.m_args[0];
	m_mapUser[msg.m_args[0]] = user;
}


// Add/replace username and realname of user
void	IRCServer::userCmd(User* user, Message &msg)
{
	if (msg.m_args.size() != 4)
		throw CmdError(ERR_NEEDMOREPARAMS);
	user->m_user = msg.m_args[0];
	user->m_real = msg.m_args[3];
}

// Answer 'PING' command
void	IRCServer::pingCmd(User* user, Message &msg)
{
	(void)msg;
	sendReply(user, "PONG " + m_name);
}

// Answer 'WHOIS' command
void	IRCServer::whoisCmd(User* user, Message &msg)
{
	std::string reply;
	if (!msg.m_args.size())
		throw CmdError(ERR_NEEDMOREPARAMS);
	else if (msg.m_args[0] == user->m_nick)
	{
		reply = static_cast<std::string>(RPL_WHOISUSER) + " " + user->m_nick + " " + user->m_user + " * " + ":" + user->m_real;
		
	}
	else if (msg.m_args[0] == m_name)
	{
		reply = static_cast<std::string>(RPL_WHOISERVER) + " " + user->m_nick + " " + m_name + " * " + ":This server is our own IRC server";
		sendReply(user, reply);
	}
}

// Make user join/create a channel
void	IRCServer::joinCmd(User* user, Message &msg)
{
	if (!msg.m_args.size())
		throw CmdError(ERR_NEEDMOREPARAMS);

	// Getting channels and passwords
	vecStr 				chans, pwds;
	std::string 		chan_buf, pwd_buf;
	std::stringstream 	chan_stream(msg.m_args[0]), pwd_stream;
	while (std::getline(chan_stream, chan_buf, ','))
		chans.push_back(chan_buf);
	if (msg.m_args.size() > 1) {
		pwd_stream << msg.m_args[1];
		while (std::getline(pwd_stream, pwd_buf, ','))
			pwds.push_back(pwd_buf);
	}
	
	// Creating new channels or Joining existing channels
	for (size_t i = 0; i < chans.size(); i++) {
		mapChannel::const_iterator it = m_mapChan.find(chans[i]);
		if (it == m_mapChan.end()) {
			if (pwds.size() >= i + 1)
				addChannel(chans[i], pwds[i]);
			else
				addChannel(chans[i]);
			m_mapChan[chans[i]]->addOps(user);
			m_mapChan[chans[i]]->addUser(user);
			user->m_allChan[chans[i]] = m_mapChan[chans[i]];
			user->m_opsChan[chans[i]] = m_mapChan[chans[i]];
		}
		else {
			Channel *channel = it->second;
			if (pwds.size() < i + 1 && !channel->m_pwd.empty())
				throw CmdError(ERR_BADCHANNELKEY, channel->m_name);
			if ((pwds.size() >= i + 1) && pwds[i] != channel->m_pwd)
				throw CmdError(ERR_BADCHANNELKEY, channel->m_name);
			if (channel->m_invitMode && !channel->checkInvit(user->m_nick))
				throw CmdError(ERR_INVITEONLYCHAN, channel->m_name);
			if (channel->m_maxUsers == static_cast<int>(channel->m_users.size()))
				throw CmdError(ERR_CHANNELISFULL, channel->m_name);
			channel->addUser(user);
			user->m_allChan[chans[i]] = channel;
		}
	}
}

// Make user leave a channel
void	IRCServer::partCmd(User* user, Message &msg)
{
	if (!msg.m_args.size())
		throw CmdError(ERR_NEEDMOREPARAMS);

	// Getting channels
	vecStr 				chans;
	std::string 		chan_buf;
	std::stringstream 	chan_stream(msg.m_args[0]);
	while (std::getline(chan_stream, chan_buf, ','))
		chans.push_back(chan_buf);

	// Removing user from channels
	for (size_t i = 0; i < chans.size(); i++)
	{
		if (m_mapChan.find(chans[i]) == m_mapChan.end())
			throw CmdError(ERR_NOSUCHCHANNEL, chans[i]);
		if (user->m_allChan.find(chan_buf) == user->m_allChan.end())
			throw CmdError(ERR_NOTONCHANNEL, chans[i]);
		m_mapChan[chans[i]]->removeUser(user->m_nick);
		m_mapChan[chans[i]]->removeOps(user->m_nick);
		user->m_allChan.erase(chans[i]);
		user->m_opsChan.erase(chans[i]);

		// Check if users are remaining
		if (m_mapChan[chans[i]]->m_users.empty())
			removeChannel(chans[i]);
	}
}

// Delete a user from IRC server
void	IRCServer::quitCmd(User* user, Message &msg)
{
	// (void)msg;
	// std::string nick = user->m_nick;
	// removeUser(nick);
}

/******************** OPERATOR COMMANDS ******************/

	// void 	kickCmd();
	// void 	inviteCmd();
	// void 	topicCmd();
	// void 	modeCmd();
