#ifndef USER_CLASS_HPP
# define USER_CLASS_HPP

#include <iostream>
#include <map>
#include <netinet/in.h>// sockaddr_in
#include <string>
#include <sys/types.h> // socket, bind, listen, recv, send
#include <sys/socket.h> //   "      "      "      "     "
#include "class/Channel.hpp"

class Channel;

class User
{
private:
	// Attributes
	sockaddr_in							_addr;

	int									_socket;

	std::string							_nickname; // Max length is 9 chars
	std::string							_username;
	std::string							_hostname;
	std::string							_servname;
	std::string							_realname;
	std::string							_password;
	std::string							_modes;
	std::string							_msg;

	bool								_isRegistered;

	time_t								_lastActivity;

	std::map<std::string, Channel *>	_channels;

	static std::string const	_availableModes;

public:
	// Constructors
	User(sockaddr_in const &addr = sockaddr_in(), int sockfd = -1);
	User(User const &src);

	// Destructors
	virtual ~User(void);

	// Accessors
	sockaddr_in const						&getAddr(void) const;

	int const								&getSocket(void) const;

	std::string const						&getNickname(void) const;
	std::string const						&getUsername(void) const;
	std::string const						&getHostname(void) const;
	std::string const						&getServname(void) const;
	std::string const						&getRealname(void) const;
	std::string const						&getPassword(void) const;
	std::string const						&getModes(void) const;
	std::string const						&getMsg(void) const;

	bool const								&getIsRegistered(void) const;

	time_t const							&getLastActivity(void) const;

	std::map<std::string, Channel *> const	&getChannels(void) const;

	static std::string const				&getAvailableModes(void);

	void									setSocket(int const sockfd);
	void									setAddr(sockaddr_in const &addr);
	void									setNickname(std::string const &nickname);
	void									setUsername(std::string const &username);
	void									setHostname(std::string const &hostname);
	void									setServname(std::string const &servname);
	void									setRealname(std::string const &realname);
	void									setPassword(std::string const &password);
	void									setModes(std::string const &modes);
	void									setMsg(std::string const &msg);
	void									setIsRegistered(bool const isRegistered);
	void									setChannels(std::map<std::string, Channel *> const &channels);

	// Member functions
	bool		init(int const &socket, sockaddr_in const &addr); // set _socket & _addr + fcntl() <-- setup non-blocking fd
};

#endif
