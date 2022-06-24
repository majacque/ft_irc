#include <arpa/inet.h>
#include <cstring> // strstr
#include <cstdio>
#include <ctime>
#include "class/Server.hpp"

#define PING 10

// ************************************************************************** //
//                                Constructors                                //
// ************************************************************************** //

Server::Server(void) :
	_socket(-1),
	_password(),
	_channels(),
	_cmds() {}

// ************************************************************************* //
//                                Destructors                                //
// ************************************************************************* //

Server::~Server(void) {}

// ************************************************************************* //
//                                 Accessors                                 //
// ************************************************************************* //

int	Server::getState(void) const
{
	return this->_state;
}

// ************************************************************************** //
//                          Private Member Functions                          //
// ************************************************************************** //

/**
 * @brief	Terminate the IRC server.
 * 			This command is reserved for IRC operators.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdDie(User &user, std::string const &params)
{
	Server::logMsg(RECEIVED, "Command DIE " + params + ", From " + user.getNickname());
	if (user.getIsOperator())
	{
		Server::logMsg(SENT, "Command DIE " + params + ", To " + user.getNickname());
		this->_state = SHUTDOWN;
		return true;
	}
	else
	{
		Server::logMsg(SENT, "Command DIE " + params + ", To " + user.getNickname() + "; ERROR: Not an operator");
		return false;
	}
	return true;
}

/**
 * @brief	Make an user joining a channel.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdJoin(User &user, std::string const &params)
{
	Server::logMsg(RECEIVED, "Command JOIN " + params + ", From " + user.getNickname());
	return true;
}

/**
 * @brief	Kick an user from a channel.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdKick(User &user, std::string const &params)
{
	Server::logMsg(RECEIVED, "Command KICK " + params + ", From " + user.getNickname());
	return true;
}

/**
 * @brief	Remove an user from the network.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdKill(User &user, std::string const &params)
{
	Server::logMsg(RECEIVED, "Command KILL " + params + ", From " + user.getNickname());
	return true;
}

/**
 * @brief	Send a message either to a channel or to an user.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdMsg(User &user, std::string const &params)
{
	Server::logMsg(RECEIVED, "Command MSG " + params + ", From " + user.getNickname());
	return true;
}

/**
 * @brief	Set a new nickname for an user.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdNick(User &user, std::string const &params)
{
	std::map<int, User>::iterator	it;

	Server::logMsg(RECEIVED, "Command NICK " + params + ", From " + user.getNickname());
	if (params.empty())
		return this->reply(user, "431 " + user.getNickname() + " :No nickname given");
	for (it = this->_users.begin() ;
		it != this->_users.end() && it->second.getNickname() != params ;
		++it);
	if (it != this->_users.end())
		return this->reply(user, "433 " + user.getNickname() + " :Nickname already in use");
	user.setNickname(params);
	return this->reply(user, "NICK " + user.getNickname());
}

/**
 * @brief	Make an user being promoted to operator status.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdOper(User &user, std::string const &params)
{
	Server::logMsg(RECEIVED, "Command OPER " + params + ", From " + user.getNickname());
	return true;
}

/**
 * @brief	Make an user leaving a channel.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdPart(User &user, std::string const &params)
{
	Server::logMsg(RECEIVED, "Command PART " + params + ", From " + user.getNickname());
	return true;
}

/**
 * @brief	Check if a provided password is correct to connect to the server.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdPass(User &user, std::string const &params)
{
	Server::logMsg(RECEIVED, "Command PASS " + params + ", From " + user.getNickname());
	return true;
}

/**
 * @brief 
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdPing(User &user, std::string const &params)
{
	std::string	reply;

	Server::logMsg(RECEIVED, "Command PING " + params + ", From " + user.getNickname());
	reply = "PING " + user.getNickname() + "\r\n";
	if (send(user.getSocket(), reply.c_str(), reply.size() + 1, 0) == -1)
	{
		perror("send");
		return false;
	}
	Server::logMsg(SENT, "Command PING " + user.getNickname() + ", To " + user.getNickname());
	return true;
}

/**
 * @brief	Disconnect an user from the server.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdQuit(User &user, std::string const &params)
{
	Server::logMsg(RECEIVED, "Command QUIT " + params + ", From " + user.getNickname());
	this->_state = SHUTDOWN; // XXX To be removed, temporary solution to end properly the server.
	return true;
}

/**
 * @brief	Update data of an user.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdSet(User &user, std::string const &params)
{
	Server::logMsg(RECEIVED, "Command SET " + params + ", From " + user.getNickname());
	return true;
}

/**
 * @brief	Set a new username, hostname and realname for an user.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::cmdUser(User &user, std::string const &params)
{
	Server::logMsg(RECEIVED, "Command USER " + params + ", From " + user.getNickname());
	return true;
}

/**
 * @brief	Write a formated log message to the standard output.
 * 
 * @param	msg The message to write.
 */
