#!/usr/bin/sh

chmod +s /u01/transactive/bin/SystemStatusAgent
chmod +s /u01/transactive/bin/SystemController
chmod +s /u01/transactive/bin/RadioTetraAgent

ln -s /etc/init.d/transact /etc/rc3.d/K01transactive
ln -s /etc/init.d/transact /etc/rc3.d/S10transactive

mkdir /u01/transactive/log
chown transactive:scada /u01/transactive/log
mkdir /u01/transactive/core_files
chown transactive:scada /u01/transactive/core_files
mkdir /u01/transactive/core_dumps
chown transactive:scada /u01/transactive/core_dumps

sed "s/db-connection=[^ --]* /db-connection=$DBCONNECTIONSTRING /" /u01/transactive/bin/transactive > /u01/transactive/bin/output
mv /u01/transactive/bin/output /u01/transactive/bin/transactive

sed "s/db-connection=[^ --]* /db-connection=$DBCONNECTIONSTRING /" /u01/transactive/bin/transactive.station > /u01/transactive/bin/output
mv /u01/transactive/bin/output /u01/transactive/bin/transactive.station

sed "s/central-database=[^ --]* /central-database=$CENTRALDBCONNECTIONSTRING /" /u01/transactive/bin/transactive > /u01/transactive/bin/output
mv /u01/transactive/bin/output /u01/transactive/bin/transactive

sed "s/central-database=[^ --]* /central-database=$CENTRALDBCONNECTIONSTRING /" /u01/transactive/bin/transactive.station > /u01/transactive/bin/output
mv /u01/transactive/bin/output /u01/transactive/bin/transactive.station

sed "s/monitored-database=[^ --]* /monitored-database=$MONITORDBCONNECTIONSTRING /" /u01/transactive/bin/transactive.station > /u01/transactive/bin/output
mv /u01/transactive/bin/output /u01/transactive/bin/transactive.station

# grace++ to get the correct db-connection-file
CONNECTION_STRING_FILE=`echo $LOCATION_NAME`ConnectionStrings.csv
sed "s/db-connection-file=[^ --]* /db-connection-file=$CONNECTION_STRING_FILE /" /u01/transactive/bin/transactive > /u01/transactive/bin/output
mv /u01/transactive/bin/output /u01/transactive/bin/transactive

sed "s/db-connection-file=[^ --]* /db-connection-file=$CONNECTION_STRING_FILE /" /u01/transactive/bin/transactive.station > /u01/transactive/bin/output 
mv /u01/transactive/bin/output /u01/transactive/bin/transactive.station

# temporery remove till multi-db is ready (for TD 14551 and TD 14552)
# grace++ (Currently only doing for 4 station at FAT test platform)
# need to update all ConnectionString.csv file to have the correct database connection string
# NEWDBCONNECTIONSTRING==`echo $DBCONNECTIONSTRING | sed "s/:/,/g"`

# sed "s/instance,userid,password/$NEWDBCONNECTIONSTRING/g" /u01/transactive/bin/OCCConnectionStrings.csv > /u01/transactive/bin/output
# mv /u01/transactive/bin/output /u01/transactive/bin/OCCConnectionStrings.csv 
# sed "s/instance,userid,password/$NEWDBCONNECTIONSTRING/g" /u01/transactive/bin/DBGConnectionStrings.csv > /u01/transactive/bin/output
# mv /u01/transactive/bin/output /u01/transactive/bin/DBGConnectionStrings.csv 
# sed "s/instance,userid,password/$NEWDBCONNECTIONSTRING/g" /u01/transactive/bin/KDCConnectionStrings.csv > /u01/transactive/bin/output
# mv /u01/transactive/bin/output /u01/transactive/bin/KDCConnectionStrings.csv 
# sed "s/instance,userid,password/$NEWDBCONNECTIONSTRING/g" /u01/transactive/bin/MSMConnectionStrings.csv > /u01/transactive/bin/output
# mv /u01/transactive/bin/output /u01/transactive/bin/MSMConnectionStrings.csv 
# sed "s/instance,userid,password/$NEWDBCONNECTIONSTRING/g" /u01/transactive/bin/CVCConnectionStrings.csv > /u01/transactive/bin/output
# mv /u01/transactive/bin/output /u01/transactive/bin/CVCConnectionStrings.csv 
#++grace

chown transactive:scada /u01/transactive/bin/transactive
chown transactive:scada /u01/transactive/bin/transactive.station

chmod 0755 /u01/transactive/bin/transactive
chmod 0755 /u01/transactive/bin/transactive.station
 
crle -u -l /u01/transactive/lib/
chown root:other /u01/transactive/bin/RadioTetraAgent
