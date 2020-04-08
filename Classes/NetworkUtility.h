#ifndef __NETWORK_UTILITY_H__
#define __NETWORK_UTILITY_H__

#include <string>
#include <vector>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "NetMessage.h"
#include "NetworkError.h"
#include "port_config.h"
#include <atomic>
#include <iostream>
#include "Composition.h"

struct server_info
{
	std::string name;
	std::string address;
	server_info(std::string& name, std::string& address)
	{
		this->name = name;
		this->address = address;
	}
	server_info()
	{
		name = "";
		address = "";
	}
};

namespace Utility
{
	enum UserStatus {
		HOST,
		CLIENT
	};

	/** Get a valid font size for current screen size settings
	 * params: void
	*/
	unsigned int getTitleFontSize();

	unsigned int getMinorFontSize();

	/** Encode a message to special format
	* Params:
	* IN unsigned int PACKET_TYPE
	* IN const char* MESSAGE
	* OUT std::string BUFF
	*/
	void encodeMessage(unsigned int, const char*, std::string&);

	/** Encode a message to special format
	* Params:
	* IN struct Msg& Message
	*/

	void encodeMessage(const NetMessage&, std::string&);

	/** Decode a message from special format to Msg& struct
	* Params:
	* IN const char
	* OUT struct Msg& Message
	*/

	void decodeMessage(const char*, NetMessage&);

	/** Broadcast through LAN to find servers
	* Params:
	* NONE
	*/

	void refresh_list(cocos2d::ui::ScrollView* scrollview, cocos2d::Ref* sender);

	/** Constantly send messages through LAN to notify clients of your existance
	  *
	*/

	void notify_clients(std::atomic<bool> &quit);

	SOCKET send_connection_request(sockaddr_in &address);
	SOCKET send_connection_request(std::string& addr);

	SOCKET accept_connections(std::atomic<bool> &quit, cocos2d::Ref* pSender);
}

#endif 
