
#ifndef _LOG_INFO_H_
#define _LOG_INFO_H_

namespace TA_IRS_App
{
	class LogInfo
	{
	public:
		LogInfo(HWND hndle);
		~LogInfo();

		static LogInfo* getInstance();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		LogInfo (const LogInfo&);
		LogInfo& operator= ( const LogInfo&);
		static LogInfo* m_hInstance;
	};
}
#endif