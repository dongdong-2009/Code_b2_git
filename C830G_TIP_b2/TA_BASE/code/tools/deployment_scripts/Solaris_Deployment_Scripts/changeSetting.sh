#!/usr/bin/sh

HOME=${1}
INSTALL_STUBBED_MMS=${2}
LOCALDB=TRA_${3}
SCHEMA=${4}
LOCATION_NAME=${3}


DBCONNECTIONSTRING="${LOCALDB}:${SCHEMA}:${SCHEMA}"
CENTRALDBCONNECTIONSTRING="TRANSACT:${SCHEMA}:${SCHEMA}"
MONITORDBCONNECTIONSTRING="${LOCALDB}:${SCHEMA}:${SCHEMA}"

# transactive, transactive.station
sed "s/db-connection=[^ --]* /db-connection=$DBCONNECTIONSTRING /" ${HOME}/transactive > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive

sed "s/db-connection=[^ --]* /db-connection=$DBCONNECTIONSTRING /" ${HOME}/transactive.station > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive.station

sed "s/central-database=[^ --]* /central-database=$CENTRALDBCONNECTIONSTRING /" ${HOME}/transactive > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive

sed "s/central-database=[^ --]* /central-database=$CENTRALDBCONNECTIONSTRING /" ${HOME}/transactive.station > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive.station

sed "s/monitored-database=[^ --]* /monitored-database=$MONITORDBCONNECTIONSTRING /" ${HOME}/transactive.station > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive.station

CONNECTION_STRING_FILE=`echo $LOCATION_NAME`ConnectionStrings.csv
sed "s/db-connection-file=[^ --]* /db-connection-file=$CONNECTION_STRING_FILE /" ${HOME}/transactive > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive

sed "s/db-connection-file=[^ --]* /db-connection-file=$CONNECTION_STRING_FILE /" ${HOME}/transactive.station > ${HOME}/output 
mv ${HOME}/output ${HOME}/transactive.station

# transactive_system_controller, transactive_system_controller.station
sed "s/db-connection=[^ --]* /db-connection=$DBCONNECTIONSTRING /" ${HOME}/transactive_system_controller > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive_system_controller

sed "s/db-connection=[^ --]* /db-connection=$DBCONNECTIONSTRING /" ${HOME}/transactive_system_controller.station > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive_system_controller.station

sed "s/central-database=[^ --]* /central-database=$CENTRALDBCONNECTIONSTRING /" ${HOME}/transactive_system_controller > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive_system_controller

sed "s/central-database=[^ --]* /central-database=$CENTRALDBCONNECTIONSTRING /" ${HOME}/transactive_system_controller.station > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive_system_controller.station

sed "s/monitored-database=[^ --]* /monitored-database=$MONITORDBCONNECTIONSTRING /" ${HOME}/transactive_system_controller.station > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive_system_controller.station

CONNECTION_STRING_FILE=`echo $LOCATION_NAME`ConnectionStrings.csv
sed "s/db-connection-file=[^ --]* /db-connection-file=$CONNECTION_STRING_FILE /" ${HOME}/transactive_system_controller > ${HOME}/output
mv ${HOME}/output ${HOME}/transactive_system_controller

sed "s/db-connection-file=[^ --]* /db-connection-file=$CONNECTION_STRING_FILE /" ${HOME}/transactive_system_controller.station > ${HOME}/output 
mv ${HOME}/output ${HOME}/transactive_system_controller.station

# MmsAgent
if [ "$INSTALL_STUBBED_MMS" = "" ]  || [ "$INSTALL_STUBBED_MMS" = "Y" ] || [ "$INSTALL_STUBBED_MMS" = "y" ]; then
    mv $HOME/MmsAgent $HOME/MmsAgent.org
    ln -s $HOME/MmsAgent_Stubbed $HOME/MmsAgent
    cp -p $HOME/MmsConfig/Stubbed/* $HOME/. 
else
    mv $HOME/MmsAgent $HOME/MmsAgent.org
    ln -s $HOME/MmsAgent_BEA $HOME/MmsAgent
    cp -p $HOME/MmsConfig/BEA/* $HOME/.
fi
