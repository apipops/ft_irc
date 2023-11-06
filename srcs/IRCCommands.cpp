#include "../includes/IRCServer.hpp"

/********************** BASIC USER COMMANDS **********************/

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
			if (pwds.size() < i + 1 && !channel->m_pwd.empty()) {
				writeToClient(user, m_name, buildReply(user, ERR_BADCHANNELKEY, channel->m_name));
				continue;
			}
			if ((pwds.size() >= i + 1) && pwds[i] != channel->m_pwd) {
				writeToClient(user, m_name, buildReply(user, ERR_BADCHANNELKEY, channel->m_name));
				continue;
			}
			if (channel->m_invitMode && !channel->checkInvit(user->m_nick)) {
				writeToClient(user, m_name, buildReply(user, ERR_INVITEONLYCHAN, channel->m_name));
				continue;
			}
			if (channel->m_maxUsers >= static_cast<int>(channel->m_users.size())) {
				writeToClient(user, m_name, buildReply(user, ERR_CHANNELISFULL, channel->m_name));
				continue;
			}
			channel->addUser(user);
			user->m_allChan[chans[i]] = channel;
			writeToChannel(user, channel, true, "JOIN :" + channel->m_name + CRLF);
		}
		Message cmdMsg("", "CMD", chans[i]);
		if (!m_mapChan[chans[i]]->m_topic.empty()) {
			std::cout << "je passe ici" << std::endl;
			topicCmd(user, cmdMsg);
		}
		namesCmd(user, cmdMsg);
	}
}

// [NAMES] List the users of a channel
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
		if (m_mapChan.find(chans[i]) == m_mapChan.end()) {
			writeToClient(user, m_name, buildReply(user, ERR_NOSUCHCHANNEL, chans[i]));
			continue;
		}
		if (user->m_allChan.find(chans[i]) == user->m_allChan.end()) {
			writeToClient(user, m_name, buildReply(user, ERR_NOTONCHANNEL, chans[i]));
			continue;
		}
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

// [PRIVMSG] Send a message to channels or users
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
			if (m_mapChan.find(recip[i]) == m_mapChan.end()){
				writeToClient(user, m_name, buildReply(user, ERR_NOSUCHCHANNEL, recip[i]));
				continue;
			}
			reply = "PRIVMSG " + recip[i] + " :" + msg.m_args[1] + CRLF;
			writeToChannel(user, m_mapChan[recip[i]], false, reply);
		}
		else {
			if (m_mapUser.find(recip[i]) == m_mapUser.end()){
				writeToClient(user, m_name, buildReply(user, ERR_NOSUCHNICK, recip[i]));
				continue;
			}
			User *recipUser = m_mapUser[recip[i]];
			reply = "PRIVMSG " + recip[i] + " :" + msg.m_args[1] + CRLF;
			writeToClient(recipUser, user->getPrefix(), reply);
		}
	}
}

// [NOTICE] Send a message to channels or users - without checking errors
void	IRCServer::noticeCmd(User* user, Message& msg)
{
	// Parsing errors
	if (msg.m_args.size() < 2)
		return;

	// Getting recipients
	vecStr		recip = parseMsgArgs(msg.m_args[0]);
	std::string reply;

	// Writing message in recipients' buffers
	for (size_t i = 0; i < recip.size(); i++)
	{
		if (recip[i][0] == '#') {
			if (m_mapChan.find(recip[i]) == m_mapChan.end())
				continue;
			reply = "NOTICE " + recip[i] + " :" + msg.m_args[1] + CRLF;
			writeToChannel(user, m_mapChan[recip[i]], false, reply);
		}
		else {
			if (m_mapUser.find(recip[i]) == m_mapUser.end())
				continue;
			User *recipUser = m_mapUser[recip[i]];
			reply = "NOTICE " + recip[i] + " :" + msg.m_args[1] + CRLF;
			writeToClient(recipUser, user->getPrefix(), reply);
		}
	}
}

// [TOPIC] Show or modify the topic of a channel
void	IRCServer::topicCmd(User* user, Message& msg)
{
	// Parsing arguments
	if (!msg.m_args.size())
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	if (m_mapChan.find(msg.m_args[0]) == m_mapChan.end())
		throw CmdError(ERR_NOSUCHCHANNEL, user, msg.m_args[0]);
	Channel *channel = m_mapChan[msg.m_args[0]];

	// Showing the topic or Modifying the topic
	if (msg.m_args.size() == 1) {
		if (channel->m_topic.empty())
			writeToClient(user, m_name, buildReply(user, RPL_NOTOPIC, channel->m_name));
		else {
			writeToClient(user, m_name, buildReply(user, RPL_TOPIC, channel->m_name + " :" + channel->m_topic));
			writeToClient(user, m_name, buildReply(user, RPL_TOPICWHOTIME, channel->m_modifInfo));
		}
	}
	else {
		if (!(channel->checkUser(user->m_nick)))
			throw CmdError(ERR_NOTONCHANNEL, user, channel->m_name);
		if (channel->m_topicRestrict && !(channel->checkOps(user->m_nick)))
			throw CmdError(ERR_CHANOPRIVSNEEDED, user, channel->m_name);
		channel->m_topic = msg.m_args[1];
		channel->setModifInfo(user);
		writeToChannel(user, channel, 1, "TOPIC " + channel->m_name + " :" +  msg.m_args[1] + CRLF);
	}
}


