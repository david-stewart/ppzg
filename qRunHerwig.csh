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

setenv intype herwigtree
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
#setenv pcmin 0.0

setenv R 0.4
setenv ht -1
setenv trig all
setenv bg 0
setenv OutBase ForPaper_Herwig_NoEff_NoBgSub_NoHadro
setenv trig all

if ( $R != 0.4 ) then
    setenv OutBase R${R}_$OutBase
endif

set submitted=0
#foreach File ( Data/herwig/rhic_*root )
foreach File ( Data/herwig/NoHadro_rhic_*root )
    set nefcut=' -jetnef 1'
    #set nefcut ''

    set FileBase    = `basename ${File} | sed s/.root//g`
    set OutName     = ${ResultDir}/${OutBase}_${FileBase}.root

    # Logfiles.
    set LogFile     = logs/${OutBase}_${FileBase}.out
    set ErrFile     = logs/${OutBase}_${FileBase}.err

    set Args = ( -i $File -intype ${intype} -c ${chainname} -trig ${trig} -o ${OutName} -bg ${bg} -ht ${ht} -N $Nevent -pj ${pjmin} ${pjmax} -pc ${pcmin} ${pcmax} -lja $lja -ec $etacut -R $R  -embi ${embi} ${nefcut} )
    
    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo to resubmit:
    echo qsub -V -q  erhiq -l mem=2gb -W umask=0022 -N HerwigGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    echo

    #qsub -V -q  erhiq -l mem=2gb -W umask=0022 -N HerwigGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args || exit -1
    qsub -V -q  erhiq -l mem=2gb -W umask=0022 -N HerwigGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args

    @ submitted = $submitted + 1
end

unset noglob
echo Submitted $submitted jobs to the grid.





