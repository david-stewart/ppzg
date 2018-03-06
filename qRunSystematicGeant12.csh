#!/usr/bin/env csh


## NOTE: All shifts and smears are done AFTER cuts.
## Conceptually, we take the data as reconstructed, then check to see what happens
## if the actual pT, ET is different.

## Sources:
## -- Tracking efficiency. On Geant, just throw away an extra 4% independent of pT
## Source of 4%: Kevin's thesis, and ref 81,
## L. Huo, “In-Jet Tracking Efficiency Analysis for the STAR Time Projection Chamber in Polarized Proton-Proton Collisions at √s = 200 GeV,” Master’s thesis, Texas A&M University, May 2012.
## which finds 3.3% for embedding jets into real Run6 data, rounded up.

## -- Tracking resolution.
## primaries in p+p
## DeltapT/pT = 0.01+0.005 pT from
## https://arxiv.org/pdf/1205.2735.pdf
## http://www.sciencedirect.com/science/article/pii/S0370269306004928
## primaries in Au+Au
## http://www.star.bnl.gov/protected/highpt/jklay/QM02/plots/momres/P02gd.HighpT_piminus_minbias.M1111ny.resPrFit.gif
## Globals in Au+Au
## http://www.star.bnl.gov/protected/highpt/jklay/QM02/plots/momres/P02gd.HighpT_piminus_minbias.M1111ny.resGlFit.gif
## Alternative for AuAu primaries:
## deltak/k ∼ 0.0078 + 0.0098 · pT(GeV/c) [23]
## where k is the track curvature, proportional to 1/pT
## https://arxiv.org/pdf/nucl-ex/0407003.pdf

## -- Tower calibration
## Uncertainty <~5% or 2%?
## https://arxiv.org/pdf/nucl-ex/0407003.pdf
# By combining the MIP calibration and the electron
# calibration of the EMC we obtain an overall estimated
# systematic uncertainty of less than 2% on the total energy
# measured by the calorimeter
## But also
## we estimate a combined systematic uncertainty in ET of 7% and an event-by-event resolution of 17%.
## using 4.8% for now. Can't find the source, will add later.

## -- Hadronic correction - test 50% and MIP% 
## Use difference between 100% and MIP

## -- Tower inefficiency uncertainty 1%
## Not done for now


## -- quark vs glue jets? quark enhanced above 10 GeV (bc of trigger), <~ 2% of jet pT
## Not done. We have to assume our Pythia describes the data well.

set ExecPath = `pwd`
set Exec = "./bin/RunPpZg"

# make sure executable exists
make $Exec || exit

#parameters
setenv lja antikt
#setenv lja cambri

setenv efffac 0.04 # 4% uncertainty in p+p is conservative. 5% is standard, 3.3% is the result of a better study

setenv pcmax 10000
setenv Nevent -1

setenv pcmin 0.2
setenv R 0.6
setenv bg 0
setenv chainname JetTree
setenv intype pico
setenv etacut 1
setenv trig ppJP2

setenv hadcorr 0.99999
setenv ht -1
setenv OutBase SystGeant12_NoEff_NoBg_JP2

# setenv hadcorr 0.5
# setenv ht -1
# setenv OutBase SystGeant12_HC50_NoEff_NoBg_JP2

# setenv hadcorr -1
# setenv ht -1
# setenv OutBase SystGeant12_MIP_NoEff_NoBg_JP2

if ( $R != 0.4 ) then
    setenv OutBase R${R}_$OutBase
endif

set rerunlist = Rerun_${OutBase}.txt
rm -f $rerunlist

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

set ResultDir   = Results/Pieces

#set noglob
set submitted=0

foreach File ( Data/AddedEmbedPythiaRun12pp200/Cleanpp12*root )
    foreach Tow ( 0 -1 1 )
    	foreach Smear ( 0 1 )
    	    foreach Eff ( 0 -1 )
    # foreach Tow ( 0 )
    # 	foreach Smear ( 0 )
    # 	    foreach Eff ( 0 )

		@ TowEff = $Tow * $Eff
    		@ TowSmear = $Tow * $Smear
    		@ SmearEff = $Eff * $Smear

    		# Only combinations where at most 1 is !=0
		# echo Tow = $Tow  Smear = $Smear  Eff = $Eff
		# echo TowEff = $TowEff  TowSmear = $TowSmear  SmearEff = $SmearEff
		if ( $TowEff != 0  || $TowSmear != 0 || $SmearEff != 0 ) continue;
		# Except also allow Tow=1, Eff=-1
    		# if ( ( $TowEff != 0 && $TowEff != -1 )  || $TowSmear != 0 || $SmearEff != 0 ) continue;
		# Allow all combinations...

		# echo Accepted
		# echo Tow = $Tow  Smear = $Smear  Eff = $Eff
		# continue;

		set FileBase    = AEff${Eff}_PtSmear${Smear}_ATow${Tow}_${OutBase}_`basename ${File} | sed s/.root//g`
		set OutName     = ${ResultDir}/${FileBase}.root
		
		# Logfiles.
		set LogFile     = logs/${FileBase}.out
		set ErrFile     = logs/${FileBase}.err
		rm -f $LogFile $ErrFile

		set fakeeff = `echo "1 + $Eff * $efffac" | bc`
		set Args = ( -i $File -intype ${intype} -c ${chainname} -trig ${trig} -o ${OutName} -bg ${bg} -ht ${ht}  )
		set Args = ( $Args -N $Nevent -pj ${pjmin} ${pjmax} -pc ${pcmin} ${pcmax} -lja $lja -ec $etacut -R $R  -embi ${embi} -hadcorr ${hadcorr} )
		set Args = ( $Args -tracksmear ${Smear} -fakeeff ${fakeeff} -towunc ${Tow} )

    
		echo Submitting:
		echo $Exec $Args
		# echo "Logging output to " $LogFile
		# echo "Logging errors to " $ErrFile
		# echo to resubmit:

		# set qcommand = "qsub -V -q  mwsuq -l mem=4gb -W umask=0022 -r y -N SystGeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args"
		set qcommand = "qsub -V -q  erhiq -l mem=4gb -W umask=0022 -r y -N SystGeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args"
		set qresult=`$qcommand` || exit
		echo $qresult
		echo $qresult $qcommand >> $rerunlist
		@ submitted = $submitted + 1
		echo
	    end
	end
    end
end
#unset noglob
echo Submitted $submitted jobs to the grid.

