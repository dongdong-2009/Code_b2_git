#if !defined(_DEFAULTREGIONASSIGNMENTMATRIX_H__8CAEAE1B_A05D_453F_9000_E7C46D3F2CA0__INCLUDED_)
#define _DEFAULTREGIONASSIGNMENTMATRIX_H__8CAEAE1B_A05D_453F_9000_E7C46D3F2CA0__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_monitor_framework/src/DefaultRegionAssignmentMatrix.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class stores the static operator-region duty assignments 
  * as configured in the database.
  * The assignments are stored in the form of a two-dimensional matrix,
  * where rows represent operators and columns represent regions.
  * Individual elements of the matrix are boolean values indicating
  * whether the corresponding region is assigned to the corresponding
  * operator.
  *
  */

namespace TA_Base_Bus
{
    class DefaultRegionAssignmentMatrix : public TA_Base_Core::Matrix<OperatorKey, RegionKey, bool>
    {
    public:
        DefaultRegionAssignmentMatrix();
        virtual ~DefaultRegionAssignmentMatrix();

        /**
          * Reads from the database the static region-operator assignments and
          * fills the contents of the matrix.
          * Throws a DutyMonitorFrameworkException if region/operator data cannot 
          * be retrieved or an error occurs while setting elements of the matrix.
          *
          */
        void populate();

    private:
        DefaultRegionAssignmentMatrix(const DefaultRegionAssignmentMatrix& DefaultRegionAssignmentMatrix);
        DefaultRegionAssignmentMatrix& operator=(const DefaultRegionAssignmentMatrix& DefaultRegionAssignmentMatrix);


    private:
        bool m_assignmentsLoaded;
    };
}

#endif // !defined(_DEFAULTREGIONASSIGNMENTMATRIX_H__8CAEAE1B_A05D_453F_9000_E7C46D3F2CA0__INCLUDED_)
