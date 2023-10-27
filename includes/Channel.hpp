/**
 * Class for channels
 * Channels are groups of users identified by
 * a name, a  topic and sometimes a password.
*/

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "User.hpp"
# include "params.hpp"
# include "../raph/TCP_IPv4"

using namespace TCP_IPv4;

class Channel 
{
public: 
	friend class IRCServer;

	// CONSTRUCTORS AND DESTRUCTOR
	Channel(std::string name);
	Channel(std::string name, std::string pwd);
	Channel(const Channel & src);
	Channel & operator=(const Channel & src);
	~Channel();

	// SETTERS & GETTER (users)
	void addUser(User *user);
	void addOps(User *user);
	void addInvit(User *user);
	void removeUser(std::string nick);
	void removeOps(std::string nick);
	bool checkInvit(std::string nick);


	// EXCEPTIONS
	class ChannelError : public Error {
		public:
			ChannelError(std::string what);
	};

private:
	// IDENTIFICATION
	std::string	m_name;
	std::string m_topic;
	std::string m_pwd;

	// USERS & OPERATORS
	vecUser m_users;
	vecUser m_ops;
	vecUser m_invited;

	// MODE 
	bool	m_invitMode;
	bool	m_topicRestrict; 
	int		m_maxUsers;
};

#endif