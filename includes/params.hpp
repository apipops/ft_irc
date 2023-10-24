#ifndef PARAMS_HPP
#define PARAMS_HPP

# include <map>
# include <vector>
# include <deque>
# include <algorithm>
# include <iostream>
# include <string>

// Typedef
class User;
class Channel;
typedef std::map<std::string, User* > mapUser;
typedef std::map<std::string, Channel* > mapChannel;

// Username policy
#define USER_MINCHAR 1
#define USER_MAXCHAR 32

// Channel policy
#define NONE -1
#define CHAN_MINCHAR 2
#define CHAN_MAXCHAR 200
#define TOPIC_MAXCHAR 1000
#define PWD_MINCHAR 1
#define PWD_MAXCHAR 32


#endif