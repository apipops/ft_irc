#include "IRCServerTest.hpp"

/*------------------------------------*/
/*     Constructors and destructor    */
/*------------------------------------*/

IRC::Server::Server(std::string name) : TCP_IPv4::Server(name) {}

IRC::Server::Server(const Server &other) : TCP_IPv4::Server(other.m_name) {
	*this = other;
}

IRC::Server::~Server() {}

/*------------------------------------*/
/*              Operators             */
/*------------------------------------*/

IRC::Server &IRC::Server::operator=(const Server &other) {
	TCP_IPv4::Server::operator=(other);
	return *this;
}

/*------------------------------------*/
/*               Methods              */
/*------------------------------------*/

