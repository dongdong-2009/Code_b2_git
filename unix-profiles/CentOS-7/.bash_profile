# .bash_profile

# Get the aliases and functions
if [ -f ~/.bashrc ]; then
	. ~/.bashrc
fi

# User specific environment and startup programs

PATH=$PATH:$HOME/bin

export PATH

###########
#
# LIMIN
#
###########
export PATH=/home/local/bin:$PATH
export LD_LIBRARY_PATH=/lib:/usr/lib:/home/local/lib

###########
#
# CORE-DUMP
#
###########
ulimit -n 65535
ulimit -s unlimited # TODO
ulimit -c unlimited
sysctl kernel.core_pattern=/u01/transactive/core_files/core.%e-%p >/dev/null

###########
#
# TRANSACTIVE
#
###########
export PATH=/u01/transactive:/u01/transactive/bin:$PATH
export LD_LIBRARY_PATH=/u01/transactive/lib:$LD_LIBRARY_PATH

###########
#
# P4
#
###########
export P4USER=limin.zhu
export P4PASSWD=Coreteam
export P4CLIENT=limin.zhu_CentOS-7_verify
export P4PORT=192.168.253.15:1666

###########
#
# ALIAS
#
###########
alias ls='ls -lrt --color=auto'
alias start='transactive start'
alias START='start'
alias stop='watson kill'
alias STOP='stop'
alias rm='rm -f'
alias PWD='pwd'

###########
#
# ORACLE
#
###########
export ORACLE_HOME=/u01/app/oracle/instantclient_11_2
export TNS_ADMIN=$ORACLE_HOME
export NLS_LANG="SIMPLIFIED CHINESE_CHINA.ZHS16GBK"
export LD_LIBRARY_PATH=$ORACLE_HOME:$LD_LIBRARY_PATH
export PATH=$ORACLE_HOME:$PATH





