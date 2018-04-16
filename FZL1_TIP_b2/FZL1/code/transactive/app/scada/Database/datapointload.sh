#!/bin/ksh
##
## Description	: Script to load datapoint data into the database
## Created 	: Matt Chalwell
## Date		: 14th April 1968
##
#
for j in Station StationSystem StationSubSystem Equipment DataPoint
do
  for i in 1 2 3 4
  do
    echo "Processing file ${j}${i}.csv"
    my_output=`sqlplus -s its/its@trans1 << END
    set heading off
    set feedback off
    set serveroutput on size 20000
    exec datapointload.read_file('/u01/app/oracle/dataload','${j}${i}.csv');  
END`
    entries=`cat ${j}${i}.csv | wc -l`
    entries=`expr $entries - 1`
    duplicates=`echo ${my_output} | grep "dup" | wc -l`
    echo "Total Entries is ${entries}"
    if [ ${duplicates} -ne "0" ]
    then
    echo "Duplicates Found."
    fi
  done
done
