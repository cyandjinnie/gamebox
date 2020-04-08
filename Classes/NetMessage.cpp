#include "NetMessage.h"

NetMessage::NetMessage(const char* _text, MessageType _type)
{
	this->msg_text = _text;
	this->msg_type = _type;
}

NetMessage::NetMessage()
{
	this->msg_text = "";
	this->msg_type = UNDEFINED;
}