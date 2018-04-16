PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/app/response_plans/plan_step_plugin/common
PROJ_NAME	= TA_PlanStepCommon
PROJ_SOURCE	= \
	BaseStepParameter.cpp \
	BasicCompareRule.cpp \
	BasicParameter.cpp \
	BoolParamWrapper.cpp \
	ContainerCompareRule.cpp \
	GridParamWrapper.cpp \
	InteractiveParameter.cpp \
	ListParamWrapper.cpp \
	SerializableParameter.cpp \
	StringParamWrapper.cpp \
	TreeParamWrapper.cpp \
	VectorParamWrapper.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/maintenance_management/mmsCorbaDef/idl \
	$(PROJ_ROOT_DIR)/code/transactive/bus/cctv/video_switch_agent/IDL \
				
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
