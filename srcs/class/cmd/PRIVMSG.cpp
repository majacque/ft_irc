#include "class/Server.hpp"

// /**
//  * @brief Config the message to send form CMD Privmsg
//  * 
//  * @param user the people to send the reply
//  * @param channel_name the name of the channel he join
//  * @return void
//  */
// void	Server::privmsgSend(User &user, std::string &target)
// {

// 	return true;
// }

/**
 * @brief	Send a message either to a channel or to an user.
 * 
 * @param	user The user that ran the command.
 * @param	params The parameters of the command.
 * 
 * @return	true if success, false otherwise.
 */
bool	Server::PRIVMSG(User &user, std::string &params)//!!!
{
	// FIXME Have to recode this function
	std::string tmp;
	std::string	target_name = params.substr(0, params.find(' '));
	std::string	msg_send =  params.substr(params.find(' ') + 1, params.length());

	if (*target_name.begin() == '#')
	{
		target_name = target_name.substr(1, target_name.length());
		for (std::map<std::string, Channel>::iterator ite = this->_lookupChannels.begin() ; ite != this->_lookupChannels.end() ; ite++)
		{
			if (ite->second.getName().compare(target_name) == 0){
				for (std::vector<User *>::iterator itv = ite->second.getUsers().begin(); itv != ite->second.getUsers().end(); itv++)
				{
					tmp = ":" + user.getNickname() + "!" + user.getUsername() + "@" + this->_config["host"] + " PRIVMSG " + (*itv)->getNickname() + " :" + msg_send;
					(*itv)->setMsg(tmp);
					Server::replySend(*(*itv));
				}
				return true;
			}
		}
	}
	else
	{
		for (std::list<User>::iterator ite = this->_users.begin() ; ite != this->_users.end() ; ite++)
		{
			if (ite->getNickname().compare(target_name) == 0)
			{
				tmp = ":" + user.getNickname() + "!" + user.getUsername() + "@" + this->_config["host"] + " PRIVMSG " + target_name + " :" + msg_send;
				ite->setMsg(tmp);
				Server::replySend(*ite);
				// Server::replyPush(*ite, "PRIVMSG "+ user.getNickname() + " " + msg_send);
				// Server::replySend(*ite);
				return true;
			}
		}
	}
	return true;
}
