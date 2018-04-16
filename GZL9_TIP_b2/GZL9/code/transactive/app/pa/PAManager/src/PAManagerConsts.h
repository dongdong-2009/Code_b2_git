const int MAX_SCHEDULE_FREQUENCY_TYPE_COUNT = 5;
const int DAYS_OF_WEEK_COUNT = 7;
const int DAYS_OF_MONTH_COUNT = 31;
const int SEARCH_KEYWORD_MAX_CHARS = 50;
const int SCHEDULE_NAME_MAX_CHARS = 40;
const int DEFAULT_CYCLE_BROADCAST_CHECK = 0;				// 0 for uncheck, 1 for check
const int DEFAULT_SCHEDULE_BROADCAST_IN_DAY_CHECK = 0; 		// 0 for uncheck, 1 for check
const int DVA_MSG_LIST_ID_INDEX = 0;
const int DVA_MSG_LIST_NAME_INDEX = 1;
const int DVA_MSG_LIST_COL_ID_WIDTH = 63;
const int DVA_MSG_LIST_COL_NAME_WIDTH = 177;
const int SCHEDULE_BROADCAST_LIST_COL_NAME_INDEX = 0;
const int SCHEDULE_BROADCAST_LIST_COL_START_TIME_INDEX = 1;
const int SCHEDULE_BROADCAST_LIST_COL_END_TIME_INDEX = 2;
const int SCHEDULE_BROADCAST_LIST_COL_FREQUENCY_INDEX = 3;
const int SCHEDULE_BROADCAST_LIST_COL_AUTO_DEL_INDEX = 4;
const int SCHEDULE_BROADCAST_LIST_COL_NAME_WIDTH = 100;
const int SCHEDULE_BROADCAST_LIST_COL_START_TIME_WIDTH = 200;
const int SCHEDULE_BROADCAST_LIST_COL_END_TIME_WIDTH = 200;
const int SCHEDULE_BROADCAST_LIST_COL_FREQUENCY_WIDTH = 130;
const int SCHEDULE_BROADCAST_LIST_COL_AUTO_DEL_WIDTH = 50;
const int SCHEDULE_BROADCAST_LIST_ITEM_MAX_COUNT = 100;
const int SCHEDULE_IN_DAY_LIST_COL_NAME_INDEX = 0;
const int SCHEDULE_IN_DAY_LIST_COL_START_TIME_INDEX = 1;
const int SCHEDULE_IN_DAY_LIST_COL_STATUS_INDEX = 2;
const int SCHEDULE_IN_DAY_LIST_COL_NAME_WIDTH = 0; // No need to show the name yet
const int SCHEDULE_IN_DAY_LIST_COL_START_TIME_WIDTH = 170;
const int SCHEDULE_IN_DAY_LIST_COL_STATUS_WIDTH = 80;
const int PA_MANAGER_FONT_SIZE = 10;
const int LIST_CTRL_FONT_SIZE = 9;
const int LIST_NO_SEL = -1;
const int CHECKBOX_UNCHECKED = 0;
const int CHECKBOX_CHECKED = 1;
const int REPEAT_TIME_MAX_CHARS = 4;
const int DWELL_TIME_MAX_CHARS = 4;
const int SIG_DELAY_MAX_CHARS = 10;
const int BACKUP_KEY_ICON_SIZE = 45;
const int ZONE_GROUP_SHOW_MAX_ITEMS = 16;

// Limit constraints
const int MAX_ZONE_GOURP_COUNT = 30;
const int DVA_DESC_MAX_CHAR_COUNT = 254;            // 127 Chinese char
const int GROUP_NAME_MAX_CHAR_COUNT = 254;      // 127 Chinese char
const int GROUP_NAME_BUFFER_SIZE = DVA_DESC_MAX_CHAR_COUNT + 4;
const int DVA_MSG_DESC_BUFFER_SIZE = GROUP_NAME_MAX_CHAR_COUNT + 4;
const int DVA_MIN_CYCLE_TIMES = 1;
const int DVA_MAX_CYCLE_TIMES = 255;
const int MAX_PLAY_DVA_MSG_NUMBER = 50;

