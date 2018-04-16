/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/IAtsBranchTrack.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * IAtsBranchTrack is an interface to an AtsBranchTrack object. It allows the AtsBranchTrack object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IAtsBranchTrack_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IAtsBranchTrack_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IAtsBranchTrack : public virtual TA_Base_Core::IItem
    {

    public:

        virtual std::string getAtsBranchId() =0;
        virtual std::string getTrack() =0;
        
        virtual ~IAtsBranchTrack() {};

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };


    typedef std::vector<IAtsBranchTrack*> IAtsBranchTracks;
    typedef IAtsBranchTracks::iterator    IAtsBranchTracksIt;
} //close namespace TA_IRS_Core

#endif // !defined(IAtsBranchTrack_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
