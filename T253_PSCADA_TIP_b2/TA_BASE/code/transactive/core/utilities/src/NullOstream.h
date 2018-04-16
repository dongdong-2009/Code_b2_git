/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/utilities/src/NullOstream.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#ifndef NULL_OSTREAM_H
#define NULL_OSTREAM_H

#include <iostream>
//#include <streambuf>
using std::streambuf;

using std::ostream;

namespace TA_Base_Core
{
	class NullStreamBuf: public streambuf
	{
	public:
		NullStreamBuf(): streambuf(){}
		virtual ~NullStreamBuf(){}

		void	put_buffer(void){};
		void	put_char(int){};
	};


	class NullOstream: public ostream
	{
	public:
		NullOstream(): ostream(new NullStreamBuf()){}
		virtual ~NullOstream(){}
	};
};

#endif // NULL_OSTREAM_H
