#! bash
set start=$1 # Thread count
set step=$2
set end=$3

set runs=$4 # Per thread
set task_name=$5
set OMP_DYNAMIC=0

for thread_count in $(seq $start $end $step)
do
    set OMP_NUM_THREADS=$thread_count
    ./StudienarbeitCode $runs "$task_name$tread_count" > /dev/null
done
