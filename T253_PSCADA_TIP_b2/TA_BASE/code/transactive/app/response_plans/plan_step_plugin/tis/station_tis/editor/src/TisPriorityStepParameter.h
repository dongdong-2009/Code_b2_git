/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TisPriorityStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents a message priority parameter in a Station/Train TIS step.
  *
  **/

#if !defined(AFX_TISPRIORITYSTEPPARAMETER_H__53AE78C2_7710_47CE_A426_E761EB604C5A__INCLUDED_)
#define AFX_TISPRIORITYSTEPPARAMETER_H__53AE78C2_7710_47CE_A426_E761EB604C5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <set>
#include "core/data_access_interface/tis_agent/IDL/src/TISMessageDataDefinitions.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/NumericStepParameter.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TisPriorityStepParameter

    class TisPriorityStepParameter : public NumericStepParameter
    {
    // Operations
    public:
        TisPriorityStepParameter(PlanStep& parent, std::string name, unsigned long priority, bool isHidden = false);
        virtual ~TisPriorityStepParameter();

		virtual std::vector<std::string> getStepParameterValues();
		virtual const std::string displayValue() const;

        unsigned short getPriority() const  { return static_cast<unsigned short>(value()); }

        std::vector<unsigned short> getTisPriorities(TA_Base_Core::ELibrarySection librarySection) const;
        std::string getPriorityLabel(unsigned short priority) const;

	private:
		std::string convertToString(const TA_Base_Core::ELibrarySection librarySection) const;
    private:
        TisPriorityStepParameter& operator=(const TisPriorityStepParameter& parameter);

    // Types
    private:
        struct PriorityRange
        {
            PriorityRange(unsigned short min_priority, unsigned short max_priority,
                          const std::set<unsigned short>& reserved_priorities = std::set<unsigned short>()) :
                minPriority(min_priority),
                maxPriority(max_priority),
                reserved(reserved_priorities)
            {
            }

            PriorityRange(const PriorityRange& priorityRange) :
                minPriority(priorityRange.minPriority),
                maxPriority(priorityRange.maxPriority),
                reserved(priorityRange.reserved)
            {
            }

            PriorityRange& operator=(const PriorityRange& priorityRange)
            {
                if (this != &priorityRange)
                {
                    minPriority = priorityRange.minPriority;
                    maxPriority = priorityRange.maxPriority;
                    reserved = priorityRange.reserved;
                }

                return *this;
            }

            unsigned short minPriority;
            unsigned short maxPriority;
            std::set<unsigned short> reserved;
        };

        typedef std::map<TA_Base_Core::ELibrarySection, PriorityRange> PriorityMap;

    // Attributes
    private:
        PriorityMap m_priorityMap;
    };
}

#endif // !defined(AFX_TISPRIORITYSTEPPARAMETER_H__53AE78C2_7710_47CE_A426_E761EB604C5A__INCLUDED_)