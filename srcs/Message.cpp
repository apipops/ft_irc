#include "../includes/Message.hpp"

/**************** CONSTRUCTORS & DESTRUCTORS ***************/

Message::Message()
{
}


Message::Message(std::string msg)
{
	if (msg.empty())
		throw MsgError("Message is empty.");
	parseMsg(msg);
}

Message::Message(std::string pfx, std::string cmd, std::string args)
{
	m_prefix = pfx;
	checkCmdFormat(cmd);
	m_cmd = cmd;
	parseArgs(args);
}

Message::Message(Message & src)
{
	*this = src;
}

Message & Message::operator=(Message & src)
{
	this->m_prefix = src.m_prefix;
	this->m_cmd = src.m_cmd;
	vecStr::const_iterator it = src.m_args.begin();
	for(; it != src.m_args.end(); it++)
		this->m_args.push_back(*it);

	return *this;
}

Message::~Message()
{
}

/************************* GETTERS *************************/

std::string	Message::getMessage() const {
	std::string msg;
	if (!m_prefix.empty())
		msg = m_prefix + " " + m_cmd;
	else
		msg = m_cmd;
	if (m_args.size())
		msg += " " + getArgs();
	return msg;
}

std::string	Message::getArgs() const {
	std::string args;
	if (!m_args.size())
		return args;
	for(size_t i = 0; i < m_args.size(); i++) {
		if (i < m_args.size() - 1)
			args += m_args[i] + " ";
		else if (i == m_args.size() - 1 && m_args[i].find(' ') != std::string::npos)
			args += ":" + m_args[i];
		else
			args += m_args[i];
	}
	return args;
}

/********************* UTILS FOR TESTING *******************/

void Message::showMessage() const {
	std::cout << "- prefix: " << m_prefix << std::endl;
	std::cout << "- cmd: " << m_cmd << std::endl;
	vecStr::const_iterator it = m_args.begin();
	int i = 0;
	for(; it != m_args.end(); it++) {
		std::cout << "- arg" << i << ": " << *it << std::endl;
		i++;
	}
}

/************************* PARSING ************************/

void Message::parseMsg(std::string msg)
{
	std::stringstream 	stream(msg);
	std::string			token;

	std::getline(stream, token, ' ');

	// Prefix
	if (token[0] == ':') {
		m_prefix = token;
		std::getline(stream, token, ' '); 
	}

	// Command
	checkCmdFormat(token);
	m_cmd = token;

	// Arguments
	int cursor = static_cast<int>(stream.tellg());
	parseArgs(msg.substr(cursor, msg.length() - cursor + 1));
}

void Message::parseArgs(std::string args)
{
	int					i = 0;
	std::stringstream	stream(args);
	std::string			token;

	while (std::getline(stream, token, ' ') && token[0] != ':')
	{
		m_args.push_back(token);
		i++;
	}
	if (i > 0 && token[0] == ':') {
		std::string first = token.substr(1, token.length() - 1);
		if (std::getline(stream, token))
			m_args.push_back(first + " " + token);
		else
			m_args.push_back(first);
	}
}

void Message::checkCmdFormat(std::string str)
{
	int	digit = 0;
	int	alpha = 0;

	if (str.empty())
		throw MsgError("Invalid message format");
	for(int i = 0; str[i]; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			if (!digit)
				alpha++;
			else
				throw MsgError("Invalid message format");
		}
		else if (std::isdigit(str[i])) {
			if (!alpha)
				digit++;
			else
				throw MsgError("Invalid message format");
		}
		else
			throw MsgError("Invalid message format");
	}
	if (!alpha && digit != 3)
		throw MsgError("Invalid message format");
}

/********************** EXCEPTION ********************/

Message::MsgError::MsgError(std::string what): Error(what)
{
}
