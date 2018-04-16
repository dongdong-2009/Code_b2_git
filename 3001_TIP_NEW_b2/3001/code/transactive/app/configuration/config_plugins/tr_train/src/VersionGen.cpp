/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tr_train/src/VersionGen.cpp $
  * @author:  : Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/01/15 8:15:14 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * DO NOT MANUALLY EDIT THIS FILE.  THIS FILE IS
  * CREATED AUTOMATICALLY BY THE BUILD SYSTEM
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
