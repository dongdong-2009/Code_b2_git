/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/ResourceAccessFactory.h $
  * @author:  Julian Chevalley
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/06/07 16:40:53 $
  * Last modified by:  $Author: hui.wang $
  *
  * ResourceAccessFactory is a singleton used to create Resource objects
  *
  */


#if !defined(AFX_RESOURCEACCESSFACTORY_H__4E024382_B6B5_45A6_96B4_84E8CB2EB446__INCLUDED_)
#define AFX_RESOURCEACCESSFACTORY_H__4E024382_B6B5_45A6_96B4_84E8CB2EB446__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class IResource;

	class ResourceAccessFactory
	{

	public:
        virtual ~ResourceAccessFactory() 
		{

		};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ResourceAccessFactory object.
         */
		static ResourceAccessFactory& getInstance();


        /** 
          * getResourceFromEntity
          *
          * Get a Resource based on an entity key
          *
          * @return IResource* the resource. the caller is responsible for deleting the object
          *
          * @param unsigned long EntityKey
		  *
          */
		IResource* getResourceFromEntity( unsigned long EntityKey );

    private:
        //
        // These are private as this method is a singleton
        //
		ResourceAccessFactory();
		ResourceAccessFactory( const ResourceAccessFactory& theResourceAccessFactory);
		ResourceAccessFactory& operator=(const ResourceAccessFactory &) 
		{ 
		};

		// the one and only one instance
		static ResourceAccessFactory* m_instance;
		// protect singleton creation
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

	};

} // closes TA_Base_Core

#endif // !defined(AFX_RESOURCEACCESSFACTORY_H__4E024382_B6B5_45A6_96B4_84E8CB2EB446__INCLUDED_)