const unsigned long SIG_MIN_DELAY_TIME = 1;
const unsigned long SIG_MAX_DELAY_TIME = 15;
const unsigned long MAX_DVA_MSG_SHORTCUT_COUNT = 26u;

// Initial const
const unsigned long UNINITIALIZE_UNSIGNED_LONG = -1;
const unsigned long INVALID_PKEY = 0u;
const unsigned long INVALID_DVA_MSG_KEY = -1;
const unsigned long INVALID_DVA_MSG_SHORTCUT_INDEX = 0u;
const unsigned long ACTIVE_CUR_SEL_ZONE_GROUP_KEY = 0u;

const char* const PA_MANAGER_FONT_FACE_NAME = "Arial Bold";
const char* const GWX_DISPLAY_PROG_ID = "Gwx32.Display";
const char* const OCC_PA_DLG_TITTLE = "����㲥������";
const char* const STATION_PA_DLG_TITTLE = "��վ�㲥������";
const char* const GROUP_NAME_CUR_SEL = "��ǰѡ��";
const char* const WARNING_TITLE = "��ʾ";
const char* const ERROR_TITLE = "����";
const char* const CONFIRM_TITLE = "ȷ��";
const char* const TIME_FORMAT_YYYY_MM_DD_HH_MM_SS = "yyyy-MM-dd HH:mm:ss";
const char* const TIME_FORMAT_HH_MM_SS = "HH:mm:ss";
const char* const DVA_MSG_LIST_COL_ID = "���";
const char* const DVA_MSG_LIST_COL_NAME = "�㲥����";
const char* const SCHEDULE_BROADCAST_LIST_COL_NAME = "�ƻ�����";
const char* const SCHEDULE_BROADCAST_LIST_COL_START_TIME = "��ʼʱ��";
const char* const SCHEDULE_BROADCAST_LIST_COL_END_TIME = "ֹͣʱ��";
const char* const SCHEDULE_BROADCAST_LIST_COL_FREQUENCY = "���ʱ��";
const char* const SCHEDULE_BROADCAST_LIST_COL_AUTO_DEL = "����";
const char* const SCHEDULE_IN_DAY_LIST_COL_NAME = "�ƻ�����";
const char* const SCHEDULE_IN_DAY_LIST_COL_START_TIME = "��ʼʱ��";
const char* const SCHEDULE_IN_DAY_LIST_COL_STATUS = "״̬";
const char* const UNKNOWN_ZONE_NAME = "δ֪����";
const char* const SCHEDULE_FREQUENCY_LIST[MAX_SCHEDULE_FREQUENCY_TYPE_COUNT] = { "��һ��", "ÿ��", "ÿ��", "ÿ��", "ÿ��" };
const char* const DAYS_OF_WEEK_LIST[DAYS_OF_WEEK_COUNT] = { "������", "����һ", "���ڶ�", "������", "������", "������", "������" };

const char* const RUN_PARAM_SELECT_TAG = "Select";
const char* const RUN_PARAM_DESELECT_TAG = "Deselect";
const char* const RUN_PARAM_UPDATE_SELECTION_TAG = "SynchroniseSelected";

const char* const DIVIDE_FORMAT_MARK = " - ";
const char* const DVA_MSG_SHORTCUT_BTN_PRE_NAME = "����";
const char* const DVA_MSG_SHORTCUT_FORMAT_AFT_PRE_NAME = "    ( ";
const char* const DVA_MSG_SHORTCUT_FORMAT_MAKR = " - ";
const char* const DVA_MSG_SHORTCUT_FORMAT_END_NAME = " )";

const char* const SCHEDULE_IN_DAY_STATUS_FAILED = "ʧ��";
const char* const SCHEDULE_IN_DAY_STATUS_WAITING = "�ȴ���";
const char* const SCHEDULE_IN_DAY_STATUS_SUCCESSFUL= "�ɹ�";
const char* const SCHEDULE_IN_DAY_STATUS_UNKNOWN= "δ֪";

