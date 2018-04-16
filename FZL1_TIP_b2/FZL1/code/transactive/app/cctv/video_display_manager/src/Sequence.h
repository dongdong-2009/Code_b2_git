/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/Sequence.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#if !defined(Sequence_H)
#define Sequence_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/entity_access/src/Sequence.h"
#include "VideoInput.h"

#include <list>

namespace TA_IRS_App
{
    class Camera;

    class Sequence : public VideoInput
    {
    public:

        Sequence( unsigned long entityKey,
                  const std::string& name,
				  const std::string& addr,
                  const std::string& description,//it's not sequence description
                  const std::string& group, unsigned long locationid );


        virtual ~Sequence() {}

        /**
          * getDwellTime
          *
          * Gets the dwell time (1-99). 0 for empty sequences.
          *
          * @return the dwell time
          */
        virtual unsigned short getDwellTime();

        /**
          * setDwellTime
          *
          * Sets the dwell time (1-99). 0 for empty sequences.
          *
          * @param dwellTime the new dwell time
          */
        virtual void setDwellTime(unsigned short dwellTime);
		
		/**
          * getAddress
          *
          * return the address.
          *
          * @param none
          */
		virtual std::string getAddress();

		/**
          * setAddress
          *
          * Sets the address for this sequences.
          *
          * @param addr the new address
          */
		virtual void setAddress(std::string addr);

        /**
          * getReadOnly
          *
          * Get whether the sequence is read only
          *
          * @return whether its read only
          */
        virtual bool getReadOnly();

        /**
          * setReadOnly
          *
          * Sets whether the sequence is read only
          *
          * @param readOnly true if the sequence cannot be written
          */
        virtual void setReadOnly(bool readOnly);

        // TD16691
        /**
          * getSequenceDescription
          *
          * Get the sequence description
          *
          * @return sequence description
          */
        virtual std::string getSequenceDescription();

        /**
          * setSequenceDescription
          *
          * Sets the sequence description
          *
          * @param sequence description
          */
        virtual void setSequenceDescription(std::string sequenceDescription);
        // TD16691

        /**
          * getCameras
          *
          * Gets the cameras associated with this sequence.
          *
          * @return the list of cameras. Order is important.
          */
        virtual std::list<Camera*> getCameras();

        /**
          * setCameras
          *
          * Sets the cameras associated with this sequence.
          *
          * @param cameras  the list of cameras. Order is important.
          */
        virtual void setCameras(std::list<Camera*> cameras);

        /**
          * getDetails
          *
          * Gets a more detailed description of the item.
          * This is formatted for printing in a text box.
          *
          * @return The detailed description.
          */
        virtual std::string getDetails();

		/**
          * setSequenceDescription
          *
          * Sets the current description of this Sequence.
          *
          * Precondition: !isReadOnly()
          *
          * @param      string newDescription
          *             The description to be set for this Sequence.
          * @param      string sessionId
          *             Used to determine if the user has sufficient
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
          *             2. A VideoInput key is not recognised.
          *             3. The agent is in Monitor mode.
          *             4. The user does not have sufficient privileges for this operation.
          *             5. There is a communications failure with the hardware.
          */

        bool saveSequenceDescription(const char* newDescription);

		


    protected:

        unsigned short m_dwellTime;
        std::list<Camera*> m_cameras;
        bool m_readOnly;
        // TD16691
        std::string m_sequenceDescription;
        // TD16691

    private:

        // These arent needed
        Sequence();
        Sequence( const Sequence& );
        Sequence& operator=( const Sequence& );
		std::string m_address;
    };
}

#endif // !defined(Sequence_H)

