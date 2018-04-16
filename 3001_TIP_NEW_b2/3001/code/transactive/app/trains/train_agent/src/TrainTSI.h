/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/trains/train_agent/src/TrainTSI.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

///////////////////////////////////////////////////////////
//  TrainAgent.h
//  Implementation of the Class TrainAgent
//  Created on:      26-Jul-2005 01:11:39 PM
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// Class: TrainTSI
// A simple class that holds both train TSIs and which of
// the two is active.  Each train can communicate using one
//  of its two radios; primary and secondary.  A radio is
// identified by a unique TSI number.  Only one radio is
// active at any time and generally the primary radio is
// active.  Train comms is only performed on the active
// radio
//
// The class instance starts in an unknown state until
// both TSIs are set and the active TSI is selected.  Until
// then any requests on the instance for an active TSI will
// cause an exception.

#if !defined(TrainTSI_INCLUDED_)
#define TrainTSI_INCLUDED_

#include <string>
#include <vector>
#include "TrainTypes.h"

namespace TA_IRS_App
{
    class TrainTSI
    {
    public:

        static const std::string UNKNOWN_TSI;

        TrainTSI();
        TrainTSI(const std::string& primary, const std::string& secondary,
			TrainTypes::SelectTSI selectTSI);
        TrainTSI(const TrainTSI& trainTSI);
        TrainTSI operator=(const TrainTSI& trainTSI);
        void copy(const TrainTSI& trainTSI);

        void setTSI(TrainTypes::SelectTSI selectTSI, const std::string& TSI);
        void setActiveTSI(TrainTypes::SelectTSI selectTSI);

        std::string getActiveTSI() const;

		TrainTypes::SelectTSI getActiveSelectTSI() const;
        std::string getTSI(TrainTypes::SelectTSI selectTSI) const;
		void getTSI(TrainTSIRec& trainTSI) const;

    private:

		TrainTSIRec m_trainTSI;

        void validateSelectTSI(TrainTypes::SelectTSI selectTSI) const;

    };

};

#endif