const char* const DVA_MSG_TYPE_EMERGENCY_TEXT = "Ӧ���㲥";
const char* const DVA_MSG_TYPE_NORMAL_TEXT = "�ճ��㲥";
const char* const DVA_MSG_TYPE_OTHER_TEXT = "�����㲥";

// Success message
const char* const SUCCESS_MSG_BOX_TITTLE = "�ɹ�";
const char* const SUCCESS_DVA_BROADCAST_START = "\r\n     �㲥���ųɹ�     \r\n";
const char* const SUCCESS_DVA_BROADCAST_STOP = "\r\n     ֹͣ�㲥�ɹ�     \r\n";
const char* const SUCCESS_MONITOR_BROADCAST_START = "\r\n     �����㲥�ɹ�     \r\n";//added by lixiaoxia
const char* const SUCCESS_MONITOR_BROADCAST_STOP = "\r\n     ֹͣ�����ɹ�     \r\n";//added by lixiaoxia
const char* const SUCCESS_REMOVE_SCHEDULE_DVA_BROADCAST = "\r\nɾ�����ռƻ��㲥�ɹ�\r\n";
const char* const SUCCESS_LIVE_BROADCAST_START = "\r\n��ʼ�˹��㲥�ɹ�\r\n";
const char* const SUCCESS_LIVE_BROADCAST_STOP = "\r\nֹͣ�˹��㲥�ɹ�\r\n";
const char* const SUCCESS_BACKGROUND_MUSIC_START = "\r\n��ʼ��ӹ㲥�ɹ�\r\n";
const char* const SUCCESS_BACKGROUND_MUSIC_STOP = "\r\nֹͣ��ӹ㲥�ɹ�\r\n";
const char* const SUCCESS_DVA_MSG_PLAYBACK = "\r\n     ������Ƶ�ɹ�     \r\n";

const char* const EMPTY_STRING = "";

enum DVA_MSG_TYPE
{
    INVALID_DVA_MSG,
    EMERGENCY_DVA_MSG,
    NORMAL_DVA_MSG,
    OTHER_DVA_MSG
};
enum SCHEDULE_FREQUENCY_TYPE_ID
{
    FREQUENCY_ONCE = 0,
    FREQUENCY_DAY,
    FREQUENCY_WEEK,
    FREQUENCY_MONTH,
    FREQUENCY_YEAR
};
enum SCHEDULE_BROADCAST_DLG_MODE
{
    VIEW_MODE = 0,
    ADD_NEW_MODE,
    MODIFY_EXIST_MODE
};
enum BROADCAST_MODE
{
    LIVE_BROADCAST_MODE = 0,
    DVA_BROADCAST_MODE,
	BACKGROUND_MUSIC_BROADCAST_MODE
};
enum BACKUP_KEY_STATUS
{
    BACKUP_KEY_UNKNOWN_STATUS = 0,
    BACKUP_KEY_LOCK_MODE,
    BACKUP_KEY_UNLOCK_MODE
};
enum SCHEDULE_UPDATE_TYPE
{
    SCHEDULE_UPDATE_CREATE = 0,
    SCHEDULE_UPDATE_REMOVE,
    SCHEDULE_UPDATE_MODIFY,
    SCHEDULE_UPDATE_UNKNOWN
};
enum ZONE_SELECTED_MODE
{
    ZONE_SELECTED_UNKNOWN = 0,
    ZONE_SINGLE_SELECTED,
    ZONE_SYNCHRONISE_SELECTED
};

