#!/usr/bin/sh

echo "Installing transActive\n\n"

echo "Please input the database connection string:"
read DBCONNECTIONSTRING

echo "\nDo you wish to install the Stubbed MMS Agent?:(Y)es"
read INSTALL_STUBBED_MMS

echo "\nPlease input the 3 letters (uppercase) location name: (e.g. OCC,DCC,KQL)"
read LOCATION_NAME

echo "Please input the central database connection string:"
read CENTRALDBCONNECTIONSTRING

echo "Please input the monitor database connection string:"
read MONITORDBCONNECTIONSTRING

cat >$1 <<EOB
DBCONNECTIONSTRING=$DBCONNECTIONSTRING
INSTALL_STUBBED_MMS=$INSTALL_STUBBED_MMS
LOCATION_NAME=$LOCATION_NAME
CENTRALDBCONNECTIONSTRING=$CENTRALDBCONNECTIONSTRING
MONITORDBCONNECTIONSTRING=$MONITORDBCONNECTIONSTRING
EOB
