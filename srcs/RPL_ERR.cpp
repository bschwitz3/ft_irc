#include "RPL_ERR.hpp"

std::string RPL_WELCOME(const std::string username, const std::string nickname)
{
	return (":localhost 001 " + nickname + " :Welcome to the Internet Relay Network " + ":" + nickname + "!" + username + "@localhost" + "\r\n");
}

std::string	ERR_UNKNOWNCOMMAND(const std::string client, const std::string command)
{
	return (":localhost 421 " + client + " " + command + " :Unknown command\r\n");
}

// INVITE
std::string	ERR_NEEDMOREPARAMS(const std::string client, const std::string command)
{
	return (":localhost 461 " + client + " " + command + " :Not enough parameters.\r\n");
}

std::string	ERR_NOSUCHCHANNEL(const std::string client, const std::string channel)
{
	return (":localhost 403 " + client + " " + channel + " :No such channel\r\n");
}

std::string	ERR_NOTONCHANNEL(const std::string client, const std::string nick, const std::string channel)
{
	return (":localhost 442 " + client + " " + nick + " " + channel + " :The user is not on this channel.\r\n");
}

std::string	ERR_USERONCHANNEL(const std::string client, const std::string nick, const std::string channel)
{
	return (":localhost 443 " + client + " " + nick + " " + channel + " :Is already on channel\r\n");
}

std::string	RPL_INVITING(const std::string client, const std::string username, const std::string nickname, const std::string channel)
{
	return (":" + nickname + "!" + username + "@localhost" + " 341 " + client + " " + nickname + " " + channel + "\r\n");
}

std::string	RPL_INVITE(std::string nickname, const std::string username, const std::string invited, const std::string channel)
{
	return (":" + nickname + "!" + username + "@localhost" + " INVITE " + invited + " " + channel + "\r\n");
}

// JOIN
std::string	RPL_JOIN(const std::string nickname, const std::string username, const std::string channel)
{
	return (":" + nickname + "!" + username + "@localhost" + " JOIN :" +  channel + "\r\n");
}

std::string	ERR_BANNEDFROMCHAN(const std::string client, const std::string channel, const std::string mode)
{
	return (":localhost 474 " + client + " " + channel + " :Cannot join channel " + mode + "\r\n");
}

std::string	ERR_BADCHANNELKEY(const std::string client, const std::string channel)
{
	return (":localhost 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n");
}

// KICK
std::string ERR_USERNOTINCHANNEL(const std::string client, const std::string nickname, const std::string channel)
{
	return (":localhost 441 " + client + " " + nickname + " " + channel + " :They aren't on that channel\r\n");
}

std::string	ERR_CHANOPRIVSNEEDED(const std::string client, const std::string channel)
{
	return (":localhost 482 " + client + " " +  channel + " :You're not channel operator\r\n");
}

std::string RPL_KICK(const std::string nickname, const std::string username, const std::string channel, const std::string kicked, const std::string reason)
{
	return (":" + nickname + "!" + username + "@localhost" + " KICK " + channel + " " + kicked + " " + reason + "\r\n");
}

// MODE
/* user mode */
std::string	MODE_USERMSG(const std::string client, const std::string mode)
{
	return (":" + client + " MODE " + client + " :" + mode + "\r\n");
}

std::string	ERR_UMODEUNKNOWNFLAG(const std::string client)
{
	return (":localhost 501 " + client + " :Unknown MODE flag\r\n");
}

std::string	ERR_USERSDONTMATCH(const std::string client)
{
	return (":localhost 502 " + client + " :Cant change mode for other users\r\n");
}

std::string	RPL_UMODEIS(const std::string client, const std::string mode)
{
	return (":localhost 221 " + client + " " + mode + "\r\n");
}

/* channel mode */
std::string	MODE_CHANNELMSG(const std::string channel, const std::string mode)
{
	return (":localhost MODE " + channel + " " + mode + "\r\n");
}


std::string	MODE_CHANNELMSGWITHPARAM(const std::string channel, const std::string mode, const std::string param)
{
	return (":localhost MODE " + channel + " " + mode + " " + param + "\r\n");
}

