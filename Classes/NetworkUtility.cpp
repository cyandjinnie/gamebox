#include "NetworkUtility.h"
#include "LobbyConnection.h"
#include "LobbyHost.h"
#include "DataTransfer.h"

using namespace cocos2d;
using namespace cocos2d::ui;

/////////////////////////////////////
// namespace Utility implementation

void Utility::encodeMessage(unsigned int type, const char* text, std::string &buff)
{
	buff = std::string("[[msg=(" + std::string(text) + "),type=(" + std::to_string(type) + ")]]");
}

void Utility::encodeMessage(const NetMessage& msg, std::string &buff)
{
	buff = "[[msg=(" + msg.msg_text + "),type=(" + std::to_string(msg.msg_type) + ")]]";
}

void Utility::decodeMessage(const char* msg_text, NetMessage& msg)
{
	bool write = false;
	unsigned short field_num = 1;
	unsigned short iter;
	msg.msg_type = UNDEFINED;
	msg.msg_text = "";
	std::string buff(msg_text);
	std::string msg_type_str = "";
	for (auto it : buff)
	{
		if ((it == '(') || (it == ')'))
		{
			write = !write;
			if (it == ')')
			{
				field_num++;
			}
			iter = 0;
		}
		else if (write)
		{
			if (field_num == 1)
			{
				msg.msg_text = msg.msg_text + it;
			}
			else if (field_num == 2)
			{
				msg_type_str += it;
			}
		}
	}
	msg.msg_type = (MessageType)atoi(msg_type_str.c_str());
}

void Utility::refresh_list(cocos2d::ui::ScrollView* scrollview, cocos2d::Ref* sender)
{
	SOCKET DatagramSock = INVALID_SOCKET;
	int iResult;
	DatagramSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (DatagramSock == INVALID_SOCKET)
		throw network_error(WSAGetLastError(), " socket() failed ");

	//////////////////////////
	// Setting up socket options

	int nTimeout = 500;
	bool nBroadcast = true;

	iResult = setsockopt(DatagramSock, SOL_SOCKET, SO_BROADCAST, (char*)&nBroadcast, sizeof(nBroadcast));
	iResult = setsockopt(DatagramSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeout, sizeof(nTimeout));

	///////////////////////////////


	// Address to send to
	struct sockaddr_in Sender_addr;
	Sender_addr.sin_family = AF_INET;

	/* Client -> Messages are sent to server */ Sender_addr.sin_port = htons(DEFAULT_UDP_SERVER_PORT);
	// Address to send broadcast datagrams

	inet_pton(AF_INET, DataTransfer::NetMask.c_str(), &Sender_addr.sin_addr.s_addr);

	/////////////////////////////////////////

	// Current address
	struct sockaddr_in Recv_addr;
	Recv_addr.sin_family = AF_INET;

	/* Client -> */ Recv_addr.sin_port = htons(DEFAULT_UDP_CLIENT_PORT);

	Recv_addr.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket to current address 
	// IMPORTANT! Both client and server have to be bound to a port in order to recvfrom()
	iResult = bind(DatagramSock, (sockaddr*)&Recv_addr, sizeof(Recv_addr));
	if (iResult < 0)
	{
		throw network_error(WSAGetLastError(), " bind() failed ");
	}

	char buffer[128];

	// Info returned from recvfrom
	sockaddr_in from;

	// Size
	int from_size = sizeof(from);

	// What to send
	std::string string_message;
	std::string presentable_addr_str;

	auto server_list = std::vector<server_info>(0);
	NetMessage format_msg;

	encodeMessage(MessageType::SEARCH, "searching here", string_message);

	for (int i = 0; i < 10; i++)
	{
		iResult = sendto(DatagramSock, string_message.c_str(), string_message.size(), 0, (sockaddr*)&Sender_addr, sizeof(Sender_addr));
		if (iResult < 0)
			throw network_error(WSAGetLastError(), " sendto() failed in refresh_list ");

		while (1)
		{
			iResult = recvfrom(DatagramSock, buffer, sizeof(buffer), 0, (sockaddr*)&from, &from_size);
			if (iResult < 0)
			{
				if (WSAGetLastError() != 10060)
				{
					throw network_error(WSAGetLastError(), " recvfrom() failed in refresh_list ");
				}
				break;
			}
			else 
			{
				decodeMessage(buffer, format_msg);

				// Convert 'from' to presentable
				inet_ntop(AF_INET, &from.sin_addr, buffer, sizeof(buffer));
				presentable_addr_str.append(buffer);

				if (format_msg.msg_type == REPLY_TO_SEARCH)
				{
					if (server_list.size() == 0)
					{
						server_list.push_back(server_info(format_msg.msg_text, presentable_addr_str));
						break;
					}
					else
					{
						bool found = false;
						for (auto it = server_list.begin(); it != server_list.end(); it++)
							if (it->name == format_msg.msg_text)
								found = true;

						if (!found)
							server_list.push_back(server_info(format_msg.msg_text, presentable_addr_str));
					}
				}
			}
		}
	}

	closesocket(DatagramSock);

	auto scheduler = Director::getInstance()->getScheduler();

	std::function<void()> func = std::bind([](std::vector<server_info> list, cocos2d::ui::ScrollView* scrollview, cocos2d::Ref* sender)
	{
		((LobbyConnection*)sender)->RefreshList(list, scrollview);
	}, server_list, scrollview, sender);

	scheduler->performFunctionInCocosThread(func);
}

