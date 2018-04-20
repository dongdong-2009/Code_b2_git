#if !defined(_REGIONDUTYMATRIXELEMENT_H__D8D397A0_F86F_4636_AD79_EF7D5EAEFB5B__INCLUDED_)
#define _REGIONDUTYMATRIXELEMENT_H__D8D397A0_F86F_4636_AD79_EF7D5EAEFB5B__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_monitor_framework/src/RegionDutyMatrixElement.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is a representation of a single element of the region duty matrix.
  * It is associated with a single session and a single region.
  * It contains a list of duties allocated to the session for the region. 
  * The list may be empty.
  *
  */

namespace TA_Base_Bus
{
    class RegionDutyMatrixElement
    {
    public:
        /**
          * Standard constructor
          *
          */
        RegionDutyMatrixElement();

        /**
          * Copy constructor
          *
          */
        RegionDutyMatrixElement(const RegionDutyMatrixElement& regionDutyMatrixElement);

        /**
          * Destructor
          *
          */
        virtual ~RegionDutyMatrixElement();

        /**
          * Assignment operator
          *
          */
        RegionDutyMatrixElement& operator=(const RegionDutyMatrixElement& regionDutyMatrixElement);

        /**
          * Returns true if the list of active region duties for the session is not empty.
          *
          */
        bool dutiesExist() const;

        /**
          * Returns the list of all active region duties for the session in the region.
          *
          */
        RegionDutyList getRegionDuties() const;

        /**
          * Adds the specified region duty to the list, provided it is not already in there.
          * Returns true to indicate that the duty has been added, and false otherwise.
          * 
          */
        bool addDuty(const DutyAgentTypeCorbaDef::RegionDuty& regionDuty);

        /**
          * Removes the specified region duty from the list, provided it is found.
          * Returns true to indicate that the duty has been removed, and false otherwise.
          * 
          */
        bool removeDuty(const DutyAgentTypeCorbaDef::RegionDuty& regionDuty);


    private:
        /**
          * Compares two region duties to determine their sort order in the RegionDutySet.
          *
          */
        struct RegionDutyComparator
        {
            bool operator()(const DutyAgentTypeCorbaDef::RegionDuty& duty1, 
                            const DutyAgentTypeCorbaDef::RegionDuty& duty2) const
            {
                return duty1.profileKey < duty2.profileKey;
            }
        };

        //typedef std::set<DutyAgentTypeCorbaDef::RegionDuty, RegionDutyComparator> RegionDutySet;
		typedef std::vector< DutyAgentTypeCorbaDef::RegionDuty > RegionDutySet;


    private:
        RegionDutySet m_regionDuties;
    };

}

#endif // !defined(_REGIONDUTYMATRIXELEMENT_H__D8D397A0_F86F_4636_AD79_EF7D5EAEFB5B__INCLUDED_)
