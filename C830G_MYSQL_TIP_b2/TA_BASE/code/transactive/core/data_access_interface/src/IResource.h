/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/IResource.h $
  * @author:  Julian Chevalley
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/06/07 16:40:53 $
  * Last modified by:  $Author: hui.wang $
  *
  * IResource interface declaration. The IResource interface represents 
  * any Access Controlled resource.
  *
  */


#if !defined(AFX_IRESOURCE_H__FD93CE24_225A_4A69_AC03_915D98770DAB__INCLUDED_)
#define AFX_IRESOURCE_H__FD93CE24_225A_4A69_AC03_915D98770DAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Core
{
	class IResource  
	{
	public:
		virtual ~IResource() 
			{
			};

		/** 
		  * getKey
		  *
		  * get the resource key
		  *
		  * @return unsigned long 
		  *
		  */
		virtual unsigned long getKey() = 0;

		/**
		  * getSubsystem
		  *
		  * @return The subsystem associated with this Resource as an unsigned long.
		  */
		virtual unsigned long getSubsystem() = 0;

	};
};


#endif // !defined(AFX_IRESOUCE_H__FD93CE24_225A_4A69_AC03_915D98770DAB__INCLUDED_)
