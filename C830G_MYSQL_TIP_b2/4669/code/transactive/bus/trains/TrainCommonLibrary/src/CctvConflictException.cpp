/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "bus/trains/TrainCommonLibrary/src/CctvConflictException.h"


namespace TA_IRS_Bus
{

    CctvConflictException::CctvConflictException() throw ()
    {
    }

    
    CctvConflictException::~CctvConflictException() throw ()
    {
    }

    
    CctvConflictException::CctvConflictException(const std::string& msg, CommonTypes::TrainIdType otherTrain) throw ()
        : TrainException( msg ),
          m_trainId( otherTrain )
    {
    }

    
    CommonTypes::TrainIdType CctvConflictException::getConflictingTrain() const throw ()
    {
        return m_trainId;
    }
}
