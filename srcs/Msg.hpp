#ifndef MSG_HPP
# define MSG_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <vector>
#include <deque>
#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <vector>
#include "User.hpp"

const std::string red("\033[0;31m");
const std::string redbg("\033[0;41m");
const std::string green("\033[1;32m");
const std::string greenbg("\033[1;42m");
const std::string yellow("\033[1;33m");
const std::string yellowbg("\033[1;43m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string magentabg("\033[0;45m");
const std::string reset("\033[0m");

class User;

typedef	struct	incomingMessage
{
	int					accepted_socket; //pour identifier d'ou cela vient du cote du serveur
	User				*user;
	char				buffer_in[512];
	size_t				recv_size; //la qut de caracteres recus
	std::deque<char>	buffer;
} incomingMessage;

typedef	struct	outgoingMessage
{
	int					accepted_socket; //pour identifier d'ou cela vient du cote du serveur
	User				*user;
	std::string			message;
	char				buffer_in[512];
	size_t				message_size; //la qut de caracteres recus
} outgoingMessage;

class Msg
{
	public:
		Msg();
		~Msg();
		int			load_buffer(char * buff, size_t size);
		void		split(std::string sep);
		void		split2(std::string sep);
		std::string	getMessage();// a modifier pour qu il prenne la queue
		int			getMessageSize();
		int			initialize(int acc_socket, User &user, char * buff, int recv_size);

	private:
		char							buffer_in[512];
		char							buffer_out[512];
		std::deque<char>				buffer;
		incomingMessage					aMessage;
		outgoingMessage					uniqueMessage;
		std::deque<incomingMessage>		received_message;
		std::deque<outgoingMessage>		message_list;
};

#endif