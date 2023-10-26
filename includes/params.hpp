#ifndef PARAMS_HPP
#define PARAMS_HPP

// LIBRARIES
# include <iostream>
# include <map>
# include <vector>
# include <algorithm>
# include <iostream>
# include <string>
# include <sstream>
# include <exception>

// TYPEDEF
class User;
class Channel;
typedef std::map<std::string, User* > mapUser;
typedef std::map<std::string, Channel* > mapChannel;

// USERNAME POLICY
#define USER_MINCHAR 1
#define USER_MAXCHAR 32

// CHANNEL POLICY
#define NONE -1
#define CHAN_MINCHAR 2
#define CHAN_MAXCHAR 200
#define TOPIC_MAXCHAR 1000
#define PWD_MINCHAR 1
#define PWD_MAXCHAR 32

// MESSAGE POLICY
#define MSG_MAXCHAR 510

#endif