TA_HOME=/u01/transactive

ORACLE_HOME=/u01/app/oracle/instantclient_11_2
export ORACLE_HOME

PATH=$TA_HOME/bin:$ORACLE_HOME:$PATH
export PATH

LD_LIBRARY_PATH=$ORACLE_HOME:$TA_HOME/lib:/usr/local/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

TNS_ADMIN=/u01/app/oracle/instantclient_11_2
export TNS_ADMIN

export OMNIORB_CONFIG=/u01/transactive/bin/omniORB.cfg

export LANG=C
export TERM=ansi
export SHELL=/bin/bash

