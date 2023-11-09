#include "Msg.hpp"

Msg::Msg() {};
Msg::~Msg() {};

std::string		Msg::getMessage()
{
	std::string	tmp = this->message_list[0].message;
	this->message_list.pop_front();
	return (tmp);
}

int		Msg::initialize(int acc_socket, User &user, char * buff, int recv_size)
{
	this->aMessage.accepted_socket = acc_socket;
	this->aMessage.user = &user;
	this->aMessage.recv_size = recv_size;
	for (size_t i=0; i < 512; i++)
		this->aMessage.buffer_in[i] = buff[i];
	this->received_message.push_back(this->aMessage);
	return (0);
}

void	Msg::split2(std::string sep)
{
	std::string tmp(received_message[0].buffer_in);
	tmp = tmp.substr(0, this->aMessage.recv_size);
	std::string tmp1(this->aMessage.buffer.begin(), this->aMessage.buffer.end());
	size_t	found;
	while ((found = tmp.find(sep)) != std::string::npos)
	{
		tmp1.append(tmp.substr(0, found));
		this->aMessage.buffer.clear();
		tmp.replace(0, found+2, "");
		this->uniqueMessage.accepted_socket = this->aMessage.accepted_socket;
		this->uniqueMessage.user = this->aMessage.user;
		this->uniqueMessage.message = tmp1;
		this->uniqueMessage.message_size = tmp1.length();
		this->message_list.push_back(this->uniqueMessage);
		tmp1 = "";
	}
	for (size_t i = 0; i < tmp.length(); ++i)
		this->aMessage.buffer.push_back(tmp[i]);
	this->received_message.erase(received_message.begin());

}

int		Msg::getMessageSize()
{
	return (this->message_list.size());
}
