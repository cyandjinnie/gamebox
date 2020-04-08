#ifndef __NETWORK_ERROR__
#define __NETWORK_ERROR__

#include <exception>
#include <string>

class network_error :public std::exception
{
private:
	std::string error_msg;
public:
	network_error(int, const char*);
	network_error(int);
	// Get the error message
	virtual const char* what();
};

#endif

