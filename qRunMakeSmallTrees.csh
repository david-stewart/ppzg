#!/usr/bin/env csh

# make sure executable exists
set ExecPath = `pwd`
set Exec = "bin/MakeSmallerTrees"

# make sure executable exists
make $Exec || exit

set ResultDir   = Data/SmallAuAu14
#set ResultDir   = Data/Small_LowMid_Y14HT

set OutBase     = Small
set TriggerName = HT2HT3

set noglob

set submitted=0

# set rerunlist = Rerun_${OutBase}_nolumi.txt
# rm -f $rerunlist
# foreach Files ( `find Data/P17id/AuAu_200_production_2014/HT/ -maxdepth 1 -name "*root" ` )

# set rerunlist = Rerun_${OutBase}_high.txt
# rm -f $rerunlist
# foreach Files ( `find Data/P17id/AuAu_200_production_high_2014/HT/ -maxdepth 1 -name "*root" ` )

# set rerunlist = Rerun_${OutBase}_mid.txt
# rm -f $rerunlist
# foreach Files ( `find Data/P17id/AuAu_200_production_mid_2014/HT/ -maxdepth 1 -name "*root" ` )

set rerunlist = Rerun_${OutBase}_low.txt
rm -f $rerunlist
foreach Files ( `find Data/P17id/AuAu_200_production_low_2014/HT/ -maxdepth 1 -name "*root" ` )
    
    set FileBase    = `basename $Files`
    
    set OutName     = ${ResultDir}/${OutBase}_${FileBase}

    set LogFile     = logs/MakeSmallerTrees_${OutBase}_${FileBase}.out
    set ErrFile     = logs/MakeSmallerTrees_${OutBase}_${FileBase}.err
        
    set Args = ( $Files $OutName )

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo
    
    # qsub -V -q erhiq -l mem=8gb -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    set qcommand = "qsub -V -q erhiq -l mem=2gb -W umask=0022 -r y -N SmallTrees -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args"

    set qresult=`$qcommand` || exit
    echo $qresult
    echo $qresult $qcommand >> $rerunlist
   		
    @ submitted = $submitted + 1
end

unset noglob
echo Submitted $submitted jobs to the grid.

