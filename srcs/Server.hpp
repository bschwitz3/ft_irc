#ifndef SERVER_HPP
# define SERVER_HPP
#include "Msg.hpp"
#include "User.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "RPL_ERR.hpp"
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
#include <map>
#include <climits>
#include <set>

#define MAX_USER 1000

class User;
class Command;
class Msg;
class Channel;

class Server
{

	public:
		Server();
		~Server();

		void			setPort(int port);
		int				getPort();
		static void		setStop(bool status);
		void			setPasswd(std::string passwd);
		std::string		getPasswd();
		void			setTV(int sec, int musec);

		void	*get_in_addr(struct sockaddr *sa);
		int		get_listener_socket(void);
		void	add_to_pfds(int newfd);
		void	del_from_pfds(int index);
		void	setHint(int family, int type, int flag);

		void	setListeningSocket();
		void	handleNewConnection();
		void	handleClient(Msg &aMess,int index);
		void	run();
		void	addUser(int fd);
		void	removeUser(int fd);

		bool				isNickAvailable(const std::string& nick);
		User				*getUserByNick(const std::string& nick);
		int 				getPfdsIndex(int fd);
		int					getUserIndex(int fd);
		void 				displayUsers();
		std::map<int, User>	getUser();

		Channel		*getChannel(std::string channelName);
		void		createChannel(std::string channelName, User *user);
		void		rmChannel(std::string channelName);

		static void		signalHandler(int signum);
		bool			getStop();
		void			sendToAllUser(std::string msg);

		int	getPfdsFD(int index);


	private:
		int			port;
		std::string	passwd;
		static bool	stop;

		// variable main...
		int				listener_socket;
		int				accepted_socket;
		char			buffer[512];
		std::string		remoteIP;
		socklen_t		addrlen;

		struct		timeval				tv;
		struct	addrinfo				hints;
		std::vector<pollfd>				pfds;
		std::map<int, User> 			users;
		struct sockaddr_storage			remoteaddr;
		std::map<std::string, Channel>	channels;
};

#endif
