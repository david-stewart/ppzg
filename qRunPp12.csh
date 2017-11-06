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

setenv intype pico
setenv chainname JetTree
setenv etacut 1

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
setenv bg 1

setenv pjmin 5
setenv pjmax 2000

#set ResultDir   = Results
set ResultDir   = Results/Pieces

setenv pcmin 0.2

# setenv R 0.4
# setenv ht 5.4
# setenv trig ppJP2
# setenv bg 0
# setenv OutBase ForPaper_Pp12_HT54_JP2_NoEff_NoBgSub

setenv R 0.4
setenv ht -1
setenv trig ppJP2
setenv bg 0
setenv hadcorr 0.9999999
setenv OutBase ForPaper_Pp12_JP2_NoEff_NoBgSub

# setenv hadcorr 0.5
# setenv OutBase ForPaper_Pp12_JP2_HC50_NoEff_NoBgSub

# setenv hadcorr -1
# setenv OutBase ForPaper_Pp12_JP2_MIP_NoEff_NoBgSub



if ( $R != 0.4 ) then
    setenv OutBase R${R}_$OutBase
endif


set submitted=0
foreach File ( Data/ppJP2Run12/sum*root )
    set FileBase    = `basename ${File} | sed s/.root//g`
    set OutName     = ${ResultDir}/${OutBase}_${FileBase}.root

    # Logfiles.
    set LogFile     = logs/${OutBase}_${FileBase}.out
    set ErrFile     = logs/${OutBase}_${FileBase}.err

    set Args = ( -i $File -intype ${intype} -c ${chainname} -trig ${trig} -o ${OutName} -bg ${bg} -ht ${ht} -N $Nevent -pj ${pjmin} ${pjmax} -pc ${pcmin} ${pcmax} )
    set Args = ( $Args -lja $lja -ec $etacut -R $R  -embi ${embi} -hadcorr ${hadcorr} )

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo to resubmit:
    echo qsub -V -q  erhiq -l mem=2gb -W umask=0022 -N Pp12Groom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    echo

    qsub -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N Pp12Groom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args

    @ submitted = $submitted + 1
end

unset noglob
echo Submitted $submitted jobs to the grid.

