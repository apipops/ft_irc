#ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <string>
# include <map>
# include "Channel.hpp"

class Channel;

class User
{
public: 
	User(); // impossible, a mettre dans prive ?
	User(std::string user, std::string nick);
	User(const User & src);
	User & operator=(const User & src);
	~User();

	// commandes

private:
	std::string	m_user;
	std::string	m_nick;
	bool	m_chanOps;
	bool	m_servOps;

	// Channels joined by user
	std::map<std::string, Channel>	m_allChan;
	std::map<std::string, Channel>	m_opsChan;

	// Autres attributs a penser
	// - nombre de canaux rejoints (max 10)

};

#endif 