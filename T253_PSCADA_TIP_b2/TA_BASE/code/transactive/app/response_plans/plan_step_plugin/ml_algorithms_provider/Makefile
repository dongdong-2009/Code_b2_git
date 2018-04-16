# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/ml_algorithms_provider/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2017/10/31 16:21:54 $
# Last modified by:  $Author: CM $
#
# Make file for the event triggered plan classes.
#
PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/app/response_plans/plan_step_plugin/ml_algorithms_provider
PROJ_NAME	= TA_ml_algorithms_provider
PROJ_SOURCE	= DataFormatHelper.cpp \
			KMeansHelper.cpp \
			MLAlgorithmsImpl.cpp \
			NCAHelper.cpp \
			TA_MLDataTypes.cpp
PROJ_TYPE	= dlib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive \
				$(PROJ_ROOT_DIR)/code/cots/MLL/mlpack/deps/OpenBLAS-0.2.20/build/include \
				$(PROJ_ROOT_DIR)/code/cots/MLL/mlpack/deps/armadillo-8.200.1/include \
				$(PROJ_ROOT_DIR)/code/cots/MLL/mlpack/mlpack-2.2.5/build/include
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     =\
		TA_Utilities \
		TA_Version \
		mlpack \
		openblas 

# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES = \
					$(PROJ_ROOT_DIR)/code/transactive/core/utilities \
					$(PROJ_ROOT_DIR)/code/transactive/core/versioning

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak
STD_LIB_DIRS += $(PROJ_ROOT_DIR)/code/cots/MLL/mlpack/mlpack-2.2.5/build/lib/$(OMNI_OS_IDENTIFIER)  \
				$(PROJ_ROOT_DIR)/code/cots/MLL/mlpack/deps/OpenBLAS-0.2.20/build/lib/$(OMNI_OS_IDENTIFIER)

install:
        #Do Nothing
