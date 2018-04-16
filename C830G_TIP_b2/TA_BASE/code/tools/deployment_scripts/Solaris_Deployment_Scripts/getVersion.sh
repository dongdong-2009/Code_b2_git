#!/bin/bash

echo "Gathering File Information. Please Wait..."
if [ -d $1 ] 
then
   echo $1 

   if [ -s $2 ]
   then
     echo $2 
   else
     echo "file is not existed..."
     exit 
   fi
else
   echo " folder is not existed "
   exit
fi

BIN_DIR=$1
TEMP_FILE=temp.txt
VERIFICATION_FILE=./Agents-`hostname`.csv

echo "VERIFICATION_FILE=$VERIFICATION_FILE"
echo $1

rm $VERIFICATION_FILE

cd $BIN_DIR
find . -name "*Agent" > $TEMP_FILE



cat $TEMP_FILE | while read LINE
do
#        echo "$LINE"
        :
		$LINE --version >> $VERIFICATION_FILE 2>&1
done

#SystemContoller

./SystemController --version >> $VERIFICATION_FILE 2>&1
ls -ls >> $VERIFICATION_FILE 2>&1

#cheanup
rm $TEMP_FILE

echo "Verification Completed... $VERIFICATION_FILE is created"

