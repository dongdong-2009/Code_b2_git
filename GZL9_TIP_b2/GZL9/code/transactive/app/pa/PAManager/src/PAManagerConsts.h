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
const char* const OCC_PA_DLG_TITTLE = "中央广播管理器";
const char* const STATION_PA_DLG_TITTLE = "车站广播管理器";
const char* const GROUP_NAME_CUR_SEL = "当前选择";
const char* const WARNING_TITLE = "提示";
const char* const ERROR_TITLE = "错误";
const char* const CONFIRM_TITLE = "确认";
const char* const TIME_FORMAT_YYYY_MM_DD_HH_MM_SS = "yyyy-MM-dd HH:mm:ss";
const char* const TIME_FORMAT_HH_MM_SS = "HH:mm:ss";
const char* const DVA_MSG_LIST_COL_ID = "编号";
const char* const DVA_MSG_LIST_COL_NAME = "广播名称";
const char* const SCHEDULE_BROADCAST_LIST_COL_NAME = "计划名称";
const char* const SCHEDULE_BROADCAST_LIST_COL_START_TIME = "开始时间";
const char* const SCHEDULE_BROADCAST_LIST_COL_END_TIME = "停止时间";
const char* const SCHEDULE_BROADCAST_LIST_COL_FREQUENCY = "间隔时间";
const char* const SCHEDULE_BROADCAST_LIST_COL_AUTO_DEL = "启用";
const char* const SCHEDULE_IN_DAY_LIST_COL_NAME = "计划名称";
const char* const SCHEDULE_IN_DAY_LIST_COL_START_TIME = "开始时间";
const char* const SCHEDULE_IN_DAY_LIST_COL_STATUS = "状态";
const char* const UNKNOWN_ZONE_NAME = "未知区域";
const char* const SCHEDULE_FREQUENCY_LIST[MAX_SCHEDULE_FREQUENCY_TYPE_COUNT] = { "仅一次", "每天", "每周", "每月", "每年" };
const char* const DAYS_OF_WEEK_LIST[DAYS_OF_WEEK_COUNT] = { "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六" };

const char* const RUN_PARAM_SELECT_TAG = "Select";
const char* const RUN_PARAM_DESELECT_TAG = "Deselect";
const char* const RUN_PARAM_UPDATE_SELECTION_TAG = "SynchroniseSelected";

const char* const DIVIDE_FORMAT_MARK = " - ";
const char* const DVA_MSG_SHORTCUT_BTN_PRE_NAME = "常用";
const char* const DVA_MSG_SHORTCUT_FORMAT_AFT_PRE_NAME = "    ( ";
const char* const DVA_MSG_SHORTCUT_FORMAT_MAKR = " - ";
const char* const DVA_MSG_SHORTCUT_FORMAT_END_NAME = " )";

const char* const SCHEDULE_IN_DAY_STATUS_FAILED = "失败";
const char* const SCHEDULE_IN_DAY_STATUS_WAITING = "等待中";
const char* const SCHEDULE_IN_DAY_STATUS_SUCCESSFUL= "成功";
const char* const SCHEDULE_IN_DAY_STATUS_UNKNOWN= "未知";

const char* const DVA_MSG_TYPE_EMERGENCY_TEXT = "应急广播";
const char* const DVA_MSG_TYPE_NORMAL_TEXT = "日常广播";
const char* const DVA_MSG_TYPE_OTHER_TEXT = "其它广播";

