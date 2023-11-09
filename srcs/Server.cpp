#include "Server.hpp"
#include "User.hpp"
#include "Msg.hpp"
#include "Command.hpp"

bool Server::stop = false;

Server::Server() 
{
	this->stop = false;
}
Server::~Server() {}

void	Server::setPort(int port)
{
	this->port = port;
}
void	Server::setStop(bool status)
{
	Server::stop = status;
}

void	Server::setPasswd(std::string passwd)
{
	this->passwd = passwd;
}

void	Server::setTV(int sec, int musec)
{
	this->tv.tv_sec = sec;
	this->tv.tv_usec = musec;
}

int	 Server::getPort()
{
	return this->port;
}

bool	Server::getStop()
{
	return this->stop;
}

void	Server::signalHandler(int signum) 
{
	if (signum == SIGINT)
	{
		std::cout << "-> Server shutdown requested. Exiting..." << std::endl << std::endl;
		Server::stop = true;
	}
}

void	*Server::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(reinterpret_cast<struct sockaddr_in*>(sa)->sin_addr);
	return &(reinterpret_cast<struct sockaddr_in6*>(sa)->sin6_addr);
}

void	Server::setHint(int family, int type, int flag)
{
	std::memset(&this->hints, 0, sizeof(this->hints));
	this->hints.ai_family = family;
	this->hints.ai_socktype = type;
	this->hints.ai_flags = flag;
}

int	 Server::get_listener_socket(void)
{
	int listener;	 // Listening socket descriptor
	int yes=1;		// For setsockopt() SO_REUSEADDR, below
	int rv;

	struct addrinfo *ai, *p;
	setHint(AF_UNSPEC, SOCK_STREAM, AI_PASSIVE);
	// voir si possible amelioration
	std::string s;
	std::stringstream out;
	out << getPort();
	s = out.str();
	const char * port = s.c_str();
	if ((rv = getaddrinfo(NULL, port, &this->hints, &ai)) != 0)
	{
		std::cerr << "selectserver: " << gai_strerror(rv) << std::endl;
		exit(1);
	}
	for(p = ai; p != NULL; p = p->ai_next)
	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0)
			continue;
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listener);
			continue;
		}

		break;
	}
	if (p == NULL)
		return -1;
	freeaddrinfo(ai); // All done with this
	// Listen
	if (listen(listener, 10) == -1)
		return -1;
	return listener;
}

void	Server::add_to_pfds(int newfd)
{
	struct pollfd   data;
	data.fd = newfd;
	data.events = POLLIN;
	this->pfds.push_back(data);
}

void	Server::del_from_pfds(int index)
{// a voir, manque test erreur
	if (index < (int)this->pfds.size())
	{
		removeUser(this->pfds[index].fd);
		this->pfds.erase(this->pfds.begin() + index);
	}
}

void	Server::setListeningSocket()
{
	this->listener_socket = get_listener_socket();
	if (this->listener_socket == -1)
	{
		std::cerr << "error getting listening socket\n";
		exit (1);
	}
	add_to_pfds(this->listener_socket);
}

void	Server::handleNewConnection()
{
	std::string connection;

	this->addrlen = sizeof(this->remoteaddr);
	this->accepted_socket = accept(this->listener_socket,reinterpret_cast<struct sockaddr *>(&this->remoteaddr), &(this->addrlen));
	if (this->accepted_socket == -1)
		perror("accept");
	else
	{
		add_to_pfds(this->accepted_socket);
		addUser(this->accepted_socket);
		const char* c =this->remoteIP.c_str();
		connection = inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr),(char *)c, INET6_ADDRSTRLEN);
	}
}

