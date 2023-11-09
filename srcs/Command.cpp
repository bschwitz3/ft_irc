#include "Command.hpp"
/* #include "User.hpp"
#include "Server.hpp"
#include "Msg.hpp" */

Command::Command(Server &server, std::string prefix, std::string command, std::vector<std::string> params)
: server(server), prefix(prefix), command(command), params(params)
{}

Command::Command(Server &server)
: server(server)
{}

const Command::CmdFunc Command::cmdArr[] = {&Command::cap,
											&Command::join,
											&Command::pass,
											&Command::ping,
											&Command::nick,
											&Command::user,
											&Command::mode,
											&Command::privmsg,
											&Command::topic,
											&Command::invite,
											&Command::kick};
Command::~Command() {}

void		Command::setPrefix(std::string prefix)
{
	this->prefix = prefix;
}

std::string	Command::getPrefix()
{
	return this->prefix;
}

void	Command::setCommand(std::string command)
{
	this->command = command;
}

void	Command::setParams(std::vector<std::string> params)
{
	this->params = params;
}
std::string	Command::getCommand()
{
	return this->command;
}

std::string	Command::getParams()
{
	std::string params;

	for (std::vector<std::string>::iterator it = this->params.begin(); it != this->params.end(); ++it)
	{
		params += *it;
		params += " + ";
	}
	return params;
}

void 	Command::execute(User &user)
{
	static const std::string arr[] = {"CAP", "JOIN", "PASS", "PING", "NICK", "USER", "MODE", "PRIVMSG", "TOPIC", "INVITE", "KICK"};
	if (this->command == "PASS")
	{
		(this->*cmdArr[2])(user, this->prefix, this->params);
			return;
	}
	if (user.getDidPass() == true)
	{
		for (size_t i = 0; i < 11; i++)
		{
			if (this->command == arr[i])
			{
				(this->*cmdArr[i])(user, this->prefix, this->params);
				return;
			}
		}
	}
}

// ICI
void	Command::privmsg(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	Channel *chan = server.getChannel(params[0]);
	std::string	source = user.getNick();
	std::string	message;
	if (params.size() >= 2)
		message = params[params.size() - 1];
	if (!server.isNickAvailable(params[0]))
	{
		User	*usr= server.getUserByNick(params[0]);
		if (usr)
			send(usr->getFd(), RPL_PRIVMSG(user.getNick(), user.getUser(), usr->getNick(), message).c_str(), RPL_PRIVMSG(user.getNick(), user.getUser(), usr->getNick(), message).length(), 0);
	}
	else if (chan)
	{
		if (!chan->isUser(user))
		{
			send(user.getFd(), ERR_NOTONCHANNEL(user.getNick(), user.getNick(), params[0]).c_str(), ERR_NOTONCHANNEL(user.getNick(), user.getNick(), params[0]).length(), 0);
			return;
		}
		std::vector<User *> users = chan->getUsers();
		for (size_t i = 0; i < users.size(); i++)
		{
			if (user.getNick() != users[i]->getNick())
				send(users[i]->getFd(), RPL_PRIVMSG(user.getNick(), user.getUser(), chan->getName(), message).c_str(), RPL_PRIVMSG(user.getNick(), user.getUser(), chan->getName(), message).length(), 0);
		}
	}
	else
		send(user.getFd(), ERR_NOSUCHNICK(user.getNick(), params[0]).c_str(), ERR_NOSUCHNICK(user.getNick(), params[0]).length(), 0);
}

void	Command::ping(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	std::string pong;
	if (user.getIsAlive() == true)
	{
		if (params.size() > 0) 
			send(user.getFd(), RPL_PONG(user.getNick(), user.getUser(), params[0]).c_str(), RPL_PONG(user.getNick(), user.getUser(), params[0]).length(), 0);
		else 
			send(user.getFd(), RPL_PONG(user.getNick(), user.getUser(), ":").c_str(), RPL_PONG(user.getNick(), user.getUser(), ":").length(), 0);
	}
}