void Utility::notify_clients(std::atomic<bool> &quit)
{
	SOCKET DatagramSock = INVALID_SOCKET;
	int iResult;
	DatagramSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (DatagramSock == INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		throw network_error(err, " socket() failed ");
	}

	//////////////////////////
	// Setting up socket options

	bool nBroadcast = true;
	int nTimeout = 1000;

	iResult = setsockopt(DatagramSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeout, sizeof(nTimeout));
	if (iResult < 0)
		throw network_error(WSAGetLastError(), " setsockopt(RCVTIMEO) failed in notify_clients ");

	iResult = setsockopt(DatagramSock, SOL_SOCKET, SO_BROADCAST, (char*)&nBroadcast, sizeof(nBroadcast));
	if (iResult < 0)
		throw network_error(WSAGetLastError(), " setsockopt(BROADCAST) failed in notify_clients ");

	///////////////////////////////

	// Address to send to
	struct sockaddr_in Sender_addr;
	Sender_addr.sin_family = AF_INET;

	/* Server -> Messages are sent to clients */ Sender_addr.sin_port = htons(DEFAULT_UDP_CLIENT_PORT);
	// Address to send broadcast datagrams

	inet_pton(AF_INET, DataTransfer::NetMask.c_str(), &Sender_addr.sin_addr);

	/////////////////////////////////////////

	// Current address
	struct sockaddr_in Recv_addr;
	Recv_addr.sin_family = AF_INET;

	/* Server -> */ Recv_addr.sin_port = htons(DEFAULT_UDP_SERVER_PORT);

	Recv_addr.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket to current address 
	// IMPORTANT! Both client and server have to be bound to a port in order to recvfrom()
	iResult = bind(DatagramSock, (sockaddr*)&Recv_addr, sizeof(Recv_addr));
	if (iResult < 0)
	{
		throw network_error(WSAGetLastError(), " bind() failed ");
	}

	std::string name = UserDefault::getInstance()->getStringForKey("RoomName");

	char buffer[128];
	sockaddr_in from;
	std::string str_buffer;
	NetMessage format_msg;
	int from_size = sizeof(from);

	while (!quit)
	{
		iResult = recvfrom(DatagramSock, buffer, sizeof(buffer), 0, (sockaddr*)&from, &from_size);
		if (iResult < 0)
		{
			if (WSAGetLastError() == 10060)
			{
				continue;
			}
			throw network_error(WSAGetLastError(), "recvfrom() failed in utility::notify_clients");
		}

		decodeMessage(buffer, format_msg);

		if (format_msg.msg_type == SEARCH)
		{
			encodeMessage(REPLY_TO_SEARCH, name.c_str(), str_buffer);
			iResult = sendto(DatagramSock, str_buffer.c_str(), str_buffer.size(), 0, (sockaddr*)&from, sizeof(from));
		}
	}

	closesocket(DatagramSock);
}

