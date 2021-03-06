#!/bin/bash

test_dir=$(dirname $0)
cd $test_dir

dg_path="../../drumgizmo"

memcheck_log_file="valgrind_memcheck.log"
helgrind_log_file="valgrind_helgrind.log"
dg_log_file="drumgizmo.log"
memcheck_options="--tool=memcheck --log-file=$memcheck_log_file --leak-check=full --show-leak-kinds=all --track-origins=yes"
helgrind_options="--tool=helgrind --log-file=$helgrind_log_file"
midi_file="../../test/midi/file1.mid"
wav_file_prefix="valgrind_test"
midimap="../../test/kit/midimap.xml"
drumkit="../../test/kit/kit1.xml"

# check for drumgizmo
if ! [ -e $dg_path/drumgizmo ]
then
	echo "ERROR: The drumgizmo binary doesn't exist. Maybe you forgot to compile?"
	exit
fi

# check for valgrind
if ! command -v valgrind >/dev/null 2>&1
then
	echo "ERROR: Cannot find valgrind. Maybe it isn't installed?"
	exit
fi

# print nice header
echo "==========================================="
echo "Starting the valgrind test"
echo
echo "Memcheck output file: $memcheck_log_file"
echo "Helgrind output file: $helgrind_log_file"
echo "==========================================="
echo
echo "----------------"
echo "Running memcheck"
echo "----------------"
echo

# run memcheck and print summary
valgrind $memcheck_options $dg_path/./drumgizmo -i midifile -I file=$midi_file,midimap=$midimap -o wavfile -O file=$wav_file_prefix $drumkit > /dev/null
sed -n '/LEAK SUMMARY:/,$p' $memcheck_log_file
echo
echo "=> For details see: $memcheck_log_file"

echo
echo "----------------"
echo "Running helgrind"
echo "----------------"
echo

# run helgrind and print summary
valgrind $helgrind_options $dg_path/./drumgizmo -i midifile -I file=$midi_file,midimap=$midimap -o wavfile -O file=$wav_file_prefix $drumkit > $dg_log_file
sed -n '/ERROR SUMMARY:/,$p' $helgrind_log_file
echo
echo "=> For details see: $helgrind_log_file"

echo

# delete created wav files
rm *.wav
