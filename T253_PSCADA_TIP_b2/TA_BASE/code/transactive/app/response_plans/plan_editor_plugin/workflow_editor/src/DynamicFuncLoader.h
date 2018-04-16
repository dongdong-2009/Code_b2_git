#pragma once


#include <string>
#include <vector>

class IActivityComponent;

class AFX_EXT_CLASS DynamicFuncLoader
{
public:
    static void* loadFuncFromDynamicLibrary( const std::string& strLibraryPath, const std::string& strFuncName );
    static std::vector<IActivityComponent*> getActivityComponentsFormLib( const std::vector<std::string>& vecLibraryPath );
};

