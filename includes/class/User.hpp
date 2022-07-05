#ifndef USER_CLASS_HPP
# define USER_CLASS_HPP

#include <iostream>
#include <map>
#include <netinet/in.h>// sockaddr_in
#include <string>
#include <sys/types.h> // socket, bind, listen, recv, send
#include <sys/socket.h>//   "      "      "      "     "
#include "class/Channel.hpp"

class Channel;

class User {
private:
	// Attributes
	int									_socket;

	sockaddr_in							_addr;

	std::string							_nickname; // Max length is 9 chars
	std::string							_username;
	std::string							_hostname;
	std::string							_realname;
	std::string							_password;

	std::map<std::string, Channel *>	_channels;// ??

	bool								_isOperator;
	bool								_isRegistered;

public:
	// Constructors
	User(void);

	// Destructors
	virtual ~User( void );

	// Accessors
	sockaddr_in const						&getAddr(void) const;

	int const								&getSocket(void) const;

	std::string const						&getNickname(void) const;
	std::string const						&getUsername(void) const;
	std::string const						&getHostname(void) const;
	std::string const						&getRealname(void) const;
	std::string const						&getPassword(void) const;

	std::map<std::string, Channel *> const	&getChannels(void) const;

	bool const								&getIsOperator(void) const;
	bool const								&getIsRegisterable(void) const;
	bool const								&getIsRegistered(void) const;

	void									setSocket(int const sockfd);
	void									setAddr(sockaddr_in const &addr);
	void									setNickname(std::string const &nickname);
	void									setUsername(std::string const &username);
	void									setHostname(std::string const &hostname);
	void									setRealname(std::string const &realname);
	void									setPassword(std::string const &password);
	void									setChannels(std::map<std::string, Channel *> const &channels);
	void									setIsOperator(bool const isOperator);
	void									setIsRegisterable(bool const isRegisterable);
	void									setIsRegistered(bool const isRegistered);

	// Member functions
	bool	init(int const &socket, sockaddr_in const &addr); // set _socket & _addr + fcntl() <-- setup non-blocking fd
	void	disconnect(void);
	// void	print(void) const;

	bool	sendTo( User const & user ); // send private message
	bool	sendToAll( Channel const & chan ); // send message to every user in the channel (except myself)
};

#endif
