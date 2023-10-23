#include "../includes/Channel.hpp"

/************** CONSTRUCTORS & DESTRUCTORS ****************/

Channel::Channel()
{
}

Channel::Channel(std::string name, std::string topic)
{
	this->m_name = name;
	this->m_topic = topic;
	this->m_invitMode = 0;
	this->m_topicRestrict = 0;
	this->m_maxUsers = NONE;
}

Channel::Channel(std::string name, std::string topic, std::string pwd)
{
	this->m_name = name;
	this->m_topic = topic;
	this->m_pwd = pwd;
	this->m_invitMode = 0;
	this->m_topicRestrict = 0;
	this->m_maxUsers = NONE;
}

Channel::Channel(const Channel & src)
{
	*this = src;
}

Channel & Channel::operator=(const Channel & src)
{
	this->m_name = src.m_name;
	this->m_topic = src.m_topic;
	this->m_pwd = src.m_pwd;
	this->m_users = src.m_users;
	this->m_ops = src.m_ops;
	this->m_maxUsers = src.m_maxUsers;

	return *this;
}

Channel::~Channel()
{
}

/************************ PARSING ************************/

// Check if channel "name" matches policy
void Channel::checkChanFormat(std::string name)
{
	if (name.length() < CHAN_MINCHAR)
		throw Channel::ChannelError("Channel name \'" + name +"\' is too short");
	if (name.length() > CHAN_MAXCHAR)
		throw Channel::ChannelError("Channel name \'" + name +"\' is too long");
	if (name[0] != '#')
		throw Channel::ChannelError("Channel name must start with character \'#\'");
	for(int i = 0; name[i]; i++) {
		if (!isprint(name[i]) || name[i] == ' ' || name[i] == ',' || name[i] == ':')
			throw User::UserError("Chanel name \'" + name +"\' has invalid character(s)");
	}
}

// Check if topic description format matches the policy
void Channel::checkTopicFormat(std::string topic)
{
	if (topic.length() > TOPIC_MAXCHAR)
		throw Channel::ChannelError("Topic description is too long");
	for(int i = 0; topic[i]; i++) {
		if (!isprint(topic[i]))
			throw User::UserError("Topic description has invalid character(s)");
	}
 }

// Check if password format matches the policy
void Channel::checkPwdFormat(std::string pwd)
{
	if (pwd.length() < PWD_MINCHAR)
		throw Channel::ChannelError("Password is too short");
	if (pwd.length() > PWD_MAXCHAR)
		throw Channel::ChannelError("Password is too long");
}

/************************ SETTERS ************************/

// void Channel::addUser(User & user)
// {
// 	// verifier si le user existe deja ?
// 	m_users[user.m_nick] = user;
// }

void Channel::removeUser(std::string nick)
{
	std::vector<User*>::iterator it = m_users.begin();
	
	while (it != m_users.end() && (*it)->m_nick != nick)
		it++;
	if (it != m_users.end())
		m_users.erase(it);
}

// void Channel::addOps(User & ops)
// {
// 	m_ops[ops.m_nick] = ops;
// }

void Channel::removeOps(std::string nick)
{
	std::vector<User*>::iterator it = m_ops.begin();
	
	while (it != m_ops.end() && (*it)->m_nick != nick)
		it++;
	if (it != m_ops.end())
		m_ops.erase(it);
}

/********************** EXCEPTIONS ***********************/

Channel::ChannelError::ChannelError(std::string what): Error(what)
{
}
