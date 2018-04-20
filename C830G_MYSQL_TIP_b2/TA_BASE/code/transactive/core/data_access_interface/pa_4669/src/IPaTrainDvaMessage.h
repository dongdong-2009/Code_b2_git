/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/IPaTrainDvaMessage.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IPaTrainDvaMessage is an interface to a PaTrainDvaMessage object.  It allows the PaTrainDvaMessage object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  */

#if !defined(IPaTrainDvaMessage_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IPaTrainDvaMessage_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IPaTrainDvaMessage;

    class IPaTrainDvaMessage : public virtual TA_Base_Core::IItem
    {

    public:

        virtual unsigned long getKey() =0;
        virtual std::string getLabel() =0;

        virtual ~IPaTrainDvaMessage() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IPaTrainDvaMessage*> IPaTrainDvaMessages;
    typedef IPaTrainDvaMessages::iterator    IPaTrainDvaMessagesIt;

} //close namespace TA_Base_Core


#endif // !defined(IPaTrainDvaMessage_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
