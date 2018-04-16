PROJ_ROOT_DIR = ../../../../../..
PROJ_CODE_DIR	= transactive/app/radio/RadioTetraAgent/RadioPDSProtocol
PROJ_NAME = TA_RadioPDSProtocol

# The source files
#
PROJ_SOURCE	= IPDSEvent.cpp \
		PDSDatagramClient.cpp \
		PDSDatagramServer.cpp \
		PDSMessageCreator.cpp \
		RadioPDEventReader.cpp \
		RadioPDSEvent.cpp \
		RadioPDSEventBegin.cpp \
		RadioPDSEventFrameRepeat.cpp \
		RadioPDSHandler.cpp \
		RadioPDSManager.cpp \
		RadioPDSMethod.cpp \
		RadioPDSMethodCancel.cpp \
		RadioPDSMethodSendData.cpp \
		RadioPDSSender.cpp \
		UdpSocket.cpp \
		UDPSocketManager.cpp \
		FrameCheckSequence.cpp \
		ProtocolTtisTypes.cpp \
		TtdFrame.cpp \
		TtisTypes.cpp
		

PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

#
# DEPENDENT_LIBRARIES = 
#				$(PROJ_ROOT_DIR)/core/message/IDL \

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
