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

set ResultDir   = Results

setenv pcmin 0.2

#setenv OutBase Groom_McGeant
#setenv pcmin 2.0
#setenv NameBase HighConsGroom

setenv R 0.4
setenv ht 5.4
setenv trig ppHT

# setenv bg 0
# setenv hadcorr 0.999999
# setenv OutBase Recut_Pp_HT54_NoEff_NoBgSub

setenv bg 0
setenv hadcorr -1
setenv OutBase Recut_Pp_HT54_MIP_NoEff_NoBgSub

# setenv bg 1
# setenv OutBase Pp_HT54_NoEff_AreaSub

# setenv bg 2
# setenv OutBase Pp_HT54_NoEff_ConstSubPre

set noglob
set Files       = Data/ppHT/*.root

set LogFile     = logs/${OutBase}.out
set ErrFile     = logs/${OutBase}.err

set OutName     = ${ResultDir}/${OutBase}.root

set Args = ( -i $Files -intype ${intype} -c ${chainname} -trig ${trig} -o ${OutName} -bg ${bg} -ht ${ht}  )
set Args = ( $Args -N $Nevent -pj ${pjmin} ${pjmax} -pc ${pcmin} ${pcmax} -lja $lja -ec $etacut -R $R  -embi ${embi} -hadcorr ${hadcorr} )
		
echo Submitting:
echo $Exec $Args
echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile
echo to resubmit:
echo qsub -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N PpGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
echo

qsub -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N PpGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args

unset noglob





