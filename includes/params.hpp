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
# include <ctime>

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
typedef	void (IRCServer::*t_cmd)(User *, Message &);
typedef std::map<std::string, t_cmd> mapCmd;

// OPERATOR
#define OPER_PWD "root1234"

// NICKNAME POLICY
#define NICKNAME_MINCHAR 1
#define NICKNAME_MAXCHAR 9

// CHANNEL POLICY
#define NONE -1
#define CHAN_MINCHAR 1
#define CHAN_MAXCHAR 50

// REPLIES
#define RPL_UMODEIS				"221\r\n"
#define RPL_WHOISUSER			"311\r\n"
#define RPL_WHOISERVER			"312\r\n"
#define RPL_ENDOFWHO			"315 :End of /WHO list\r\n"
#define	RPL_ENDOFWHOIS			"318 :End of /WHOIS list\r\n"
#define RPL_CHANNELMODEIS		"324\r\n"
#define RPL_CREATIONTIME		"329\r\n"
#define RPL_NOTOPIC				"331 :No topic is set\r\n"
#define RPL_TOPIC				"332\r\n"
#define RPL_TOPICWHOTIME		"333\r\n"
#define RPL_INVITING			"341\r\n"
#define	RPL_WHOREPLY			"352\r\n"
#define RPL_NAMREPLY			"353\r\n"
#define RPL_ENDOFNAMES			"366 :End of /NAMES\r\n"
#define	RPL_YOUREOPER			"381 :You are now an IRC operator\r\n"

// ERRORS

// generic
#define ERR_NEEDMOREPARAMS		"461 :Not enough parameters\r\n"

// operator
#define ERR_PASSWDMISMATCH		"464 :Incorrect password\r\n"	
#define ERR_USERSDONTMATCH		"502 :Cannot change mode for other users\r\n"	

// nickname
#define ERR_NOSUCHNICK			"401 :No such nick\r\n"
#define ERR_NONICKNAMEGIVEN 	"431 :No nickname given\r\n"
#define ERR_ERRONEOUSNICKNAME	"432 :Erroneous nickname\r\n"
#define ERR_NICKNAMEINUSE		"433 :Nickname is already in use\r\n"

// channels
#define ERR_NOSUCHCHANNEL		"403 :No such channel\r\n"
#define ERR_INVALIDCHANNELNAME	"403 :Invalid channel name\r\n"
#define ERR_USERNOTINCHANNEL	"441 :They aren't on that channel\r\n"
#define ERR_NOTONCHANNEL		"442 :You're not on that channel\r\n"
#define ERR_USERONCHANNEL		"443 :is already on channel\r\n"
#define ERR_CHANNELISFULL		"471 :Cannot join channel (+l)\r\n"
#define ERR_UNKNOWNMODE			"472 :is unknown mode char to me\r\n"
#define ERR_INVITEONLYCHAN		"473 :Cannot join channel (+i)\r\n"
#define ERR_BADCHANNELKEY		"475 :Cannot join channel (+k)\r\n"
#define ERR_INVALIDKEYFORMAT	"475 :Cannot create channel (+k)\r\n"
#define ERR_CHANOPRIVSNEEDED	"482 :You're not channel operator\r\n"

// message
#define ERR_CANNOTSENDTOCHAN	"404 :Cannot send to channel\r\n"
#define ERR_NORECIPIENT			"411 :No recipient given\r\n"
#define ERR_NOTEXTTOSEND		"412 :No text to send\r\n"

#endif