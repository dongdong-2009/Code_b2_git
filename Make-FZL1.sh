#!/bin/sh

FZL1=/build/code/FZL1_TIP/TA_BASE/code
COMMON=/build/code/TA_Common_V1_TIP/code

ln -sf /build/perforce/COTS													$FZL1/cots
ln -sf $COMMON/tools														$FZL1/tools
ln -sf $COMMON/transactive/bus												$FZL1/transactive/bus
ln -sf $COMMON/transactive/core												$FZL1/transactive/core
ln -sf $COMMON/transactive/app/alarm/alarm_agent							$FZL1/transactive/app/alarm/alarm_agent
ln -sf $COMMON/transactive/app/alarm/alarm_raiser							$FZL1/transactive/app/alarm/alarm_raiser
ln -sf $COMMON/transactive/app/alarm/alarm_store_service					$FZL1/transactive/app/alarm/alarm_store_service
ln -sf $COMMON/transactive/app/alarm/AlarmSharedMapReader					$FZL1/transactive/app/alarm/AlarmSharedMapReader
ln -sf $COMMON/transactive/app/alarm/AlarmStoreHarness						$FZL1/transactive/app/alarm/AlarmStoreHarness
ln -sf $COMMON/transactive/app/alarm/ATSInterface							$FZL1/transactive/app/alarm/ATSInterface
ln -sf $COMMON/transactive/app/checker										$FZL1/transactive/app/checker
ln -sf $COMMON/transactive/app/datalog										$FZL1/transactive/app/datalog
ln -sf $COMMON/transactive/app/dbsynch										$FZL1/transactive/app/dbsynch
ln -sf $COMMON/transactive/app/event/ATSBridgeUtil							$FZL1/transactive/app/event/ATSBridgeUtil
ln -sf $COMMON/transactive/app/event/event_agent							$FZL1/transactive/app/event/event_agent
ln -sf $COMMON/transactive/app/event/event_extractor						$FZL1/transactive/app/event/event_extractor
ln -sf $COMMON/transactive/app/event/EventGenerator							$FZL1/transactive/app/event/EventGenerator
ln -sf $COMMON/transactive/app/event/EventViewerSimulator					$FZL1/transactive/app/event/EventViewerSimulator
ln -sf $COMMON/transactive/app/event/online_printing_agent					$FZL1/transactive/app/event/online_printing_agent
ln -sf $COMMON/transactive/app/notification_service							$FZL1/transactive/app/notification_service
ln -sf $COMMON/transactive/app/response_plans/plan_agent					$FZL1/transactive/app/response_plans/plan_agent
ln -sf $COMMON/transactive/app/response_plans/plan_agent_test				$FZL1/transactive/app/response_plans/plan_agent_test
ln -sf $COMMON/transactive/app/response_plans/plan_mft_processor			$FZL1/transactive/app/response_plans/plan_mft_processor
ln -sf $COMMON/transactive/app/response_plans/plan_step_plugin				$FZL1/transactive/app/response_plans/plan_step_plugin
ln -sf $COMMON/transactive/app/scada/DataNodeAgent							$FZL1/transactive/app/scada/DataNodeAgent
ln -sf $COMMON/transactive/app/scada/DataNodeController						$FZL1/transactive/app/scada/DataNodeController
ln -sf $COMMON/transactive/app/scada/ModbusAgent							$FZL1/transactive/app/scada/ModbusAgent
ln -sf $COMMON/transactive/app/scada/ModbusServerAgent						$FZL1/transactive/app/scada/ModbusServerAgent
ln -sf $COMMON/transactive/app/scada/OpcBridgeLib							$FZL1/transactive/app/scada/OpcBridgeLib
ln -sf $COMMON/transactive/app/scada/OpcBridgeService						$FZL1/transactive/app/scada/OpcBridgeService
ln -sf $COMMON/transactive/app/scada/PMSAgent								$FZL1/transactive/app/scada/PMSAgent
ln -sf $COMMON/transactive/app/scada/PowerDemandEditor						$FZL1/transactive/app/scada/PowerDemandEditor
ln -sf $COMMON/transactive/app/scada/PScadaAgent							$FZL1/transactive/app/scada/PScadaAgent
ln -sf $COMMON/transactive/app/scada/SNMPStatusAgent						$FZL1/transactive/app/scada/SNMPStatusAgent
ln -sf $COMMON/transactive/app/scada/VirtualDataPointAgent					$FZL1/transactive/app/scada/VirtualDataPointAgent
ln -sf $COMMON/transactive/app/scheduling/scheduling_agent					$FZL1/transactive/app/scheduling/scheduling_agent
ln -sf $COMMON/transactive/app/security/authentication_agent				$FZL1/transactive/app/security/authentication_agent
ln -sf $COMMON/transactive/app/security/duty_agent							$FZL1/transactive/app/security/duty_agent
ln -sf $COMMON/transactive/app/security/rights_agent						$FZL1/transactive/app/security/rights_agent
ln -sf $COMMON/transactive/app/security/rights_service						$FZL1/transactive/app/security/rights_service
ln -sf $COMMON/transactive/app/simulator									$FZL1/transactive/app/simulator
ln -sf $COMMON/transactive/app/sound										$FZL1/transactive/app/sound
ln -sf $COMMON/transactive/app/system_control/dll_host						$FZL1/transactive/app/system_control/dll_host
ln -sf $COMMON/transactive/app/system_control/dummy_agent					$FZL1/transactive/app/system_control/dummy_agent
ln -sf $COMMON/transactive/app/system_control/system_controller				$FZL1/transactive/app/system_control/system_controller
ln -sf $COMMON/transactive/app/system_control/system_status_agent			$FZL1/transactive/app/system_control/system_status_agent
ln -sf $COMMON/transactive/app/system_control/transact_screensaver			$FZL1/transactive/app/system_control/transact_screensaver
ln -sf $COMMON/transactive/app/train_timetable								$FZL1/transactive/app/train_timetable
ln -sf $COMMON/transactive/app/trending										$FZL1/transactive/app/trending
ln -sf $COMMON/transactive/app/WebPortal									$FZL1/transactive/app/WebPortal
