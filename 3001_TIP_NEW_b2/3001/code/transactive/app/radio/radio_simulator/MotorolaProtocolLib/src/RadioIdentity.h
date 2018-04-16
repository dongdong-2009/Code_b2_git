
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/RadioIdentity.h $
  * @author:  Wen Ching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#ifndef  _RADIO_IDENTITY_H_
#define  _RADIO_IDENTITY_H_


class RadioIdentity{

public:
	RadioIdentity();
	RadioIdentity(unsigned int SSI, unsigned long passNumber);
	RadioIdentity(const RadioIdentity &radioIdentity);
	~RadioIdentity();

	void setSSI(unsigned int SSI);
	void setPassNumber(unsigned long passNumber);
	unsigned int getSSI();
	unsigned long getPassNumber();

private:

	unsigned int m_SSIAddress;
	unsigned long m_PassNumber;
};


#endif