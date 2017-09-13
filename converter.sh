#! /usr/bin/env bash
## USE THIS ON A MAC!

for pdff in figs/*pdf ;
do
    # if [ -f figs/$f.pdf ]; then
    # 	pdff=figs/$f.pdf
    # echo $pdff
	
    out=`echo $pdff |sed s/pdf/png/g`
    echo Converting' ' $pdff 
    echo to'         ' $out
    sips -s format png $pdff --out $out > /dev/null
    # else
    # 	echo Could not find $pdff
    # fi
    echo ===
done