std::string	RPL_CHANNELMODEIS(const std::string client, const std::string channel, const std::string mode)
{
	return (":localhost 324 " + client + " " + channel + " " + mode + "\r\n");
}

std::string	RPL_CHANNELMODEISWITHKEY(const std::string client, const std::string channel, const std::string mode, const std::string password)
{
	return (":localhost 324 " + client + " " + channel + " " + mode + " " + password + "\r\n");
}

std::string	ERR_CANNOTSENDTOCHAN(const std::string client, const std::string channel)
{
	return (":localhost 404 " + client + " " + channel + " :Cannot send to channel\r\n");
}

std::string	ERR_CHANNELISFULL(const std::string client, const std::string channel)
{
	return (":localhost 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n");
}

std::string	ERR_INVALIDMODEPARAM(const std::string client, const std::string channel, const std::string mode)
{
	return (":localhost 696 " + client + " " + channel + " " + mode + " : Invalid argument\r\n");
}

std::string	RPL_ADDVOICE(const std::string nickname, const std::string username, const std::string channel, const std::string mode, const std::string param)
{
	return (":" + nickname + "!" + username + "@localhost MODE " + channel + " " + mode + " " + param + "\r\n");
}

// NAMES
std::string	RPL_NAMREPLY(const std::string client, const std::string channel, const std::string list_of_nicks)
{
	return (":localhost 353 " + client + "  " + channel + " :" + list_of_nicks + "\r\n");
}

std::string	RPL_ENDOFNAMES(const std::string client, const std::string channel)
{
	return (":localhost 366 " + client + " " + channel + " :End of /NAMES list.\r\n");
}

// NICK
std::string	ERR_NONICKNAMEGIVEN(const std::string client)
{
	return (":localhost 431 " + client + " :There is no nickname.\r\n");
}

std::string	ERR_ERRONEUSNICKNAME(const std::string client, const std::string nickname)
{
	return (":localhost 432 " + client + " " + nickname + " :Erroneus nickname\r\n");
}

std::string	ERR_NICKNAMEINUSE(const std::string client, const std::string nickname)
{
	return (":localhost 433 " + client + " " + nickname + " :Nickname is already in use.\r\n");
}

std::string	RPL_NICK(const std::string oclient, const std::string uclient, const std::string client)
{
	return (":" + oclient + "!" + uclient + "@localhost NICK " +  client + "\r\n");
}

// PASS
std::string	ERR_PASSWDMISMATCH(const std::string client)
{
	return (":localhost 464 " + client + " :Password incorrect.\r\n");
}

// PING
std::string	RPL_PONG(const std::string nickname, const std::string username, const std::string token)
{
	return (":" + nickname + "!" + username + "@localhost" + " PONG " + token + "\r\n");
}

// PRIVMSG
std::string	ERR_NOSUCHNICK(const std::string client, const std::string target)
{
	return (":localhost 401 " + client + " " + target + " :No such nick/channel\r\n");
}

std::string	ERR_NORECIPIENT(const std::string client)
{
	return (":localhost 411 " + client + " :No recipient given PRIVMSG\r\n");
}

std::string	ERR_NOTEXTTOSEND(const std::string client)
{
	return (":localhost 412 " + client + " :No text to send\r\n");
}

std::string	RPL_PRIVMSG(const std::string nick, const std::string username, const std::string target, const std::string message)
{
	return (":" + nick + "!" + username + "@localhost PRIVMSG " + target + " " + message + "\r\n");
}

// TOPIC
std::string	RPL_TOPIC(const std::string client, const std::string channel, const std::string topic)
{
	return (":localhost 332 " + client + " " + channel + " " + topic + "\r\n");
}

std::string	RPL_NOTOPIC(const std::string client, const std::string channel)
{
	return (":localhost 331 " + client + " " + channel + " :No topic is set\r\n");
}

// USER
std::string	ERR_ALREADYREGISTERED(const std::string client)
{
	return (":localhost 462 " + client + " :You may not reregister.\r\n");
}