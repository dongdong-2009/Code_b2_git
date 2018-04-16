
const std::string REQUIRED_MSG( "Required" );
const std::string NUM_CDS_MSG( " CD(s) will be required to complete the transfer to CD.\nThe following files will be transferred:" );
const std::string NUM_DVDS_MSG( " DVD(s) will be required to complete the transfer.\nThe following files will be transferred:" );
const std::string INSERT_CD_MSG( "Please insert a blank CD and press OK to continue." );
const std::string INSERT_DVD_MSG( "Please insert a blank DVD and press OK to continue." );

//const std::string REQUIRED_MSG( "需要" );
//const std::string NUM_CDS_MSG( " 块CD来完成备份.\n以下文件将会被备份:" );
//const std::string NUM_DVDS_MSG( " 块DVD来完成备份.\n以下文件将会被备份:" );
//const std::string INSERT_CD_MSG( "请插入一块空白的DVD，然后按确定键继续。" );
//const std::string INSERT_DVD_MSG( "请插入一块空白的DVD，然后按确定键继续。" );
const CString DATE_MSG( "Date" );
//const CString DATE_MSG( "日期" );
const CString FILENAME_MSG( "Filename" );
//const CString FILENAME_MSG( "文件名" );
const CString SIZE_MSG( "Size" );
//const CString SIZE_MSG( "大小" );
const std::string ARCHIVE_DATA_FROM_MSG( "Archiving data from " );
//const std::string ARCHIVE_DATA_FROM_MSG( "归档数据从 " );
const std::string ARCHIVE_DATA_TO_MSG( " to " );
//const std::string ARCHIVE_DATA_TO_MSG( " 到 " );
const CString DATA_DATE_MSG( "Data Date" );
//const CString DATA_DATE_MSG( "数据日期" );
const CString RESTORED_BY_MSG( "Restored By" );
//const CString RESTORED_BY_MSG( "恢复人" );
const CString RESTORED_DATE_MSG( "Restoration Date" );
//const CString RESTORED_DATE_MSG( "恢复日期" );
const CString DELETION_DATE_MSG( "Deletion Date" );
//const CString DELETION_DATE_MSG( "删除日期" );
const CString STATUS_MSG( "Status" );
//const CString STATUS_MSG( "状态" );
const std::string CANCELLING_MSG( "Cancelling..." );
//const std::string CANCELLING_MSG( "正在取消..." );
const std::string OPENING_NEROAPI_MSG( "Opening NeroAPI.DLL");
//const std::string OPENING_NEROAPI_MSG( "打开 NeroAPI.DLL" );
const std::string RETRIVE_VERSION_MSG( "Retrieving version information.");
//const std::string RETRIVE_VERSION_MSG( "重新获取版本信息." );
//const std::string NERO_API_MSG( "Nero API version ");
//const std::string NERO_API_MSG( "Nero API 版本 " );
////const std::string FILLING_NERO_SET_STRUCTURE_MSG( "Filling NERO_SETTINGS structure");
//const std::string FILLING_NERO_SET_STRUCTURE_MSG( "填充 NERO_SETTINGS 结构体" );
////const char NERO_FILE_MSG[] = "NeroFiles";
//const char NERO_FILE_MSG[] = "Nero文件" ;
////const char AHEAD_MSG[] = "ahead";
//const char AHEAD_MSG[] = "往前" ;
////const char NERO_BURN_ROM_MSG[] = "Nero - Burning Rom";
//const char NERO_BURN_ROM_MSG[] = "Nero - 正在刻录" ;
//const char INITIALIZATION_NEROAPI_SUCC_MSG[] = "Initialization of the NeroAPI successful.";
//const char INITIALIZATION_NEROAPI_SUCC_MSG[] = "初始化NeroAPI成功." ;

const char INITIALIZING_MSG[] = "initialising..." ;

