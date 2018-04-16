ITS_HOME=/u01/transactive
export ITS_HOME

ORACLE_HOME=/u01/app/oracle/product/11.2.0/dbhome_1
export ORACLE_HOME

PATH=$PATH:/usr/local/bin:/etc:$ITS_HOME/bin:$ORACLE_HOME/bin
export PATH

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ITS_HOME/lib:$ORACLE_HOME/lib:$ORACLE_HOME/network/lib:/usr/local/lib:/usr/lib
export LD_LIBRARY_PATH

TA_HOME=/u01/transactive
export TA_HOME
coreadm -p $TA_HOME/core_files/%n.%f.%p $$

export NLS_LANG="SIMPLIFIED CHINESE_CHINA.ZHS32GB18030"
export LANG=C
export LC_ALL=C

