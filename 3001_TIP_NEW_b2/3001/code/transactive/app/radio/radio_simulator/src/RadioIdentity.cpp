
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/RadioIdentity.cpp $
  * @author:  Wen Ching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#include "app/radio/radio_simulator/src/RadioIdentity.h"


RadioIdentity::RadioIdentity()
{

}


RadioIdentity::RadioIdentity(unsigned int SSI, unsigned long passNumber)
:m_SSIAddress(SSI), m_PassNumber(passNumber)
{

}

RadioIdentity::RadioIdentity(const RadioIdentity &radioIdentity)
{
	m_PassNumber = radioIdentity.m_PassNumber;
	m_SSIAddress = radioIdentity.m_SSIAddress;
}

RadioIdentity::~RadioIdentity()
{

}

void RadioIdentity::setSSI(unsigned int SSI)
{
	m_SSIAddress = SSI;
}

void RadioIdentity::setPassNumber(unsigned long passNumber)
{
	m_PassNumber = passNumber;
}

unsigned int RadioIdentity::getSSI()
{
	return m_SSIAddress;
}

unsigned long RadioIdentity::getPassNumber()
{
	return m_PassNumber;
}
