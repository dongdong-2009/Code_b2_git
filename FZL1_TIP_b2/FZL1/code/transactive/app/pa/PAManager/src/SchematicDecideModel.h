/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifndef SCHEMATICDECIDEMODEL_H__INCLUDED_
#define SCHEMATICDECIDEMODEL_H__INCLUDED_

#include <string>

class SchematicDecideModel
{
public:
    SchematicDecideModel();
    virtual ~SchematicDecideModel();
    static void setSchematicLocationKey( unsigned long ulLocationKey );
    static void setSubsystemKey( unsigned long ulSubsystem );

    std::string getSchematicFileName();

private:
    static unsigned long s_ulSchematicLocationKey;
    static unsigned long s_ulSubsystemKey;
};

#endif  // ifndef SCHEMATICDECIDEMODEL_H__INCLUDED_
