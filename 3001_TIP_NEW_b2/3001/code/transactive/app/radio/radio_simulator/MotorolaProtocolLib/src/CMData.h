
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/CMData.h $
  * @author:  Wen Ching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */
#pragma once
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include <string>

class CMData{

public:
	CMData();
	~CMData();

	void setSrcAddr(unsigned int address);
	void setDstAddr(unsigned int address);
	void setProtocolId(CMProtocolIdentifier Id);
	void setFlags(unsigned char flags);
	void setMsgRef(unsigned char msgRef);
	void setAreaSel(unsigned char areaSel);
	void setTLPduType(TLPDUType pduType);
	void setCMPduType(CMPDUType pduType);
	void setServiceSelection(CMServiceSelection serviceSelection);
	void setTLlength(unsigned int length);
	void setTLheader(std::string header);
	void setUserData(std::string data);

	unsigned int getSrcAddr();
	unsigned int getDstAddr();
	CMProtocolIdentifier getProtocolId();
	unsigned char getFlags();
	unsigned char getMsgRef();
	unsigned char getAreaSel();
	TLPDUType getTLPduType();
	CMPDUType getCMPduType();
	CMServiceSelection getServiceSelection();
	unsigned int getTLlength();
	std::string getTLheader();
	std::string getUserData();



private:
	unsigned int m_srcAddr;
	unsigned int m_dstAddr;
	CMProtocolIdentifier m_protocolId;
	unsigned char m_flags;
	unsigned char m_msgRef;
	unsigned char m_areaSel;
	TLPDUType m_tlPduType;
	CMPDUType m_cmPduType;
	CMServiceSelection m_cmServiceSelection;
	unsigned int m_tlLength;
	std::string m_tlHeader;
	std::string m_userData;
};
