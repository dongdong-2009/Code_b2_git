/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/RawTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
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

		unsigned long getAsciiNumber(unsigned short position, unsigned short numDigits) ;
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
		
		//Added Lucky
		/**
          * Get the message Train Table data in printable format
          */
         void getLogText(std::string& hexText, std::string& consoleText, unsigned int recordByteOffset,int size);

	private:
									RawTable() {};

	protected:

		TA_Base_Bus::DataBlock<WORD>		m_dataBlock;
	};
}

#endif 
