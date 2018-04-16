

/* The source code in this file is the property of 
* ComBuilder Pte. Ltd and is not for redistribution
* in any form.
* Source:   $Source$
* @author:  Lucky
* @version: $Revision$
*
* Last modification: $Date$
* Last modified by:  $Author$
* 
* RadioPDSEventFrameRepeat module wrapped data coming from the client. 
* The wrapping is specific for FrameRepeat
*/

#ifndef RadioPDSEventFrameRepeat_H
#define RadioPDSEventFrameRepeat_H


#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventReader.h"

namespace TA_IRS_App
{
	class RadioPDSEventFrameRepeat : public RadioPDSEventReader
	{
		public:
			RadioPDSEventFrameRepeat( boost::shared_ptr<RadioPDSEvent> event );
			virtual ~RadioPDSEventFrameRepeat();

		private:
			static const unsigned long EVENT_MESSAGE_TYPE;
	};
}
#endif //END of RadioPDSEventFrameRepeat_H