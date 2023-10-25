#ifndef USERTEST_HPP
#define USERTEST_HPP

#include "TCP_IPv4"

namespace IRC {

	class User {
		public :
			User();
			User(TCP_IPv4::ASocket *);
			User(const User &);
			~User();
	
			User &operator=(const User &);
	
		private :
			TCP_IPv4::ASocket *m_socket;
	};
}

#endif