#include "UserTest.hpp"


/*------------------------------------*/
/*     Constructors and destructor    */
/*------------------------------------*/

IRC::User::User() {}

IRC::User::User(TCP_IPv4::ASocket *socket) : m_socket(socket) {}

IRC::User::User(const User &other) : m_socket(other.m_socket) {
	*this = other;
}

IRC::User::~User() {}

/*------------------------------------*/
/*              Operators             */
/*------------------------------------*/

IRC::User &IRC::User::operator=(const User &other) {
	m_socket = other.m_socket;
	return *this;
}

/*------------------------------------*/
/*               Methods              */
/*------------------------------------*/