/******************** OPERATOR COMMANDS ******************/

// [OPER] Identifying as the operator of the server
void	IRCServer::operCmd(User *user, Message &msg)
{
	if (msg.m_args.size() < 2)
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	if (msg.m_args[0] != user->m_nick)
		return;
	if (msg.m_args[1] != OPER_PWD)
		throw CmdError(ERR_PASSWDMISMATCH, user);
	user->m_servOps = true;
	writeToClient(user, m_name, "MODE " + user->m_nick + " " + user->getModeStr() + CRLF);
	writeToClient(user, m_name, buildReply(user, RPL_YOUREOPER));
}

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
		if (m_mapChan.find(chans[i]) == m_mapChan.end()) {
			writeToClient(user, m_name, buildReply(user, ERR_NOSUCHCHANNEL, chans[i]));
			continue;
		}
		channel = m_mapChan[chans[i]];
		if (!channel->checkUser(user->m_nick)){
			writeToClient(user, m_name, buildReply(user, ERR_NOTONCHANNEL, chans[i]));
			continue;
		}
		if (!user->m_servOps && !channel->checkOps(user->m_nick)) {
			writeToClient(user, m_name, buildReply(user, ERR_CHANOPRIVSNEEDED, chans[i]));
			continue;
		}
		for (size_t j = 0; j < users.size(); j++) {
			if (m_mapUser.find(users[j]) == m_mapUser.end() || !(channel->checkUser(users[j]))) {
				writeToClient(user, m_name, buildReply(user, ERR_USERNOTINCHANNEL, users[j] + " " + chans[i]));
				continue;
			}
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

// [INVITE] Invite user to invitation list
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
	if (!user->m_servOps && !(channel->checkOps(user->m_nick)))
		throw CmdError(ERR_CHANOPRIVSNEEDED, user, channel->m_name);

	// Add to invit list
	if (!channel->checkInvit(target->m_nick))
		channel->addInvit(target);

	// Send confirmation to user, target and operators
	writeToClient(user, m_name, buildReply(user, RPL_INVITING, target->m_nick + " " + channel->m_name));
	writeToClient(target, user->getPrefix(), "INVITE " + target->m_nick + " " + channel->m_name + CRLF);
	std::string opsNotice = " :" + user->m_nick + " invited " + target->m_nick + " into channel " + channel->m_name + CRLF;
	writeToOps(channel, "NOTICE @" + channel->m_name + opsNotice);
}

