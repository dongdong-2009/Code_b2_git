@ECHO OFF

set VERSION=T01271382
set USERNAME=transactive
set PASSWORD=tra

set SERVER=./servers.txt

mkdir my_daily_logs

for /F "eol=# tokens=1 delims=, " %%i in (%SERVER%) do @(
    echo "Get Log List on %%i"

	call MyGetDailyLogPerServer.bat tra %%i
	.\plink.exe %USERNAME%@%%i -pw %PASSWORD% "/scripts/rm_daily_logs.sh"
)

echo "Get Log list Completed..."

pause