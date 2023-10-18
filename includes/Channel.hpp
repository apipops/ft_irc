#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <map>
# include "User.hpp"

class User;

class Channel 
{
public: 
	Channel(); // impossible, a mettre dans prive ?
	Channel(std::string name, std::string topic);
	Channel(const Channel & src);
	Channel & operator=(const Channel & src);
	~Channel();

	// faire les operations sur le channel

private:
	std::string	m_name;
	std::string m_topic;
	std::map<std::string, User> m_users;
	std::map<std::string, User> m_ops;

	// Attributs a penser : 
	// - mode du canal : sur invitation ?
	// - mode du canal : topic modifiable ?
	// - mot de passe du canal s'il y en a un 
	// - nb of max users
};

#endif