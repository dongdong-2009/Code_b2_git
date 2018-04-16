/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/corba/src/ServantActivator.h $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * This class is used to nicely activate & de-activate
  * CORBA servants (interface implementations)
  */

#ifndef SERVANT_ACTIVATOR_H
#define SERVANT_ACTIVATOR_H

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "omniORB4/CORBA.h"

namespace TA_Base_Core
{
	class ServantActivator : public POA_PortableServer::ServantActivator,
									  public PortableServer::RefCountServantBase
	{
	public:	
		ServantActivator();

		virtual ~ServantActivator();


		PortableServer::Servant incarnate(const PortableServer::ObjectId& oid,
								PortableServer::POA_ptr adapter);
  
		void etherealize(const PortableServer::ObjectId& oid,
								PortableServer::POA_ptr adapter,
								PortableServer::Servant serv,
								CORBA::Boolean cleanup_in_progress, 
								CORBA::Boolean remaining_activations);
	};


}; // namespace TA_Base_Core

#endif // EVENT_LISTENER_H
