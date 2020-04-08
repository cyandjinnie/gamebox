#include "LinkUnit.h"
#include "cocos2d.h"

USING_NS_CC;

LinkUnit::LinkUnit()
{
	this->_Socket = INVALID_SOCKET;
	this->_Initialized = false;
	this->_Finished = false;
}

LinkUnit::~LinkUnit()
{
	if (this->_Initialized)
	{
		exitViability = true;
		exitCommunication = true;

		checkForViability.get();
		communicationLink.get();
	}
}

void LinkUnit::init(SOCKET Socket, std::function<void(MessageType, const char*)> Handler)
{
	if ((!this->_Initialized) && (Socket != INVALID_SOCKET))
	{
		this->_Socket = Socket;
		this->_Handler = Handler;
		this->_Initialized = true;

		this->exitCommunication = false;
		this->exitViability = false;

		this->checkForViability = std::async(CC_CALLBACK_0(LinkUnit::viabilityThread, this));
		this->communicationLink = std::async(CC_CALLBACK_0(LinkUnit::communicationThread, this));
	}
	else
	{
		throw network_error(999, " LinkUnit::init has been called multiple times ");
	}
}

void LinkUnit::setHandler(std::function<void(MessageType, const char*)> Handler)
{
	if (this->_Initialized)
	{
		this->_Handler = Handler;
	}
	else
	{
		throw network_error(999, " LinkUnit::setHandler has been called while not initialized ");
	}
}

void LinkUnit::communicationThread()
{
	std::string buffer;
	NetMessage msg;
	char buff[256];
	int Result = 1;

	int timeval = 1000;

	Result = setsockopt(_Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeval, sizeof(timeval));
	if (Result < 0)
	{
		int Errno = WSAGetLastError();
		throw network_error(Errno, " setsockopt() failed in LinkUnit ");
	}

	Result = 1;
	while ((!this->exitCommunication) && (Result >= 0))
	{
		Result = recv(_Socket, buff, sizeof(buff), 0);

		if (Result > 0)
		{
			Utility::decodeMessage(buff, msg);

			Director::getInstance()->getScheduler()->performFunctionInCocosThread(
				std::bind(
					this->_Handler, msg.msg_type, msg.msg_text.c_str()
				)
			);
		}
		else if (Result == 0)
		{
			Director::getInstance()->getScheduler()->performFunctionInCocosThread(
				std::bind(
					this->_Handler, MessageType::CONNECTION_CLOSED, ""
				)
			);
		}
		else if (Result < 0)
		{ 
			Result = WSAGetLastError();
			if (Result == 10060)
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}

	this->_Finished = true;
	if (!this->exitCommunication)
		Director::getInstance()->getScheduler()->performFunctionInCocosThread(CC_CALLBACK_0(LinkUnit::destroyWithNotification, this));
}

void LinkUnit::viabilityThread()
{
	timeval time_out;
	time_out.tv_sec = 0;
	time_out.tv_usec = 500000; // 0.5 sec ????
	int iResult;

	fd_set setW;

	FD_ZERO(&setW);
	FD_SET(this->_Socket, &setW);

	while (!this->exitViability)
	{
		iResult = select(0, NULL, &setW, NULL, &time_out);

		if (iResult <= 0)
			break;
		else
			std::this_thread::sleep_for(std::chrono::seconds(2));
	}

	this->_Finished = true;
	if (!this->exitViability)
		Director::getInstance()->getScheduler()->performFunctionInCocosThread(CC_CALLBACK_0(LinkUnit::destroyWithNotification, this));
}

SOCKET LinkUnit::getSocket()
{
	if (this->_Initialized)
		return this->_Socket;
	else
		return INVALID_SOCKET;
}

int LinkUnit::sendMsg(const char* message)
{
	std::string buff(message);

	return send(_Socket, buff.c_str(), buff.size(), 0);
}

void LinkUnit::destroy()
{
	if (this->_Initialized)
	{
		this->exitViability = true;
		this->exitCommunication = true;
		this->_Finished = true;

		checkForViability.get();
		communicationLink.get();

		this->_Initialized = false;
		this->_Socket = INVALID_SOCKET;
	}
}

void LinkUnit::destroyWithNotification()
{
	if (this->_Initialized)
	{
		this->destroy();

		Director::getInstance()->getScheduler()->performFunctionInCocosThread(
			std::bind(
				this->_Handler, CONNECTION_CLOSED, ""
			)
		);
	}
}

bool LinkUnit::initialized()
{
	return this->_Initialized;
}

int LinkUnit::sendMsg(MessageType type, const char* message)
{
	NetMessage msg;
	msg.msg_text = message;
	msg.msg_type = type;

	std::string buff;
	Utility::encodeMessage(msg, buff);

	return send(this->_Socket, buff.c_str(), buff.size(), 0);
}