#include "NetworkError.h"

network_error::network_error(int _error_code, const char* message)
{
	this->error_msg = message + std::to_string(_error_code);
}

network_error::network_error(int _error_code)
{
	this->error_msg = std::to_string(_error_code);
}

const char* network_error::what()
{
	return this->error_msg.c_str();
}
