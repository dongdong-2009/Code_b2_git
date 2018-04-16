PROJ_ROOT_DIR = ../../../../../..
PROJ_CODE_DIR	= transactive/app/radio/RadioTetraAgent/RadioMethodProtocol
PROJ_NAME = TA_RadioMethodProtocol

# The source files
#
PROJ_SOURCE	= RadioMethodSubscriberMonitor.cpp \
		RadioMethodQueryCallForwarding.cpp \
		RadioMethodProfileRequest.cpp \
		RadioMethodDynamicGroup.cpp \
		RadioMethodCancelCallForwarding.cpp \
		RadioMethodCallForwardRequest.cpp \
		RadioMethodSystemStatus.cpp \
		RadioMethodProfileUpdate.cpp \
		RadioMethod.cpp \
		RadioEventSubscriberMonitor.cpp \
		RadioEventReader.cpp \
		RadioEventQueryCallForwarding.cpp \
		RadioEventProfileRequest.cpp \
		RadioEventDynamicGroup.cpp \
		RadioEventCancelCallForwarding.cpp \
		RadioEventCallFowardRequest.cpp \
		RadioEventSystemStatus.cpp \
		RadioEventProfileUpdate.cpp \
		RadioEvent.cpp \
		ITcpMethod.cpp \
		ITcpEvent.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

#
# DEPENDENT_LIBRARIES = 
#				$(PROJ_ROOT_DIR)/core/message/IDL \

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing