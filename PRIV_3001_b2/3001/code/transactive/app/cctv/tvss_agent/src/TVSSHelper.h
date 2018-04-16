
#ifndef TVSSHELPER_H
#define TVSSHELPER_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File:  $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2010/01/05 14:12:57 $
  * Last modified by : $Author:  $
  *
  */
#include <string>
#include "TVSS.h"

namespace TA_IRS_App
{
	class TVSS;
	
	class TVSSHelper
	{
	public:
		static int tokenise(const char * s, std::vector<std::string> & tokens, const char * delimiters=" ", int maxtokens=0xFFFF);
	private:
		/**
             * Constructor "hidden", since all methods are static this class is
             * not intended for instantiation
             */
            TVSSHelper();

            /**
             * Destructor "hidden", since all methods are static this class is
             * not intended for instantiation or destruction
             */
            ~TVSSHelper();

	};
}

#endif