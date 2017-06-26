#! /usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/RunPpZg"

# make sure executable exists
make $Exec || exit

#parameters
setenv lja antikt
#setenv lja cambri

setenv pcmax 10000
setenv Nevent -1

setenv pcmin 0.2
setenv R 0.4
setenv bg 0
setenv ht -1
setenv chainname JetTree
setenv intype pico
setenv etacut 1
setenv OutBase Geant_NoEff_NoBg_MB
setenv trig all

# setenv pcmin 0.2
# setenv R 0.4
# setenv bg 0
# setenv ht 5.4
# setenv chainname JetTree
# setenv intype pico
# setenv etacut 1
# setenv OutBase Geant_NoEff_NoBg_HT54
# setenv trig all

# #setenv pcmin 0.2
# setenv pcmin 0.0
# setenv R 0.4
# setenv bg 0
# setenv ht -1
# setenv chainname JetTreeMc
# setenv intype mcpico
# setenv etacut 7
# setenv OutBase McGeant_NoEff_NoBg_MB
# setenv trig all


if ( $chainname == JetTreeMc ) then
    if ( $etacut != 7 ) then
	echo Please double check the eta cut
	exit
    endif
else if ( $etacut != 1 ) then
    echo Please double check the eta cut
    exit    
endif

setenv embi NONE

setenv pjmin 5
setenv pjmax 2000

#set ResultDir   = Results
set ResultDir   = Results/Pieces


set submitted=0
foreach File ( Data/AddedGeantPythia/pico*root )
    set FileBase    = `basename ${File} | sed s/.root//g`
    set OutName     = ${ResultDir}/${OutBase}_${FileBase}.root

    # Logfiles.
    set LogFile     = logs/${OutBase}_${FileBase}.out
    set ErrFile     = logs/${OutBase}_${FileBase}.err

    set Args = ( -i $File -intype ${intype} -c ${chainname} -trig ${trig} -o ${OutName} -bg ${bg} -ht ${ht} -N $Nevent -pj ${pjmin} ${pjmax} -pc ${pcmin} ${pcmax} -lja $lja -ec $etacut -R $R  -embi ${embi} )

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo to resubmit:
    echo qsub -V -q  erhiq -l mem=2gb -W umask=0022 -N GeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    echo

    qsub -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N GeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args

    @ submitted = $submitted + 1
end

unset noglob
echo Submitted $submitted jobs to the grid.





