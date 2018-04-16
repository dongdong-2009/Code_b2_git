/**
* This class encapulates the data associated with a ats method.
* @author Lucky Cuizon
* @version 1.0
* @generator Enterprise Architect
* @created 18-January-2011 08:32:15
*/

#ifndef ATS_METHOD_OA1_TABLE_H	
#define ATS_METHOD_OA1_TABLE_H

#include "app/ats/AtsAgent/AtsMethod/src/AtsITcpMethod.h"

namespace TA_IRS_App
{
	class AtsMethodOA1Table : public AtsITcpMethod
	{
	public:

	   /**
	    * Construtor
		* @param: trainID, criticalAlarmEvents
		*/
		AtsMethodOA1Table(DataGramVector criticalAlarmEvents);
	   
	   /**
		* Destructor
		*/
		virtual ~AtsMethodOA1Table();
	private:

		/**
		 * Disable the assignment and copy constructors
		 */
	     AtsMethodOA1Table& operator=(const AtsMethodOA1Table &);
		 AtsMethodOA1Table( const AtsMethodOA1Table& itcpMethod);
	};
};

#endif // end of ATS_METHOD_OA1_TABLE_H