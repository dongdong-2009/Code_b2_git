#include <afxext.h>
#include <afxdll_.h>

#include "app/response_plans/plan_step_plugin/common_editor/src/IEditorStepComponent.h"
#include "DynamicFuncLoader.h"


void* DynamicFuncLoader::loadFuncFromDynamicLibrary( const std::string& strLibraryPath, const std::string& strFuncName )
{
    HINSTANCE hDll = AfxLoadLibrary( strLibraryPath.c_str( ) );

    void* pFunc = NULL;

    if ( NULL != hDll )
    {
        pFunc = ::GetProcAddress( hDll, strFuncName.c_str( ) );
    }

    return pFunc;
}

std::vector<IActivityComponent*> DynamicFuncLoader::getActivityComponentsFormLib( const std::vector<std::string>& vecLibraryPath )
{
    std::vector<IActivityComponent*> vecStepComponents;

    for ( std::vector<std::string>::const_iterator itLoop = vecLibraryPath.begin(); vecLibraryPath.end() != itLoop; ++itLoop )
    {
        void* pFunc = DynamicFuncLoader::loadFuncFromDynamicLibrary( *itLoop, "createEditorStepComponent" );

        if ( NULL == pFunc )
        {
            continue;;
        }

        TA_Base_App::GetMftStepComponentFuncPtr pGetComponent = reinterpret_cast<TA_Base_App::GetMftStepComponentFuncPtr>(pFunc);

        if ( NULL == pGetComponent )
        {
            continue;
        }

        IActivityComponent& refComponent = pGetComponent().getActivityComponent();

        vecStepComponents.push_back( &refComponent );
    }

    return vecStepComponents;
}