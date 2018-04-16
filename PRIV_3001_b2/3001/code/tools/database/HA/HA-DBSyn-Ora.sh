#!/bin/ksh
i=0
g_datasyn_synid=1
g_datasyn_status="null"
g_datasyn_exe_path=/u01/app/datasyn/bin/SynchEngine
g_datasyn_config_file_path=/u01/app/datasyn/bin/s_cfg.cnf


  # notify SynchEngine the DB current status
 notify_datasyn()
 {	
	# check the pass in mode
	case $1 in
		'control') 
		v_proc_count=`ps -ef | grep SynchEngine | grep -v grep | awk '{print $2}' |wc -l`
		if [ $v_proc_count -eq 0 ]; then			
			$g_datasyn_exe_path --default-file=$g_datasyn_config_file_path --mode=stop --SynID=$g_datasyn_synid 
			$g_datasyn_exe_path --default-file=$g_datasyn_config_file_path --mode=control --SynID=$g_datasyn_synid &	
			echo "datasyn_status:"$g_datasyn_status			
		fi		
		;;		
		'stop')
		v_proc_count=`ps -ef | grep SynchEngine | grep -v grep | awk '{print $2}' |wc -l`
		if [ $v_proc_count -eq 0 ]; then
			echo "there is no SynchEngine running..."
		else			
			$g_datasyn_exe_path --default-file=$g_datasyn_config_file_path --mode=stop --SynID=$g_datasyn_synid 
			echo "stop datasyn..."
		fi 
		;;		
		*)
		echo "Usage: start_datasyn  {control|monitor|stop}"
		exit 1
		;;
	esac
 }


 main()
 {
##########################################
# Main
##########################################
notify_datasyn "control"
}

##########################################
#Loop Begin
##########################################
while :; do
main
i=$((i+1))
echo $i
sleep 1
done