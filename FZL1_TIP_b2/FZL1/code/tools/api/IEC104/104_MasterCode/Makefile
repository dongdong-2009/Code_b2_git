CC       = g++
CXX      = g++
CPP      = g++
OBJ  = FepDataStruct.o FtpException.o Iec104.o IecSocket.o main.o ProtocolBase.o ReadConfig.o ConnectThread.o Bexception.o Bf.o Object.o Profile.o Threads.o Command.o Adaptee.o Iec104Api.o Iec104Event.o  MyEventHandler.o LogThread.o
LINKOBJ  = FepDataStruct.o FtpException.o Iec104.o IecSocket.o main.o ProtocolBase.o ReadConfig.o ConnectThread.o Bexception.o Bf.o Object.o Profile.o Threads.o Command.o Adaptee.o Iec104Api.o Iec104Event.o  MyEventHandler.o LogThread.o 


CFLAGS   =  -g   -W -O2  -I.  -DSolaris -D_DEBUG 
CXXFLAGS =  -g   -W -O2  -I.  -DSolaris -D_DEBUG
LEXFLAGS = 
YACCFLAGS= -d
INCPATH  = -I. -I. 
LINK     = g++
LFLAGS   = -Wl,+s  -Wl,+b
RM = rm -f
BIN  = iec104client
.PHONY: all clean

all:iec104client

clean:
	${RM} $(OBJ) $(BIN)
	
$(BIN): $(LINKOBJ)
	#g++  $(LINKOBJ) $(LIBS) -lthread -dl -lsocket   -o $(BIN)
	g++  $(LINKOBJ) $(LIBS) -lthread -dl -lsocket  -lnsl -o $(BIN)
	cp iec104client bin/

FepDataStruct.o: FepDataStruct.cpp
	$(CPP) -c FepDataStruct.cpp -o FepDataStruct.o $(CXXFLAGS)

FtpException.o: FtpException.cpp
	$(CPP) -c FtpException.cpp -o FtpException.o $(CXXFLAGS)

Command.o : Command.cpp
	$(CPP) -c Command.cpp -o Command.o $(CXXFLAGS) 

Iec104.o: Iec104.cpp
	$(CPP) -c Iec104.cpp -o Iec104.o $(CXXFLAGS)

IecSocket.o: IecSocket.cpp
	$(CPP) -c IecSocket.cpp -o IecSocket.o $(CXXFLAGS)

Iec104Event.o : Iec104Event.cpp
	$(CPP) -c Iec104Event.cpp -o Iec104Event.o $(CXXFLAGS)

Adaptee.o : Adaptee.cpp
	$(CPP) -c Adaptee.cpp -o Adaptee.o $(CXXFLAGS)

Iec104Api.o: Iec104Api.cpp
	$(CPP) -c Iec104Api.cpp -o Iec104Api.o $(CXXFLAGS)

main.o: linux_main.cpp
	$(CPP) -c linux_main.cpp -o main.o $(CXXFLAGS)

ProtocolBase.o: ProtocolBase.cpp
	$(CPP) -c ProtocolBase.cpp -o ProtocolBase.o $(CXXFLAGS)

ReadConfig.o: ReadConfig.cpp
	$(CPP) -c ReadConfig.cpp -o ReadConfig.o $(CXXFLAGS)
ConnectThread.o: ConnectThread.cpp
	$(CPP) -c ConnectThread.cpp -o ConnectThread.o $(CXXFLAGS)
Bexception.o: Bexception.cpp
	$(CPP) -c Bexception.cpp -o Bexception.o $(CXXFLAGS)
Bf.o: Bf.cpp
	$(CPP) -c Bf.cpp -o Bf.o $(CXXFLAGS)
Object.o: Object.cpp
	$(CPP) -c Object.cpp -o Object.o $(CXXFLAGS)
Profile.o: Profile.cpp
	$(CPP) -c Profile.cpp -o Profile.o $(CXXFLAGS)
Threads.o: Threads.cpp
	$(CPP) -c Threads.cpp -o Threads.o $(CXXFLAGS)
MyEventHandler.o: MyEventHandler.cpp
	$(CPP) -c MyEventHandler.cpp -o MyEventHandler.o $(CXXFLAGS)
LogThread.o: LogThread.cpp
	$(CPP) -c LogThread.cpp -o LogThread.o $(CXXFLAGS)
	
