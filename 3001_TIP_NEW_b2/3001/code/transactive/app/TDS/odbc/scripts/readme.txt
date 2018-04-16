(TDS is using OBDC 3 version to connect database)
1. Install MySQL ODBC driver, from https://dev.mysql.com/downloads/connector/odbc/   (Windows, 32bit)
2. Install ORACLE ODBC driver, from http://www.oracle.com/technetwork/database/windows/downloads/utilsoft-098155.html .

3. modify config file: loadDBConfig.txt
	Username, Password and Servicename should match the ODBC configuratoin. Servicename is data source name in ODBC

4. run configEditer.exe to load data from database with config file: loadDBConfig.txt
	just running, no parameters;
	System environment needs: office 2007 and oracle client have installed.

5. using excel config xxxx.xls file. select system, then location, .....
	enable Macros of execl options before any operation. excel options-->trust center-->macro settings-->enable all macros.

6. run plcSimulator.exe, then execute xxxx.xls macros to send command to plcsimulator.

7. if entity > 65535, using xxxx.xlsm file. plcsimulator version >= 1.0.1.2.
