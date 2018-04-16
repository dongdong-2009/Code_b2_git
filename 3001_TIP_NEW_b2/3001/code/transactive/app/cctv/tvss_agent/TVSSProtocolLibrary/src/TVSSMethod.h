#pragma once

#include <string>
#include  "app/cctv/tvss_agent/TVSSProtocolLibrary/src/ITcpMethod.h"

namespace TA_IRS_App
{

class TVSSMethod :public ITcpMethod
{

protected:

	TVSSMethod(unsigned int methodLength, unsigned char methodType ) ;

public:
    void setTransactionId ( unsigned short transactionId ) ;
	void setTrainId ( unsigned char trainId ) ;

	
			/**
			* Return the specified length of the message header
			*/
	virtual unsigned long getMethodHeaderLength() const;
			/**
		* Return the specified length of the message header
			*/
	virtual unsigned long getShortStringLength() const;

	virtual void appendCRC();

	
public:
	~TVSSMethod(void);
};
};
