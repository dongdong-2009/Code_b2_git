
const char* const EMPTY_STRING = "";
const unsigned long UNINITIALIZE_UNSIGNED_LONG = -1;
const char* const GWX_DISPLAY_PROG_ID = "Gwx32.Display";

enum PA_MESSAGE_ID
{
    ERROR_NOT_DEFINE = -1,
    SUCCESS_NO_ERROR = 0,

    // Internal error message for log
    
    ERROR_UNEXPECTED_NULL_POINTER,
    
    ERROR_UNEXPECTED_LOGIC,
    
    // Error message show to operator
    
    ERROR_FAILED_TO_LOCATE_SCHEMATIC_FILE,
    ERROR_FAILED_TO_INVOKE_GWX_INTERFACE,
    ERROR_GWX_FAILED_TO_OPEN_SCHEMATIC_FILE,
    
    // The count must be the last one
    ERROR_MSG_COUNT
};

const char* const PA_ERROR_MSG[ERROR_MSG_COUNT] = 
{
    // Internal error message for log
    
    "Unexpected NULL pointer",                                              // ERROR_UNEXPECTED_NULL_POINTER
    "The logic is unexpected",                                              // ERROR_UNEXPECTED_LOGIC
    
     //Error message show to operator
    
    "无法找到广播区域显示文件,请寻求软件技术支持",                          // ERROR_FAILED_TO_LOCATE_SCHEMATIC_FILE
    "调用GWX32接口失败,请寻求软件技术支持",                                 // ERROR_FAILED_TO_INVOKE_GWX_INTERFACE
    "GWX32无法打开指定文件,请寻求软件技术支持",                             // ERROR_GWX_FAILED_TO_OPEN_SCHEMATIC_FILE
    

    
};
// Error message handler //