const int WM_CB_SETMINVISIBLE                       = 0x1700 + 1;
const int WM_SEL_QUICK_BROADCAST_DVA_MSG            = WM_USER + 0x100;
const int WM_UPDATE_DVA_MSG_LIST                    = WM_USER + 0x101;
const int WM_UPDATE_ZONE_GROUP_LIST                 = WM_USER + 0x102;
const int WM_POPUP_MESSAGE_BOX                      = WM_USER + 0x103;
const int WM_UPDATE_SCHEDULE_COMBOBOX               = WM_USER + 0x104;
const int WM_PA_STN_HMI_ZONE_SEL_CHANGE             = WM_USER + 0x105;
const int WM_UPDATE_DVA_MSG_SHORTCUT_BTN_TEXT       = WM_USER + 0x106;
const int WM_UPDATE_OPERATION_SCOPE_GUI             = WM_USER + 0x107;
const int WM_UPDATE_SIG_BROADCAST_SETTING           = WM_USER + 0x108;
const int WM_UPDATE_BACKUP_KEY_STATUS               = WM_USER + 0x109;
const int WM_LIST_ITEM_SELECTING_CHANGED            = WM_USER + 0x110;
const int WM_SCHEDULE_BROADCAST_UPDATE              = WM_USER + 0x111;
const int WM_SCHEDULE_IN_DAY_DLG_HIDE               = WM_USER + 0x112;

// Error message handler //
enum PA_MESSAGE_ID
{
    ERROR_NOT_DEFINE = -1,
    SUCCESS_NO_ERROR = 0,

    // Internal error message for log
    ERROR_INVALID_INPUT_PARAMETER,
    ERROR_DVA_MSG_NOT_EXIST,
    ERROR_CURRENT_EDIT_SCHEDULE_NOT_EXIST,
    ERROR_MAIN_WND_FOR_MSG_BOX_NOT_EXIST,
    ERROR_FAILED_TO_CREATE_INSTANCE,
    ERROR_COULD_NOT_FIND_EXPECTED_DATA,
    ERROR_UNEXPECTED_NULL_POINTER,
    ERROR_CATCHED_UNKNOWN_EXCEPTION,
    ERROR_UNAUTHORIZED_OPERATION,
    ERROR_UNEXPECTED_LOGIC,
    ERROR_NO_EXTERNAL_PA_INTERACTION_ALLOWED,
    ERROR_STATION_CANNOT_RUN_FOR_OVERALL,
    ERROR_FAILED_TO_INIT_COM,
    ERROR_NO_SCHEMATIC_FILE_NAME_IN_DATABASE,
    ERROR_INVALID_STRING,
    ERROR_RECEIVE_UNKNOWN_MESSAGE,

