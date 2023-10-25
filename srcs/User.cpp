#include "../includes/User.hpp"

/************** CONSTRUCTORS & DESTRUCTORS ****************/

User::User()
{
}

User::User(std::string nick, std::string user, ASocket* socket)
{

	this->m_nick = nick;
	this->m_user = user;
	this->m_socket = socket;
	this->m_servOps = 0;
}

User::User(const User & src)
{
	*this = src;
}

User & User::operator=(const User & src)
{
	this->m_nick = src.m_nick;
	this->m_user = src.m_user;
	this->m_socket = src.m_socket;
	this->m_servOps = src.m_servOps;
	this->m_allChan = src.m_allChan;
	this->m_opsChan = src.m_opsChan;
	
	return *this;
}

User::~User()
{
}

/********************** EXCEPTIONS ***********************/

User::UserError::UserError(std::string what): Error(what)
{
}
