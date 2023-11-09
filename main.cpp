#include "srcs/Server.hpp"
#include "srcs/Msg.hpp"
#include "srcs/Command.hpp"

int main(int argc, char *argv[])
{
	if (argc != 3 && std::string(argv[1]).std::string::find_first_not_of("0123456789",0) != std::string::npos)
	{
		std::cout << red << "Usage: ./Servers port password\n" << reset;
		return (-1);
	}
	//Msg message;// ?
	Server aServer;
	struct sigaction sa;
	sa.sa_handler = Server::signalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	//signal(SIGINT, Server::signalHandler);
	aServer.setPasswd(argv[2]);
	aServer.setPort(atoi(argv[1]));
	while (aServer.getStop() == false)
		aServer.run();
	std::string msg = "Server is shutting down. Goodbye!\r\n";
	aServer.sendToAllUser(msg);
	system("leaks Servers");
	return (0);
}