// Success message
const char* const SUCCESS_MSG_BOX_TITTLE = "成功";
const char* const SUCCESS_DVA_BROADCAST_START = "\r\n     广播播放成功     \r\n";
const char* const SUCCESS_DVA_BROADCAST_STOP = "\r\n     停止广播成功     \r\n";
const char* const SUCCESS_MONITOR_BROADCAST_START = "\r\n     监听广播成功     \r\n";//added by lixiaoxia
const char* const SUCCESS_MONITOR_BROADCAST_STOP = "\r\n     停止监听成功     \r\n";//added by lixiaoxia
const char* const SUCCESS_REMOVE_SCHEDULE_DVA_BROADCAST = "\r\n删除今日计划广播成功\r\n";
const char* const SUCCESS_LIVE_BROADCAST_START = "\r\n开始人工广播成功\r\n";
const char* const SUCCESS_LIVE_BROADCAST_STOP = "\r\n停止人工广播成功\r\n";
const char* const SUCCESS_BACKGROUND_MUSIC_START = "\r\n开始外接广播成功\r\n";
const char* const SUCCESS_BACKGROUND_MUSIC_STOP = "\r\n停止外接广播成功\r\n";
const char* const SUCCESS_DVA_MSG_PLAYBACK = "\r\n     试听音频成功     \r\n";

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
    "保存新的广播计划失败,请联系软件技术支持",                                  // ERROR_FAILED_TO_SAVE_NEW_SCHEDULE
    "修改广播消息描述失败,请联系软件技术支持",                                  // ERROR_FAILED_TO_SAVE_DVA_MSG_DESCRIPTION
    "保存新的广播群组失败,请联系软件技术支持",                                  // ERROR_FAILED_TO_SAVE_ZONG_GROUP
    "删除选定广播群组失败,请联系软件技术支持",                                  // ERROR_FAILED_TO_DELETE_ZONG_GROUP
    "播放音频广播失败,请联系软件技术支持",                                      // ERROR_FAILED_TO_START_DVA_BROADCAST
    "停止音频广播失败,请联系软件技术支持",                                      // ERROR_FAILED_TO_STOP_DVA_BROADCAST
    "删除当日计划音频广播失败,请联系软件技术支持",                              // ERROR_FAILED_TO_REMOVE_SCHEDULE_DVA_BROADCAST
    "播放实时广播失败,请联系软件技术支持",                                      // ERROR_FAILED_TO_START_LIVE_BROADCAST
    "停止实时广播失败,请联系软件技术支持",                                      // ERROR_FAILED_TO_STOP_LIVE_BROADCAST
	"播放外接广播失败,请联系软件技术支持",                                      // ERROR_FAILED_TO_START_BACKGROUND_MUSIC
	"停止外接广播失败,请联系软件技术支持",                                      // ERROR_FAILED_TO_STOP_BACKGROUND_MUSIC
    "试听音频广播失败,请联系软件技术支持",                                      // ERROR_FAILED_TO_START_DVA_MSG_PLAYBACK
    "停止试听音频广播失败,请联系软件技术支持",                                  // ERROR_FAILED_TO_STOP_DVA_MSG_PLAYBACK
    "监听广播失败" ,                                                            //ERROR_FAILED_TO_START_MONITOR_BROADCAST(added by lixiaoxia)
    "停止监听失败" ,                                                            //ERROR_FAILED_TO_STOP_MONITOR_BROADCAST//added by lixiaoxia
    "无法找到广播区域显示文件,请联系软件技术支持",                              // ERROR_FAILED_TO_LOCATE_SCHEMATIC_FILE
    "调用GWX32接口失败,请联系软件技术支持",                                     // ERROR_FAILED_TO_INVOKE_GWX_INTERFACE
    "GWX32无法打开指定文件,请联系软件技术支持",                                 // ERROR_GWX_FAILED_TO_OPEN_SCHEMATIC_FILE
    "无法连接到ISCS服务器PA系统",                                               // ERROR_FAILED_TO_SOLVE_AGENT_OBJECT
    "无法连接到PA硬件系统",                                                     // ERROR_FAILED_TO_CONNECT_TO_HARDWARE
    "无法在数据库中找到硬件连接配置",                                           // ERROR_PA_FEP_CONFIG_NOT_FOUND
    "PA广播组群已被删除",                                                       // ERROR_ZONE_GROUP_DELETED_ALREADY
    "数据库读取失败",                                                           // ERROR_FAILED_TO_READ_DATABASE
    "数据库写入失败",                                                           // ERROR_FAILED_TO_WRITE_DATABASE
    "没有试听音频的权限",                                                       // ERROR_NO_RIGHTS_TO_BROADCAST_DVA_PLAYBACK
    "没有广播播放的权限",                                                       // ERROR_NO_RIGHTS_TO_BROADCAST_OCC_DVA_MSG
    "没有广播播放的权限",                                                       // ERROR_NO_RIGHTS_TO_BROADCAST_STATION_DVA_MSG
    "没有停止广播的权限",                                                       // ERROR_NO_RIGHTS_TO_STOP_STATION_DVA_MSG
    "没有监听广播的权限",                                                       //ERROR_NO_RIGHTS_TO_START_MONITOR_ZONE//added by lixiaoxia
    "没有停止监听的权限",                                                       //ERROR_NO_RIGHTS_TO_STOP_MONITOR_ZONE//added by lixiaoxia
    "没有人工广播的权限",                                                       // ERROR_NO_RIGHTS_TO_BROADCAST_STATION_LIVE
	"没有外接广播的权限",                                                       // ERROR_NO_RIGHTS_TO_BROADCAST_STATION_BACKGROUND_MUSIC
    "没有修改广播区域组群的权限",                                               // ERROR_NO_RIGHTS_TO_MODIFY_ZONE_GROUP
    "没有修改预存广播消息描述的权限",                                           // ERROR_NO_RIGHTS_TO_MODIFY_DVA_MSG_DESCRIPTION
    "无法从ISCS服务器PA系统获取数据,\r\nSIG广播设置将暂不可用",    // ERROR_AGENT_DATA_INVALID
    "Gwx32 COM 初始化失败",                                                     // ERROR_FAILED_TO_INIT_GWX_COM
    "获取后备钥匙状态失败",                                                       // ERROR_FAILED_TO_GET_BACKUP_KEY
    "数据库中后备钥匙配置错误, \r\n后备钥匙状态将不可用",  // ERROR_BACKUP_KEY_NOT_CONFIG
    "无法启动时间表管理器,请联系软件技术支持", // ERROR_FAILED_TO_RUN_SCHEDULE_MANAGER

    // Warning message show to operator
    "请输入\"计划名称\"",          // WARNING_SCHEDULE_NAME_NOT_SET
    "请选择\"广播频率\"",
    "请选择\"频率详细\"",
    "组群名不能为空",
    "组群名最多为127个中文字符",
    "组群名不能为\"当前选择\"",
    "组群名已存在,请重新输入",         // WARNING_ZONE_GROUP_NAME_ALREADY_EXIST
    "请选择广播区域",
    "人工播放模式无法添加//删除广播区域",
	"外接播放模式无法添加//删除广播区域", //WARNING_ZONE_SELECTING_IN_BACKGROUND_MUSIC_BROADCAST_MODE
    "广播延迟范围为: 1秒 到 15秒",
    "循环广播次数范围为: 1 到 255",
    "请输入循环广播次数",
    "广播间隔范围为: 1 到 63秒",
    "请输入广播间隔时间",
    "预录消息描述最多为127个中文字符",
    "广播群组数最多为: 30个",
    "当日计划播放时间不能早于当前系统时间",     // WARNING_SCHEDULE_TIME_IN_PAST
    "当日计划广播已完成或已被删除",    // WARNING_SCHEDULE_NOT_SELECT
    "监听广播仅支持一个广播区域,请重新选择广播区域" ,        //WARNING_ZONE_SELECTED_MANY added by lixiaoxia

    // Confirm message show to operator
    "请确认是否需要删除广播群组\?",                // CONFIRM_DELETE_ZONE_GROUP
    "请确认是否需要切换到自动广播模式\?",          // CONFIRM_SWITCH_TO_DVA_BROADCAST_MODE
    "请确认是否需要切换到人工广播模式\?",          // CONFIRM_SWITCH_TO_LIVE_BROADCAST_MODE
	"请确认是否需要切换到外接广播模式\?",          // CONFIRM_SWITCH_TO_BACKGROUND_MUSIC_BROADCAST_MODE
    "请确认是否需要播放音频广播\?",                // CONFIRM_START_DVA_BROADCAST
    "请确认是否需要停止音频广播\?",                // CONFIRM_STOP_DVA_BROADCAST
    "请确认是否需要监听广播\?",                     //CONFIRM_START_MONITOR_BROADCAST,//added by lixiaoxia
    "请确认是否需要停止监听\?",                    //CONFIRM_STOP_MONITOR_BROADCAST,//added by-lixiaoxia
    "请确认是否需要试听音频广播\?",                // CONFIRM_START_DVA_MSG_PLAYBACK
    "请确认是否需要删除今日计划广播\?"             // CONFIRM_DELETE_SCHEDULE_BROADCAST_IN_DAY
};

const char* const PA_ERROR_MSG_ID_NOT_EXIST = "Try to log by not exist ID";
// Error message handler //