SOCKET Utility::send_connection_request(sockaddr_in &address)
{
	SOCKET ConnectionSocket = INVALID_SOCKET;
	int iResult;

	ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectionSocket == INVALID_SOCKET)
		throw network_error(WSAGetLastError(), " socket() failed in send_connection_request ");

	unsigned long block = 1;
	iResult = ioctlsocket(ConnectionSocket, FIONBIO, &block);

	iResult = connect(ConnectionSocket, reinterpret_cast<sockaddr*>(&address), sizeof(address));

	timeval time_out;
	time_out.tv_sec = 0;
	time_out.tv_usec = 5000000; // 5 seconds

	fd_set setW, setE;

	FD_ZERO(&setW);
	FD_SET(ConnectionSocket, &setW);
	FD_ZERO(&setE);
	FD_SET(ConnectionSocket, &setE);
	select(0, NULL, &setW, &setE, &time_out);

	if (FD_ISSET(ConnectionSocket, &setW))
	{
		// Connection succeeded
		// Back to blocking mode
		block = 0;
		ioctlsocket(ConnectionSocket, FIONBIO, &block);
		return ConnectionSocket;

	}
	else if (FD_ISSET(ConnectionSocket, &setE))
	{
		// Connection failed
		closesocket(ConnectionSocket);
		return INVALID_SOCKET;
	}
	else
	{
		std::cout << "CONNECTION TIMED OUT \n";
		closesocket(ConnectionSocket);
		return INVALID_SOCKET;
		// Timeout
	}

	// Back to blocking mode
	/*block = 0;
	ioctlsocket(ConnectionSocket, FIONBIO, &block);*/
}

SOCKET Utility::send_connection_request(std::string &addr)
{
	SOCKET ConnectionSocket = INVALID_SOCKET;
	int iResult;

	ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectionSocket == INVALID_SOCKET)
		throw network_error(WSAGetLastError(), " socket() failed in send_connection_request ");

	unsigned long block = 1;
	iResult = ioctlsocket(ConnectionSocket, FIONBIO, &block);

	sockaddr_in address;
	inet_pton(AF_INET, addr.c_str(), &address.sin_addr);
	address.sin_family = AF_INET;
	address.sin_port = htons(DEFAULT_TCP_PORT);

	iResult = connect(ConnectionSocket, reinterpret_cast<sockaddr*>(&address), sizeof(address));

	timeval time_out;
	time_out.tv_sec = 0;
	time_out.tv_usec = 5000000; // 5 seconds

	fd_set setW, setE;

	FD_ZERO(&setW);
	FD_SET(ConnectionSocket, &setW);
	FD_ZERO(&setE);
	FD_SET(ConnectionSocket, &setE);
	select(0, NULL, &setW, &setE, &time_out);

	if (FD_ISSET(ConnectionSocket, &setW))
	{
		// Connection succeeded
		// Back to blocking mode
		block = 0;
		ioctlsocket(ConnectionSocket, FIONBIO, &block);

		return ConnectionSocket;
	}
	else if (FD_ISSET(ConnectionSocket, &setE))
	{
		// Connection failed
		closesocket(ConnectionSocket);
		return INVALID_SOCKET;
	}
	else
	{
		std::cout << "CONNECTION TIMED OUT \n";
		closesocket(ConnectionSocket);
		return INVALID_SOCKET;
		// Timeout
	}

	// Back to blocking mode
	/*block = 0;
	ioctlsocket(ConnectionSocket, FIONBIO, &block);*/
}

