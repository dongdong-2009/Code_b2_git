 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/AtsBranchTrack.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * AtsBranchTrack is an implementation of IAtsBranchTrack. It holds the data specific to an AtsBranchTrack entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(AtsBranchTrack_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define AtsBranchTrack_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/ats/src/AtsBranchTrackHelper.h"
#include "core/data_access_interface/ats/src/IAtsBranchTrack.h"

namespace TA_IRS_Core
{
    class AtsBranchTrack : public IAtsBranchTrack
    {

    public:

        /**
         * AtsBranchTrack (constructor)
         *
         * Construct an AtsBranchTrack class based around a key.
         *
         * @param key The key of this AtsBranchTrack in the database
         */
        AtsBranchTrack(const unsigned long idKey);

        /**
         * AtsBranchTrack (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see AtsBranchTrackHelper::AtsBranchTrackHelper(IData*)
         *
         */
        AtsBranchTrack(unsigned long row, TA_Base_Core::IData& data);

        virtual ~AtsBranchTrack();

		virtual std::string getAtsBranchId();
        virtual std::string getTrack();
 
        virtual void invalidate();

		virtual unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of this item. For alarm plan associations this is a name built up from the
         * entity or entity type and the alarm type.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

	public:
		static const std::string TRACK_INNERBOUND;
		static const std::string TRACK_OUTERBOUND;
        static const std::string TRACK_CENTRAL_INNERBOUND;
		static const std::string TRACK_CENTRAL_OUTERBOUND;

    private:

        // Assignment AtsBranchTrack not used so it is made private
		AtsBranchTrack& operator=(const AtsBranchTrack &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the AtsBranchTrackHelper
        // has only been written for ConfigLocation objects and will not copy the AtsBranchTrack helper
        // for a read-only object (ie this one).
        AtsBranchTrack( const AtsBranchTrack& theAtsBranchTrack);  


        TA_IRS_Core::AtsBranchTrackHelper * m_AtsBranchTrackHelper;
    };
} // closes TA_Core

#endif // !defined(AtsBranchTrack_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
