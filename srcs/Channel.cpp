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
	this->m_invitMode = src.m_invitMode;
	this->m_topicRestrict = src.m_topicRestrict;
	this->m_users = src.m_users;
	this->m_ops = src.m_ops;
	this->m_maxUsers = src.m_maxUsers;

	return *this;
}

Channel::~Channel()
{
}

/************************ SETTERS ************************/

void Channel::removeUser(std::string nick)
{
	std::deque<User*>::iterator it = m_users.begin();
	
	while (it != m_users.end() && (*it)->m_nick != nick)
		it++;
	if (it != m_users.end())
	{
		m_users.erase(it);
	}
}

void Channel::removeOps(std::string nick)
{
	std::deque<User*>::iterator it = m_ops.begin();
	
	while (it != m_ops.end() && (*it)->m_nick != nick)
		it++;
	if (it != m_ops.end())
		m_ops.erase(it);
}

/********************** EXCEPTIONS ***********************/

Channel::ChannelError::ChannelError(std::string what): Error(what)
{
}
