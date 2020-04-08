#ifndef __LINK_UNIT_H__
#define __LINK_UNIT_H__

#include <functional>
#include <future>
#include <atomic>
#include "NetworkUtility.h"
#include "cocos2d.h"

class LinkUnit
{
public:
	LinkUnit();
	~LinkUnit();
	void init(SOCKET _Socket, std::function<void(MessageType, const char*)> _Handler);
	void setHandler(std::function<void(MessageType, const char*)> _Handler);
	void destroy();
	void destroyWithNotification();
	SOCKET getSocket();
	int sendMsg(const char* message);
	int sendMsg(MessageType type, const char* message);
	bool initialized();
private:
	std::function<void(MessageType, const char*)> _Handler;

	void communicationThread();
	void viabilityThread();

	SOCKET _Socket;
	bool _Initialized;
	bool _Finished;

	std::future<void> checkForViability;
	std::atomic<bool> exitViability;

	std::future<void> communicationLink;
	std::atomic<bool> exitCommunication;
};

#endif // __LINK_UNIT_H__