//const char INITIALIZING_MSG[] = "正在初始化..." ;
const char INITIALIZATION_SUCC_MSG[] = "initialisation Successful" ;
//const char INITIALIZATION_SUCC_MSG[] = "初始化成功" ;
const char INITIALIZATION_FAIL_MSG[] = "initialisation Failed" ;
//const char INITIALIZATION_FAIL_MSG[] = "初始化失败" ;
const char FOUND_DEVICES_MSG[] = "Found the following devices:";
//const char FOUND_DEVICES_MSG[] = "找到如下设备:" ;
const std::string DRIVE_MSG( "Drive " );
//const std::string DRIVE_MSG( "驱动 " );
const std::string SELECTED_ARCHIVE_MSG( ": selected for archiving.");
//const std::string SELECTED_ARCHIVE_MSG( ": 被选择了." );
//const std::string CONFIGURING_MSG ("正在刻录参数 ...");

const std::string CONFIGURING_MSG ("Preparing to write parameter ...");

const std::string PREPARE_WRITE_MSG( "Preparing to write DVD...");
//const std::string PREPARE_WRITE_MSG( "准备刻录 DVD..." );
const std::string PREPARE_WRITE_CD_MSG( "Preparing to write CD...");
//const std::string PREPARE_WRITE_CD_MSG( "准备刻录 CD..." );
const std::string WRITE_COMPLETED_SUCC_MSG( "Write Completed Successfully");
//const std::string WRITE_COMPLETED_SUCC_MSG( "写成功" );
const std::string WRITE_FAILED_MSG( "Write Failed" );
//const std::string WRITE_FAILED_MSG( "写失败" );

const std::string ARCHIVE_CLOSE_MSG( "Close");
//const std::string ARCHIVE_CLOSE_MSG( "关闭" );
const CString ARCHIVE_CLOSED_MSG( "Close");
//const CString ARCHIVE_CLOSED_MSG( "关闭" );
const CString WRITE_SUCC_MSG( "Write successful");
//const CString WRITE_SUCC_MSG( "刻录成功" );
const CString CD_WRITE_SUCC_MSG( "CD Write Successful");
//const CString CD_WRITE_SUCC_MSG( "CD刻录成功" );
const CString DVD_WRITE_SUCC_MSG( "DVD Write Successful");
//const CString DVD_WRITE_SUCC_MSG( "DVD刻录成功" );
const CString CD_WRITE_FAILED_MSG( "CD Write Failed");
//const CString CD_WRITE_FAILED_MSG( "CD刻录失败" );
const CString DVD_WRITE_FAILED_MSG( "DVD Write Failed");
//const CString DVD_WRITE_FAILED_MSG( "DVD刻录失败" );
const std::string ARCHIVE_LOG_MSG( "Log: ");
//const std::string ARCHIVE_LOG_MSG( "日志" );
const CString READING_DIREC_MSG( "Reading directories");
//const CString READING_DIREC_MSG( "正在读目录" );
const CString CREATING_DIREC_MSG( "Creating directories");
//const CString CREATING_DIREC_MSG( "正在创建目录" );
const CString CHECKING_DIREC_MSG( "Checking discs");
//const CString CHECKING_DIREC_MSG( "正在检查磁盘" );
const std::string MAJOR_PHASE_MSG( "Major phase: ");
//const std::string MAJOR_PHASE_MSG( "执行步骤: " );
const std::string FILE_NAME_MSG( "File: ");
//const std::string FILE_NAME_MSG( "文件名: " );

const CString VALIDATING_MEDIA ("Validatating Media");
//const CString VALIDATING_MEDIA ("校验光盘中");
const CString FORMAT_MEDIA ("Formatting Media");
//const CString FORMAT_MEDIA ("格式化光盘中");
const CString INITIALISING_HARWARE ("Initialising Hardware");
//const CString INITIALISING_HARWARE ("初始化设备中");
const CString CALIBRATING_POWER ("Calibrating Power");
//const CString CALIBRATING_POWER (" 功率校准中");
const CString WRITING ("Writing");
//const CString WRITING ("写数据中");
const CString WRITE_FINALIZATION ("Write Finalization");
//const CString WRITE_FINALIZATION (" 写完成");
const CString WRITE_COMPLETED ("Write Completed");
//const CString WRITE_COMPLETED (" 写结束");
const CString WRITE_VERIFYING ("Write Verifying");
//const CString WRITE_VERIFYING ("正在校验写");




