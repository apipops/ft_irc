#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

# include "Channel.hpp"
# include "User.hpp"
# include "../raph/TCP_IPv4"
# include "params.hpp"

using namespace TCP_IPv4;

class IRCServer: public TCP_IPv4::Server
{
public:
	// CONSTRUCTORS AND DESTRUCTORS
	IRCServer(std::string name);
	IRCServer(const IRCServer & src);
	IRCServer & operator=(const IRCServer & src);
	virtual ~IRCServer();
	
	// SERVER SETTER (users, channels, memory)
	void addUser(std::string nick, std::string user, ASocket* socket);
	void addChannel(std::string name, std::string topic);
	void addChannel(std::string name, std::string topic, std::string pwd);
	void removeUser(std::string nick);
	void removeChannel(std::string name);
	void freeMemory(void);

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
	// VECTOR FOR STORAGE (users, channels)
	vecUser		m_users;
	vecChan	m_channels;

	// MAPS FOR EASY FIND (users, channels)
	mapChannel	m_mapChan;
	mapUser 	m_mapUser;

	// PARSING
	void checkUserDup(std::string nick);
	void checkChanDup(std::string channel);
	void checkUserFormat(std::string type, std::string name);
	void checkChanFormat(std::string name);
	void checkTopicFormat(std::string topic);
	void checkPwdFormat(std::string pwd);

	// BASIC COMMANDS
	void	nickCmd(User* user, std::string newNick);
	void	userCmd(User* user, std::string newUser);
	void	joinCmd(User* user, std::string name);
	void	joinCmd(User* user, std::string name, std::string pwd);
	void	partCmd(User* user, std::string name);

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