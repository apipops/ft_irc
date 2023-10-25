#ifndef IRCSERVERTEST_HPP
#define IRCSERVERTEST_HPP

#include "TCP_IPv4"
#include "UserTest.hpp"
#include <vector>
#include <map>
#include <iostream>

namespace IRC {
	
	class Server : public TCP_IPv4::Server {
		public :
			typedef std::vector<User *> User_vec;
			typedef std::map<TCP_IPv4::ASocket *, User *> soc_User_map;
			typedef std::map<std::string, User *> nick_User_map;

			Server(std::string);
			Server(const Server &);
			~Server();

			Server &operator=(const Server &);

			void run();
			void newUser();

		private :
			User_vec m_users;
			soc_User_map m_socUserMap;
			nick_User_map m_nickUserMap;
	};
}

#endif