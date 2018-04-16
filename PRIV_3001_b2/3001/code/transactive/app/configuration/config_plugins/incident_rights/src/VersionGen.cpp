/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/incident_rights/src/VersionGen.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
 
namespace TA_Base_Core
{
    char componentName[] = "#componentName";
	char buildVersion[] = "#buildVersion";
	char componentVersion[] = "#buildVersion";
	char buildDate[] = __DATE__" "__TIME__;
	char identString[] = "";
    int headerArraySize = 1;
    char * headerArray[] =
    {
        "#headerArray"
    };
    
    int libraryArraySize = 1;
    char * libraryArray[] =
    {
        " "
    };
    int otherArraySize = 1;
    char * otherArray[] =
    {
        " "
    };
}
