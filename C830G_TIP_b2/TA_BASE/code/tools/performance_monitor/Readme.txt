// 1. this zip contains:-
      performance_monitor_server
	1) cronjobs.txt -- need use 'root' to add in current cron jobs
	2) agent_mem.sh -- running every 5min to collect cpu/memory information for each agent
	3) alarm_event_cnt.sh -- main script, running daily to collect top 100 alarm and event from OCCDB
	4) cnt_alarm.sql -- sub script to collect alarm data
	5) cnt_event.sql -- sub script to collect event data
      performance_monitor_client

// 2. This performance monitor server scripts need deploy to FAT, Stage Work and Main line
      under /scripts directory
      the output file will be inside /tmp/log, before deploy and install, need make sure
      above directory is availiable, and have 'read and write' access right for both 'root' and 'oracle' user

// 3. agent_mem.sh need deploy in each application server like: occa, occb, sms-dbg, etc.
      alarm_event_cnt only need deploy to occb.

// 4. in main line, need maintenance engr. collect data daily to avoid no enough disk space.

// 5. rm_daily_logs.sh -- will trigger by other script to remove previous log for house keeping. 

// 6. performance monitor client is for maintenance engr. get the log file and do house keeping.
      1) need configure the server.txt for current working servers
      2) then run MyGetDailyLog.bat from MFT.


