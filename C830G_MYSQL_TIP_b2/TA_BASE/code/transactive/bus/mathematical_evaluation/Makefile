# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $
#
# Make file for the mathematical_evaluation classes.
#
#
PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/bus/mathematical_evaluation
PROJ_NAME	= TA_MathEvaluation
PROJ_SOURCE	= AndExpression.cpp \
		  BooleanConstant.cpp \
		  BooleanVariable.cpp \
		  ExpressionEvaluator.cpp \
		  OrExpression.cpp \
		  NotExpression.cpp \
		  RealNumberSquareRootExpression.cpp \
		  RealNumberAbsoluteExpression.cpp \
		  RealNumberAddExpression.cpp \
		  RealNumberConstant.cpp \
		  RealNumberDivideExpression.cpp \
		  EqualityComparison.cpp \
		  RealNumberGreaterThanComparison.cpp \
		  RealNumberGreaterThanOrEqualComparison.cpp \
		  InequalityComparison.cpp \
		  RealNumberLessThanComparison.cpp \
		  RealNumberLessThanOrEqualComparison.cpp \
		  RealNumberMultiplyExpression.cpp \
		  RealNumberPowerOfExpression.cpp \
		  RealNumberSubtractExpression.cpp \
		  RealNumberVariable.cpp \
		  StringLiteral.cpp \
		  StringVariable.cpp \
		  Token.cpp \
		  Tokeniser.cpp \
		  XorExpression.cpp \
		  ContextContainer.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES =   $(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
