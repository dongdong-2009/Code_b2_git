/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4712_TIP/4712/transactive/bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/12/11 17:47:13 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#ifndef	_FRAMECHECK_SEQUENCE_H
#define _FRAMECHECK_SEQUENCE_H

#include <vector>


namespace TA_IRS_App
{
	class FrameCheckSequence
	{
		private:
			class FCSLookupTable
			{
				public:
					FCSLookupTable();
					unsigned short getLookup(unsigned char iPosition);

				private:
					void createLookupTable();

					unsigned short m_lookupTable[256];
					const unsigned short m_CRC16;
			};

		public:
			FrameCheckSequence();
			virtual ~FrameCheckSequence();

		public:
			static unsigned short calculate(unsigned char* buffer,unsigned int bufferLen);
			static unsigned short calculate(std::vector<unsigned char>buffer, unsigned int bufferLen=0);

		private:
			static FCSLookupTable m_lookupTable;
	};
};

#endif // !defined(FrameCheckSequence__INCLUDED_)