// [MODE] Change mode of the channel - supported modes: itkol
void	IRCServer::modeCmd(User* user, Message& msg)
{
	// Parsing arguments
	if (!msg.m_args.size())
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	if (msg.m_args[0] == user->m_nick && msg.m_args.size() == 1)
		throw CmdError(RPL_UMODEIS, user, user->getModeStr());
	if (msg.m_args[0] == user->m_nick && msg.m_args.size() > 1)
		return;
	if (m_mapUser.find(msg.m_args[0]) != m_mapUser.end())
		throw CmdError(ERR_USERSDONTMATCH, user);
	if (m_mapChan.find(msg.m_args[0]) == m_mapChan.end())
		throw CmdError(ERR_NOSUCHCHANNEL, user, msg.m_args[0]);
	Channel *channel = m_mapChan[msg.m_args[0]];

	// Showing the mode or Modifying the mode
	if (msg.m_args.size() == 1) {
		writeToClient(user, m_name, buildReply(user, RPL_CHANNELMODEIS, channel->getModeStr()));
		writeToClient(user, m_name, buildReply(user, RPL_CREATIONTIME, channel->m_createInfo));
	}
	else {
		if (!user->m_servOps && !(channel->checkOps(user->m_nick)))
			throw CmdError(ERR_CHANOPRIVSNEEDED, user, channel->m_name);
		std::string flags = msg.m_args[1], reply, reply_params;
		bool		i = 0, t = 0, k = 0, o = 0, l = 0;
		int			add = 0, sub = 0;
		for (size_t c = 0; flags[c]; c++) {
			if (flags[c] == '+')
				add = sub + 1;
			else if (flags[c] == '-')
				sub = add + 1;
			else if (flags[c] == 'i') {
				if (add > sub && !i) {
					channel->m_invitMode = true;
					(reply.find('+') != std::string::npos) ? reply += "i" : reply += "+i";
				}
				else if (sub > add && !i) {
					channel->m_invitMode = false;
					i = true;
					(reply.find('-') != std::string::npos) ? reply += "i" : reply += "-i";
				}
				i = true;
			}
			else if (flags[c] == 't') {
				if (add > sub && !t) {
					channel->m_topicRestrict = true;
					(reply.find('+') != std::string::npos) ? reply += "t" : reply += "+t";
				}
				else if (sub > add && !t) {
					channel->m_topicRestrict = false;
					(reply.find('-') != std::string::npos) ? reply += "t" : reply += "-t";
				}	
				t = true;
			}
			else if (flags[c] == 'o') {
				if (msg.m_args.size() < 3)
					continue;
				else if (m_mapUser.find(msg.m_args[2]) == m_mapUser.end()) {
					writeToClient(user, m_name, buildReply(user, ERR_USERNOTINCHANNEL, msg.m_args[2] + " " + channel->m_name));
					continue;				
				}
				User *otherUser = m_mapUser[msg.m_args[2]];
				if (add > sub && !o && !k && !l) {
					channel->addOps(otherUser);
					otherUser->m_opsChan[channel->m_name] = channel;
					(reply.find('+') != std::string::npos) ? reply += "o" : reply += "-o";
					reply_params = " " + msg.m_args[2];
				}
				else if (sub > add && !o) {
					channel->removeOps(user->m_nick);
					user->m_opsChan.erase(channel->m_name);
					(reply.find('-') != std::string::npos) ? reply += "o" : reply += "-o";
					reply_params += " " + msg.m_args[2];
				}
				o = true;
			}
			else if (flags[c] == 'k') {
				if (add > sub && !o && !k && !l && msg.m_args.size() > 2) {
					channel->m_pwd = msg.m_args[2];
					reply_params = " " + msg.m_args[2];
					(reply.find('+') != std::string::npos) ? reply += "k" : reply += "+k";
				}
				else if (sub > add && !k && !channel->m_pwd.empty()) { // ne s'applique pas si pas de mdp
					reply_params += " " + channel->m_pwd;
					channel->m_pwd.clear();
					(reply.find('-') != std::string::npos) ? reply += "k" : reply += "-k";
				}
				k = true;
			}
			else if (flags[c] == 'l') {
				if (add > sub && !o && !k && !l) {
					if (msg.m_args.size() < 3) { // propre au flag l
						writeToClient(user, m_name, buildReply(user, ERR_NEEDMOREPARAMS, "MODE +l"));
					}
					else if (std::atoi(msg.m_args[2].c_str()) > 0) {
						channel->m_maxUsers = std::atoi(msg.m_args[2].c_str());
						(reply.find('+') != std::string::npos) ? reply += "l" : reply += "+l";
						reply_params = msg.m_args[2];
					}
				}
				else if (sub > add && !l) {
					std::stringstream maxStr;
					maxStr << channel->m_maxUsers;
					reply_params += " " + maxStr.str();
					channel->m_maxUsers = NONE;
					(reply.find('-') != std::string::npos) ? reply += "l" : reply += "-l";
				}
				l = true;
			}
			else if (flags[c] != 'b')
				writeToClient(user, m_name, buildReply(user, ERR_UNKNOWNMODE, flags.substr(c, 1)));
		}
		if (!reply.empty())
			writeToChannel(user, channel, true, "MODE " + channel->m_name + " " + reply + reply_params + CRLF);
	}
}


/********************** CLIENT COMMANDS **********************/

// [PING] Notify the client the server is up
void	IRCServer::pingCmd(User* user, Message& msg)
{
	(void)msg;
	std::string reply = "PONG " + m_name + " :" + user->m_nick + CRLF;
	writeToClient(user, m_name, reply);
}

// [WHOIS] Give information on user or server
void	IRCServer::whoisCmd(User* user, Message& msg)
{
	if (!msg.m_args.size())
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	std::string reply;
	if (msg.m_args[1] == user->m_nick) {
		reply = buildReply(user, RPL_WHOISUSER, user->m_user + " * :" + user->m_real);
		writeToClient(user, m_name, reply);
		writeToClient(user, m_name, buildReply(user, RPL_ENDOFWHOIS));
	}
	if (msg.m_args[1] == m_name) {
		reply = buildReply(user, RPL_WHOISERVER, m_name + " * " + ":This server is our own IRC Server");
		writeToClient(user, m_name, reply);
		writeToClient(user, m_name, buildReply(user, RPL_ENDOFWHOIS));
	}
}

// [WHO] Give information on a channel
// syntax: <channel> <username> <host> <server> <nick> <flags> :<hopcount> <realname>
void	IRCServer::whoCmd(User *user, Message& msg)
{
	if (!msg.m_args.size())
		throw CmdError(ERR_NEEDMOREPARAMS, user);
	std::string reply;
	if (user->m_allChan.find(msg.m_args[0]) != user->m_allChan.end()) {
		Channel *channel = user->m_allChan[msg.m_args[0]];
		reply = channel->m_name + " ";
		reply += user->m_user + " ";
		reply += user->m_socket->host() + " ";
		reply += m_name + " ";
		reply += user->m_nick + " ";
		if (channel->checkOps(user->m_nick))
			reply += "H@ ";
		else
			reply += "H ";
		reply += ":0 " + user->m_real;
		reply = buildReply(user, RPL_WHOREPLY, reply);
		writeToClient(user, m_name, reply);
		writeToClient(user, m_name, buildReply(user, RPL_ENDOFWHO, channel->m_name));
	}
}
