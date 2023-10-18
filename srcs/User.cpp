#include "../includes/User.hpp"

User::User()
{
	// impossible, a mettre dans prive ?
}

User::User(std::string user, std::string nick)
{
	// Check user and nick formats, duplicates ?
	this->m_user = user;
	this->m_nick = nick;
	this->m_chanOps = 0;
	this->m_servOps = 0;
}

User::User(const User & src)
{
	*this = src;
}

User & User::operator=(const User & src)
{
	this->m_user = src.m_user;
	this->m_nick = src.m_nick;
	this->m_chanOps= src.m_chanOps;
	this->m_servOps = src.m_servOps;
	this->m_allChan = src.m_allChan;
	this->m_opsChan = src.m_opsChan;
}

User::~User()
{
}
