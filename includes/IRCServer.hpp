#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

# include "Channel.hpp"
# include "User.hpp"
# include "Message.hpp"
# include "../raph/TCP_IPv4"
# include "params.hpp"

using namespace TCP_IPv4;

class IRCServer: public TCP_IPv4::Server
{
public:
	// CONSTRUCTORS AND DESTRUCTORS
	IRCServer(std::string name);
	IRCServer(std::string name, std::string pwd);
	IRCServer(const IRCServer & src);
	IRCServer & operator=(const IRCServer & src);
	virtual ~IRCServer();
	
	// LAUNCHING
	void checkCommands();
	void sendWelcome(User *user, std::string nick);
	void executeCommand(User *user, std::string cmd);

	// SERVER SETTER (users, channels, memory)
	void addUser(ASocket* socket);
	void addUser(std::string nick, std::string user, ASocket* socket);
	void addChannel(std::string name);
	void addChannel(std::string name, std::string pwd);
	void removeUser(std::string nick);
	void removeChannel(std::string name);
	void freeMemory(void);
	void sendReply(User *user, std::string reply);

	// UTILS FOR TESTING
	void fonctionTest();
	Channel & getChannel(std::string name) const;
	mapChannel getChannels() const;
	mapUser getUsers() const;
	void showMapUsers() const;
	void showMapChannels() const;
	void showVecUsers() const;
	void showvecChans() const;
	void showChannelsOfUser(std::string nick) const;
	void showUsersOfChannel(std::string channel) const;

private:
	// PASSWORD
	std::string m_pwd;

	// VECTOR FOR STORAGE (users, channels)
	vecUser		m_users;
	vecChan		m_channels;

	// MAPS FOR EASY FIND (users, channels)
	mapChannel	m_mapChan;
	mapUser 	m_mapUser;

	// PARSING
	void checkNickDup(std::string nick);
	void checkNickFormat(std::string type);
	void checkChanFormat(std::string name);
	void checkTopicFormat(std::string topic);
	void checkPwdFormat(std::string pwd);

	// BASIC COMMANDS
	void	nickCmd(User* user, Message &msg);
	void	userCmd(User* user, Message &msg);
	void	pingCmd(User* user, Message &msg);
	void	whoisCmd(User* user, Message &msg);
	void	joinCmd(User* user, Message &msg);
	void	partCmd(User* user, Message &msg);
	void	quitCmd(User* user, Message &msg);

	// OPERATOR COMMANDS
	void 	kickCmd();
	void 	inviteCmd();
	void 	topicCmd();
	void 	modeCmd();

	// EXCEPTION
	class CmdError : public Error {
		public:
			CmdError(std::string what);
	};

};

#endif