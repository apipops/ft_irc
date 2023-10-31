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
	
	// EXECUTION
	void	checkCommands();
	void	executeCommand(User *user, std::string cmd);
	void	writeWelcome(User *user, std::string nick);
	void	writeReply(User *user, int prefix, const std::string reply);

	// SERVER SETTER (users, channels, memory)
	void	addUser(ASocket* socket);
	void	addChannel(std::string name, User *user);
	void	addChannel(std::string name, std::string pwd, User *user);
	void	removeUser(std::string nick);
	void	removeChannel(std::string name);
	void	freeMemory(void);

	// UTILS
	static std::string buildReply(User *user, std::string what);
	static std::string buildReply(User *user, std::string what, std::string arg);
	static std::string buildReply(User *user, std::string what, std::string arg1, std::string arg2);

	// UTILS FOR TESTING
	void		fonctionTest();
	Channel &	getChannel(std::string name) const;
	mapChannel	getChannels() const;
	mapUser		getUsers() const;
	void		showMapUsers() const;
	void		showMapChannels() const;
	void		showVecUsers() const;
	void		showvecChans() const;
	void		showChannelsOfUser(std::string nick) const;
	void		showUsersOfChannel(std::string channel) const;

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
	void checkNickDup(std::string nick, User *user);
	void checkNickFormat(std::string type, User *user);
	void checkChanFormat(std::string name, User *user);
	void checkTopicFormat(std::string topic, User *user);
	void checkPwdFormat(std::string pwd, User *user);

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
	class CmdError : public std::exception {
		public:
			CmdError(std::string what, User *user);
			CmdError(std::string what, User *user, std::string arg);
			~CmdError() _NOEXCEPT;

			const char *what() const _NOEXCEPT;
		private:
			std::string m_what;
	};

};

#endif