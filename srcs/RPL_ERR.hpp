#ifndef RPL_ERR_HPP
#define RPL_ERR_HPP

#include "Server.hpp"

std::string RPL_WELCOME(const std::string username, const std::string nickname);

std::string ERR_UNKNOWNCOMMAND(const std::string client, const std::string command);

std::string ERR_NEEDMOREPARAMS(const std::string client, const std::string command);

std::string ERR_NOSUCHCHANNEL(const std::string client, const std::string channel);

std::string ERR_NOTONCHANNEL(const std::string client, const std::string nick, const std::string channel);

std::string ERR_USERONCHANNEL(const std::string client, const std::string nick, const std::string channel);

std::string RPL_INVITING(const std::string client, const std::string username, const std::string nickname, const std::string channel);

std::string RPL_INVITE(std::string nickname, const std::string username, const std::string invited, const std::string channel);

std::string RPL_JOIN(const std::string nickname, const std::string username, const std::string channel);

std::string ERR_BANNEDFROMCHAN(const std::string client, const std::string channel, const std::string mode);

std::string ERR_BADCHANNELKEY(const std::string client, const std::string channel);

std::string ERR_USERNOTINCHANNEL(const std::string client, const std::string nickname, const std::string channel);

std::string ERR_CHANOPRIVSNEEDED(const std::string client, const std::string channel);

std::string RPL_KICK(const std::string nickname, const std::string username, const std::string channel, const std::string kicked, const std::string reason);

std::string MODE_USERMSG(const std::string client, const std::string mode);

std::string ERR_UMODEUNKNOWNFLAG(const std::string client);

std::string ERR_USERSDONTMATCH(const std::string client);

std::string RPL_UMODEIS(const std::string client, const std::string mode);

std::string MODE_CHANNELMSG(const std::string channel, const std::string mode);

std::string MODE_CHANNELMSGWITHPARAM(const std::string channel, const std::string mode, const std::string param);

std::string RPL_CHANNELMODEIS(const std::string client, const std::string channel, const std::string mode);

std::string RPL_CHANNELMODEISWITHKEY(const std::string client, const std::string channel, const std::string mode, const std::string password);

std::string ERR_CANNOTSENDTOCHAN(const std::string client, const std::string channel);

std::string ERR_CHANNELISFULL(const std::string client, const std::string channel);

std::string ERR_INVALIDMODEPARAM(const std::string client, const std::string channel, const std::string mode);

std::string RPL_ADDVOICE(const std::string nickname, const std::string username, const std::string channel, const std::string mode, const std::string param);

std::string RPL_NAMREPLY(const std::string client, const std::string channel, const std::string list_of_nicks);

std::string RPL_ENDOFNAMES(const std::string client, const std::string channel);

std::string ERR_NONICKNAMEGIVEN(const std::string client);

std::string ERR_ERRONEUSNICKNAME(const std::string client, const std::string nickname);

std::string ERR_NICKNAMEINUSE(const std::string client, const std::string nickname);

std::string RPL_NICK(const std::string oclient, const std::string uclient, const std::string client);

std::string ERR_PASSWDMISMATCH(const std::string client);

std::string RPL_PONG(const std::string nickname, const std::string username, const std::string token);

std::string ERR_NOSUCHNICK(const std::string client, const std::string target);

std::string ERR_NORECIPIENT(const std::string client);

std::string ERR_NOTEXTTOSEND(const std::string client);

std::string RPL_PRIVMSG(const std::string nick, const std::string username, const std::string target, const std::string message);

std::string RPL_TOPIC(const std::string client, const std::string channel, const std::string topic);

std::string RPL_NOTOPIC(const std::string client, const std::string channel);

std::string ERR_ALREADYREGISTERED(const std::string client);


#endif