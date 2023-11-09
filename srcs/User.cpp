#include "User.hpp"
#include "Server.hpp"

User::User(int fd, std::string nick, std::string user) : fd(fd), nick(nick), user(user), passwd("none"), realname("none"), hostname("none"), mode("0"), isRegistered(0), isAlive(true), didPass(false)
{}

User::User(int fd) : fd(fd), nick("none"), user("none"), passwd("none"), realname("none"), hostname("none"), mode("0"), isRegistered(0), isAlive(true), didPass(false)
{}

User::~User() 
{
	isAlive = false;
}

bool	User::getIsAlive() const
{
	return this->isAlive;
}

void	User::operator=(User const &src)
{
	this->nick = src.nick;
	this->user = src.user;
	this->passwd = src.passwd;
	this->realname = src.realname;
	this->hostname = src.hostname;
	this->mode = src.mode;
	this->isRegistered = src.isRegistered;
}

bool	User::operator==(const User& other) const
{
	return (this->fd == other.fd &&
			this->nick == other.nick &&
			this->user == other.user &&
			this->passwd == other.passwd &&
			this->realname == other.realname &&
			this->hostname == other.hostname &&
			this->mode == other.mode &&
			this->isRegistered == other.isRegistered);
}

int		User::getFd()
{
	if (!this->fd)
		return 0;
	return this->fd;
}

void	User::setNick(std::string nick)
{
	this->nick = nick;
}

std::string	User::getNick()
{
	return this->nick;
}

void	User::setUser(std::string user)
{
	this->user = user;
	checkRegistration();
}

std::string	User::getUser()
{
	return this->user;
}

void User::setPasswd(std::string passwd)
{
	this->passwd = passwd;
	checkRegistration();
}

std::string	User::getPasswd()
{
	return this->passwd;
}

void User::setRealname(std::string realname)
{
	this->realname = realname;
}

std::string	User::getRealname()
{
	return this->realname;
}

void User::setHostname(std::string hostname)
{
	this->hostname = hostname;
}

std::string	User::getHostname()
{
	return this->hostname;
}

void User::setMode(std::string mode)
{
	this->mode = mode;
}

std::string	User::getMode()
{
	return this->mode;
}

void User::checkRegistration() 
{
	if (this->getNick() != "none" && this->getNick() != "*" && this->getUser() != "none" && this->getPasswd() != "none")
		this->isRegistered = 1;
	else
		this->isRegistered = 0;
}

void User::setIsRegistered(int isRegistered)
{
	this->isRegistered = isRegistered;
}

int User::getIsRegistered()
{
	return this->isRegistered;
}

std::ostream& operator<<(std::ostream& o, User &src) 
{
	o << "User = " << src.getUser() << ", Nick = " << src.getNick() << ", FD = " << src.getFd();
	return o;
}

void	User::addInvitedChannel(std::string channelName)
{
	this->invitedChannels.push_back(channelName);
}

void User::rmInvitedChannel(std::string channelName)
{
	std::vector<std::string>::iterator it = invitedChannels.begin();
	while (it != invitedChannels.end())
	{
		if (*it == channelName)
			it = invitedChannels.erase(it);
		else
			++it;
	}
}

bool	User::isInvited(std::string channelName) const
{
	std::vector<std::string>::const_iterator it;
	for (it = this->invitedChannels.begin(); it != this->invitedChannels.end(); ++it)
	{
		if (*it == channelName)
			return true;
	}
	return false;
}

void	User::addJoinedChannel(Channel *channel)
{
	this->joinedChannels.push_back(channel);
}

void	User::removeJoinedChannel(Channel *channel)
{
	std::vector<Channel *>::iterator it;
	for (it = this->joinedChannels.begin(); it != this->joinedChannels.end(); ++it)
	{
		if (*it == channel)
		{
			this->joinedChannels.erase(it);
			return;
		}
	}
}

std::vector<Channel *>	User::getJoinedChannels()
{
	return this->joinedChannels;
}

void	User::sendAllJoinedChannels(std::string msg)
{
	if (this->joinedChannels.size() == 0)
		return;
	else
	{
		for (int i = 0; i != static_cast<int>(this->joinedChannels.size()); i++)
		{
			std::vector<User *> channelUsers = this->joinedChannels[i]->getUsers();
			for (int i = 0; i < static_cast<int>(channelUsers.size()); i++)
			{
				if (channelUsers[i]->getNick() != this->nick)
					send(channelUsers[i]->getFd(), msg.c_str(), msg.length(), 0);
			}
		}
	}
}

void	User::addOperatorChannel(std::string channelName)
{
	this->operatorChannels.push_back(channelName);
}

void	User::rmOperatorChannel(std::string channelName)
{
	for (std::vector<std::string>::iterator it = operatorChannels.begin(); it != operatorChannels.end(); ++it)
	{
		if (*it == channelName)
			it = operatorChannels.erase(it);
	}
}

bool	User::isOperator(std::string channelName) const
{
	std::vector<std::string>::const_iterator it;
	for (it = this->operatorChannels.begin(); it != this->operatorChannels.end(); ++it)
	{
		if (*it == channelName)
			return true;
	}
	return false;
}

void	User::setDidPass()
{
	this->didPass = true;
}

bool	User::getDidPass()
{
	return this->didPass;
}