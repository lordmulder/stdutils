#############################
# StdUtils plug-in for NSIS #
#############################

!define StdUtils.Time            '!insertmacro _StdUtils_Time'        #time()
!define StdUtils.Rand            '!insertmacro _StdUtils_Rand'        #rand()
!define StdUtils.RandMax         '!insertmacro _StdUtils_RandMax'     #rand() with maximum
!define StdUtils.RandMinMax      '!insertmacro _StdUtils_RandMinMax'  #rand() with minimum/maximum
!define StdUtils.RandList        '!insertmacro _StdUtils_RandList'    #rand() with list support
!define StdUtils.FormatStr       '!insertmacro _StdUtils_FormatStr'   #sprintf() with one format tag (only %d supported!)
!define StdUtils.FormatStr2      '!insertmacro _StdUtils_FormatStr2'  #sprintf() with two format tags (only %d supported!)
!define StdUtils.FormatStr3      '!insertmacro _StdUtils_FormatStr3'  #sprintf() with three format tags (only %d supported!)
!define StdUtils.ScanStr         '!insertmacro _StdUtils_ScanStr'     #sscanf() with one format tag (only %d supported!)
!define StdUtils.ScanStr2        '!insertmacro _StdUtils_ScanStr2'    #sscanf() with two format tags (only %d supported!)
!define StdUtils.ScanStr3        '!insertmacro _StdUtils_ScanStr3'    #sscanf() with three format tags (only %d supported!)
!define StdUtils.SHFileMove      '!insertmacro _StdUtils_SHFileMove'  #SHFileOperation with FO_MOVE
!define StdUtils.SHFileCopy      '!insertmacro _StdUtils_SHFileCopy'  #SHFileOperation with FO_COPY
!define StdUtils.ExecShellAsUser '!insertmacro _StdUtils_ExecShlUser' #ShellExecute() with user context (for elevated installers)
!define StdUtils.Unload          '!insertmacro _StdUtils_Unload'      #Unload DLL for proper clean-up (don't forget!)
!define StdUtils.SetVerbose      '!insertmacro _StdUtils_SetVerbose'  #Verbose mode (for debugging)


!macro _StdUtils_Time out
	StdUtils::Time /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_Rand out
	StdUtils::Rand /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_RandMax out max
	push ${max}
	StdUtils::RandMax /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_RandMinMax out min max
	push ${min}
	push ${max}
	StdUtils::RandMinMax /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_RandList count max
	push ${max}
	push ${count}
	StdUtils::RandList /NOUNLOAD
!macroend

!macro _StdUtils_FormatStr out format val
	push '${format}'
	push ${val}
	StdUtils::FormatStr /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_FormatStr2 out format val1 val2
	push '${format}'
	push ${val1}
	push ${val2}
	StdUtils::FormatStr2 /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_FormatStr3 out format val1 val2 val3
	push '${format}'
	push ${val1}
	push ${val2}
	push ${val3}
	StdUtils::FormatStr3 /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_ScanStr out format input default
	push '${format}'
	push '${input}'
	push ${default}
	StdUtils::ScanStr /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_ScanStr2 out1 out2 format input default1 default2
	push '${format}'
	push '${input}'
	push ${default1}
	push ${default2}
	StdUtils::ScanStr2 /NOUNLOAD
	pop ${out1}
	pop ${out2}
!macroend

!macro _StdUtils_ScanStr3 out1 out2 out3 format input default1 default2 default3
	push '${format}'
	push '${input}'
	push ${default1}
	push ${default2}
	push ${default3}
	StdUtils::ScanStr3 /NOUNLOAD
	pop ${out1}
	pop ${out2}
	pop ${out3}
!macroend

!macro _StdUtils_SHFileMove out from to hwnd
	push '${from}'
	push '${to}'
	push ${hwnd}
	StdUtils::SHFileMove /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_SHFileCopy out from to hwnd
	push '${from}'
	push '${to}'
	push ${hwnd}
	StdUtils::SHFileCopy /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_ExecShlUser out file verb args
	push '${file}'
	push '${verb}'
	push '${args}'
	StdUtils::ExecShellAsUser /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_Unload
	StdUtils::Unload
!macroend

!macro _StdUtils_SetVerbose on
	!if "${on}" != "0"
		StdUtils::EnableVerboseMode /NOUNLOAD
	!else
		StdUtils::DisableVerboseMode /NOUNLOAD
	!endif
!macroend
