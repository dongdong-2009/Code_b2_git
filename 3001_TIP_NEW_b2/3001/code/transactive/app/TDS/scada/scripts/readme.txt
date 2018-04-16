
1, modify config file: loadDBConfig.txt
	Username, Password and Servicename should match the contents of oracle file:tnsnames.ora

2, run configEditer.exe to load data from database with config file: loadDBConfig.txt
	just running, no parameters;
	System environment needs: office 2007 and oracle client have installed.

3, using excel config xxxx.xls file. select system, then location, .....
	enable Macros of execl options before any operation. excel options-->trust center-->macro settings-->enable all macros.

4, run plcSimulator.exe, then execute xxxx.xls macros to send command to plcsimulator.

5, if entity > 65535, using xxxx.xlsm file. plcsimulator version >= 1.0.1.2.
