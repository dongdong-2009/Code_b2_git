/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/4712_TIP/4712/transactive/bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.cpp $
* @author:  Andy Parker
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2008/12/11 17:47:13 $
* Last modified by:  $Author: builder $
*
* This class implements the Frame Check Sequence
* used to perform sum checks on train messages and frames.
* The main code is used as supplied by Alstom in document
* MAP27 Version 1.5 January 1998 Appendix A1 - An example
* of parallel FCS calculation
*
*/
///////////////////////////////////////////////////////////
//  FrameCheckSequence.cpp
//  Implementation of the Class FrameCheckSequence
//  Created on:      2-Jan-2004
///////////////////////////////////////////////////////////

#include "app/ats/AtsAgent/AtsMethod/src/FrameCheckSequence.h"
//#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{
	
	
	
	FrameCheckSequence::FCSLookupTable FrameCheckSequence::m_lookupTable;
	
	
	FrameCheckSequence::FrameCheckSequence()
	{
		//	FUNCTION_ENTRY("Constructor");
		//	FUNCTION_EXIT;
	}
	
	
	FrameCheckSequence::~FrameCheckSequence()
	{
		//	FUNCTION_ENTRY("Destructor");
		//	FUNCTION_EXIT;
	}
	
	//************************************************************
	//    Function:	fcs_calc	calculates the FCS sequence
	//    Input:	*mtab	pointer to modification table
	//    	*buff	pointer to character buffer
	//    	len	length of character string
	//    Output: 	fcs	frame check sequence
	//    Note:	fcs is initialised with all ones
	//************************************************************
	unsigned short FrameCheckSequence::calculate(std::vector<unsigned char> buffer, unsigned int bufferLen /*=0*/)
	{
		//	FUNCTION_ENTRY("calculate <vector>");
		
		unsigned short calcCRC;
		
		if (bufferLen==0)
		{
			bufferLen = buffer.size();
		}
		
		// Create the buffer
		unsigned char *  newBuffer=NULL;
		if (bufferLen>0)
		{
			newBuffer = new unsigned char[bufferLen];
			//TA_ASSERT(newBuffer!=NULL,"Buffer could not be created.");
			for (unsigned int iLoop=0;iLoop<bufferLen;iLoop++)
			{
				newBuffer[iLoop] = buffer.at(iLoop);
			}
		}
		
		// Generate the checksum
		calcCRC = calculate(newBuffer,bufferLen);
		
		// Clean up
		delete[] newBuffer;
		
		return calcCRC;
	}
	
	//************************************************************
	//    Function:	fcs_calc	calculates the FCS sequence
	//    Input:	*mtab	pointer to modification table
	//    	*buff	pointer to character buffer
	//    	len	length of character string
	//    Output: 	fcs	frame check sequence
	//    Note:	fcs is initialised with all ones
	//************************************************************
	unsigned short FrameCheckSequence::calculate(unsigned char* buffer,unsigned int bufferLen)
	{
		//	FUNCTION_ENTRY("calculate");
		
		unsigned short fcs; 	/* frame check sequence */
		unsigned short q; 	    /* calculation register */
		
		fcs=0xffff; 	/* fcs initialised with all ones */
		while(bufferLen--)
		{
			q=m_lookupTable.getLookup(*buffer++ ^ (fcs>>8));
			fcs=((q&0xff00) ^ (fcs<<8)) | (q&0x00ff);
		}
		
		// TIP5 change
#ifdef TIP5_FIX_ENDIAN
		unsigned short result; // to hold swapped bytes.
		
		/// GJK-TIP5 - crc bytes are reversed, swap them to correct the problem
		result = (fcs&0xff)<<8 | (fcs&0xff00)>>8;
		//	std::cout << "orig " << std::hex << (unsigned int)fcs << " new " << std::hex << (unsigned int)result << std::endl;
		return (fcs^0xffff); /* return the fcs ones complement */
		
#else
		
		//	FUNCTION_EXIT;
		return (fcs^0xffff); /* return the fcs ones complement */
#endif
	}
	
	//********************************************
	// Lookup table private class implementation *
	//********************************************
	
	FrameCheckSequence::FCSLookupTable::FCSLookupTable() : m_CRC16(0xA001)
	{
		//	FUNCTION_ENTRY("Constructor");
		createLookupTable();
		//	FUNCTION_EXIT;
	}
	
	unsigned short FrameCheckSequence::FCSLookupTable::getLookup(unsigned char iPosition)
	{
		//	FUNCTION_ENTRY("getLookup");
		//	FUNCTION_EXIT;
		return m_lookupTable[iPosition];
	}
	
	//************************************************************
	//    Function: create_table	produce the look-up table
	//    Input:    *mtab	pointer to modification table
	//    Output:
	//    Note:     CRC16 is used
	//************************************************************
	void FrameCheckSequence::FCSLookupTable::createLookupTable()
	{
		//	FUNCTION_ENTRY("createLookupTable");
		
		unsigned short btab[8];	// table btab 
		unsigned short i,j; 		// loop parameters 
		unsigned short q;		// calculation register 
		unsigned short shreg;	// shift-register 
		unsigned short carry;
		unsigned short bit;	    // bit parameters
		
		//************************************************************
		//*	Calculate the table btab:
		//************************************************************
		carry=1;	// carry flag set to one 
		shreg=0;	// shreg initialised with 0 
		for(i=0; i<8; i++)
		{
			if(carry) shreg^=m_CRC16;
			btab[i]=(shreg<<8) | (shreg>>8);	// swap bytes 
			carry=shreg & 1;
			shreg >>=1;
		}
		//************************************************************
		//*	Calculate the lookup table
		//************************************************************
		for (i=0; i<256; i++)
		{
			q=0;
			bit=0x80;
			for (j=0; j<8; j++)
			{
				if (bit & i) q^=btab[j];
				bit>>=1;
			}
			m_lookupTable[i]=q;
		}
		
		//	FUNCTION_EXIT;
	}
}