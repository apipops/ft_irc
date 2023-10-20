/**
 * Class for channels
 * Channels are groups of users identified by
 * a name, a  topic and sometimes a password.
*/

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <map>
# include <algorithm>
# include "User.hpp"
# include "../raph/TCP_IPv4"

using namespace TCP_IPv4;
class User;
typedef std::map<std::string, User> mapUser;

class Channel 
{
public: 
	// Constructors and destructor
	Channel();
	Channel(std::string name, std::string topic);
	Channel(std::string name, std::string topic, std::string pwd);
	Channel(const Channel & src);
	Channel & operator=(const Channel & src);
	~Channel();

	// Parsing
	static void checkChanFormat(std::string name);
	static void checkTopicFormat(std::string topic);
	static void checkPwdFormat(std::string pwd);

	// Exception
	class ChannelError : public Error {
		public:
			ChannelError(std::string what);
	};

private:
	// Identification
	std::string	m_name;
	std::string m_topic;
	std::string m_pwd;

	// Users and operators, map <nickname, User>
	mapUser m_users;
	mapUser m_ops;

	// Mode
	bool m_invitMode;
	bool m_topicRestrict; 

	// Other info
	int	m_maxUsers;

};

#endif