#include "../includes/Message.hpp"

/************** CONSTRUCTORS & DESTRUCTORS **************/

Message::Message()
{
}


Message::Message(std::string msg)
{
	if (msg.empty())
		throw MsgError("Message is empty.");
	if (msg.length() > MSG_MAXCHAR)
		throw MsgError("Message is too long.");
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
	for(int i = 0; i < 15; i++)
		this->m_args[i] = src.m_args[i];
	
	return *this;
}

Message::~Message()
{
}

/*********************** GETTERS *********************/
std::string	Message::getMessage() const {
	std::string msg = m_prefix + m_cmd;
	for (int i = 0; i < 15; i++)
		msg += m_args[i];
	return msg;
}

/****************** UTILS FOR TESTING ****************/
void Message::showMessage() const {
	std::cout << "- prefix: " << m_prefix << std::endl;
	std::cout << "- cmd: " << m_cmd << std::endl;
	for (int i = 0; i < 15; i++) {
		if (!m_args[i].empty())
			std::cout << "- arg" << i << ": " << m_args[i] << std::endl;
	}
}


/*********************** PARSING *********************/

void Message::parseMsg(std::string msg)
{
	std::stringstream 	stream(msg);
	std::string			token;

	// Prefix
	std::getline(stream, token, ' ');
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
		m_args[i] = token;
		i++;
	}
	if (i > 0 && token[0] == ':') {
		std::string first = token;
		if (std::getline(stream, token))
			m_args[i] = first + " " + token;
		else
			m_args[i] = first;
	}
}

void Message::checkCmdFormat(std::string str)
{
	int	digit = 0;
	int	alpha = 0;

	if (str.empty())
		throw MsgError("Invalid format message");
	for(int i = 0; str[i]; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			if (!digit)
				alpha++;
			else
				throw MsgError("Invalid format message");
		}
		else if (std::isdigit(str[i])) {
			if (!alpha)
				digit++;
			else
				throw MsgError("Invalid format message");
		}
		else
			throw MsgError("Invalid format message");
	}
	if (!alpha && digit != 3)
		throw MsgError("Invalid format message");
}

/********************** EXCEPTION ********************/

Message::MsgError::MsgError(std::string what): Error(what)
{
}
