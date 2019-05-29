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

data=(blobs2M circles2M noise2M)
eps=("0.1" "0.1" "0.2")
minPts=(5 5 2)

touch ~/CPU_REQUEST
touch ~/REQUEST_CPU
sleep 10

echo "program=${program} start=${sta} step=${step} end=${end} runs=${runs} task_name=${task_name}" >&2

for i in ${!data[*]}
do
    if [ "$mode" = "single" ]; then
        echo "Doing single ${data[$i]}" >&2
        $program "${task_name}\$${data[$i]}\$0" ";" $runs "data/${data[$i]}.data" 2 ";" "${eps[$i]}" "${minPts[$i]}" 2>&1 > /dev/null
    else
        for thread_count in $(seq $sta $end $step)
        do
            echo "Doing ${data[$i]}\$${thread_count}" >&2
            set OMP_NUM_THREADS=$thread_count
            $program "${task_name}\$${data[$i]}\$${thread_count}" ";" $runs "data/${data[$i]}.data" 2 ";" "${eps[$i]}" "${minPts[$i]}" 2>&1 > /dev/null
        done
    fi
done

rm -f ~/CPU_REQUEST ~/REQUEST_CPU
