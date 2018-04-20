/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/RadioEntityAccess/src/RadioEntityAccess.h $
 * @author:  Robert van Hugten
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * A utility class that contains static functions for accessing commonly used
 * RadioEntity access.
 */

#if !defined(A3004D8E_ECB6_4c5e_A11A_F421820AE632__INCLUDED_)
#define A3004D8E_ECB6_4c5e_A11A_F421820AE632__INCLUDED_

#include <string>

#include "core\data_access_interface\entity_access\src\IRadioEntityData.h"
#include "core\data_access_interface\entity_access\src\IRadioSessionEntityData.h"

class RadioEntityAccess
{

public:
	/**
	 * uses the sessionid runparam and ConsoleAccessFactory to retrieve the current
	 * mft's console key.
	 */
	static unsigned long getConsoleKey();

	/**
	 * uses the sessionid runparam and ConsoleAccessFactory to retrieve the current
	 * mft's location key.
	 */
	static unsigned long getLocationKey();
	static unsigned long getLocationKey(unsigned long consoleKey);
	
	/**
	 * uses the EntityAccessFactory to find the Radio entity name for this location
	 */
	static std::string getRadioEntityName(unsigned long locationKey);

	/**
	 * uses the EntityAccessFactory to find the Radio entity key for this location
	 */
	static unsigned long getRadioEntityKey(unsigned long locationKey);

	/**
	 * uses the EntityAccessFactory and console key to search for this location
	 * console's RadioSession entity, return its name.
	 */
	static std::string getRadioSessionEntityName(unsigned long locationKey, unsigned long consoleKey);

	/**
	 * uses the EntityAccessFactory and console key to search for this location
	 * console's RadioSession entity, return its name.
	 */
	static unsigned long getRadioSessionEntityKey(unsigned long locationKey, unsigned long consoleKey);


	static TA_Base_Core::IEntityData*  getRadioEntity(unsigned long locationKey);

	static TA_Base_Core::IRadioSessionEntityData* getRadioSessionEntity(unsigned long locationKey, unsigned long consoleKey);

private:
	/**
	 * Private constructor to prevent instantiation of this class.
	 */
	RadioEntityAccess();
	/**
	 * Private destructor to match the invisibility of the constructor.
	 */
	~RadioEntityAccess();

};
#endif // !defined({A3004D8E-ECB6-4c5e-A11A-F421820AE632}__INCLUDED_)