void	Command::nick(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	std::string oldNick = user.getNick();
	std::string nick = params[0];
	if (params.size() != 1)
	{
		send(user.getFd(), ERR_NEEDMOREPARAMS(user.getNick(), "NICK").c_str(), ERR_NEEDMOREPARAMS(user.getNick(), "NICK").length(), 0);
		return ;
	}
	if (nick.length() > 9)
	{
		send(user.getFd(), ERR_ERRONEUSNICKNAME(user.getNick(), nick).c_str(), ERR_ERRONEUSNICKNAME(user.getNick(), nick).length(), 0);
		return ;
	}
	std::string originalNick = nick;
	if (!server.isNickAvailable(nick))
	{
		send(user.getFd(), ERR_NICKNAMEINUSE(user.getNick(), nick).c_str(), ERR_NICKNAMEINUSE(user.getNick(), nick).length(), 0);
		return ;
	}
	if (server.isNickAvailable(nick) && user.getIsRegistered() == 0)
	{
		user.setNick(nick);
		user.checkRegistration();
	}
	if (user.getIsRegistered() == 1)
	{
		send(user.getFd(), RPL_WELCOME(user.getUser(), user.getNick()).c_str(), RPL_WELCOME(user.getUser(), user.getNick()).length(), 0);
		user.setIsRegistered(2);
	}
	else if (user.getIsRegistered() == 2)
	{
		server.getUserByNick(user.getNick())->setNick(nick);
		send(user.getFd(), RPL_NICK(oldNick, user.getUser(), user.getNick()).c_str(), RPL_NICK(oldNick, user.getUser(), user.getNick()).length(), 0);
		sendToAllJoinedChannel(user, RPL_NICK(oldNick, user.getUser(), user.getNick()));
	}
}

void	Command::user(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	if (params.size() != 4)
	{
		send(user.getFd(), ERR_NEEDMOREPARAMS(user.getNick(), "USER").c_str(), ERR_NEEDMOREPARAMS(user.getNick(), "USER").length(), 0);
		return ;
	}
	user.setUser(params[0]);
	user.setMode(params[1]);
	user.setHostname(params[2]);
	user.setRealname(params[3]);
	if (user.getIsRegistered() == 1)
	{
		send(user.getFd(), RPL_WELCOME(user.getUser(), user.getNick()).c_str(), RPL_WELCOME(user.getUser(), user.getNick()).length(), 0);
		user.setIsRegistered(2);
	}
}

void	Command::pass(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	if (params.size() != 1)
	{
		send(user.getFd(), ERR_NEEDMOREPARAMS(user.getNick(), "PASS").c_str(), ERR_NEEDMOREPARAMS(user.getNick(), "PASS").length(), 0);
		int pf = server.getPfdsFD(server.getPfdsIndex(user.getFd()));
		shutdown(user.getFd(), SHUT_RDWR);
		close(pf);
		server.del_from_pfds(server.getPfdsIndex(user.getFd()));
		return ;
	}
 	if (user.getIsRegistered() >= 2)
	{
		send(user.getFd(), ERR_ALREADYREGISTERED(user.getNick()).c_str(), ERR_ALREADYREGISTERED(user.getNick()).length(), 0);
		return ;
	}
 	if (params[0] != server.getPasswd())
		return ;
	user.setDidPass();
	user.setPasswd(params[0]);
	if (user.getIsRegistered() == 1)
	{
		send(user.getFd(), RPL_WELCOME(user.getUser(), user.getNick()).c_str(), RPL_WELCOME(user.getUser(), user.getNick()).length(), 0);
		user.setIsRegistered(2);
	}
}

