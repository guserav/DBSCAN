#! bash

for eps in $(seq $4 $5 $6)
do
    for minPts in $(seq $7 $8 $9)
    do
        echo "Now doing $eps : $minPts"
        build/StudienarbeitCode $1 $2 $3 $eps $minPts | python3 scripts/visualizeCluster.py -o "scripts/out/plot${eps}_$minPts.png" --dpi 150
    done
    convert scripts/out/plot${eps}_*.png +append scripts/out/line$eps.png
done
convert scripts/out/line*.png -append scripts/out/output.png
