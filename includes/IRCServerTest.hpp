#ifndef IRCSERVERTEST_HPP
#define IRCSERVERTEST_HPP

#include "TCP_IPv4"
#include "User.hpp"

namespace IRC {

	class Server : public TCP_IPv4::Server {
		public :
			Server(std::string);
			Server(const Server &);
			~Server();

			Server &operator=(const Server &);

		protected :

		private :
	};
}

#endif