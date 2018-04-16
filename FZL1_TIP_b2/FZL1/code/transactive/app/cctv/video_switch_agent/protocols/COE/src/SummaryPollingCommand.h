#ifndef SUMMARY_POLLING_COMMAND_H
#define SUMMARY_POLLING_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Object for requesting inputs be switched to a quad and for
  * processing the response.
  *
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"
#include <boost/noncopyable.hpp>

// Forward declarations.

namespace TA_IRS_App
{
    class COECommunicationsHandler;
}

namespace TA_IRS_App
{
    class SummaryPollingCommand : public AbstractCOERequestCommand
    {
        public:

            /**
              * SummaryPollingCommand
              *
              * description
              *
              * @param communicationsChannelIndex
              * @param
              *
              */
            SummaryPollingCommand();


            /**
              * ~SummaryPollingCommand
              *
              * Standard destructor.
              */
            virtual ~SummaryPollingCommand() {};


            /**
              * processResponse
              *
              * Processes the response to this command that has been received from
              * the switching hardware.
              *
              * @return     bool
              *             True if the response was valid.
              *             False otherwise.
              *
              * @param      DataVector& data
              *             The COE to process.  This will have the checksum removed.
              */
            virtual bool processResponse(const std::vector< unsigned char >& data);
    };
}

#endif // SUMMARY_POLLING_COMMAND_H
