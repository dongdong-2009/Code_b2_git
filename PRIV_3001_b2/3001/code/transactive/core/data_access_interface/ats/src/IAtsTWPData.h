/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/IAtsTWPData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * IAtsTWPData is an interface to an AtsTWPData object.
  *
  */

#if !defined(AFX_IATSTWPDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
#define AFX_IATSTWPDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
	class IAtsTWPData : public virtual TA_Base_Core::IItem
	{	

	public:
		
		virtual ~IAtsTWPData() {};

		/**
		 *	getTWPDataPointEntityKey()
		 *
		 *  Returns the ISCS data point entity key which corresponds to given ATS TWP
		 *
		 *  @return		entity key of the ISCS data point
		 *
		 */
		virtual unsigned long getTWPDataPointEntityKey() = 0;

		/**
		 *	getTWPDataPointType()
		 *
		 *  Returns the type of the TWP datapoint. Either "Input" or "Output"
		 *
		 *  @return		data point type as string
		 *
		 */
		virtual std::string getTWPDataPointType() = 0;

		/**
		 *	getTWPCommand()
		 *
		 *  Returns the TWP command corresponding to the data point entity
		 *
		 *  @return		TWP command as string
		 *
		 */
		virtual std::string getTWPCommand() = 0;
				
		/**
		 *	getKey()
		 *
		 *  Returns the primary key of the record holding this TWP Data
		 *
		 *  @return		entity key of the record
		 *
		 */
		virtual unsigned long getKey() = 0;
	};

	typedef std::vector<IAtsTWPData*> IAtsTWPDataVector;
    typedef IAtsTWPDataVector::iterator    IAtsTWPDataVectorIt;

} // TA_Core

#endif // !defined(AFX_IATSTWPDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
