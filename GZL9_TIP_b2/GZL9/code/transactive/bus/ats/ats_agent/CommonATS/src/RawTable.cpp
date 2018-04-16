/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\src\RawTable.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #8 $
  *
  * Last modification: $DateTime: 2014/08/26 17:58:19 $
  * Last modified by:  $Author: peter.wong $
  *
  * Raw Table before interpretation
  */

#include "core/utilities/src/TAAssert.h"

#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"

namespace TA_IRS_Bus
{
	RawTable::RawTable(int startAddress, int endAddress)
		: m_dataBlock(startAddress, endAddress), m_dataBlockQuality(TA_Base_Core::ITA_QUALITY_BAD)
	{		
		m_startAddress = startAddress;
		m_endAddress = endAddress;
		m_dataBlock.setAll(0);
	}

	RawTable::RawTable(RawTable & rawtable,int startAddress, int endAddress)
		: m_dataBlock(startAddress, endAddress)
	{
		m_startAddress = startAddress;
		m_endAddress = endAddress;
		m_dataBlockQuality = rawtable.getDatablockQuality();
		for( int i= m_startAddress; i <= m_endAddress; ++i )
		{
			m_dataBlock.set( i, rawtable.m_dataBlock[i] );
		}
	}

	WORD RawTable::getWord (unsigned int byteIndex)
	{
		TA_ASSERT(byteIndex%2 == 0, "getWord() - Trying to read a word across a word boundary");
		TA_ASSERT(isInRange(byteIndex), "Trying to read out of range index");

		WORD word = m_dataBlock[(byteIndex/2) + m_dataBlock.start()];
		return word;
	}

	void RawTable::putWord (unsigned int byteIndex, WORD word)
	{
		TA_ASSERT(byteIndex%2 == 0, "putWord() - Trying to put a word across a word boundary");
		TA_ASSERT(isInRange(byteIndex), "Trying to write out of range index");

		m_dataBlock.set((byteIndex/2) + m_dataBlock.start(), word);
	}

	BYTE RawTable::getByte (unsigned int byteIndex)
	{
		TA_ASSERT(isInRange(byteIndex), "Trying to read out of range index");

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

	void RawTable::putByte (unsigned int byteIndex, BYTE byte)
	{
		TA_ASSERT(isInRange(byteIndex), "Trying to write out of range index");

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

	unsigned long RawTable::getDWord(unsigned int byteIndex)
	{
		TA_ASSERT(byteIndex%2 == 0, "getWord() - Trying to read a word across a word boundary");
		TA_ASSERT(isInRange(byteIndex), "Trying to read out of range index");
		unsigned long result = 0;
		WORD word1 = m_dataBlock[(byteIndex/2) + m_dataBlock.start()];
		byteIndex += 2;
		WORD word2 = m_dataBlock[(byteIndex/2) + m_dataBlock.start()];
		result = word1 << 16;
		result = result + word2;
		return result ;
	}

	void RawTable::displayRawTable()
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"RawTable Data blocks");
			std::string strLog;
			char cData[64] = {0};
		    for( int i= m_startAddress; i <= m_endAddress; ++i )
            {
               	//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				//"RawTable Datablock[%d] = %u",i, m_dataBlock[i]);
				if(i%100 ==0)
					sprintf(cData,"<%d> %x ",i,m_dataBlock[i]);
				else
					sprintf(cData,"%x ",m_dataBlock[i]);
				strLog.append(cData);
				if(strLog.size()>1950)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,strLog.c_str());
					strLog.clear();
				}
            }
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,strLog.c_str());
	}

	void RawTable::setDataBlock(TA_Base_Bus::DataBlock<WORD> dataBlock)
	{
		m_dataBlock = dataBlock;
	}

	void RawTable::setDatablockQuality(TA_Base_Core::EDataPointQuality quality)
	{
		m_dataBlockQuality = quality;
	}

	TA_Base_Core::EDataPointQuality RawTable::getDatablockQuality()
	{
		return m_dataBlockQuality;
	}

}
