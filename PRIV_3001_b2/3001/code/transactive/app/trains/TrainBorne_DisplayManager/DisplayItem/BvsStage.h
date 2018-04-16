/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/video_display_manager/src/BvsStage.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#if !defined(BvsStage_H)
#define BvsStage_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VideoInput.h"

#include <set>

namespace TA_IRS_App
{

    class BvsStage : public VideoInput
    {
    public:

        // this represents the active video source for a train
        enum TrainVideoSourceType
        {
            Single = 0,
            Quad = 1,
            Sequence = 2,
            None = 3
        };

        /** 
          * BvsStage
          *
          * Constructor
          *
          * @param entityKey    The stage key
          * @param name         The stage name
          * @param description  The stage description
          * @param group        The video input group
          * @param locations    An ordered list of locations - start to end location
          */
        BvsStage( unsigned long entityKey,
                  unsigned long stageId,
                  const std::string& name,
                  const std::string& description,
                  const std::string& group,
                  std::set<unsigned long> locations );


        virtual ~BvsStage() {}


        /**
         * Gets the stage ID for this bvs stage
         */
        unsigned long getStageId();

        /** 
          * getAssociatedLocations
          *
          * Gets the locations associated with this stage.
          *
          * @return a list of location keys.
          */
        virtual std::set<unsigned long> getAssociatedLocations();


        /** 
          * getLocationDescription
          *
          * Get the location description string
          *
          * @return a string eg "DBG to CVC"
          */
        virtual std::string getLocationDescription();

        // TD16902
		/** 
          * getStageNumber
          *
          * Get the stage number
          *
          * @return a string eg "01"
          */
        virtual std::string getStageNumber();
        // TD16902

        /** 
          * getActiveTrain
          *
          * Gets the active train for the stage.
          *
          * @return the train Id, 0 if no active train.
          */
        virtual unsigned short getActiveTrain();


        /** 
          * getActiveTrain
          *
          * Gets the active train for the stage.
          *
          * @return the train Id, 0 if no active train.
          */
        virtual unsigned long getNumActiveMonitors();


        /** 
          * setNumActiveMonitors
          *
          * @param numActive the active train, 0 for none.
          */
        void BvsStage::setNumActiveMonitors(unsigned long numActive);
		
		/** 
          * setActiveTrain
          *
          * Sets the active train for the stage.
          * also sets pre-conflict to false
          *
          * @param newActiveTrain the active train, 0 for none.
          */
        virtual void setActiveTrain(unsigned short newActiveTrain);


        /** 
          * isInPreConflict
          *
          * Is the active train active or in pre-conflict
          *
          * @return true to indicate pre-conflict
          */
        virtual bool isInPreConflict();

                
        /** 
          * getActiveVideoSource
          *
          * gets the active vide source for the train
          *
          * @return none, single, quad, or sequence
          */
        virtual TrainVideoSourceType getActiveVideoSource();


        /** 
          * getAssignment1
          *
          * gets the camera ID / sequence ID or the first assignment to a quad
          *
          * @return cam/seq ID
          */
        virtual unsigned long getAssignment1();


        /** 
          * getAssignment2
          *
          * gets the camera ID / sequence ID or the first assignment to a quad
          *
          * @return cam/seq ID
          */
        virtual unsigned long getAssignment2();


        /** 
          * getAssignment3
          *
          * gets the camera ID / sequence ID or the first assignment to a quad
          *
          * @return cam/seq ID
          */
        virtual unsigned long getAssignment3();


        /** 
          * getAssignment4
          *
          * gets the camera ID / sequence ID or the first assignment to a quad
          *
          * @return cam/seq ID
          */
        virtual unsigned long getAssignment4();


        /** 
          * setPreConflict
          *
          * sets whether the train is in pre-conflict
          *
          * @param isInPreConflict true if the train is in pre-conflict
          */
        virtual void setPreConflict(bool isInPreConflict);


        /** 
          * setActiveVideoSource
          *
          * sets the new active video source on the train
          *
          * @param newSource
          */
        virtual void setActiveVideoSource(TrainVideoSourceType newSource, unsigned long id1, unsigned long id2, unsigned long id3, unsigned long id4);


        /** 
          * getDetails
          *
          * Gets a more detailed description of the item.
          * This is formatted for printing in a text box.
          *
          * @return The detailed description.
          */
        virtual std::string getDetails();



    protected:

        unsigned long m_stageId;
        std::set<unsigned long> m_locations;   // associated stations
        std::string m_locationDescription;      // location description based on first and last location
        unsigned short m_activeTrain;           // the active train (0) if none
		unsigned long m_numActiveMonitors;
        
        bool m_preConflict;                     // whether the active train is in pre-conflict
        TrainVideoSourceType m_activeVideoSource;   // the active video source on the active train
		unsigned long m_assignment1;
		unsigned long m_assignment2;
		unsigned long m_assignment3;
		unsigned long m_assignment4;

    private:

        // These arent needed
        BvsStage();
        BvsStage( const BvsStage& );
        BvsStage& operator=( const BvsStage& );

    };
}

#endif // !defined(BvsStage_H)

