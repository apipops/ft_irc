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

#define USER_REMOVED 1

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
#define RPL_WHOISUSER			"311\r\n"
#define RPL_WHOISERVER			"312\r\n"
#define	RPL_ENDOFWHOIS			"318\r\n"
#define RPL_TOPIC				"332\r\n"
#define RPL_NAMREPLY			"353\r\n"
#define RPL_ENDOFNAMES			"366 :End of Names\r\n"

// ERRORS

// generic
#define ERR_NEEDMOREPARAMS		"461 :Not enough parameters\r\n"
#define ERR_PASSWDMISMATCH		"464 :Password incorrect\r\n"

// nickname
#define ERR_NOSUCHNICK			"401 :No such nick\r\n"
#define ERR_NONICKNAMEGIVEN 	"431 :No nickname given\r\n"
#define ERR_ERRONEOUSNICKNAME	"432 :Erroneous nickname\r\n"
#define ERR_NICKNAMEINUSE		"433 :Nickname is already in use\r\n"

// channels
#define ERR_NOSUCHCHANNEL		"403 :No such channel\r\n"
#define ERR_INVALIDCHANNELNAME	"403 :Invalid channel name\r\n"
#define ERR_NOTONCHANNEL		"442 :You're not on that channel\r\n"
#define ERR_CHANNELISFULL		"471 :Cannot join channel (+l)\r\n"
#define ERR_INVITEONLYCHAN		"473 :Cannot join channel (+i)\r\n"
#define ERR_BADCHANNELKEY		"475 :Cannot join channel (+k)\r\n"
#define ERR_INVALIDKEYFORMAT	"475 :Cannot create channel (+k)\r\n"

// message
#define ERR_CANNOTSENDTOCHAN	"404 :Cannot send to channel\r\n"
#define ERR_NORECIPIENT			"411 :No recipient given\r\n"
#define ERR_NOTEXTTOSEND		"412 :No text to send\r\n"

#endif