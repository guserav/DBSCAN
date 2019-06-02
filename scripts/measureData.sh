#! /usr/bin/env bash
set -e
program=$1
mode=$2
sta=$3 # Thread count
step=$4
end=$5

runs=$6 # Per thread
task_name=$7
set OMP_DYNAMIC=0

data=(blobs4M circles4M noise4M)
eps=("0.05" "0.05" "0.05")
minPts=(20 5 1)

touch ~/CPU_REQUEST
touch ~/REQUEST_CPU
sleep 10

echo "program=${program} start=${sta} step=${step} end=${end} runs=${runs} task_name=${task_name}" >&2

for i in ${!data[*]}
do
    if [ "$mode" = "single" ]; then
        date >&2
        echo "Doing single ${data[$i]}" >&2
        $program "${task_name}\$${data[$i]}\$0" ";" $runs "data/${data[$i]}.data" 2 ";" "${eps[$i]}" "${minPts[$i]}" 2>&1 > /dev/null
    else
        for thread_count in $(seq $sta $step $end)
        do
            echo "Doing ${data[$i]}\$${thread_count}" >&2
            export OMP_NUM_THREADS=$thread_count
            $program "${task_name}\$${data[$i]}\$${thread_count}" ";" $runs "data/${data[$i]}.data" 2 ";" "${eps[$i]}" "${minPts[$i]}" 2>&1 > /dev/null
        done
    fi
done

date >&2
rm -f ~/CPU_REQUEST ~/REQUEST_CPU
