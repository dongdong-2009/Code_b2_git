#ifndef ICLIENT_CONNECTION_OBSERVER_H
#define ICLIENT_CONNECTION_OBSERVER_H

#include <vector>

class IClientConnectionObserver
{
	public:
		virtual void connectionEstablish (const int & socketID, struct sockaddr* clientAddress, std::basic_string<unsigned char> socketData) = 0;

};
#endif