/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\src\RawTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2010/10/13 15:35:06 $
  * Last modified by:  $Author: shankri $
  *
  * Raw Table wrapping data block
  */
#ifndef RAWTABLE_H
#define RAWTABLE_H

#include <sstream>

#include "bus/modbus_comms/src/IModbus.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "core/types/public_types/Constants.h"


namespace TA_IRS_Bus
{
    class RawTable
    {
    public:
		/**
		 * RawTable(int startAddress, int endAddress)
		 *
		 * A constructor used to initialize RawTable data based on Start and End Address of the Data block.
		 * The Data block values are set to 0 by default
		 *
		 * @param startAddress : Start Address of Data Block 
		 * @param endAddress : End Address of Data Block
		 */
		RawTable(int startAddress, int endAddress);
		/**
		 * RawTable(RawTable & rawtable,int startAddress, int endAddress)
		 *
		 * A constructor that has a rawtable which will be used to copy the values based on the Start and End Address of the Data Block 
		 *
		 * @param rawtable : The rawtable to copy
		 * @param startAddress : Start Address of Data Block 
		 * @param endAddress : End Address of Data Block
		 */
		RawTable(RawTable & rawtable,int startAddress, int endAddress);
		/**
		 * Overloading equality for RawTable 
		 *
		 * Use to copy the values from the rawTable.
		 *
		 * @param rawtable : The rawtable to copy
		 */
		RawTable& operator= (const RawTable& rawTable);
		virtual						~RawTable() {};

		/**
		 * getWord
		 *
		 * Method used to get a value which is a WORD or (2bytes) in size based on the byte Index.
		 * Where byte index is an even numbers.
		 *
		 * @param byteIndex : Byte Index of the rawtable
		 * @return : unsigned short - The word (2bytes) in size
		 */
		unsigned short				getWord (unsigned int byteIndex);

		/**
		 * putWord
		 *
		 * Method used to write a word in the Data Block
		 *
		 * @param byteIndex : Byte Index to be written.
		 * @param word : The value to be written.
		 */
		void						putWord (unsigned int byteIndex, WORD word);

		/**
		 * getByte
		 *
		 * Method used to get a byte(1byte) based on the byte index
		 *
		 * @param byteIndex : Byte Index to be written.
		 * @param word : The value to be written.
		 * @param : unsigned char - 1Byte in size
		 */
		unsigned char				getByte (unsigned int byteIndex);

		/**
		 * putWord
		 *
		 * Method used to write a byte in the Data Block
		 *
		 * @param byteIndex : Byte Index to be written.
		 * @param byte : The value to be written.
		 */
		void						putByte (unsigned int byteIndex, BYTE byte);

		/**
		 * getBitsFromWord
		 *
		 * Method used to retrieve the specific bits from a Word
		 *
		 * @param byteIndex : Byte Index to read.
		 * @param startBit : Start bit to read.
		 * @param stopBit : Stop bit to read.
		 */
		unsigned short				getBitsFromWord(unsigned int byteIndex, unsigned int startBit, unsigned int stopBit);

		/**
		 * putBitsToWord
		 *
		 * Method used to write a bits to a Word
		 *
		 * @param byteIndex : Byte Index to write.
		 * @param startBit : Start bit to write.
		 * @param stopBit : Stop bit to write.
		 * @param bits : Bits to write
		 */
		void						putBitsToWord(unsigned int byteIndex, unsigned int startBit, unsigned int stopBit, WORD bits);

		/**
		 * testBitInWord
		 *
		 * Method used to test the specific Bit in a word
		 *
		 * @param byteIndex : Byte Index to write.
		 * @param startBit : Start bit to write.
		 */
		bool						testBitInWord(unsigned int byteIndex, unsigned int bitNum);

		/**
		 * putBitsToWord
		 *
		 * Method used to write a bits to a Word
		 *
		 * @param byteIndex : Byte Index to write.
		 * @param startBit : Start bit to write.
		 * @param stopBit : Stop bit to write.
		 * @param bits : Bits to write
		 */
		void						putBitToWord(unsigned int byteIndex, unsigned int bitNum, bool bit);
						
		/**
		 * putBlankRecord
		 *
		 * Method used to write a Blank record of the Data Block
		 *
		 * @param byteIndex : Byte Index to write.
		 * @param recordSize : The size of the record.
		 */
		void						putBlankRecord(unsigned int byteIndex, unsigned int recordSize);

		/**
		 * putBlankRecord
		 *
		 * Method used to check if the byte index is the range of the Data block
		 *
		 * @param byteIndex : Byte Index to write.
		 * @return bool : returns true/false if the byte index is in the range of the Data Block.
		 */
		bool						isInRange(unsigned int byteIndex);

		/**
		 * length
		 *
		 * Method used to return the size of the Data block
		 *
		 * @return int : size of the Data Block.
		 */
		int							length();

		/**
		 * getDataBlock
		 *
		 * Method used to get the Data block
		 *
		 * @return TA_Base_Bus::DataBlock<WORD>& : Data Block.
		 */
		TA_Base_Bus::DataBlock<WORD>&	getDataBlock();

		/**
		 * setDataBlock
		 *
		 * Method used to set the Data block
		 *
		 * @return TA_Base_Bus::DataBlock<WORD>& : Data Block.
		 */
		void setDataBlock(TA_Base_Bus::DataBlock<WORD> dataBlock);

		/**
		 * bitTest
		 *
		 * Method used to test the bit
		 *
		 * @param theWord: The word to test the bit
		 * @param binNum : The bit number to test.
		 * @return bool: The bit test result
		 */
		static bool					bitTest(unsigned short theWord, unsigned int bitNum);

		/**
		 * bitSet
		 *
		 * Method used to set the bit
		 *
		 * @param theWord: The word to set the bit
		 * @param binNum : The bit number.
		 */
		static unsigned short		bitSet(unsigned short theWord, unsigned int bitNum);

		/**
		 * bitClear
		 *
		 * Method used to clear the bit
		 *
		 * @param theWord: The word to clear bit
		 * @param binNum : The bit number.
		 */
		static unsigned short		bitClear(unsigned short theWord, unsigned int bitNum);

		/**
		 * getDWord
		 *
		 * Method used to get a value for a DWORD or (4bytes) in size based on the byte Index.
		 * Where byte index is an even numbers.
		 *
		 * @param byteIndex : Byte Index of the rawtable
		 * @return : unsigned long - The DWord (4bytes) in size
		 */
		unsigned long				getDWord (unsigned int byteIndex);

		/**
		 * displayRawTable
		 *
		 * Method used to display the Raw Table
		 */
		void						displayRawTable();

		/**
		 * setDatablockQuality
		 *
		 * Sets the quality of the Data block
		 *
		 * @param dataBlockQuality
		 */
		void setDatablockQuality(TA_Base_Core::EDataPointQuality quality);

		/**
		 * getDatablockQuality
		 *
		 * Gets the quality state of the Data block
		 *
		 * @param dataBlockQuality
		 */
		TA_Base_Core::EDataPointQuality getDatablockQuality();

	private:
									RawTable() {};					
		int							m_startAddress;
		int							m_endAddress;
		TA_Base_Core::EDataPointQuality  m_dataBlockQuality;

	protected:

		TA_Base_Bus::DataBlock<WORD>		m_dataBlock;
	};
}

#endif 
