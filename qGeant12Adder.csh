#!/usr/bin/env csh

# ls -lt logs/McGeant_NoEff_NoBg_MB*out|wc -l       --> 116
# grep -l Done logs/Geant_NoEff_NoBg_HT54_picoDst_*out
# grep -l Done logs/McGeant_NoEff_NoBg_MB*out



set ExecPath = `pwd`

#set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

# set inbase  = ReCut_McGeant12_NoEff_NoBg_MB_Cleanpp12Pico
# set outbase = ReCut_McGeant12_NoEff_NoBg_MB

# set inbase  = ReCut_Geant12_NoEff_NoBg_HT54_Cleanpp12Pico
# set outbase = ReCut_Geant12_NoEff_NoBg_HT54

# set inbase  = ReCut_Geant12_NoEff_NoBg_HT54_JP_Cleanpp12Pico
# set outbase = ReCut_Geant12_NoEff_NoBg_HT54_JP

# set inbase  = Geant12_NoEff_NoBg_MB_Cleanpp12Pico
# set outbase = Geant12_NoEff_NoBg_MB

# set inbase  = Test_Geant12_NoEff_NoBg_all_Cleanpp12Pico
# set outbase = Test_Geant12_NoEff_NoBg_all

set inbase  = McGeant12_NoEff_NoBg_all_Cleanpp12Pico
set outbase = McGeant12_NoEff_NoBg_all



set binjobs=''
set binouts=''

set bins =      ( 2_3 3_4 4_5 5_7 7_9 9_11 11_15 15_20 20_25 25_35 35_-1 )
foreach bin ( $bins )
    set In  = Results/Pieces/${inbase}_pt${bin}*.root
    set Out = Results/${outbase}_${bin}.root
    
    # Logfiles.
    set LogFile     = logs/Add_${inbase}_pt${bin}.out
    set ErrFile     = logs/Add_${inbase}_pt${bin}.err

    set binjobs=${binjobs}:`qsub -V -p 10 -q erhiq -l mem=2gb -W umask=0022 -N AddGeant12Part -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${In}`
    set binouts="${binouts} $Out"

end
	    
# finalize
set Out = Results/${outbase}.root
set LogFile     = logs/Add_${inbase}.out
set ErrFile     = logs/Add_${inbase}.err
qsub -W depend=afterok${binjobs} -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant12 -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${binouts}


