#include "../includes/User.hpp"

/************** CONSTRUCTORS & DESTRUCTOR ****************/

User::User(ASocket *socket)
{
	m_socket = socket;
	this->m_servOps = 0;
	this->m_authentified = 0;
}

User::User(const User & src)
{
	*this = src;
}

User & User::operator=(const User & src)
{
	this->m_nick = src.m_nick;
	this->m_user = src.m_user;
	this->m_real = src.m_real;
	this->m_socket = src.m_socket;
	this->m_authentified = src.m_authentified;
	this->m_servOps = src.m_servOps;
	this->m_allChan = src.m_allChan;
	this->m_opsChan = src.m_opsChan;
	this->m_invitChan = src.m_invitChan;
	
	return *this;
}

User::~User()
{
}

/********************* GETTERS & SETTERS ********************/

std::string User::getNick() const
{
	return m_nick;
}

std::string User::getPrefix() const
{
	if (m_servOps)
		return m_nick + "!@" + m_user;
	else
		return m_nick + "!" + m_user;
}


/********************** EXCEPTIONS ***********************/

User::UserError::UserError(std::string what): Error(what)
{
}
