/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/IncludeEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of Join Event Reader
  *
  */


#if !defined(IncludeEventReader_INCLUDED_)
#define IncludeEventReader_INCLUDED_

#include "RadioEventReader.h"


namespace TA_IRS_App
{
    class IncludeEventReader : public RadioEventReader
    {
        public:

            IncludeEventReader(RadioEvent & event);
            virtual ~IncludeEventReader();

            unsigned long getCallReferenceSrc() const;
            unsigned long getCallReferenceDst() const;

        private:
            static const unsigned int INCLUDE_EVENT_LENGTH;
            static const unsigned int CALL_REFERENCE_SRC_START;
            static const unsigned int CALL_REFERENCE_DST_START;
    };
};

#endif // !defined(IncludeEventReader_INCLUDED_)
