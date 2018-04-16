#pragma once

#include "afxwin.h"
#include <string>
namespace TA_IRS_App
{
    class ILoadStationDiagramService
    {
        public:
            ILoadStationDiagramService ( void )
			{
			}
            /*
             according to the station information to load the station diagram
            */
            virtual int loadStationDiagram ( const std::string& strFileFullName, const CRect& rtPosition, HWND hParent ) = 0;
            virtual void closeSchematic() = 0;
            virtual void stopSchematic() = 0;
            virtual void refreshSchematic() = 0;

        public:
            virtual ~ILoadStationDiagramService ( void )
			{
			}
    };
}
