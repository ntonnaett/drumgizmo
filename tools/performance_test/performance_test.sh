#!/bin/bash
# USAGE: ./performance_test <kit> <midimap> <midifile>
# This script needs JACK to be started or just starts it itself if it isn't.

test_dir=$(dirname $0)
cd $test_dir

dg_path="../../drumgizmo"
sample_interval=.5
cpu_plot_file="cpu_plot"
ram_plot_file="ram_plot"
cpu_data_file="cpu_data.dat"
ram_data_file="ram_data.dat"
dg_log_file="drumgizmo.log"

# check for right number of parameters
if [[ $# != 3 ]]
then
	echo "ERROR: You didn't supply the right number of parameters."
	echo "USAGE: ./performance_test <kit> <midimap> <midifile>"
	echo "Exiting..."
	exit
fi

kit="$1"
midimap="$2"
midifile="$3"

# function to check for dependencies
function check_for_deps
{
	for dep in "$@"
	do
		if ! command -v $dep >/dev/null 2>&1
		then
			echo "ERROR: Cannot find ${dep}. Maybe it isn't installed?"
			echo "Exiting..."
			exit
		fi
	done
}

# check for drumgizmo
if ! [ -e $dg_path/drumgizmo ]
then
	echo "ERROR: The drumgizmo binary doesn't exist. Maybe you forgot to compile?"
	echo "Exiting..."
	exit
fi

# check for dependencies
check_for_deps gnuplot top awk

# check for the existence of the passed files
if ! [ -e "$1" ] || ! [ -e "$2" ] || ! [ -e "$3" ]
then
	echo "ERROR: One of the files (kit/midimap/midfile) doesn't exist."
	echo "Exiting..."
	exit
fi

echo "============================"
echo "Starting the performace test"
echo "============================"
echo

# initial data values
cpu_data=""
ram_data=""
avg_cpu_usage=""
time_elapsed=0

# log ram and cpu usage of process
function logData
{
	# returns the (zero-indexed) position of $word in $line.
	# returns -1 if $word isn't contained in $line.
	function get_position
	{
		word="$1"
		line=($2)

		count=0
		for w in "${line[@]}"
		do
			if [ "$w" == "$word" ]
			then
				echo "$count"
				return
			fi

			count=$(($count+1))
		done

		echo "-1"
	}

	pid=$1

	top_output_labels="$(top -b -n 1 -p $pid | grep "^[ ]*PID")"
	top_output_values="$(top -b -n 1 -p $pid | grep "^[ ]*$pid")"

	if [ "$top_output_values" != "" ]
	then
		top_arr=($top_output_values)

		cpu_pos=$(get_position "%CPU" "$top_output_labels")
		ram_pos=$(get_position "%MEM" "$top_output_labels")

		if [ "$cpu_pos" == "-1" ] || [ "$ram_pos" == "-1" ]
		then
			echo "The top output doesn't look like expected."
			echo "Exiting..."
			exit
		fi

		cpu_usage="${top_arr[$cpu_pos]}"
		ram_usage="${top_arr[$ram_pos]}"

		cpu_data="${cpu_data}${time_elapsed} ${cpu_usage}\n"
		ram_data="${ram_data}${time_elapsed} ${ram_usage}\n"
	fi

	avg_cpu_usage_candidate="$(ps -p $pid -o %cpu | grep "^[ ]*[0-9][0-9]*")"
	# to make sure the process was still running when we executed the last command
	if [ avg_cpu_usage_candidate != "" ]; then avg_cpu_usage=$avg_cpu_usage_candidate; fi

	# hack to allow for float addition in the bash
	time_elapsed="$(awk "BEGIN {print $time_elapsed+$sample_interval; exit}")"
}

# gnuplot ram and cpu usage over time 
function plotData
{
	echo -e "$cpu_data" > $cpu_data_file
	echo -e "$ram_data" > $ram_data_file
	gnuplot performance_test.gnuplot > /dev/null 2>&1
}

# start dg
echo "The terminal output of drumgizmo is routed to ${dg_log_file}"
$dg_path/./drumgizmo -i midifile -I file="$midifile",midimap="$midimap" -o jackaudio "$kit" > $dg_log_file 2>&1 &
pid=$!

# collect data while dg is running
echo "Collecting the data now. That might take a while..."
while ps -p $pid > /dev/null
do
	logData $pid
	sleep $sample_interval
done

# plot data
plotData

echo
echo "-------------------------------------------------------------------"
echo "The average CPU usage was: ${avg_cpu_usage}%"
echo "See the data files $cpu_data_file and $ram_data_file for details."
echo "They are nicely plotted in cpu_data.png and ram_data.png."
echo "-------------------------------------------------------------------"
echo

echo "============================"
echo "Finished the performace test"
echo "============================"
