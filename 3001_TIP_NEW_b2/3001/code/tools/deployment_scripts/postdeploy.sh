#!/usr/bin/sh

chown transactive:scada /u01/transactive

chmod +s /u01/transactive/bin/SystemStatusAgent
chmod +s /u01/transactive/bin/SystemController
chmod +s /u01/transactive/bin/RadioTetraAgent

chown transactive:scada /u01/transactive/bin/transactive
chown transactive:scada /u01/transactive/bin/transactive.station

chmod 755 /u01/transactive/bin/transactive
chmod 755 /u01/transactive/bin/transactive.station

chown root:other /u01/transactive/bin/RadioTetraAgent
chown root:other /u01/transactive/bin/SystemStatusAgent
