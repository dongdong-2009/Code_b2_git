#if !defined(AFX_DUTYMANAGERCONSTANTS_H__FD918197_39F4_4690_BBFD_D486FF8ED7B4__INCLUDED_)
#define AFX_DUTYMANAGERCONSTANTS_H__FD918197_39F4_4690_BBFD_D486FF8ED7B4__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/security/DutyManager/src/DutyManagerConstants.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/01/23 17:57:05 $
  * Last modified by:  $Author: builder $
  * 
  * Constants shared between classes.
  *
  */

// Runtime parameters
const char* const RPARAM_USELOCATIONLABEL = "RegionIsLocation";
const char* const RPARAM_NORMALDUTYCOLOUR = "NormalDutyColour";
const char* const RPARAM_DELEGATEDDUTYCOLOUR = "DelegatedDutyColour";
const char* const RPARAM_DEGRADEDDUTYCOLOUR = "DegradedDutyColour";
const char* const RPARAM_UNALLOCATEDSUBSYSTEMCOLOUR = "UnallocatedSubsystemColour";

const char* const RPARAM_PROJECTNAME = "ProjectName";

// MFC 
const int LV_ERR(-1);  // List view invalid item error (c.f. CB_ERR)

#endif // !defined(AFX_DUTYMANAGERCONSTANTS_H__FD918197_39F4_4690_BBFD_D486FF8ED7B4__INCLUDED_)