    // Error message show to operator
    ERROR_FAILED_TO_SAVE_NEW_SCHEDULE,
    ERROR_FAILED_TO_SAVE_DVA_MSG_DESCRIPTION,
    ERROR_FAILED_TO_SAVE_ZONG_GROUP,
    ERROR_FAILED_TO_DELETE_ZONG_GROUP,
    ERROR_FAILED_TO_START_DVA_BROADCAST,
    ERROR_FAILED_TO_STOP_DVA_BROADCAST,
    ERROR_FAILED_TO_REMOVE_SCHEDULE_DVA_BROADCAST,
    ERROR_FAILED_TO_START_LIVE_BROADCAST,
    ERROR_FAILED_TO_STOP_LIVE_BROADCAST,
	ERROR_FAILED_TO_START_BACKGROUND_MUSIC,
	ERROR_FAILED_TO_STOP_BACKGROUND_MUSIC,
    ERROR_FAILED_TO_START_DVA_MSG_PLAYBACK,
    ERROR_FAILED_TO_STOP_DVA_MSG_PLAYBACK,
    ERROR_FAILED_TO_START_MONITOR_BROADCAST,//added by lixiaoxia
    ERROR_FAILED_TO_STOP_MONITOR_BROADCAST,//added by lixiaoxia
    ERROR_FAILED_TO_LOCATE_SCHEMATIC_FILE,
    ERROR_FAILED_TO_INVOKE_GWX_INTERFACE,
    ERROR_GWX_FAILED_TO_OPEN_SCHEMATIC_FILE,
    ERROR_FAILED_TO_SOLVE_AGENT_OBJECT,
    ERROR_FAILED_TO_CONNECT_TO_HARDWARE,
    ERROR_PA_FEP_CONFIG_NOT_FOUND,
    ERROR_ZONE_GROUP_DELETED_ALREADY,
    ERROR_FAILED_TO_READ_DATABASE,
    ERROR_FAILED_TO_WRITE_DATABASE,
    ERROR_NO_RIGHTS_TO_BROADCAST_DVA_PLAYBACK,
    ERROR_NO_RIGHTS_TO_BROADCAST_OCC_DVA_MSG,
    ERROR_NO_RIGHTS_TO_BROADCAST_STATION_DVA_MSG,
	ERROR_NO_RIGHTS_TO_STOP_STATION_DVA_MSG,
    ERROR_NO_RIGHTS_TO_START_MONITOR_ZONE,//added by lixiaoxia
    ERROR_NO_RIGHTS_TO_STOP_MONITOR_ZONE,//added by lixiaoxia
    ERROR_NO_RIGHTS_TO_BROADCAST_STATION_LIVE,
	ERROR_NO_RIGHTS_TO_BROADCAST_STATION_BACKGROUND_MUSIC,
    ERROR_NO_RIGHTS_TO_MODIFY_ZONE_GROUP,
    ERROR_NO_RIGHTS_TO_MODIFY_DVA_MSG_DESCRIPTION,
    ERROR_AGENT_DATA_INVALID,
    ERROR_FAILED_TO_INIT_GWX_COM,
    ERROR_FAILED_TO_GET_BACKUP_KEY,
    ERROR_BACKUP_KEY_NOT_CONFIG,
    ERROR_FAILED_TO_RUN_SCHEDULE_MANAGER,

    // Warning message show to operator
    WARNING_SCHEDULE_NAME_NOT_SET,
    WARNING_FREQUENCY_NOT_SEL,
    WARNING_FREQUENCY_DETAIL_NOT_SEL,
    WARNING_GROUP_NAME_EMPTY,
    WARNING_GROUP_NAME_MAX_CHARS,
    WARNING_GROUP_NAME_SAME_AS_CUR_SEL,
    WARNING_ZONE_GROUP_NAME_ALREADY_EXIST,
    WARNING_ZONE_NOT_SELECTED,
    WARNING_ZONE_SELECTING_IN_LIVE_BROADCAST_MODE,
    WARNING_ZONE_SELECTING_IN_BACKGROUND_MUSIC_BROADCAST_MODE,
    WARNING_DELAY_TIME_OUT_OF_RANGE,
    WARNING_CYCLIC_TIMES_OUT_OF_RANGE,
    WARNING_CYCLIC_TIMES_IS_NOT_SET,
    WARNING_DWELL_TIME_OUT_OF_RANGE,
    WARNING_DWELL_TIME_IS_NOT_SET,
    WARNING_DVA_MSG_DESC_MAX_CHARS,
    WARNING_ZONE_GROUP_COUNT_MAX,
    WARNING_SCHEDULE_TIME_IN_PAST,
    WARNING_SCHEDULE_NOT_SELECT,
    WARNING_ZONE_SELECTED_MANY,//added by lixiaoxia

    // Confirm message show to operator
    CONFIRM_DELETE_ZONE_GROUP,
    CONFIRM_SWITCH_TO_DVA_BROADCAST_MODE,
    CONFIRM_SWITCH_TO_LIVE_BROADCAST_MODE,
	CONFIRM_SWITCH_TO_BACKGROUND_MUSIC_BROADCAST_MODE,
    CONFIRM_START_DVA_BROADCAST,
    CONFIRM_STOP_DVA_BROADCAST,
    CONFIRM_START_MONITOR_BROADCAST,//added by lixiaoxia
    CONFIRM_STOP_MONITOR_BROADCAST,//added by-lixiaoxia
    CONFIRM_START_DVA_MSG_PLAYBACK,
    CONFIRM_DELETE_SCHEDULE_BROADCAST_IN_DAY,