SOCKET Utility::accept_connections(std::atomic<bool> &quit, cocos2d::Ref* pSender)
{
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ConnectionSocket = INVALID_SOCKET;
	int iResult;

	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET)
		throw network_error(WSAGetLastError(), " socket() failed in accept_connections ");

	sockaddr_in recvaddr;
	recvaddr.sin_family = AF_INET;
	recvaddr.sin_port = htons(DEFAULT_TCP_PORT);
	recvaddr.sin_addr.s_addr = INADDR_ANY;

	iResult = bind(ListenSocket, reinterpret_cast<sockaddr*>(&recvaddr), sizeof(recvaddr));
	if (iResult < 0)
		throw network_error(WSAGetLastError(), " bind() failed in accept_connections() ");

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult < 0)
		throw network_error(WSAGetLastError(), " listen() failed in accept_connections() ");

	bool accepted = false;
	unsigned long block = 1;
	iResult = ioctlsocket(ListenSocket, FIONBIO, &block);

	//timeval time_out;
	//time_out.tv_sec = 0;
	//time_out.tv_usec = 200; // 0.2 sec

	//fd_set setR;

	//FD_ZERO(&setR);
	//FD_SET(ListenSocket, &setR);

	while ((ConnectionSocket == INVALID_SOCKET) && (!quit))
	{
		ConnectionSocket = accept(ListenSocket, NULL, NULL);
		Sleep(500);
	}

	block = 0;
	iResult = ioctlsocket(ConnectionSocket, FIONBIO, &block);

	if (!quit)
	{
		Director::getInstance()->getScheduler()->performFunctionInCocosThread(
			std::bind(
				[](cocos2d::Ref* pSender)
		{
			((LobbyHost*)pSender)->AcceptUser();
		},
				pSender
			)
		);
	}

	return ConnectionSocket;

	/*if (ConnectionSocket != INVALID_SOCKET)
	{
		UserDefault::getInstance()->setIntegerForKey("SOCKET", ConnectionSocket);
		Director::getInstance()->replaceScene(GameScene::createScene());
	}*/

	//////////////////////////////
	//       Former code        //
	//////////////////////////////

	//while (!quit)
	//{
	//	select(NULL, &setR, NULL, NULL, &time_out);

	//	if (FD_ISSET(ListenSocket, &setR))
	//	{
	//		// Connection succeeded
	//		// Back to blocking mode		
	//		ConnectionSocket = accept(ListenSocket, NULL, NULL);

	//		block = 0;
	//		ioctlsocket(ConnectionSocket, FIONBIO, &block);

	//		if (ConnectionSocket == INVALID_SOCKET)
	//			throw network_error(WSAGetLastError(), " accept() failed in accept_connections() ");

	//		closesocket(ListenSocket);
	//		sock = ConnectionSocket;
	//		accepted = true;

	//		break;
	//	}
	//	Sleep(200);
	//}
	/*if (!accepted)
	sock = INVALID_SOCKET;*/
	//else
	//	std::cout << "ALRIGHT!!! >> SOCKET value is " << sock << "\n";


	/////////////////////////////
	//      Super old code     //
	/////////////////////////////

	/*SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ConnectionSocket = INVALID_SOCKET;
	int iResult;

	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET)
		throw network_error(WSAGetLastError(), " socket() failed in accept_connections ");

	sockaddr_in recvaddr;
	recvaddr.sin_family = AF_INET;
	recvaddr.sin_port = htons(DEFAULT_TCP_PORT);
	recvaddr.sin_addr.s_addr = INADDR_ANY;

	iResult = bind(ListenSocket, reinterpret_cast<sockaddr*>(&recvaddr), sizeof(recvaddr));
	if (iResult < 0)
		throw network_error(WSAGetLastError(), " bind() failed in accept_connections() ");

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult < 0)
		throw network_error(WSAGetLastError(), " listen() failed in accept_connections() ");

	ConnectionSocket = accept(ListenSocket, NULL, NULL);
	if (ConnectionSocket == INVALID_SOCKET)
		throw network_error(WSAGetLastError(), " accept() failed in accept_connections() ");


	closesocket(ListenSocket);

	return ConnectionSocket;*/
}

unsigned int Utility::getTitleFontSize()
{
	auto size = Director::getInstance()->getVisibleSize();
	if ((size.width < 640) || (size.height < 400))
	{
		return 36;
	}
	else
	{
		return 48;
	}
}

unsigned int Utility::getMinorFontSize()
{
	auto size = Director::getInstance()->getVisibleSize();
	if ((size.width < 640) || (size.height < 400))
	{
		return 18;
	}
	else
	{
		return 24;
	}
}