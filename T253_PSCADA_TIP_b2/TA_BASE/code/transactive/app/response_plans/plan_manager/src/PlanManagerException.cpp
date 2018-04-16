//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanManagerException.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "PlanManagerException.h"


namespace TA_Base_App
{

PlanManagerException::PlanManagerException() : TA_Base_Core::TransactiveException()
{
}


PlanManagerException::PlanManagerException(std::string msg) :
    TA_Base_Core::TransactiveException(msg)
{
}


PlanManagerException::PlanManagerException(std::string msg, std::string detail) :
    TA_Base_Core::TransactiveException(msg), m_detail(detail)
{
}


PlanManagerException::PlanManagerException(std::string msg, std::string detail, std::string comment) :
    TA_Base_Core::TransactiveException(msg), m_detail(detail), m_comment(comment)
{
}


PlanManagerException::~PlanManagerException()
{
}


std::string PlanManagerException::getDetail() const
{
    return m_detail;
}


std::string PlanManagerException::getComment() const
{
    return m_comment;
}

}