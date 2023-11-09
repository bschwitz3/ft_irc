#include "Channel.hpp"

//////// Constructor / Destructors ////////////////////////////////////////////

Channel::Channel(std::string name, User	*user)
{
	this->_users.push_back(user);
	this->_chanops.push_back(user);
	this->_name = name;
	this->_topic = "";
	this->_mode.i = false;
	this->_mode.t = false;
	this->_mode.k = false;
	this->_mode.l = false;
	this->_max = INT_MAX;
	this->_password = "";
}

Channel::~Channel()
{}

//// operators /////////////////////////////////////////////////////////////////

bool	Channel::operator==(const Channel& other) const
{
	return this->_name == other._name;
}

Channel&	Channel::operator=(const Channel& other)
{
	if (this == &other)
		return *this;
	this->_name = other._name;
	this->_topic = other._topic;
	this->_mode = other._mode;
	this->_max = other._max;
	this->_password = other._password;
	this->_users = other._users;
	this->_chanops = other._chanops;
	this->_kickedUsers = other._kickedUsers;
	return *this;
}


/////// getters  //////////////////////////////////////////////////////////////

std::string			Channel::getName() const
{
	return this->_name;
}

std::string			Channel::getTopic() const
{
	return this->_topic;
}

Mode				Channel::getMode() const
{
	return this->_mode;
}

bool				Channel::getModeI() const
{
	return this->_mode.i;
}

bool				Channel::getModeT() const
{
	return this->_mode.t;
}

bool				Channel::getModeK() const
{
	return this->_mode.k;
}

bool				Channel::getModeL() const
{
	return this->_mode.l;
}

int					Channel::getMax() const
{
	return this->_max;
}

std::string			Channel::getPassword() const
{
	return this->_password;
}

std::vector<User *>	Channel::getUsers() const
{
	if (!this->_users.empty())
		return this->_users;
	else
		return std::vector<User*>();
}

std::vector<User *>	Channel::getChanops() const
{
	return this->_chanops;
}

std::vector<User *>	Channel::getKickedUsers() const
{
	return this->_kickedUsers;
}

int					Channel::getSize() const
{
	return static_cast<int>(this->_users.size());
}

////// setters ////////////////////////////////////////////////////////////////

void	Channel::setName(std::string name)
{
	this->_name = name;
}

void	Channel::setTopic(std::string topic)
{
	this->_topic = topic;
}

void	Channel::setRmMode(std::string mode)
{
	if (mode[0] == '+')
	{
		switch (mode[1])
		{
			case 'i' :
				if (this->_mode.i == false)
					this->_mode.i = true;
				break;
			case 't' :
				if (this->_mode.t == false)
					this->_mode.t = true;
				break;
			case 'k' :
				if (this->_mode.k == false)
					this->_mode.k = true;
				break;
			case 'l' :
				if (this->_mode.l == false)
					this->_mode.l= true;
				break;
			default :
				break;
		}
	}
	if (mode[0] == '-')
	{
		switch (mode[1])
		{
			case 'i' :
				if (this->_mode.i == true)
					this->_mode.i = false;
				break;
			case 't' :
				if (this->_mode.t == true)
					this->_mode.t = false;
				break;
			case 'k' :
				if (this->_mode.k == true)
					this->_mode.k = false;
				break;
			case 'l' :
				if (this->_mode.l == true)
					this->_mode.l= false;
				break;
			default :
				break;
		}
	}
}

void	Channel::setMax(int max)
{
	this->_max = max;
}

void	Channel::setPassword(std::string password)
{
	this->_password = password;
}

///// others //////////////////////////////////////////////////////////////////

bool	Channel::isUser(User &user)
{
	std::vector<User *>::iterator it;
	it = std::find(_users.begin(), _users.end(), &user);

	if(it != _users.end())
		return true;
	else
		return false;
}


bool	Channel::isChanops(User &user)
{
	std::vector<User *>::iterator it;
	it = std::find(_chanops.begin(), _chanops.end(), &user);

	if(it != _chanops.end())
		return true;
	else
		return false;
}


bool	Channel::isKicked(User &user)
{
	std::vector<User *>::iterator it;
	it = std::find(_kickedUsers.begin(), _kickedUsers.end(), &user);

	if(it != _kickedUsers.end())
		return true;
	else
		return false;
}

std::string	Channel::getList()
{
	std::string res;
	
	for (std::vector<User *>::iterator it = this->_users.begin(); it != this->_users.end(); ++it)
	{
		if (this->isChanops(**it))
			res += "@";
		res += ((*it)->getNick() + " "); 
	}
	return res;
}


void	Channel::addUser(User &user)
{
	if (this->isUser(user))
	{
		send(user.getFd(), ERR_USERONCHANNEL(user.getNick(), user.getNick(), this->_name).c_str(), ERR_USERONCHANNEL(user.getNick(), user.getNick(), this->_name).length(), 0);
		return ;
	}
	else
		this->_users.push_back(&user);
}

void	Channel::inviteUser(User &user, User &chanop)
{
	if (this->isChanops(chanop))
	{
		if (!this->isUser(user) && !this->isKicked(user))
			user.addInvitedChannel(this->_name);
	}
}

void	Channel::addChanops(User &user, User &chanop)
{
	if (this->isChanops(chanop))
	{
		if (!this->isKicked(user))
		{
			if (!this->isUser(user) )
				this->_users.push_back(&user);
			if (!this->isChanops(user))
				this->_chanops.push_back(&user);
		}
	}
}

void	Channel::rmChanops(User &user)
{
	std::vector<User *>::iterator it = std::find(_chanops.begin(), _chanops.end(), &user);
	if (it != _chanops.end())
		_chanops.erase(it);
}

void	Channel::rmUser(User &user)
{
	std::vector<User *>::iterator it = std::find(_users.begin(), _users.end(), &user);
	if (it != _users.end())
		_users.erase(it);
}



void	Channel::kickUser(User &user, User &chanop)
{
	if (this->isChanops(chanop))
	{
		for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); ++it)
		{
			if (*it == &user)
			{
				this->_users.erase(it);
				this->_kickedUsers.push_back(&user);
				break;
			}
		}
		if (this->_users.empty())
			delete this;
	}
}


void	Channel::kickChanops(User &user, User &chanop)
{
	if (this->isChanops(chanop))
	{
		for (std::vector<User *>::iterator it = _chanops.begin(); it != _chanops.end(); ++it)
		{
			if (*it == &user)
			{
				_chanops.erase(it);
				break;
			}
		}
	}
}