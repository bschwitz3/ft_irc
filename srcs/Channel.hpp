#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>

#include "Server.hpp"

class User;

struct	Mode
{
	bool	i; // invited-only, should recieve INVITE, if not err 473
	bool	t; // protected topic, channops only TOPIC
	bool	k; // key channel, need key to JOIN, err 696, 525, 475
	bool	l; // set max user (int), err 471
};

class Channel
{
	private:
		std::string			_name;
		std::string			_topic;
		Mode				_mode;
		int					_max;
		std::string			_password;
		std::vector<User *>	_users;
		std::vector<User *>	_chanops;
		std::vector<User *>	_kickedUsers;

	public:
		Channel(std::string name, User *oper);
		~Channel();

		//operators
		bool		operator==(const Channel &other) const;
		Channel&	operator=(const Channel& other);

		// getters
		std::string			getName() const;
		std::string			getTopic() const;
		Mode				getMode() const;
		bool				getModeI() const;
		bool				getModeT() const;
		bool				getModeK() const;
		bool				getModeL() const;
		int					getMax() const;
		std::string			getPassword() const;
		std::vector<User *>	getUsers() const;
		std::vector<User *>	getChanops() const;
		std::vector<User *>	getKickedUsers() const;

		int					getSize() const;

		// setters
		void	setName(std::string name);
		void	setTopic(std::string topic);
		void	setRmMode(std::string mode);
		void	setMax(int max);
		void	setPassword(std::string password);

		// others
		bool		isUser(User &user);
		bool		isChanops(User &user);
		bool		isKicked(User &user);
		std::string	getList();

		void	addUser(User &user);
		void	inviteUser(User &user, User &chanop);
		void	addChanops(User &user, User &chanop);
		void	rmChanops(User &user);
		void	rmUser(User &user);
		void	kickUser(User &user, User &chanop);
		void	kickChanops(User &user, User &chanop);

};

bool	checkPassword(std::string passWord);










#endif