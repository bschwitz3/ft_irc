#ifndef ICOMMAND_HPP
# define ICOMMAND_HPP
#include <iostream>
#include <vector>
#include <set>
#include "User.hpp"
#include "Server.hpp"

class User;
class Server;
class Channel;

class Command
{
	public:
		Command(Server &server, std::string prefix, std::string command, std::vector<std::string> params);
		Command(Server &server);
		~Command();

		void		setPrefix(std::string prefix);
		std::string	getPrefix();
		void		setCommand(std::string command);
		std::string	getCommand();
		void		setParams(std::vector<std::string> params);
		std::string	getParams();

		void		execute(User &user);

		void		pass(User &user, std::string prefix, std::vector<std::string> params);
		void		nick(User &user, std::string prefix, std::vector<std::string> params);
		void		ping(User &user, std::string prefix, std::vector<std::string> params);
		void		user(User &user, std::string prefix, std::vector<std::string> params);
		void 		cap(User &user, std::string prefix, std::vector<std::string> params);
		void		join(User &user, std::string prefix, std::vector<std::string> params);
		void		who(User &user, std::string prefix, std::vector<std::string> params);
		void		mode(User &user, std::string prefix, std::vector<std::string> params);
		void		privmsg(User &user, std::string prefix, std::vector<std::string> params);
		void		topic(User &user, std::string prefix, std::vector<std::string> params);
		void		invite(User &user, std::string prefix, std::vector<std::string> params);
		void		kick(User &user, std::string prefix, std::vector<std::string> params);

		void		handleData(User &user, const std::string& data);
		void		parseLine(User &user, std::string line);
		void		sendToAllJoinedChannel(User &user, std::string msg);
		void		sendChannelUsers(std::vector<User *> channelUsers, std::string msg, User &user) const; 
		void		sendChannelUsersAndMe(std::vector<User *> channelUsers, std::string msg, User &user) const;

		typedef void (Command::*CmdFunc)(User&, std::string, std::vector<std::string>);
		
		static const CmdFunc cmdArr[];
		std::string		buffer;
	
	private:
		Server						&server;
		std::string					prefix;
		std::string					command;
		std::vector<std::string>	params;
		Channel						*_channel;
};

#endif