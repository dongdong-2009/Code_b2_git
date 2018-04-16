#!/usr/bin/perl

system("chown transactive:scada /u01/transactive");
system("chown transactive:scada /u01/transactive/bin");

system("chown transactive:scada /u01/transactive/bin/*");
system("chown transactive:scada /u01/transactive/bin/*.*");

system("chmod +s /u01/transactive/bin/SystemStatusAgent");
system("chmod +s /u01/transactive/bin/SystemController");

if (-f "/u01/transactive/bin/transactive") {
	system("chown transactive:scada /u01/transactive/bin/transactive");
	system("chmod 755 /u01/transactive/bin/transactive");
}
system("chown transactive:scada /u01/transactive/bin/transactive.station");

system("chmod 755 /u01/transactive/bin/transactive.station");

system("chmod ugo+x /u01/transactive/bin/*Agent");

if (-f "/u01/transactive/bin/RadioTetraAgent") {
	system("chmod +s /u01/transactive/bin/RadioTetraAgent");
	system("chown root:other /u01/transactive/bin/RadioTetraAgent");
}

system("chown root:other /u01/transactive/bin/SystemStatusAgent");
system("chown root:other /u01/transactive/bin/SystemController");
