#!/usr/bin/ksh
ORACLE_HOME=/u01/app/oracle/OraHome920;export ORACLE_HOME
PATH=$PATH:/u01/app/oracle/OraHome920/bin;export PATH
USER=$1
SID=$2
echo $PATH
cd /scripts
sqlplus /nolog @cnt_alarm.sql $USER $SID>/dev/null
sqlplus /nolog @cnt_event.sql $USER $SID>/dev/null
