//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanManagerException.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_PLANMANAGEREXCEPTION_H__D371ADD8_5CFF_4C41_AACE_C2461024E2ED__INCLUDED_)
#define AFX_PLANMANAGEREXCEPTION_H__D371ADD8_5CFF_4C41_AACE_C2461024E2ED__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanManagerException

    class PlanManagerException : public TA_Base_Core::TransactiveException
    {
    // Operations
    public:
        PlanManagerException();
        PlanManagerException(std::string msg);
        PlanManagerException(std::string msg, std::string detail);
        PlanManagerException(std::string msg, std::string detail, std::string comment);
        virtual ~PlanManagerException();

        virtual std::string getDetail() const;
        virtual std::string getComment() const;

    // Attributes:
    private:
        std::string m_detail;  // Additional detail to accompany the message
        std::string m_comment; // Remarks that may be helpful in correcting the problem
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANMANAGEREXCEPTION_H__D371ADD8_5CFF_4C41_AACE_C2461024E2ED__INCLUDED_)
