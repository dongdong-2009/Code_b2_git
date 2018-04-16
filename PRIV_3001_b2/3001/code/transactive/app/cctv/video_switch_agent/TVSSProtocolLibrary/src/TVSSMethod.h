#pragma once

#include <string>
#include  "app/cctv/video_switch_agent/TVSSProtocolLibrary/src/ITcpMethod.h"

namespace TA_IRS_App
{

class TVSSMethod :public ITcpMethod
{

protected:

	TVSSMethod(unsigned int methodLength, unsigned char methodType ) ;

public:
    void setTransactionId ( unsigned char transactionId ) ;
	void setTrainId ( unsigned char trainId ) ;

	
			/**
			* Return the specified length of the message header
			*/
	virtual unsigned long getMethodHeaderLength() const;
			/**
			* Set the specified length of the message header
			*/
	virtual void setMethodHeaderLength(unsigned int messageSize);
			/**
		* Return the specified length of the message header
			*/
	virtual unsigned long getShortStringLength() const;

	
public:
	~TVSSMethod(void);
};
};