    // The count must be the last one
    ERROR_MSG_COUNT
};

const char* const PA_ERROR_MSG[ERROR_MSG_COUNT] = 
{
    // Internal error message for log
    "No error occur",                                                           // SUCCESS_NO_ERROR
    "Invalid parameter",                                                        // ERROR_INVALID_INPUT_PARAMETER
    "The DVA message didn't exist anymore",                                     // ERROR_DVA_MSG_NOT_EXIST
    "Current editing broadcast schedule is not exist",                          // ERROR_CURRENT_EDIT_SCHEDULE_NOT_EXIST
    "Failed to popup message box because parent dialog is not exist",           // ERROR_MAIN_WND_FOR_MSG_BOX_NOT_EXIST
    "Failed to create instance, fatal error",                                   // ERROR_FAILED_TO_CREATE_INSTANCE
    "Could not find expect data in std::map",                                   // ERROR_COULD_NOT_FIND_EXPECTED_DATA
    "Unexpected NULL pointer",                                                  // ERROR_UNEXPECTED_NULL_POINTER
    "Caught unknown exception",                                                 // ERROR_CATCHED_UNKNOWN_EXCEPTION
    "Unauthorized operation",                                                   // ERROR_UNAUTHORIZED_OPERATION
    "The logic is unexpected",                                                  // ERROR_UNEXPECTED_LOGIC
    "PA Manager may only interface with one schematic at a time",               // ERROR_NO_EXTERNAL_PA_INTERACTION_ALLOWED
    "PA Manager cannot be run for a foreign station",                           // ERROR_STATION_CANNOT_RUN_FOR_OVERALL
	"Failed to CoInitialize COM",                                               // ERROR_FAILED_TO_INIT_COM
    "Failed to find schematic file name in database",                           // ERROR_NO_SCHEMATIC_FILE_NAME_IN_DATABASE
    "String is not valid",                                                          // ERROR_INVALID_STRING
    "Receiver unknown message",                                                 // ERROR_RECEIVE_UNKNOWN_MESSAGE

    // Error message show to operator
    "�����µĹ㲥�ƻ�ʧ��,����ϵ�������֧��",                                  // ERROR_FAILED_TO_SAVE_NEW_SCHEDULE
    "�޸Ĺ㲥��Ϣ����ʧ��,����ϵ�������֧��",                                  // ERROR_FAILED_TO_SAVE_DVA_MSG_DESCRIPTION
    "�����µĹ㲥Ⱥ��ʧ��,����ϵ�������֧��",                                  // ERROR_FAILED_TO_SAVE_ZONG_GROUP
    "ɾ��ѡ���㲥Ⱥ��ʧ��,����ϵ�������֧��",                                  // ERROR_FAILED_TO_DELETE_ZONG_GROUP
    "������Ƶ�㲥ʧ��,����ϵ�������֧��",                                      // ERROR_FAILED_TO_START_DVA_BROADCAST
    "ֹͣ��Ƶ�㲥ʧ��,����ϵ�������֧��",                                      // ERROR_FAILED_TO_STOP_DVA_BROADCAST
    "ɾ�����ռƻ���Ƶ�㲥ʧ��,����ϵ�������֧��",                              // ERROR_FAILED_TO_REMOVE_SCHEDULE_DVA_BROADCAST
    "����ʵʱ�㲥ʧ��,����ϵ�������֧��",                                      // ERROR_FAILED_TO_START_LIVE_BROADCAST
    "ֹͣʵʱ�㲥ʧ��,����ϵ�������֧��",                                      // ERROR_FAILED_TO_STOP_LIVE_BROADCAST
	"������ӹ㲥ʧ��,����ϵ�������֧��",                                      // ERROR_FAILED_TO_START_BACKGROUND_MUSIC
	"ֹͣ��ӹ㲥ʧ��,����ϵ�������֧��",                                      // ERROR_FAILED_TO_STOP_BACKGROUND_MUSIC
    "������Ƶ�㲥ʧ��,����ϵ�������֧��",                                      // ERROR_FAILED_TO_START_DVA_MSG_PLAYBACK
    "ֹͣ������Ƶ�㲥ʧ��,����ϵ�������֧��",                                  // ERROR_FAILED_TO_STOP_DVA_MSG_PLAYBACK
    "�����㲥ʧ��" ,                                                            //ERROR_FAILED_TO_START_MONITOR_BROADCAST(added by lixiaoxia)
    "ֹͣ����ʧ��" ,                                                            //ERROR_FAILED_TO_STOP_MONITOR_BROADCAST//added by lixiaoxia
    "�޷��ҵ��㲥������ʾ�ļ�,����ϵ�������֧��",                              // ERROR_FAILED_TO_LOCATE_SCHEMATIC_FILE
    "����GWX32�ӿ�ʧ��,����ϵ�������֧��",                                     // ERROR_FAILED_TO_INVOKE_GWX_INTERFACE
    "GWX32�޷���ָ���ļ�,����ϵ�������֧��",                                 // ERROR_GWX_FAILED_TO_OPEN_SCHEMATIC_FILE
    "�޷����ӵ�ISCS������PAϵͳ",                                               // ERROR_FAILED_TO_SOLVE_AGENT_OBJECT
    "�޷����ӵ�PAӲ��ϵͳ",                                                     // ERROR_FAILED_TO_CONNECT_TO_HARDWARE
    "�޷������ݿ����ҵ�Ӳ����������",                                           // ERROR_PA_FEP_CONFIG_NOT_FOUND
    "PA�㲥��Ⱥ�ѱ�ɾ��",                                                       // ERROR_ZONE_GROUP_DELETED_ALREADY
    "���ݿ��ȡʧ��",                                                           // ERROR_FAILED_TO_READ_DATABASE
    "���ݿ�д��ʧ��",                                                           // ERROR_FAILED_TO_WRITE_DATABASE
    "û��������Ƶ��Ȩ��",                                                       // ERROR_NO_RIGHTS_TO_BROADCAST_DVA_PLAYBACK
    "û�й㲥���ŵ�Ȩ��",                                                       // ERROR_NO_RIGHTS_TO_BROADCAST_OCC_DVA_MSG
    "û�й㲥���ŵ�Ȩ��",                                                       // ERROR_NO_RIGHTS_TO_BROADCAST_STATION_DVA_MSG
    "û��ֹͣ�㲥��Ȩ��",                                                       // ERROR_NO_RIGHTS_TO_STOP_STATION_DVA_MSG
    "û�м����㲥��Ȩ��",                                                       //ERROR_NO_RIGHTS_TO_START_MONITOR_ZONE//added by lixiaoxia
    "û��ֹͣ������Ȩ��",                                                       //ERROR_NO_RIGHTS_TO_STOP_MONITOR_ZONE//added by lixiaoxia
    "û���˹��㲥��Ȩ��",                                                       // ERROR_NO_RIGHTS_TO_BROADCAST_STATION_LIVE
	"û����ӹ㲥��Ȩ��",                                                       // ERROR_NO_RIGHTS_TO_BROADCAST_STATION_BACKGROUND_MUSIC
    "û���޸Ĺ㲥������Ⱥ��Ȩ��",                                               // ERROR_NO_RIGHTS_TO_MODIFY_ZONE_GROUP
    "û���޸�Ԥ��㲥��Ϣ������Ȩ��",                                           // ERROR_NO_RIGHTS_TO_MODIFY_DVA_MSG_DESCRIPTION
    "�޷���ISCS������PAϵͳ��ȡ����,\r\nSIG�㲥���ý��ݲ�����",    // ERROR_AGENT_DATA_INVALID
    "Gwx32 COM ��ʼ��ʧ��",                                                     // ERROR_FAILED_TO_INIT_GWX_COM
    "��ȡ��Կ��״̬ʧ��",                                                       // ERROR_FAILED_TO_GET_BACKUP_KEY
    "���ݿ��к�Կ�����ô���, \r\n��Կ��״̬��������",  // ERROR_BACKUP_KEY_NOT_CONFIG
    "�޷�����ʱ��������,����ϵ�������֧��", // ERROR_FAILED_TO_RUN_SCHEDULE_MANAGER

    // Warning message show to operator
    "������\"�ƻ�����\"",          // WARNING_SCHEDULE_NAME_NOT_SET
    "��ѡ��\"�㲥Ƶ��\"",
    "��ѡ��\"Ƶ����ϸ\"",
    "��Ⱥ������Ϊ��",
    "��Ⱥ�����Ϊ127�������ַ�",
    "��Ⱥ������Ϊ\"��ǰѡ��\"",
    "��Ⱥ���Ѵ���,����������",         // WARNING_ZONE_GROUP_NAME_ALREADY_EXIST
    "��ѡ��㲥����",
    "�˹�����ģʽ�޷����//ɾ���㲥����",
	"��Ӳ���ģʽ�޷����//ɾ���㲥����", //WARNING_ZONE_SELECTING_IN_BACKGROUND_MUSIC_BROADCAST_MODE
    "�㲥�ӳٷ�ΧΪ: 1�� �� 15��",
    "ѭ���㲥������ΧΪ: 1 �� 255",
    "������ѭ���㲥����",
    "�㲥�����ΧΪ: 1 �� 63��",
    "������㲥���ʱ��",
    "Ԥ¼��Ϣ�������Ϊ127�������ַ�",
    "�㲥Ⱥ�������Ϊ: 30��",
    "���ռƻ�����ʱ�䲻�����ڵ�ǰϵͳʱ��",     // WARNING_SCHEDULE_TIME_IN_PAST
    "���ռƻ��㲥����ɻ��ѱ�ɾ��",    // WARNING_SCHEDULE_NOT_SELECT
    "�����㲥��֧��һ���㲥����,������ѡ��㲥����" ,        //WARNING_ZONE_SELECTED_MANY added by lixiaoxia

    // Confirm message show to operator
    "��ȷ���Ƿ���Ҫɾ���㲥Ⱥ��\?",                // CONFIRM_DELETE_ZONE_GROUP
    "��ȷ���Ƿ���Ҫ�л����Զ��㲥ģʽ\?",          // CONFIRM_SWITCH_TO_DVA_BROADCAST_MODE
    "��ȷ���Ƿ���Ҫ�л����˹��㲥ģʽ\?",          // CONFIRM_SWITCH_TO_LIVE_BROADCAST_MODE
	"��ȷ���Ƿ���Ҫ�л�����ӹ㲥ģʽ\?",          // CONFIRM_SWITCH_TO_BACKGROUND_MUSIC_BROADCAST_MODE
    "��ȷ���Ƿ���Ҫ������Ƶ�㲥\?",                // CONFIRM_START_DVA_BROADCAST
    "��ȷ���Ƿ���Ҫֹͣ��Ƶ�㲥\?",                // CONFIRM_STOP_DVA_BROADCAST
    "��ȷ���Ƿ���Ҫ�����㲥\?",                     //CONFIRM_START_MONITOR_BROADCAST,//added by lixiaoxia
    "��ȷ���Ƿ���Ҫֹͣ����\?",                    //CONFIRM_STOP_MONITOR_BROADCAST,//added by-lixiaoxia
    "��ȷ���Ƿ���Ҫ������Ƶ�㲥\?",                // CONFIRM_START_DVA_MSG_PLAYBACK
    "��ȷ���Ƿ���Ҫɾ�����ռƻ��㲥\?"             // CONFIRM_DELETE_SCHEDULE_BROADCAST_IN_DAY
};

const char* const PA_ERROR_MSG_ID_NOT_EXIST = "Try to log by not exist ID";
// Error message handler //