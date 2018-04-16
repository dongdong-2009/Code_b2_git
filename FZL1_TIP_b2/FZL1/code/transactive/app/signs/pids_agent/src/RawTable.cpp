/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_TIP/4669/transactive/app/ats/AtsAgent/src/RawTable.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:15:01 $
  * Last mofified by:  $Author: grace.koh $
  *
  * Raw Table before interpretation
  */

#include "core/utilities/src/TAAssert.h"

#include "app/signs/pids_agent/src/RawTable.h"

namespace TA_IRS_App
{
	RawTable::RawTable(int startAddress, int endAddress)
		: m_dataBlock(startAddress, endAddress)
	{
		m_dataBlock.setAll(0);
	}

	WORD RawTable::getWord (unsigned int byteIndex)
	{
//		TA_ASSERT(byteIndex%2 == 0, "getWord() - Trying to read a word across a word boundary");
//		TA_ASSERT(isInRange(byteIndex), "Trying to read out of range index");
		if(!(byteIndex%2 == 0))
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"getWord() - Trying to read a word across a word boundary");
			return 0;
		}
		if(!isInRange(byteIndex))
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Trying to read out of range index");
			return 0;
		}

		WORD word = m_dataBlock[(byteIndex/2) + m_dataBlock.start()];
		return word;
	}

	void RawTable::putWord (unsigned int byteIndex, WORD word)
	{
//		TA_ASSERT(byteIndex%2 == 0, "putWord() - Trying to put a word across a word boundary");
//		TA_ASSERT(isInRange(byteIndex), "Trying to write out of range index");
		if(!(byteIndex%2 == 0))
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"putWord() - Trying to put a word across a word boundary");
			return;
		}
		if(!isInRange(byteIndex))
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Trying to write out of range index");
			return;
		}

		m_dataBlock.set((byteIndex/2) + m_dataBlock.start(), word);
	}

	BYTE RawTable::getByte (unsigned int byteIndex)
	{
//		TA_ASSERT(isInRange(byteIndex), "Trying to read out of range index");
		if(!isInRange(byteIndex))
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Trying to read out of range index");
			return 0;
		}

		BYTE byte;
		WORD word = m_dataBlock[(byteIndex/2) + m_dataBlock.start()];

		if (byteIndex%2 == 0)
		{
			byte = word / 256;
		}
		else
		{
			byte = word % 256;
		}

		return byte;
	};
	
	//高字节在前，0字节为第一个word的高字节
	void RawTable::putByte (unsigned int byteIndex, BYTE byte)
	{
//		TA_ASSERT(isInRange(byteIndex), "Trying to write out of range index");
		if(!isInRange(byteIndex))
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Trying to write out of range index");
			return;
		}

		WORD word = m_dataBlock[(byteIndex/2) + m_dataBlock.start()];
		WORD newWord = 0;

		if (byteIndex%2 == 0)
		{
			newWord = byte*256 + (word & 0xFF); 
		}
		else
		{
			newWord = (word & 0xFF00) + byte;
		}

		m_dataBlock.set((byteIndex/2) + m_dataBlock.start(), newWord); 
	};

	WORD RawTable::getBitsFromWord(unsigned int byteIndex, unsigned int startBit, unsigned int stopBit)
	{
		WORD result = 0;
		unsigned int readBit, writeBit;
		WORD word = getWord(byteIndex);

		for (readBit = startBit, writeBit = 0; readBit <= stopBit; readBit++, writeBit++) 
		{
			if (bitTest(word, readBit))
			{
				result = bitSet(result, writeBit);
			}
			else
			{
				result = bitClear(result, writeBit);
			}
		}

		return result;
	}

	void RawTable::putBitsToWord(unsigned int byteIndex, unsigned int startBit, unsigned int stopBit, WORD bits)
	{
		unsigned int readBit, writeBit;
		WORD word = getWord(byteIndex);

		for (writeBit = startBit, readBit = 0; writeBit <= stopBit; writeBit++, readBit++) 
		{
			if (bitTest(bits, readBit))
			{
				word = bitSet(word, writeBit);
			}
			else
			{
				word = bitClear(word, writeBit);
			}
		}

		putWord(byteIndex, word);
	}

	bool RawTable::testBitInWord(unsigned int byteIndex, unsigned int bitNum)
	{
		WORD word = getWord(byteIndex);

		return bitTest(word, bitNum);
	}

	void RawTable::putBitToWord(unsigned int byteIndex, unsigned int bitNum, bool bit)
	{
		WORD word = getWord(byteIndex);
		
		if (bit)
		{
			word = bitSet(word, bitNum);
		}
		else
		{
			word = bitClear(word, bitNum);
		}

		putWord(byteIndex, word);
	}

	bool RawTable::bitTest(unsigned short theWord, unsigned int bitNum)
    {
        if (bitNum > 15)
        {
            return false;
        }

        return ((theWord & (0x0001 << bitNum)) != 0);
    }

    unsigned short RawTable::bitSet(unsigned short theWord, unsigned int bitNum)
    {
        if (bitNum > 15)
        {
            return theWord;
        }

        return (theWord | (0x0001 << bitNum));
    }

    unsigned short RawTable::bitClear(unsigned short theWord, unsigned int bitNum)
    {
        if (bitNum > 15)
        {
            return theWord;
        }

        return (theWord & (~(0x0001 << bitNum)));
    }

	void RawTable::putBlankRecord(unsigned int byteIndex, unsigned int recordSize)
	{
		for (unsigned int wordIndex = 0; wordIndex < recordSize; wordIndex++)
		{
			putWord(byteIndex + (wordIndex * 2), 0);
		}
	}

	bool RawTable::isInRange (unsigned int byteIndex)
	{
		return m_dataBlock.isInRange((byteIndex/2) + m_dataBlock.start());
	}

	int RawTable::length()
	{
		return m_dataBlock.length();
	}

	TA_Base_Bus::DataBlock<WORD>& RawTable::getDataBlock()
	{
		return m_dataBlock;
	}
}
