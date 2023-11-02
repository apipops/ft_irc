/**
 * Class for users.
 * Users are IRC clients who connected to
 * the IRC server through a specific socket.
 * Identified by nickname, username and real name. 
*/

#ifndef USER_HPP
# define USER_HPP

# include "Channel.hpp"
# include "params.hpp"
# include "../raph/TCP_IPv4"

using namespace TCP_IPv4;

class User
{
public: 
	friend class IRCServer;

	// CONSTRUCTORS AND DESTRUCTOR
	User(ASocket *socket);
	User(std::string nick, std::string user, ASocket *socket);
	User(std::string nick, std::string user, std::string real, ASocket* socket);
	User(const User & src);
	User & operator=(const User & src);
	~User();

	// GETTERS
	std::string	getNick() const;
	std::string getPrefix() const;

	// EXCEPTION
	class UserError : public Error {
		public:
			UserError(std::string what);
	};

private:
	// IDENTIFICATION & STATUS
	std::string	m_nick;
	std::string	m_user;
	std::string m_real;
	ASocket*	m_socket;
	bool		m_authentified;
	bool		m_servOps;

	// CHANNELS (std::map<std::string, Channel *>(name, channel))
	mapChannel	m_allChan;
	mapChannel	m_opsChan;
	mapChannel	m_invitChan;

};

#endif 