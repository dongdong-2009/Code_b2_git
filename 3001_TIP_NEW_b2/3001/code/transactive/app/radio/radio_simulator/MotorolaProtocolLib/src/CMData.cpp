
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/CMData.cpp $
  * @author:  Wen Ching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMData.h"



CMData::CMData()
{

}

CMData::~CMData()
{

}

void CMData::setSrcAddr(unsigned int address)
{
	m_srcAddr = address;	
}

void CMData::setDstAddr(unsigned int address)
{
	m_dstAddr = address;
}

void CMData::setProtocolId(CMProtocolIdentifier Id)
{
	m_protocolId = Id;
}

void CMData::setFlags(unsigned char flags)
{
	m_flags = flags;
}

void CMData::setMsgRef(unsigned char msgRef)
{
	m_msgRef = msgRef;
}

void CMData::setAreaSel(unsigned char areaSel)
{
	m_areaSel = areaSel;
}

void CMData::setTLPduType(TLPDUType pduType)
{
	m_tlPduType = pduType;
}

void CMData::setCMPduType(CMPDUType pduType)
{
	m_cmPduType = pduType;
}

void CMData::setServiceSelection(CMServiceSelection serviceSelection)
{
	m_cmServiceSelection = serviceSelection;
}

void CMData::setTLlength(unsigned int length)
{
	m_tlLength = length;
}

void CMData::setTLheader(std::string header) 
{
	m_tlHeader = header;
}

void CMData::setUserData(std::string data)
{
	m_userData = data;
}

unsigned int CMData::getSrcAddr()
{
	return m_srcAddr;
}

unsigned int CMData::getDstAddr()
{
	return m_dstAddr;
}

CMProtocolIdentifier CMData::getProtocolId()
{
	return m_protocolId;
}

unsigned char CMData::getFlags()
{
	return m_flags;
}

unsigned char CMData::getMsgRef()
{
	return m_msgRef;
}

unsigned char CMData::getAreaSel()
{
	return m_areaSel;
}

TLPDUType CMData::getTLPduType()
{
	return m_tlPduType;
}

CMPDUType CMData::getCMPduType()
{
	return m_cmPduType;
}

CMServiceSelection CMData::getServiceSelection()
{
	return m_cmServiceSelection;
}

unsigned int CMData::getTLlength()
{
	return m_tlLength;
}

std::string CMData::getTLheader()
{
	return m_tlHeader;
}

std::string CMData::getUserData()
{
	return m_userData;
}