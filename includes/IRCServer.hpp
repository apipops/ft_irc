#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

# include "Channel.hpp"
# include "User.hpp"
# include "../raph/TCP_IPv4"
# include "params.hpp"

using namespace TCP_IPv4;

class IRCServer 
{
public:
	// Constructors and destructors
	IRCServer();
	IRCServer(const IRCServer & src);
	IRCServer & operator=(const IRCServer & src);
	virtual ~IRCServer();

	// Users & Channels
	void addUser(std::string nick, std::string user, ASocket* socket);
	void addChannel(std::string name, std::string topic);
	void addChannel(std::string name, std::string topic, std::string pwd);

	// Utils for test
	void fonctionTest();
	Channel & getChannel(std::string name) const;
	mapChannel getChannels() const;
	mapUser getUsers() const;
	void showMapUsers() const;
	void showMapChannels() const;
	void showVecUsers() const;
	void showVecChannels() const;

private:
	// Vectors for user and channel storage
	std::vector<User>		m_users;
	std::vector<Channel>	m_channels;

	// Maps for easy find channel/user
	mapChannel	m_mapChan;
	mapUser 	m_mapUser;

	// Parsing
	void checkUserDup(std::string nick);
	void checkChanDup(std::string channel);
	void checkUserFormat(std::string type, std::string name);
	void checkChanFormat(std::string name);
	void checkTopicFormat(std::string topic);
	void checkPwdFormat(std::string pwd);

	// Basic commands
	void	nickCmd(User * user, std::string newNick);
	void	userCmd(std::string newUser);
	Channel createCmd(std::string name, std::string topic); // sans pwd
	Channel createCmd(std::string name, std::string topic, std::string pwd); // avec pwd
	void	joinCmd(Channel & channel); // sans pwd
	void	joinCmd(Channel & channel, std::string pwd); // avec pwd
	void	partCmd(Channel & channel);

	// Operator commands
	void 	kickCmd();
	void 	inviteCmd();
	void 	topicCmd();
	void 	modeCmd();

	// Exception
	class CmdError : public Error {
		public:
			CmdError(std::string what);
	};

};

#endif