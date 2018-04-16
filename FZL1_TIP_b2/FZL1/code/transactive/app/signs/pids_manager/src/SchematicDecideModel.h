#pragma once
#include <string>
#include <map>
class SchematicDecideModel
{
public:
    SchematicDecideModel();
    virtual ~SchematicDecideModel();
    static void setSchematicLocationKey( unsigned long ulLocationKey );

    std::string getSchematicFileName();
    static void setSubsystemKey( unsigned long ulSubsystem );

private:
    static unsigned long s_ulSchematicLocationKey;
    static unsigned long s_ulSubsystemKey;
    // No use yet
    std::map<std::string, std::string> m_mapStationToSchematicFile;
};
