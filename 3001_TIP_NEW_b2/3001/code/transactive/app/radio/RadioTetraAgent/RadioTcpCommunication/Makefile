PROJ_ROOT_DIR = ../../../../../..
PROJ_CODE_DIR	= transactive/app/radio/RadioTetraAgent/RadioTcpCommunication
PROJ_NAME = TA_RadioTcpCommunication

# The source files
#
PROJ_SOURCE	= TcpTransactionManager.cpp \
		TcpMethodTimeoutException.cpp \
		TcpException.cpp \
		TcpConnectionManager.cpp \
		TcpConnectionException.cpp \
		TCPConnection.cpp \
		ObservedSocket.cpp \
		BaseTcpServer.cpp 

PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

#
# DEPENDENT_LIBRARIES = 
#				$(PROJ_ROOT_DIR)/core/message/IDL \

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
