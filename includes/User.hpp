/**
 * Class for users.
 * Users are IRC clients who connected to
 * the IRC server through a specific socket.
 * Identified by username and nickname. 
*/

#ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <string>
# include <map>
# include <algorithm>
# include <exception>
# include "Channel.hpp"
# include "params.hpp"
# include "../raph/TCP_IPv4"

using namespace TCP_IPv4;
class Channel;
typedef std::map<std::string, Channel> mapChannel;

class User
{
public: 
	// Constructors and destructors
	User();
	User(std::string nick, std::string user, ASocket *socket);
	User(const User & src);
	User & operator=(const User & src);
	~User();

	friend class Channel;

	// Parsing
	static void	checkFormat(std::string type, std::string name);

	// Getters and setters
	const std::string & getNick() const;

	// Basic commands
	void	nickCmd(std::string newNick);
	void	userCmd(std::string newUser);
	Channel createCmd(std::string name, std::string topic); // sans pwd
	Channel createCmd(std::string name, std::string topic, std::string pwd); // avec pwd
	void	joinCmd(Channel & channel);
	void	joinCmd(Channel & channel, std::string pwd);
	void	partCmd(Channel & channel);

	// Operator commands
	void 	kickCmd();
	void 	inviteCmd();
	void 	topicCmd();
	void 	modeCmd();

	// Exception
	class UserError : public Error {
		public:
			UserError(std::string what);
	};

private:
	// Identification and Status
	std::string	m_nick;
	std::string	m_user;
	ASocket*	m_socket;
	bool		m_servOps;

	// Channels joined, map <name, Channel>
	mapChannel	m_allChan;
	mapChannel	m_opsChan;

};

#endif 