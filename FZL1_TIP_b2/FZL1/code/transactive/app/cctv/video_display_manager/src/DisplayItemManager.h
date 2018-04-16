/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This singleton is required to keep a reference to all display
  * items. It maps each type of display item by name and provides access to
  * the items that are shared throughout the GUI - their state is updated
  * via comms messages and the display refreshed to reflect this.
  */


#if !defined(DisplayItemManager_H)
#define DisplayItemManager_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// warnings for name truncated to 255 characters
#pragma warning ( disable : 4503 )

#include "app/cctv/video_display_manager/src/SDKCommunicator.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/types/VisualComms_MessageTypes.h"
#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/threads/src/Thread.h"
#include "core/threads/src/IThreadPoolWorkerCallback.h"
#include "app/cctv/video_display_manager/src/IVideoDisplayManagerDAI.h"
#include "app/cctv/video_display_manager/src/CCTVCommon.h"
#include "app/cctv/video_display_manager/src/ThreadedMemberFunc.h"
#include <string>
#include <map>
#include <vector>
#include <boost/noncopyable.hpp>
#include <ace/Singleton.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <boost/static_assert.hpp>

BOOST_STATIC_ASSERT(sizeof(UINT64) >= 2*sizeof(unsigned long));

const static int RL_MONITOR_SEQUENCE_NORMAL    = 12;
const static int RL_MONITOR_CAMERA_NORMAL      = 13;
const static int RL_MONITOR_SEQUENCE_HIGHLIGHT = 120;
const static int RL_MONITOR_CAMERA_HIGHLIGHT   = 130;
class CWnd;

namespace TA_IRS_App
{
    //    class BvsStage;

    class Camera;
    class Sequence;
    class Monitor;
    class DisplayItem;
    class VideoInput;
    class VideoOutput;

