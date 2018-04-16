//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanManagerException.cpp $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/02/06 16:15:14 $
// Last modified by:  $Author: haijun.li $
//
// <description>

#include "PlanManagerException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


PlanManagerException::PlanManagerException() throw() : TA_Base_Core::TransactiveException()
{
}


PlanManagerException::PlanManagerException(std::string msg) throw() :
    TA_Base_Core::TransactiveException(msg)
{
}


PlanManagerException::PlanManagerException(std::string msg, std::string detail) throw() :
    TA_Base_Core::TransactiveException(msg), m_detail(detail)
{
}


PlanManagerException::PlanManagerException(std::string msg, std::string detail, std::string comment) throw() :
    TA_Base_Core::TransactiveException(msg), m_detail(detail), m_comment(comment)
{
}


PlanManagerException::~PlanManagerException() throw()
{
}


std::string PlanManagerException::getDetail() const throw()
{
    return m_detail;
}


std::string PlanManagerException::getComment() const throw()
{
    return m_comment;
}
