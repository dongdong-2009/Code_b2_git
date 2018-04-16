#!/usr/bin/sh

#Stop transactive
/etc/init.d/transact stop

#Remove predefined symbolic links.
rm /etc/rc3.d/K01transactive
rm /etc/rc3.d/S10transactive 

rm -f  /u01/transactive/bin/MmsAgent
rm -f  /u01/transactive/bin/Trace.prg /u01/transactive/bin/exitq.txt /u01/transactive/bin/getPendingMsg.txt /u01/transactive/bin/getq.txt /u01/transactive/bin/putq.txt /u01/transactive/bin/address.txt /u01/transactive/bin/dmq.ini
