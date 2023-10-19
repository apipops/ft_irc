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

/************************ PARSING ************************/

// Check if "name" (type: "Nickname" or "Username") matches policy
void checkFormat(std::string type, std::string name)
{
	if (name.length() < MINCHAR)
		throw User::UserError(type + " is too short");
	if (name.length() > MAXCHAR)
		throw User::UserError(type + " is too long");
	for(int i = 0; name[i]; i++) {
		if (!isalnum(name[i]) && name[i] != '-' && name[i] != '_')
			throw User::UserError(type + " has invalid character(s)");
	}
	if (!isalpha(name[0]))
		throw User::UserError(type + " must start with a letter");
}

/********************** EXCEPTIONS ***********************/

User::UserError::UserError(std::string what): Error(what)
{
}
