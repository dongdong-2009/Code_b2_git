/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/EntityActionFactory.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
///////////////////////////////////////////////////////////
//  EntityActionFactory.h
//  Implementation of the Class EntityActionFactory
//  Created on:      03-Feb-2005 12:34:13 PM
///////////////////////////////////////////////////////////

#if !defined(EntityActionFactory_DCD919BB_BA29_4087_8C27_F4E6FD65D57E__INCLUDED_)
#define EntityActionFactory_DCD919BB_BA29_4087_8C27_F4E6FD65D57E__INCLUDED_

#include <string>

namespace TA_Base_Bus
{
    class IEntityAction;

    /**
     * Creates concrete IEntityAction classes
     */
    class EntityActionFactory
    {

    public:
	    EntityActionFactory();
	    virtual ~EntityActionFactory();

    public:
	    /**
	     * Creates a new IEntityAction object
	     * @param dllName
	     * 
	     */
	    IEntityAction* createEntityAction(const std::string& dllName);

    private:
        // Disable copy constructor and assignment operator
	    EntityActionFactory(const EntityActionFactory& EntityActionFactory);
        EntityActionFactory& operator=(const EntityActionFactory &);
    };

} // namespace
#endif // !defined(EntityActionFactory_DCD919BB_BA29_4087_8C27_F4E6FD65D57E__INCLUDED_)