void Server::handleClient(Msg &aMess, int index)
{
	memset(this->buffer, 0, sizeof(this->buffer));
	int nbytes = recv(pfds[index].fd, this->buffer, sizeof(this->buffer), 0);
	int sender_fd = pfds[index].fd;
	std::map<int, User>::iterator it = this->users.find(sender_fd);
	if (it != this->users.end())
		aMess.initialize(sender_fd, it->second, this->buffer, nbytes);
	aMess.split2("\r\n");
	if (nbytes <= 0)
	{
		if (nbytes != 0)
			perror("recv");
		close(pfds[index].fd);
		del_from_pfds(index);
	}
	else
	{
		std::string received_data(this->buffer, nbytes);
		Command cmd(*this);
		while(aMess.getMessageSize() > 0)
		{
			std::map<int, User>::iterator it = this->users.find(sender_fd);
			if (it != this->users.end())
				cmd.parseLine(it->second, aMess.getMessage());
		}
	}
}

int	Server::getPfdsFD(int index)
{
	return this->pfds[index].fd;
}

void	Server::run()
{
	setListeningSocket();
	Msg	aMess;
	for (;;)
	{
		int poll_count = poll(&pfds[0], this->pfds.size(), -1);
		if (poll_count == -1)
		{
			if (errno == EINTR)
				return;
			perror("poll");
			exit(1);
		}
		for(size_t i = 0; i < this->pfds.size(); i++)
		{
			if (this->pfds[i].revents & POLLIN)
			{
				if (this->pfds[i].fd == this->listener_socket)
					handleNewConnection();
				else
					handleClient(aMess,i);
			}
		}
	}
}

void Server::addUser(int fd)
{
	User newUser(fd);
	this->users.insert(std::pair<int, User>(fd, newUser));
}

void Server::removeUser(int fd)
{
	int index = getUserIndex(fd);
	std::map<int, User>::iterator it2 = this->users.find(fd);
	if (it2 != this->users.end())
	{
		if (index != -1)
		{
			for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
			{
				if (it->second.isUser(it2->second))
				{
					if (it->second.isChanops(it2->second))
						it->second.rmChanops(it2->second);
					it->second.rmUser(it2->second);
				}
			}
			users.erase(fd);
		}
	}
}

bool	Server::isNickAvailable(const std::string& nick)
{
	for (std::map<int, User>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->second.getNick() == nick)
			return false;
	}
	return true;
}

int	 Server::getUserIndex(int fd)
{
	for (std::map<int, User>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->first == fd)
			return fd;
	}
	return -1;
}

User	*Server::getUserByNick(const std::string& nick)
{
	for (std::map<int, User>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->second.getNick() == nick)
			return &(it->second);
	}
	return nullptr;
}

int	 Server::getPfdsIndex(int fd)
{
	for (size_t i = 0; i < pfds.size(); ++i)
	{
		if (pfds[i].fd == fd)
			return i;
	}
	return -1;
}

std::string Server::getPasswd()
{
	return this->passwd;
}

void	Server::displayUsers()
{
	std::cout << yellow << "Nombre d'utilisateurs connectes : " << users.size() << reset << std::endl;
	for (std::map<int, User>::iterator it = users.begin(); it != users.end(); ++it)
		std::cout << yellowbg << "FD = " << it->first << "\t NICKNAME = " << it->second.getNick() << "\t USERNAME = " << it->second.getUser() << "\t REALNAME = " << it->second.getRealname() << "\t HOSTNAME = " << it->second.getHostname() <<  reset << std::endl;
}

Channel	*Server::getChannel(std::string channelName)
{
	std::map<std::string, Channel>::iterator it = this->channels.find(channelName);
	if (it != this->channels.end())
		return &(it->second);
	else
		return nullptr;
}

void	Server::createChannel(std::string channelName, User *user)
{
	Channel channel(channelName, user);
	this->channels.insert(std::make_pair(channelName, channel));
}

void	Server::rmChannel(std::string channelName)
{
	this->channels.erase(channelName);
}

std::map<int, User>	Server::getUser()
{
	return this->users;
}

void	Server::sendToAllUser(std::string msg)
{
	for (std::map<int, User>::iterator it2 = this->users.begin(); it2 != this->users.end(); ++it2)
		send(it2->first, msg.c_str(), msg.length(), 0);
}