void	Command::cap(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	if (params.size() <= 0)
	{
		send(user.getFd(), ERR_NEEDMOREPARAMS(user.getNick(), "CAP").c_str(), ERR_NEEDMOREPARAMS(user.getNick(), "CAP").length(), 0);
		return ;
	}
	std::string subcommand = params[0];
	std::string cap = params[1];
	if (subcommand == "LS")
	{
		std::string cap_end = ":localhost CAP * ACK :none\r\n";
		send(user.getFd(), cap_end.c_str(), cap_end.length(), 0);
	}
	else if (subcommand == "REQ")
	{
		std::string cap_end = ":localhost CAP * ACK :multi-prefix\r\n";
		send(user.getFd(), cap_end.c_str(), cap_end.length(), 0);
	}
	else if (subcommand == "END")
	{
		std::string cap_end = ":localhost CAP * ACK :none\r\n";
		send(user.getFd(), cap_end.c_str(), cap_end.length(), 0);
	}
	else
	{
		std::string err = ":server 410 " + user.getNick() + " :Invalid CAP subcommand\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
}


void	Command::join(User &user, std::string prefix, std::vector<std::string> params)
{
	// checks
	(void) prefix;
	if (params.size() < 1)
	{
		send(user.getFd(), ERR_NEEDMOREPARAMS(user.getNick(), "JOIN").c_str(), ERR_NEEDMOREPARAMS(user.getNick(), "JOIN").length(), 0);
		return ;
	}
	std::string channel = params[0];
	if (channel.c_str()[0] != '#')
	{
		send(user.getFd(), ERR_NOSUCHCHANNEL(user.getNick(), channel).c_str(), ERR_NOSUCHCHANNEL(user.getNick(), channel).length(), 0);
		return ;
	}
	// No channel
	if (server.getChannel(channel) == nullptr)
	{
		// Create
		server.createChannel(channel, &user);
		this->_channel = server.getChannel(channel);
		user.addJoinedChannel(this->_channel);
		// RPL
		std::vector<User *> channelUsers = this->_channel->getUsers();
		sendChannelUsers(channelUsers, RPL_JOIN(user.getNick(), user.getUser(), channel), user);
		if (this->_channel->getTopic() != "")
			send(user.getFd(), RPL_TOPIC(user.getNick(), channel, this->_channel->getTopic()).c_str(), RPL_TOPIC(user.getNick(), channel, this->_channel->getTopic()).length(), 0);
		else
			send(user.getFd(), RPL_NOTOPIC(user.getNick(), channel).c_str(), RPL_NOTOPIC(user.getNick(), channel).length(), 0);
		send(user.getFd(), RPL_NAMREPLY(user.getNick(), channel, this->_channel->getList()).c_str(), RPL_NAMREPLY(user.getNick(), channel, this->_channel->getList()).length(), 0);
		send(user.getFd(), RPL_ENDOFNAMES(user.getNick(), channel).c_str(), RPL_ENDOFNAMES(user.getNick(), channel).length(), 0);
		return;
	}
	// join active channel
	if (server.getChannel(channel) != nullptr)
	{
		this->_channel = server.getChannel(channel);
		if (this->_channel->getModeI() && !user.isInvited(this->_channel->getName()))
		{
			send(user.getFd(), ERR_BANNEDFROMCHAN(user.getNick(), channel, "+i").c_str(), ERR_BANNEDFROMCHAN(user.getNick(), channel, "+i").length(), 0);
			return;
		}
		else
		{
			if (this->_channel->getModeK() && (params.size() < 2 || strcmp(params[1].c_str(), this->_channel->getPassword().c_str()) != 0))
			{
				send(user.getFd(), ERR_BADCHANNELKEY(user.getNick(), channel).c_str(), ERR_BADCHANNELKEY(user.getNick(), channel).length(), 0);
				return;
			}
			else
			{
				if (this->_channel->getSize() >= this->_channel->getMax())
				{
					send(user.getFd(), ERR_CHANNELISFULL(user.getNick(), channel).c_str(), ERR_CHANNELISFULL(user.getNick(), channel).length(), 0);
					return;
				}
				else
				{
					if (server.getChannel(params[0])->isKicked(user))
					{
						std::string err = "474 " + user.getNick() + " " + this->_channel->getName() + " :Cannot join channel after being kicked\r\n";
						send(user.getFd(), err.c_str(), err.length(), 0);
						return;
					}
					else
					{
						// adduser
						this->_channel->addUser(user);
						user.addJoinedChannel(this->_channel);
						// RPL
						std::vector<User *> channelUsers = this->_channel->getUsers(); 
						sendChannelUsers(channelUsers, RPL_JOIN(user.getNick(), user.getUser(), channel), user);
						if (this->_channel->getTopic() != "")
							send(user.getFd(), RPL_TOPIC(user.getNick(), channel, this->_channel->getTopic()).c_str(), RPL_TOPIC(user.getNick(), channel, this->_channel->getTopic()).length(), 0);
						else
							send(user.getFd(), RPL_NOTOPIC(user.getNick(), channel).c_str(), RPL_NOTOPIC(user.getNick(), channel).length(), 0);
						send(user.getFd(), RPL_NAMREPLY(user.getNick(), channel, this->_channel->getList()).c_str(), RPL_NAMREPLY(user.getNick(), channel, this->_channel->getList()).length(), 0);
						send(user.getFd(), RPL_ENDOFNAMES(user.getNick(), channel).c_str(), RPL_ENDOFNAMES(user.getNick(), channel).length(), 0);
					}
				}
			}
		}
	}
}

void Command::handleData(User &user, const std::string& data)
{
	buffer += data;
	std::size_t pos;
	while ((pos = buffer.find("\r\n")) != std::string::npos)
	{
		std::string line = buffer.substr(0, pos);
		buffer = buffer.substr(pos + 2);
		parseLine(user, line);
	}
}

void Command::parseLine(User &user, std::string line)
{
	std::istringstream iss(line);
	std::string prefix, command;
	std::vector<std::string> params;
	if (line[0] == ':')
		iss >> prefix;
	iss >> command;
	std::string param;
	std::string lastWord;
	while (iss >> param)
	{
		if (param[0] == ':')
		{
			lastWord = param.substr(1);
			std::string remainder;
			getline(iss, remainder);
			lastWord += remainder;
			params.push_back(lastWord);
			break;
		}
		else
		{
			params.push_back(param);
		}
	}
	Command cmd(server, prefix, command, params);
	cmd.execute(user);
}

void	Command::who(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	(void)params;
	(void)user;
	server.displayUsers();
}

void	Command::mode(User &user, std::string prefix, std::vector<std::string> params)
{
	////// GENERAL CHECKS ////////////////////////////////////////////////////////////////////////////////////
	(void)prefix;
	if (params.size() < 2)
	{
		send(user.getFd(), ERR_NEEDMOREPARAMS(user.getNick(), "MODE").c_str(), ERR_NEEDMOREPARAMS(user.getNick(), "MODE").length(), 0);
		return ;
	}
	/// pas sur, c'etait pour eviter les message random d'irssi /////
	if (params[0] == user.getNick())
		return;
	if (params[1] == "b")
		return;
	/////////////////////////////////////////////////////////////////
	std::string channel = params[0];
	if (server.getChannel(channel) == nullptr)
	{
		send(user.getFd(), ERR_NOSUCHCHANNEL(user.getNick(), channel).c_str(), ERR_NOSUCHCHANNEL(user.getNick(), channel).length(), 0);
		return ;
	}
	this->_channel = server.getChannel(channel);
	if (!server.getChannel(channel)->isChanops(user))
	{
		send(user.getFd(), ERR_CHANOPRIVSNEEDED(user.getNick(), channel).c_str(), ERR_CHANOPRIVSNEEDED(user.getNick(), channel).length(), 0);
		return ;
	}
	if ((params[1][0] != '+' && params[1][0] != '-')
			|| (params[1][1] != 'i' && params[1][1] != 't' && params[1][1] != 'k'
			&& params[1][1] != 'o' && params[1][1] != 'l'))
	{
		send(user.getFd(), ERR_UMODEUNKNOWNFLAG(user.getNick()).c_str(), ERR_UMODEUNKNOWNFLAG(user.getNick()).length(), 0);
		return;
	}
	if ((params[1][1] == 'o' || (params[1][0] == '+' && (params[1][1] == 'k' || params[1][1] == 'l'))) && params.size() < 3)
	{
		send(user.getFd(), ERR_NEEDMOREPARAMS(user.getNick(), "MODE").c_str(), ERR_NEEDMOREPARAMS(user.getNick(), "MODE").length(), 0);
		return ;
	}
	std::vector<User *> channelUsers = this->_channel->getUsers();
	///// OPTION +o ///////////////////////////////////////////////////////////////////////////////////////////
	if (params[1][1] == 'o')
	{
		User *paramUser = server.getUserByNick(params[2]);
		if (!this->_channel->isUser(*paramUser))
		{
			send(user.getFd(), ERR_USERNOTINCHANNEL(user.getNick(), paramUser->getNick(), channel).c_str(), ERR_USERNOTINCHANNEL(user.getNick(), paramUser->getNick(), channel).length(), 0);
			return;
		}
		else
		{
			if (params[1][0] == '+')
			{
				this->_channel->addChanops(*paramUser, user);
				paramUser->addOperatorChannel(this->_channel->getName());
				std::string msg = params[2] + " is now operator in " + params[0] + "\r\n";
				sendChannelUsersAndMe(channelUsers, msg, user);
				return;
			}
			if (params[1][0] == '-')
			{
				this->_channel->rmChanops(*paramUser);
				paramUser->rmOperatorChannel(params[0]);
				std::string msg = params[2] + " is not operator anymore in " + params[0] + "\r\n";
				sendChannelUsersAndMe(channelUsers, msg, user);
				return;
			}

		}
	}
	//////// OPTION +k / +l /////////////////////////////////////////////////////////////////////////////
	if (params[1][0] == '+' && (params[1][1] == 'k' || params[1][1] == 'l'))
	{
		if (params[1][1] == 'k')
		{
			if (!checkPassword(params[2]))
			{
				send(user.getFd(), ERR_PASSWDMISMATCH(user.getNick()).c_str(), ERR_PASSWDMISMATCH(user.getNick()).length(), 0);
				return ;
			}
			this->_channel->setRmMode("+k");
			this->_channel->setPassword(params[2]);
			std::string msg = ":localhost 324 " + user.getNick() + " " + this->_channel->getName() + " +k " + params[2] + "\r\n";
			sendChannelUsersAndMe(channelUsers, msg, user);
			return;
		}
		if (params[1][1] == 'l')
		{
			if (params[1][0] == '+')
			{
				int num = std::atoi(params[2].c_str());
				if (num < static_cast<int>(this->_channel->getUsers().size()) || num > INT_MAX || num == 0)
				{
					send(user.getFd(), ERR_INVALIDMODEPARAM(user.getNick(), channel, "+l").c_str(), ERR_INVALIDMODEPARAM(user.getNick(), channel, "+l").length(), 0);
					return ;
				}
				this->_channel->setRmMode("+l");
				this->_channel->setMax(num);
				std::string msg = user.getNick() + " sets a new limit of users on " + this->_channel->getName() + " : " + params[2] + "\r\n";
				sendChannelUsersAndMe(channelUsers, msg, user);
				return;
			}
		}
	}
	if (params[1][0] == '-' && (params[1][1] == 'k' || params[1][1] == 'l'))
	{
		if (params[1][1] == 'k')
		{
			this->_channel->setPassword("");
			this->_channel->setRmMode("-k");
			std::string msg = user.getNick() + " removes key of " + this->_channel->getName() + "\r\n";
			sendChannelUsersAndMe(channelUsers, msg, user);
			return;
		}
		if (params[1][1] == 'l')
		{
			this->_channel->setMax(INT_MAX);
			this->_channel->setRmMode("-l");
			std::string msg = user.getNick() + " removes limit of user on " + this->_channel->getName() + "\r\n";
			sendChannelUsersAndMe(channelUsers, msg, user);
			return;
		}
	}
	///////////// OPTION +i ////////////////////////////////////////////////////////////////////////////////
	if (params[1][1] == 'i')
	{
		if (params[1][0] == '-')
		{
			this->_channel->setRmMode("-i");
			for (std::vector<User *>::iterator it = channelUsers.begin(); it != channelUsers.end(); ++it)
			{
				(*it)->rmInvitedChannel(this->_channel->getName());
			}
			std::string msg = user.getNick() + " removes the invited-only mode on " + this->_channel->getName() + "\r\n";
			sendChannelUsersAndMe(channelUsers, msg, user);
			return;
		}
		if (params[1][0] == '+')
		{
			this->_channel->setRmMode("+i");
			for (std::vector<User *>::iterator it = channelUsers.begin(); it != channelUsers.end(); ++it)
			{
				(*it)->addInvitedChannel(this->_channel->getName());
			}
			std::string msg = user.getNick() + " sets the invited-only mode on " + this->_channel->getName() + "\r\n";
			sendChannelUsersAndMe(channelUsers, msg, user);
			return;
		}
	}
	////////// OPTION +t //////////////////////////////////////////////////////////////////////////////////
	if (params[1][1] == 't')
	{
		if (params[1][0] == '-')
		{
			this->_channel->setRmMode("-t");
			std::string msg = user.getNick() + " removes the protected topic mode on " + this->_channel->getName() + "\r\n";
			sendChannelUsersAndMe(channelUsers, msg, user);
			return;
		}
		if (params[1][0] == '+')
		{
			this->_channel->setRmMode("+t");
			std::string msg = user.getNick() + " sets the protected topic mode on " + this->_channel->getName() + "\r\n";
			sendChannelUsersAndMe(channelUsers, msg, user);
			return;
		}
	}
	else
		return;
}

void Command::sendToAllJoinedChannel(User &user, std::string msg)
{
	std::set<User*> usersAlreadyNotified;
	for (unsigned long i = 0; i < user.getJoinedChannels().size(); i++)
	{
		if (user.getJoinedChannels()[i])
		{
			const std::vector<User*>& channelUsers = user.getJoinedChannels()[i]->getUsers();
			for (size_t j = 0; j < channelUsers.size(); j++)
			{
				User* channelUser = channelUsers[j];
				if (usersAlreadyNotified.find(channelUser) == usersAlreadyNotified.end())
				{
					send(channelUser->getFd(), msg.c_str(), msg.length(), 0);
					usersAlreadyNotified.insert(channelUser);
				}
			}
		}
	}
}

void Command::sendChannelUsers(std::vector<User *> channelUsers, std::string msg, User &user) const
{
	for (unsigned long i = 0; i < channelUsers.size(); i++)
	{
		if (channelUsers[i]->getNick() != user.getNick())
			send(channelUsers[i]->getFd(), msg.c_str(), msg.length(), 0);
	}
}

void Command::sendChannelUsersAndMe(std::vector<User *> channelUsers, std::string msg, User &user) const
{
	(void) user;
	for (unsigned long i = 0; i < channelUsers.size(); i++)
		send(channelUsers[i]->getFd(), msg.c_str(), msg.length(), 0);
}

bool	checkPassword(std::string passWord)
{
	if (passWord.length() > 20)
		return false;
	for (size_t i = 0; i < passWord.length(); ++i)
	{
		char c = passWord[i];
		if (!isalnum(c) && !ispunct(c))
			return false;
		if (isspace(c))
			return false;
	}
	return true;
}

void	Command::topic(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	if (params.size() < 1)
	{
		send(user.getFd(), ERR_NEEDMOREPARAMS(user.getNick(), "TOPIC").c_str(), ERR_NEEDMOREPARAMS(user.getNick(), "MODE").length(), 0);
		return ;
	}
	std::string channel = params[0];
	if (server.getChannel(channel) == nullptr)
	{
		send(user.getFd(), ERR_NOSUCHCHANNEL(user.getNick(), channel).c_str(), ERR_NOSUCHCHANNEL(user.getNick(), channel).length(), 0);
		return ;
	}
	if (!server.getChannel(channel)->isUser(user))
	{
		send(user.getFd(), ERR_USERNOTINCHANNEL(user.getNick(), user.getNick(), channel).c_str(), ERR_USERNOTINCHANNEL(user.getNick(), user.getNick(), channel).length(), 0);
		return;
	}
	if (params.size() == 1)
	{
		if (server.getChannel(channel)->getTopic() == "")
		{
			send(user.getFd(), RPL_NOTOPIC(user.getNick(), channel).c_str(), RPL_NOTOPIC(user.getNick(), channel).length(), 0);
			return;
		}
		else
		{
			send(user.getFd(), RPL_TOPIC(user.getNick(), channel, server.getChannel(channel)->getTopic()).c_str(), RPL_TOPIC(user.getNick(), channel, server.getChannel(channel)->getTopic()).length(), 0);
			return;
		}
	}
	else
	{
		if (!server.getChannel(channel)->isChanops(user))
		{
			send(user.getFd(), ERR_CHANOPRIVSNEEDED(user.getNick(), channel).c_str(), ERR_CHANOPRIVSNEEDED(user.getNick(), channel).length(), 0);
			return ;
		}
		if (params.size() > 1)
		{
			if (params[1][0] != ':')
			{
				send(user.getFd(), ERR_NEEDMOREPARAMS(user.getNick(), "TOPIC").c_str(), ERR_NEEDMOREPARAMS(user.getNick(), "MODE").length(), 0);
				return ;
			}
			if (params[1] == ":")
			{
				server.getChannel(channel)->setRmMode("-t");
				server.getChannel(channel)->setTopic("");
				sendChannelUsersAndMe(server.getChannel(channel)->getUsers(), RPL_NOTOPIC(user.getNick(), channel), user);
				return;
			}
			else
			{
				server.getChannel(channel)->setRmMode("+t");
				std::string topic = "";
				for (size_t i = 1; i < params.size(); i++)
					topic += (params[i] + " ");
				server.getChannel(channel)->setTopic(topic);
				sendChannelUsersAndMe(server.getChannel(channel)->getUsers(), RPL_TOPIC(user.getNick(), channel, server.getChannel(channel)->getTopic()), user);
				return;
			}
		}
	}
}


void	Command::invite(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	if (params.size() != 2)
	{
		send(user.getFd(), ERR_NEEDMOREPARAMS(user.getNick(), "TOPIC").c_str(), ERR_NEEDMOREPARAMS(user.getNick(), "MODE").length(), 0);
		return ;
	}
	// check si p[0] = user
	if (!server.getUserByNick(params[0]))
	{
		send(user.getFd(), ERR_NOSUCHNICK(user.getNick(), params[0]).c_str(), ERR_NOSUCHNICK(user.getNick(), params[0]).length(), 0);
		return;
	}
	// check si p[1] = channel
	if (server.getChannel(params[1]) == nullptr)
	{
		send(user.getFd(), ERR_NOSUCHCHANNEL(user.getNick(), params[1]).c_str(), ERR_NOSUCHCHANNEL(user.getNick(), params[1]).length(), 0);
		return ;
	}
	else
	{
		// check si user est dans chan
		if (!server.getChannel(params[1])->isUser(user))
		{
			send(user.getFd(), ERR_USERNOTINCHANNEL(user.getNick(), user.getNick(), params[1]).c_str(), ERR_USERNOTINCHANNEL(user.getNick(), user.getNick(), params[1]).length(), 0);
			return;
		}
		else
		{
			// check si user est chanops
			if (!server.getChannel(params[1])->isChanops(user))
			{
				send(user.getFd(), ERR_CHANOPRIVSNEEDED(user.getNick(), params[1]).c_str(), ERR_CHANOPRIVSNEEDED(user.getNick(), params[1]).length(), 0);
				return ;
			}
			else
			{
				//check si userparam est kick
				if (server.getChannel(params[1])->isKicked(*server.getUserByNick(params[0])))
				{
					// jsp l'erreur ERR_
					return;
				}
				else
				{
					//check si userparam est dans chan
					if (server.getChannel(params[1])->isUser(*server.getUserByNick(params[0])))
					{
						send(user.getFd(), ERR_USERONCHANNEL(user.getNick(), params[0], params[1]).c_str(), ERR_USERONCHANNEL(user.getNick(), params[0], params[1]).length(), 0);
						return ;
					}
					else
					{
						//check si userparam est deja invite
						if (!server.getUserByNick(params[0])->isInvited(params[1]))
							server.getUserByNick(params[0])->addInvitedChannel(params[1]);
						sendChannelUsersAndMe(server.getChannel(params[1])->getUsers(), RPL_INVITE(user.getNick(), user.getUser(), server.getUserByNick(params[0])->getNick(), params[1]), user);
						return;
					}
				}
			}
		}
	}
}

void	Command::kick(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	if (params.size() < 2)
	{
		send(user.getFd(), ERR_NEEDMOREPARAMS(user.getNick(), "KICK").c_str(), ERR_NEEDMOREPARAMS(user.getNick(), "MODE").length(), 0);
		return ;
	}
	// check si p[0] = channel
	if (server.getChannel(params[0]) == nullptr)
	{
		send(user.getFd(), ERR_NOSUCHCHANNEL(user.getNick(), params[0]).c_str(), ERR_NOSUCHCHANNEL(user.getNick(), params[0]).length(), 0);
		return ;
	}
	// check si p[1] = user
	if (!server.getUserByNick(params[1])->getIsRegistered())
	{
		send(user.getFd(), ERR_NOSUCHNICK(user.getNick(), params[1]).c_str(), ERR_NOSUCHNICK(user.getNick(), params[1]).length(), 0);
		return;
	}
	else
	{
		// check si user est dans chan
		if (!server.getChannel(params[0])->isUser(user))
		{
			send(user.getFd(), ERR_USERNOTINCHANNEL(user.getNick(), user.getNick(), params[0]).c_str(), ERR_USERNOTINCHANNEL(user.getNick(), user.getNick(), params[0]).length(), 0);
			return;
		}
		else
		{
			// check si user est chanops
			if (!server.getChannel(params[0])->isChanops(user))
			{
				send(user.getFd(), ERR_CHANOPRIVSNEEDED(user.getNick(), params[0]).c_str(), ERR_CHANOPRIVSNEEDED(user.getNick(), params[0]).length(), 0);
				return ;
			}
			else
			{
				//check si userparam est dans chan
				if (!server.getChannel(params[0])->isUser(*server.getUserByNick(params[1])))
				{
					send(user.getFd(), ERR_NOTONCHANNEL(user.getNick(), params[1], params[0]).c_str(), ERR_NOTONCHANNEL(user.getNick(), params[1], params[0]).length(), 0);
					return ;
				}
				else
				{
					//check si userparam est deja kick
					if (!server.getChannel(params[0])->isKicked(*server.getUserByNick(params[1])))
					{
						if (server.getChannel(params[0])->isChanops(*server.getUserByNick(params[1])))
							server.getChannel(params[0])->kickChanops(*server.getUserByNick(params[1]), user);
						server.getChannel(params[0])->kickUser(*server.getUserByNick(params[1]), user);
						server.getUserByNick(params[1])->removeJoinedChannel(server.getChannel(params[0]));
					}
					std::string reason = "";
					if (params.size() > 2)
					{
						for (size_t i = 2; i < params.size(); i++)
							reason += (params[i] + " ");
					}
					sendChannelUsersAndMe(server.getChannel(params[0])->getUsers(), RPL_KICK(user.getNick(), user.getUser(), params[0], server.getUserByNick(params[1])->getNick(), reason), user);
					return;
				}
			}
		}
	}
}