void	Server::logMsg(enum e_logMsg const type, std::string const &msg)
{
	char	logtime[64];
	time_t	rawtime;
	int		idx;

	for (idx = 0 ; idx < 3 && _lookupLogMsgTypes[idx].first != type ; ++idx);
	time(&rawtime);
	strftime(logtime, 64, "%Y/%m/%d %H:%M:%S", localtime(&rawtime));
	std::cout
	<< "["
	<< logtime
	<< "]["
	<< _lookupLogMsgTypes[idx].second
	<< "] "
	<< msg
	<< '\n';
}

/**
 * @brief	Send a reply message to an user client.
 * 
 * @param	user The user to send the reply message to.
 * @param	msg The message to send.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::reply(User const &user, std::string const &msg) const
{
	std::string	toSend(msg + "\r\n");

	if (send(user.getSocket(), toSend.c_str(), toSend.size() + 1, 0) == -1)
	{
		perror("send");
		return false;
	}
	Server::logMsg(SENT, msg);
	return true;
}

// ************************************************************************* //
//                          Public Member Functions                          //
// ************************************************************************* //

/**
 * @brief	Configure the internal attributes of the server.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::init(void)
{
	int													idx;
	std::map<std::string const, t_fct const>::iterator	it;

	this->_name = "ircserv";
	for (idx = 0 ; Server::_lookupCmds[idx].second ; ++idx)
		this->_cmds.insert(Server::_lookupCmds[idx]);
	return true;
}

/**
 * @brief	Determine what to do depending on the given `msg`.
 * 
 * @param	user The user that sent the message.
 * @param	msg The raw message received from a client.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::judge(User &user, std::string &msg)
{
	std::string	commandName;
	std::string	params;
	std::string	line;
	t_fct		command;

	do
	{
		line = msg.substr(0, msg.find('\n') - 1);
		commandName = line.substr(0, line.find(' '));
		params = line.substr(commandName.length());
		params.erase(0, params.find_first_not_of(' '));

		command = this->_cmds[commandName];
		if (!command)
		{
			if (!commandName.empty())
				Server::logMsg(RECEIVED, "Command " + commandName + ": " RED "not found." RESET);
		}
		else if (!(this->*command)(user, params))
			return false;
		msg.erase(0, msg.find('\n') + 1);
	} while (!line.empty());
	return true;
}

/**
 * @brief	Start the server, configuring the socket and the port for listening.
 * 
 * @param	port The port to listen on.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::start(uint16_t const port)
{
	int	optval;

	this->_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (this->_socket == -1)
	{
		perror("socket");
		return false;
	}
	Server::logMsg(INTERNAL, "Socket created");

	optval = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)))
	{
		close(_socket); // stop()
		perror("setsockopt");
		return false;
	}
	Server::logMsg(INTERNAL, "Socket options set");

	sockaddr_in	addr;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // ip hard code for now
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	if (bind(_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)))
	{
		close(_socket); // stop()
		perror("bind");
		return false;
	}
	if (listen(_socket, SOMAXCONN))
	{
		close(_socket); // stop()
		perror("listen");
		return false;
	}
	_pollfds.push_back(pollfd());
	_pollfds.back().fd = _socket;
	_pollfds.back().events = POLLIN;
	this->_state = RUNNING;
	return true;
}

bool	Server::welcomeDwarves(void){
	sockaddr_in addr = {};
	socklen_t addrlen = sizeof(addr);
	
	int newUser = accept(_socket, reinterpret_cast<sockaddr *>(&addr), &addrlen);
	if (newUser == -1){
		// perror("accept");
	}
	else{
		std::cout << "New people connected !" <<std::endl;
		_pollfds.push_back(pollfd());
		_pollfds.back().fd = newUser;
		_pollfds.back().events = POLLIN | POLLOUT;
		std::pair<int, User>	new_pair(newUser, User(newUser, addr));
		_users.insert(new_pair);
	}
	if (poll(&_pollfds[0], _pollfds.size(), (PING * 1000) / 10) == -1){// Ecoute les FD donnes
		perror("poll server");//check for error
		return (false);
	}
	return (true);
}

// typedef typename std::map<int, User>::iterator ite_map;

bool	Server::listenAll(void){
	char		buff[1024];
	size_t		buffsize = sizeof(buff);
	std::string	message;
	std::string	endOfMessage("\r\n");
	bool		EOM = false;
	size_t		retRecv;

	if (_users.empty())
		return (true);
	for (std::map<int, User>::iterator ite = _users.begin(); ite != _users.end(); ite++){
		while (EOM == false){
			retRecv = recv(_pollfds.back().fd, buff, buffsize, 0);
			// Server::logMsg()
			if (retRecv <= 0){
				perror("recv");
				// if (retRecv == 0)
				// 	std::cerr << "retRecv = 0" << std::endl;
				return (false);
			}
			message.append(buff);
			if (strstr(buff, endOfMessage.c_str()))
				EOM = true;
		}
		this->judge(ite->second, message);
		// _exit(121);
		// Server::logMsg(RECEIVED, message + ", From " + ite->second.getNickname());
		if (message.find("QUIT") != std::string::npos)
			break;
		message.clear();
		EOM = false;
		if (send(_pollfds.back().fd, ":our.server 001 majacque :Welcome to IRC\r\n", sizeof(":our.server 001 majacque :Welcome to IRC\r\n"), 0) == -1){
			perror("send");
			return (false);
		}
	}
	return (true);
}

/**
 * @brief 
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::stop(void)
{
	return true;
}

/**
 * @brief 
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::run(void)
{
	return true;
}

/**
 * @brief 
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::update(void)
{
	return true;
}

int	Server::getSocket(void) const
{
	return _socket;
}

// ************************************************************************** //
//                             Private Attributes                             //
// ************************************************************************** //

std::pair<std::string const, t_fct const> const	Server::_lookupCmds[] = {
	std::make_pair<std::string const, t_fct const>(std::string("DIE"), &Server::cmdDie),
	std::make_pair<std::string const, t_fct const>(std::string("JOIN"), &Server::cmdJoin),
	std::make_pair<std::string const, t_fct const>(std::string("KICK"), &Server::cmdKick),
	std::make_pair<std::string const, t_fct const>(std::string("KILL"), &Server::cmdKill),
	std::make_pair<std::string const, t_fct const>(std::string("MSG"), &Server::cmdMsg),
	std::make_pair<std::string const, t_fct const>(std::string("NICK"), &Server::cmdNick),
	std::make_pair<std::string const, t_fct const>(std::string("OPER"), &Server::cmdOper),
	std::make_pair<std::string const, t_fct const>(std::string("PART"), &Server::cmdPart),
	std::make_pair<std::string const, t_fct const>(std::string("PASS"), &Server::cmdPass),
	std::make_pair<std::string const, t_fct const>(std::string("PING"), &Server::cmdPing),
	std::make_pair<std::string const, t_fct const>(std::string("QUIT"), &Server::cmdQuit),
	std::make_pair<std::string const, t_fct const>(std::string("SET"), &Server::cmdSet),
	std::make_pair<std::string const, t_fct const>(std::string("USER"), &Server::cmdUser),
	std::make_pair<std::string const, t_fct const>(std::string(""), NULL),
};

std::pair<enum e_logMsg const, char const *> const	Server::_lookupLogMsgTypes[] = {
	std::make_pair<enum e_logMsg const, char const *>(INTERNAL, WHITE "Internal" RESET),
	std::make_pair<enum e_logMsg const, char const *>(RECEIVED, GREEN "Received" RESET),
	std::make_pair<enum e_logMsg const, char const *>(SENT, MAGENTA "  Sent  " RESET),
};

/* void	Server::stop( void ) {
	for (std::map<int, User>::iterator it = _users.begin(); it != _users.end(); it++) {
		it->second.disconnect();
	}
	_users.clear();
	_channels.clear();
	if (_socket != -1) {
		close(_socket);
		_socket = -1;
	}
} */
