/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3002_TIP/3002/transactive/app/signs/PidsAgent/src/RawTable.h $
  * @author:   Zhang Hongzhi
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:15:01 $
  * Last mofified by:  $Author: grace.koh $
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
	const int MODBUS_START_ADDRESS = 1;
	const int MODBUS_END_ADDRESS = 510;
//	const int MODBUS_MAX_LENGTH = 510;		//word
	const int MESSAGE_HEADER_LENGTH = 10;	//word index

	const unsigned long STATION_SELECT_BIT_IN_WORD = 0u;
	const unsigned long TRAIN_SELECT_BIT_IN_WORD = 1u;
	const unsigned long STATION_MESSAGE_PRIORITY_BIT_IN_WORD = 2u;
	const unsigned long STATION_DISPLAY_LOCATION_BIT_IN_WORD = 3u;
	const unsigned long STATION_CLEAR_MESSAGE_BIT_IN_WORD = 4u;
	const unsigned long TRAIN_MESSAGE_PRIORITY_BIT_IN_WORD = 5u;
	const unsigned long TRAIN_DISPLAY_LOCATION_BIT_IN_WORD = 6u;
	const unsigned long TRAIN_CLEAR_MESSAGE_BIT_IN_WORD = 7u;
	const unsigned long TRAIN_MESSAGE0_BIT_IN_WORD = 8u;
	const unsigned long TRAIN_MESSAGE1_BIT_IN_WORD = 9u;
	const unsigned long TRAIN_MESSAGE2_BIT_IN_WORD = 10u;
	const unsigned long TRAIN_MESSAGE3_BIT_IN_WORD = 11u;
	const unsigned long TRAIN_MESSAGE4_BIT_IN_WORD = 12u;
	const unsigned long TRAIN_MESSAGE5_BIT_IN_WORD = 13u;
	const unsigned long TRAIN_MESSAGE6_BIT_IN_WORD = 14u;
	const unsigned long ALL_TRAIN_MESSAGE_BIT_IN_WORD = 15u;
//	const char* const PARAM_PRIORITY_ID = "(PriorityId)";

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
