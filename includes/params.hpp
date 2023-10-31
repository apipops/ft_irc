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
class IRCServer;
class User;
class Channel;
class Message;
typedef std::map<std::string, User* > mapUser;
typedef std::map<std::string, Channel* > mapChannel;
typedef std::vector<User*> vecUser;
typedef std::vector<Channel*> vecChan;
typedef std::vector<std::string> vecStr;
typedef	void	(IRCServer::*t_cmd)(User *, Message &);
typedef std::map<std::string, t_cmd> mapCmd;

// USERNAME POLICY
#define USER_MINCHAR 1
#define USER_MAXCHAR 9

// CHANNEL POLICY
#define NONE -1
#define CHAN_MINCHAR 2
#define CHAN_MAXCHAR 50
#define TOPIC_MAXCHAR 1000
#define PWD_MINCHAR 1
#define PWD_MAXCHAR 32

// MESSAGE POLICY
#define MSG_MAXCHAR 510

// REPLIES
#define RPL_WHOISUSER			"311"
#define RPL_WHOISERVER			"312"
// #define	RPL_ENDOFWHOIS
//               "<nick> :End of WHOIS list"

// ERRORS

// generic
#define ERR_NEEDMOREPARAMS		"461 :Not enough parameters"

// nickname
#define ERR_NONICKNAMEGIVEN 	"431 :No nickname given"
#define ERR_ERRONEOUSNICKNAME	"432 :Erroneous nickname"
#define ERR_NICKNAMEINUSE		"433 :Nickname is already in use"

// channels
#define ERR_NOSUCHCHANNEL		"403 avast :No such channel"
#define ERR_INVALIDCHANNELNAME	"403 :Invalid channel name"
#define ERR_NOTONCHANNEL		"442 :You're not on that channel"
#define ERR_CHANNELISFULL		"471 :Cannot join channel (+l)"
#define ERR_INVITEONLYCHAN		"473 :Cannot join channel (+i)"
#define ERR_BADCHANNELKEY		"475 :Cannot join channel (+k)"
#define ERR_INVALIDKEYFORMAT	"475 :Cannot create channel (+k)"

#endif