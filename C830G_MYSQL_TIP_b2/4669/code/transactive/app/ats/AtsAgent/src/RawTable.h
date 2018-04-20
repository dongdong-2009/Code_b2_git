/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/RawTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last mofified by:  $Author: builder $
  *
  * Raw Table wrapping data block
  */
#ifndef RAWTABLE_H
#define RAWTABLE_H

#include <sstream>

#include "bus/modbus_comms/src/IModbus.h"
#include "bus/modbus_comms/src/DataBlock.h"

namespace TA_IRS_App
{
    class RawTable
    {
    public:
									RawTable(int startAddress, int endAddress);
		virtual						~RawTable() {};

		unsigned short				getWord (unsigned int byteIndex);
		void						putWord (unsigned int byteIndex, WORD word);

		unsigned char				getByte (unsigned int byteIndex);
		void						putByte (unsigned int byteIndex, BYTE byte);

		unsigned short				getBitsFromWord(unsigned int byteIndex, unsigned int startBit, unsigned int stopBit);
		void						putBitsToWord(unsigned int byteIndex, unsigned int startBit, unsigned int stopBit, WORD bits);
		bool						testBitInWord(unsigned int byteIndex, unsigned int bitNum);
		void						putBitToWord(unsigned int byteIndex, unsigned int bitNum, bool bit);
						
		void						putBlankRecord(unsigned int byteIndex, unsigned int recordSize);

		bool						isInRange(unsigned int byteIndex);
		int							length();

		TA_Base_Bus::DataBlock<WORD>&	getDataBlock();

		static bool					bitTest(unsigned short theWord, unsigned int bitNum);
		static unsigned short		bitSet(unsigned short theWord, unsigned int bitNum);
		static unsigned short		bitClear(unsigned short theWord, unsigned int bitNum);

	private:
									RawTable() {};

	protected:

		TA_Base_Bus::DataBlock<WORD>		m_dataBlock;
	};
}

#endif 
