#include "ServerTest.hpp"

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

void IRC::Server::run() {
	if (!this->isup())
		throw TCP_IPv4::Server::Failure("server is down");
	this->setState(RUNNING);
	while (this->isrunning()) {
		m_socEvent.wait();
		if (this->pendingConnection()) {
			//new user
		}
		//handle incoming data
	}
}

void IRC::Server::newUser() {
	TCP_IPv4::ASocket *newASocket = this->newConnection();
	newASocket->setNonBlock();
	IRC::User *newUser = new IRC::User(newASocket);
	m_socUserMap[newASocket] = newUser;
}