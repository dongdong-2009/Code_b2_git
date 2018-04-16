#pragma once
#include <vector>
#include "afxwin.h"
#include "afxcmn.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
//#include "StationMode.h"
//#include "core/data_access_interface/src/ILocation.h"

namespace TA_IRS_App
{
	class VideoInput;
	class Sequence;
	class Monitor;
	class MonitorDisplayItem;
    class MonitorWnd;
	class IGFVideoSwitchDisplayView : public TA_Base_Bus::TransActiveDialog
    {
        public :
			IGFVideoSwitchDisplayView(TA_Base_Bus::IGUIAccess& genericGUICallback,
		                              const UINT dialogId,
		                              CWnd* pParent = NULL)
									  : TA_Base_Bus::TransActiveDialog(genericGUICallback,
																	   dialogId,
																	   pParent)
			{
			}
            virtual ~IGFVideoSwitchDisplayView () {}
        public:

            //virtual  void DisplayStations( std::vector<TA_Base_Core::ILocation*>& stations, unsigned long selectId ) =0;
            virtual void DisplaySequences ( std::vector<VideoInput*> & sequences ) = 0;
            virtual void DisplaySequence ( Sequence* sequence ) = 0;
            virtual void GetSchematicCRET ( CRect& markerRect ) = 0;
            virtual HWND GetWndHandle() = 0;
            virtual unsigned long GetSelectedSequenceId () = 0;
            virtual unsigned long GetSelectedStation () = 0;
            virtual unsigned long GetStationIdForGwax() = 0;
            virtual void EnableEditSeqButton ( bool enabled ) = 0;
            virtual MonitorDisplayItem* getMonitorDisplayitem ( int index , Monitor* monitor  ) = 0;
            virtual void beginDragImage () = 0;
            virtual void paintStandControl ( MonitorWnd * item ) = 0;
            virtual void paintHightedControl ( MonitorWnd * item ) = 0;
            virtual void assignMonitor ( MonitorDisplayItem * item  ) = 0;
            virtual void changeWallMontior( MonitorDisplayItem * item, bool split) = 0;
            virtual void endDrage ( ) = 0;
            virtual VideoInput* getSelectedVideoSource( ) = 0;
            virtual void setDragIcon () = 0;

            virtual void handleDragImageOnMouseMove ( BOOL begin , CPoint& point ) = 0;

            virtual void loadSequenceSubMenu ( CPoint& point , Monitor * monitor, Sequence * seq, bool bIsLocalMonitor ) = 0;
            virtual void loadCameraSubMenu ( CPoint& point , Monitor * monitor , bool bIsLocalMonitor ) = 0;

            virtual void displayErrorMessage ( CString& message ) = 0;

            virtual void enablePTZButton ( BOOL enabled ) = 0;

            virtual void displayStaionCameraDiagram ( std::string& fullFileName ) = 0;

            virtual void setDragImage ( CImageList* dragImage ) = 0;

            virtual void setDragAdjust ( CPoint& dragAdjust ) = 0;

            virtual void enableStationSelection ( BOOL enable ) = 0;

            virtual void setSelectedStation ( unsigned int index ) = 0;

            virtual void UpdateSequenceTreeCtrl ( Sequence* sequence )  = 0;

            //virtual void refreshIndexLable ( int index, bool isQuad) = 0;
    };
}