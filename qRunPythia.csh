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

setenv intype mctree
setenv chainname tree
setenv etacut 1
#setenv etacut 7

# if ( $etacut != 7 ) then
#     echo Please double check the eta cut
#     exit
# endif

setenv embi NONE
setenv bg 1

setenv pjmin 5
setenv pjmax 2000

#set ResultDir   = Results
set ResultDir   = Results/Pieces

setenv pcmin 0.2

setenv R 0.4
setenv ht -1
setenv trig ppJP2
setenv bg 0
setenv OutBase ForPaper_Pythia8_NoEff_NoBgSub
setenv trig all

set submitted=0
foreach File ( Data/AlternateRhicPythia/LargeEtaPythiaOnly_1_*root )

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
    echo to resubmit:
    echo qsub -V -q  erhiq -l mem=2gb -W umask=0022 -N PythiaGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    echo

    qsub -V -q  erhiq -l mem=2gb -W umask=0022 -N PythiaGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args

    @ submitted = $submitted + 1
end

unset noglob
echo Submitted $submitted jobs to the grid.





