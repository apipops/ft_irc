#include "../includes/Channel.hpp"

Channel::Channel()
{
	// impossible, a mettre dans prive 
}

Channel::Channel(std::string name, std::string topic)
{
	this->m_name = name;
	this->m_topic = topic;
}

Channel::Channel(const Channel & src)
{
	*this = src;
}

Channel & Channel::operator=(const Channel & src)
{
	this->m_name = src.m_name;
	this->m_topic = src.m_topic;
}

Channel::~Channel()
{
}