/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/RawTable.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * Raw Table before interpretation
  */

#include "core/utilities/src/TAAssert.h"

#include "app/ats/AtsAgent/src/RawTable.h"

//#include "app/ats/AtsAgent/src/ProtocolException.h"
#include "core/exceptions/src/AtsAgentException.h"
#include <string>
namespace TA_IRS_App
{
	RawTable::RawTable(int startAddress, int endAddress)
		: m_dataBlock(startAddress, endAddress)
	{
		m_dataBlock.setAll(0);
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
	unsigned long  RawTable::getAsciiNumber(unsigned short position, unsigned short numDigits) 
	{
		FUNCTION_ENTRY("getAsciiNumber");
		unsigned long num = 0;
		unsigned char digit = 0;
		std::string strNum;
		for(int i = 0; i < numDigits; i++)
		{
			digit= getByte(position+i);			
            strNum += digit;
        }
		//num = atoi(strNum.c_str());
		if(numDigits == 3)
			num = ((strNum.at(0)<<16)& 0xff0000) + ((strNum.at(1)<<8) & 0x00ff00) + (strNum.at(2) & 0x0000ff);
		if(numDigits == 2)
			num = ((strNum.at(0)<<8) & 0xff00) + (strNum.at(1) & 0x00ff);
		if(numDigits == 1)
			num = (strNum.at(0) & 0x0000ff);
	
	
		FUNCTION_EXIT;
		return num;

		
	}

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
		return m_dataBlock.isInRange((byteIndex/2)+ m_dataBlock.start());
	}

	int RawTable::length()
	{
		return m_dataBlock.length();
	}

	TA_Base_Bus::DataBlock<WORD>& RawTable::getDataBlock()
	{
		return m_dataBlock;
	}

	void RawTable::getLogText(std::string& hexText, std::string& consoleText, unsigned int recordByteOffset, int size) 
	{
		char* temp = new char[24];
		memset(temp, 0, 24);
		for(unsigned int byteIndex=0; byteIndex< size; byteIndex++)
		{
			consoleText += "..";
			sprintf(temp," %.4X",m_dataBlock[ (byteIndex + (recordByteOffset/2)) + m_dataBlock.start()]);
			hexText += temp;
		}
		delete[] temp;
	}

}
