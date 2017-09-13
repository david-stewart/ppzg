#!/usr/bin/env csh

set ExecPath = `pwd`

set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

# set inbase  = ${RMod}SystGeant12_NoEff_NoBg_HT54_JP2
# set outbase = ${RMod}SystGeant12_NoEff_NoBg_HT54_JP2

# set inbase  = ${RMod}SystGeant12_NoEff_NoBg_JP2
# set outbase = ${RMod}SystGeant12_NoEff_NoBg_JP2
# set fileroot = Cleanpp12Pico
# set datapath = Data/AddedEmbedPythiaRun12pp200

# set inbase  = ${RMod}SystGeant12_HC50_NoEff_NoBg_JP2
# set outbase = ${RMod}SystGeant12_HC50_NoEff_NoBg_JP2
# set fileroot = Cleanpp12Pico
# set datapath = Data/AddedEmbedPythiaRun12pp200

# set inbase  = ${RMod}SystGeant12_MIP_NoEff_NoBg_JP2
# set outbase = ${RMod}SystGeant12_MIP_NoEff_NoBg_JP2
# set fileroot = Cleanpp12Pico
# set datapath = Data/AddedEmbedPythiaRun12pp200

# set inbase  = ${RMod}SystGeant_NoEff_NoBg_HT54
# set outbase = ${RMod}SystGeant_NoEff_NoBg_HT54
# set fileroot = picoDst
# set datapath = Data/AddedGeantPythia

set inbase  = ${RMod}SystGeant_MIP_NoEff_NoBg_HT54
set outbase = ${RMod}SystGeant_MIP_NoEff_NoBg_HT54
set fileroot = picoDst
set datapath = Data/AddedGeantPythia

# foreach Tow ( 0 -1 1 )
# 	foreach Smear ( 0 1 )
# 	    foreach Eff ( 0 -1 )
foreach Tow ( 0 )
    foreach Smear ( 0 )
	foreach Eff ( 0 )
	    
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
	    
	    set binjobs=''
	    set binouts=''

	    # obscure shall magic to get pt bins here
	    if  ( $fileroot == Cleanpp12Pico ) then
		set bins = `find $datapath -name "${fileroot}*root" | grep -o 'pt[0-9]*_[0-9]*' |sort|uniq `
	    else if  ( $fileroot == picoDst ) then
		set bins = `find  $datapath -name "${fileroot}*root"| sed 's/Dst_/Dst_XX/g' | grep -o 'XX[0-9]*_[0-9]*' |sed 's/XX//g'|sort|uniq`
	    else
		echo "Can't deal with fileroot " $fileroot
	    endif
	    
	    foreach bin ( $bins)
		set In  = Results/Pieces/AEff${Eff}_PtSmear${Smear}_ATow${Tow}_${inbase}_${fileroot}_${bin}*.root
		set Out  = Results/SysStuff/AEff${Eff}_PtSmear${Smear}_ATow${Tow}_${outbase}_${bin}.root

		# Logfiles.
		set LogFile     = logs/Add_AEff${Eff}_PtSmear${Smear}_ATow${Tow}_${inbase}_${fileroot}_${bin}.out
		set ErrFile     = logs/Add_AEff${Eff}_PtSmear${Smear}_ATow${Tow}_${inbase}_${fileroot}_${bin}.err

		set binjobs=${binjobs}:`qsub -V -p 10 -q erhiq -l mem=2gb -W umask=0022 -N AddGeantPart -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${In}`
		set binouts="${binouts} $Out"	      
	    end

	    # finalize
	    set Out  = Results/AEff${Eff}_PtSmear${Smear}_ATow${Tow}_${outbase}.root
	    set LogFile  = logs/Add_AEff${Eff}_PtSmear${Smear}_ATow${Tow}_${inbase}.out
	    set ErrFile  = logs/Add_AEff${Eff}_PtSmear${Smear}_ATow${Tow}_${inbase}.err
	    qsub -W depend=afterok${binjobs} -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${binouts}

	end
    end
end
