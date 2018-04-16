 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/IndexedAtsDssPlanMaps.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IndexedAtsDssPlanMaps is an indexed collection of AtsDssPlanMaps for easy lookup.
  */


#if !defined(IndexedAtsDssPlanMaps_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define IndexedAtsDssPlanMaps_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <map>

#include "core/data_access_interface/ats/src/IAtsDssPlanMap.h"

namespace TA_IRS_Core
{
    class IndexedAtsDssPlanMaps
    {

    public:

        /**
         * IndexedAtsDssPlanMaps (constructor)
         *
         * Constructs a new instance using a vector of IAtsDssPlanMap
         *
         */
        IndexedAtsDssPlanMaps(IAtsDssPlanMaps dssPlanMaps);

        virtual ~IndexedAtsDssPlanMaps();

		std::string getPlanPath(long dssEvent);

    private:

        // Assignment AtsDssPlanMap not used so it is made private
		IndexedAtsDssPlanMaps& operator=(const IndexedAtsDssPlanMaps &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken.
        IndexedAtsDssPlanMaps( const IndexedAtsDssPlanMaps& theIndexedAtsDssPlanMaps);  


		typedef std::map<unsigned long, IAtsDssPlanMap* > IndexedDssPlanMaps;
		typedef IndexedDssPlanMaps::iterator IndexedDssPlanMapsIt;
		
		IndexedDssPlanMaps		m_atsDssPlanMaps;
    };
} // closes TA_Core

#endif // !defined(AtsDssPlanMap_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
