#include "../includes/Channel.hpp"

/***************** CONSTRUCTORS AND DESTRUCTOR ********************/

Channel::Channel(std::string name)
{
	std::time_t	timestamp = std::time(NULL);
	std::stringstream ss;
    ss << timestamp;
	m_name = name;
	m_createInfo = name + " " + ss.str();
	m_invitMode = 0;
	m_topicRestrict = 0;
	m_maxUsers = NONE;
}

Channel::Channel(std::string name, std::string pwd)
{
	std::time_t	timestamp = std::time(NULL);
	std::stringstream ss;
    ss << timestamp;
	m_name = name;
	m_pwd = pwd;
	m_createInfo = name + " " + ss.str();
	m_invitMode = 0;
	m_topicRestrict = 0;
	m_maxUsers = NONE;
}

Channel::Channel(const Channel & src)
{
	*this = src;
}

Channel & Channel::operator=(const Channel & src)
{
	m_name = src.m_name;
	m_topic = src.m_topic;
	m_pwd = src.m_pwd;
	m_createInfo = src.m_createInfo;
	m_modifInfo = src.m_modifInfo;
	m_invitMode = src.m_invitMode;
	m_topicRestrict = src.m_topicRestrict;
	m_users = src.m_users;
	m_ops = src.m_ops;
	m_maxUsers = src.m_maxUsers;

	return *this;
}

Channel::~Channel()
{
}

/*********************** SETTERS & GETTERS ***********************/

// Add user to standard user list
void Channel::addUser(User *user)
{
	m_users.push_back(user);
}

// Add user to channel operator list
void Channel::addOps(User *user)
{
	m_ops.push_back(user);
}

// Add user to invitation list
void Channel::addInvit(User *user)
{
	m_invited.push_back(user);
}

// Remove user from the channel
void Channel::removeUser(std::string nick)
{
	vecUser::iterator it = m_users.begin();
	
	while (it != m_users.end() && (*it)->getNick() != nick)
		it++;
	if (it != m_users.end())
		m_users.erase(it);
}

// Remove ops user from the channel
void Channel::removeOps(std::string nick)
{
	vecUser::iterator it = m_ops.begin();
	
	while (it != m_ops.end() && (*it)->getNick() != nick)
		it++;
	if (it != m_ops.end())
		m_ops.erase(it);
}

// Remove user from the invit list
void Channel::removeInvit(std::string nick)
{
	vecUser::iterator it = m_invited.begin();
	
	while (it != m_invited.end() && (*it)->getNick() != nick)
		it++;
	if (it != m_invited.end())
		m_invited.erase(it);
}

// Check if user user of the channel
bool Channel::checkUser(std::string nick)
{
	vecUser::iterator it = m_users.begin();
	
	while (it != m_users.end() && (*it)->getNick() != nick)
		it++;
	if (it != m_users.end())
		return true;
	return false;
}

// Check if user is ops of the channel
bool Channel::checkOps(std::string nick)
{
	vecUser::iterator it = m_ops.begin();
	
	while (it != m_ops.end() && (*it)->getNick() != nick)
		it++;
	if (it != m_ops.end())
		return true;
	return false;
}

// Check if user has been invited to the channel
bool Channel::checkInvit(std::string nick)
{
	vecUser::iterator it = m_invited.begin();
	
	while (it != m_invited.end() && (*it)->getNick() != nick)
		it++;
	if (it != m_invited.end())
		return true;
	return false;
}

// Set the modification information for RPL_TOPICWHOTIME (333)
void	Channel::setModifInfo(User *user)
{
	std::time_t	timestamp = std::time(NULL);
	std::stringstream ss;
    ss << timestamp;
    std::string timestampStr = ss.str();

	m_modifInfo = m_name + " " + user->getPrefix() + " " + timestampStr;
}

// Get the mode information for RPL_CHANNELMODEIS (324) 
std::string	Channel::getModeStr()
{
	std::string modeStr = m_name + " +";
	std::string params;
	if (m_topicRestrict)
		modeStr += "t";
	if (m_invitMode)
		modeStr += "i";
	if (m_maxUsers > 0) {
		modeStr += "l";
		params += " " + m_maxUsers;
	}
	if (!m_pwd.empty()) {
		modeStr += "k";
		params += " " + m_pwd;
	}
	return modeStr + params;
}

/************************* EXCEPTIONS **************************/

Channel::ChannelError::ChannelError(std::string what): Error(what)
{
}