    class DisplayItemManager : public TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >,
        public TA_Base_Core::IOnlineUpdatable,
		public TA_Base_Core::IThreadPoolWorkerCallback,
        boost::noncopyable
    {
        private:
            friend class ACE_Singleton<DisplayItemManager, ACE_Recursive_Thread_Mutex>;
            virtual ~DisplayItemManager();
            DisplayItemManager();
        public:
            static DisplayItemManager& getInstance();
			static void removeInstance();
            Camera* getCamera(std::string name);
            Camera* getCamera(unsigned long pkey);
            Camera* getCameraByAddress(const std::string& address);  // and also by Address, eg "1:26" (TD 13455)
            Monitor* getMonitor(std::string name);
            Monitor* getMonitor(unsigned long pkey);
            Sequence* getSequence(std::string name);
            Sequence* getSequence(unsigned long pkey);
            void updateSequence(Sequence * sequence);
            void updateSequenceDesc(Sequence * sequence, std::string description);
            // get any matching input type
            VideoInput* getInput(std::string name);
            VideoInput* getInput(unsigned long pkey);
            // get any matching output type
            VideoOutput* getOutput(std::string name);
            VideoOutput* getOutput(unsigned long pkey);
            // used to retrieve all items - so that property pages can populate lists
            // items are mapped on name
            NameToCameraMap getAllCameras();
            NameToSequenceMap getAllSequences();
            NameToMonitorMap getAllMonitors();
            /*
            retreive the sequences based on the location id
            */
            void getSequenceByLocation(unsigned long locationId , std::vector< VideoInput*>& sequences);

            /* These are used to register and deregister interest in certain
              types of display devices.
              when a device changes the the appropriate windows message will be sent to
              the window. If a device referenced by a device changes - windows listening
              for the parent device will also be notified.
              eg if a window is listening for monitor updates, and a camera is modified
              then the window will receive monitor updates for monitors displaying the camera
              and for monitors displaying quads or sequences that contain the camera.
              A pointer to the modified device will be sent as the lParam.
              */
            void registerForMonitorChanges(CWnd* window);
            void registerForSequenceChanges(CWnd* window);
            void registerForCameraChanges(CWnd* window);
            void registerForStageChanges(CWnd* window);

            void deregisterForMonitorChanges(CWnd* window);
            void deregisterForSequenceChanges(CWnd* window);
            void deregisterForCameraChanges(CWnd* window);
            void deregisterForStageChanges(CWnd* window);

            /**
              * notifyGUIofDisplayItemUpdate
              *
              * *ANY* time one of the DisplayItems are updated, then this
              * must be called. This will determine what has changed annd force the GUI
              * to update accordingly.
              *
              * @param itemThatChanged  A pointer to the item that changed
              */
            void notifyGUIofDisplayItemUpdate(DisplayItem* itemThatChanged);

            /**
             * processUpdate
             *
             * When there is a configuration update of the type and key matching
             * one registered by this class, this method will be invoked
             * to process the update accordingly.
             */
            virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


            /**
              * receiveSpecialisedMessage
              *
              * Handle comms messages to update the state of objects.
              */
            virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

            /**
             * Starts the initialisation thread
             * As items are loaded, these messages will be sent to the window in order:
             * TA_INIT_CAMERAS
             * TA_INIT_SEQUENCES
             * TA_INIT_MONITORS
             * TA_INIT_CCTV
             */
            void initialiseDisplayItems(CWnd* callbackWindow);


            virtual void saveSequencesRelationship(unsigned long parentKey);
            /**
             * Loading methods to load display items in parallel
             */
            virtual void loadCamerasFromDatabase();
            virtual void loadSequencesFromDatabase();
            virtual void loadMonitorsFromDatabase();
            virtual void loadStationsFromDatabase();

            /**
             * Methods to load items from the agent
             */
            virtual void loadEntityRelationship(unsigned long parentKey,
                                                unsigned long& pkey,
                                                std::string& description,
                                                int* ptype = NULL);
            virtual void saveEntityRelationship(unsigned long parentkey,
                                                unsigned long pkey,
                                                const std::string& description, // SPLIT | SINGLE
                                                int type = RL_MONITOR_CAMERA_NORMAL);

            virtual void delEntityRelationship(unsigned long parentKey);

            void setVideoDisplayManagerDAI(IVideoDisplayManagerDAI* videoDisplayManagerDAI);
            /*
              get the stations from the locations
              checking the locationType
            */
            std::vector<TA_Base_Core::ILocation*>&  getStations();

            // level: 0->over view, 1->station office, 2->up plt, 3->down plt, 4->b1, 5->b2
            void getGraphicfullPath(unsigned long stationId,
                                    std::string& fullpath,
                                    unsigned long level = 0);

            void setVideoWallMonitorKeys(std::vector<unsigned long> keys);

            void loadStations();

            void LoadSequenceRelationFromDatabase();

            typedef TA_IRS_App::Monitor* (&VirtualQuadArrayRef)[4];

            Monitor* getLocalMonitor();

			virtual void workerFree()
			{
			}

            void deleteMonitor(Monitor* monitor);

            unsigned long getCctvSubystemKey();

        private:
			bool isVirtualMonitor(VideoOutput* output);
            void initItems();
            /**
             * In case the agent is down this allows to load all items from the database
             */
            void loadItemsFromDatabase();
            /**
             * Methods to create display items whether loaded from agent or database
             */
            Camera* createCamera(unsigned long key,
                                 const std::string& name,
                                 const std::string& description,
                                 const std::string& group,
                                 const std::string& address ,
                                 unsigned long locationId,
                                 bool isMovementSupported);

            Sequence* createSequence(unsigned long key,
                                     const std::string& name,
                                     const std::string& addr,
                                     const std::string& description,
                                     const std::string& group,
                                     bool readOnly,
                                     const std::string& sequenceDescription , unsigned long locationId);

            Monitor* createMonitor(unsigned long key,
                                   const std::string& name,
                                   const std::string& description,
                                   bool isConsoleMonitor,
                                   unsigned long consoleKey,
                                   int order,
                                   const std::string& address,
                                   bool isQuadMon = false);

            unsigned long getLocationKey();

            /**
              * deleteAllItems
              *
              * cleans up all objects
              */
            void deleteAllItems();

            /**
              * subscribeToMessages
              *
              * subscribes for comms messages and online updates
              * must be called after local objects are created
              */
            void subscribeToMessages();
            //bool CompareEntity(const EntityDataPtr& entity1,const EntityDataPtr& entity2);

            // Comms message handlers
            void processVideoOutputStateMessage(unsigned long outputKey, const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& videoOutputState);
            void processSequenceConfigMessage(unsigned long sequenceKey, const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig);

            // GUI update handlers
            void registerWindow(std::list<CWnd*>& list, CWnd* window);
            void deregisterWindow(std::list<CWnd*>& list, CWnd* window);
            void notifyMonitorListeners(Monitor* modifiedItem);
            void notifySequenceListeners(Sequence* modifiedItem);
            void notifyCameraListeners(Camera* modifiedItem);
            //     void notifyStageListeners(BvsStage* modifiedItem);
            void postMessageToWindows(const std::list<CWnd*>& windows, int message, DisplayItem* itemPtr);
            /*
             init the fullpath of graphic
            */
            void initGraphicFullPath();

            // local Monitor
            Monitor* m_localMonitor;
            // 4 virtual quad monitor
            unsigned long m_localConsoleKey;
            // inputs - mapped on name
            NameToCameraMap m_cameras;
            KeyToCameraMap m_camerasByPkey;
            AddressToCameraMap m_camerasByAddress;
            NameToSequenceMap m_sequences;
            KeyToSequenceMap m_sequencesByPkey;

            // outputs - mapped on name
            NameToMonitorMap m_monitors;
            KeyToMonitorMap m_monitorsByPkey;

            std::vector <Sequence *> m_seqVec;
            std::map<unsigned long, std::string > m_GraphicByStation;

            // windows to post changes to
            TA_Base_Core::ReEntrantThreadLockable m_windowListLock;
            std::list<CWnd*> monitorListeners;
            std::list<CWnd*> sequenceListeners;
            std::list<CWnd*> cameraListeners;
            std::list<CWnd*> stageListeners;

            std::vector<unsigned long> m_videoWallMonitorKeys;

            unsigned long  m_locationkey;
            TA_Base_Core::ILocation::ELocationType m_locationType;
            CWnd* m_callbackWindow;
            std::vector<TA_Base_Core::ILocation*> m_stations;
            IVideoDisplayManagerDAI* m_videoDisplayManagerDAI;
            unsigned long m_cctvSubSystemKey;
            std::auto_ptr<ThreadedMemberFunc> m_itemLoadingThread;
			std::map<UINT64, std::string> m_schematicPathCache;
    };
}

#endif // !defined(DisplayItemManager_H)

