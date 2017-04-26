#!/usr/bin/env csh

# ls -lt logs/McGeant_NoEff_NoBg_MB*out|wc -l       --> 116
# grep -l Done logs/Geant_NoEff_NoBg_HT54_picoDst_*out
# grep -l Done logs/McGeant_NoEff_NoBg_MB*out



set ExecPath = `pwd`

#set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

# set inbase  = McGeant_NoEff_NoBg_HT54_picoDst
# set outbase = McGeant_NoEff_NoBg_HT54

# set inbase  = McGeant_NoEff_NoBg_MB_picoDst
# set outbase = McGeant_NoEff_NoBg_MB

set inbase  = Geant_NoEff_NoBg_HT54_picoDst
set outbase = Geant_NoEff_NoBg_HT54


set binjobs=''
set binouts=''
set ab25binouts=''

foreach bin ( `find Data/GeantPythia/ -name '[0-9]*' -exec basename {} \; ` )
    set In  = Results/Pieces/${inbase}_${bin}*.root
    set Out = Results/${outbase}_${bin}.root

    # Logfiles.
    set LogFile     = logs/Add_${inbase}_${bin}.out
    set ErrFile     = logs/Add_${inbase}_${bin}.err

    set binjobs=${binjobs}:`qsub -V -p 10 -q erhiq -l mem=2gb -W umask=0022 -N AddGeantPart -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${In}`
    set binouts="${binouts} $Out"

    if ( $bin =~ *25_35* || $bin =~ *35_45* || $bin =~ *45_55* || $bin =~ *55_65*  ) set ab25binouts="${ab25binouts} $Out"
end
	    
# finalize
set Out = Results/${outbase}.root
set LogFile     = logs/Add_${inbase}.out
set ErrFile     = logs/Add_${inbase}.err
qsub -W depend=afterok${binjobs} -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${binouts}

# # finalize
# set Out = Results/Above25_${outbase}.root
# set LogFile     = logs/Add_Above25_${inbase}.out
# set ErrFile     = logs/Add_Above25_${inbase}.err
# qsub -W depend=afterok${binjobs} -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${ab25binouts}

