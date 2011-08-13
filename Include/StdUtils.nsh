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
	StdUtils::RandMax /NOUNLOAD
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

!define StdUtils.Time       '!insertmacro _StdUtils_Time'
!define StdUtils.Rand       '!insertmacro _StdUtils_Rand'
!define StdUtils.RandMax    '!insertmacro _StdUtils_RandMax'
!define StdUtils.RandMinMax '!insertmacro _StdUtils_RandMinMax'
!define StdUtils.RandList   '!insertmacro _StdUtils_RandList'
!define StdUtils.FormatStr  '!insertmacro _StdUtils_FormatStr'
!define StdUtils.FormatStr2 '!insertmacro _StdUtils_FormatStr2'
!define StdUtils.FormatStr3 '!insertmacro _StdUtils_FormatStr3'
!define StdUtils.SHFileMove '!insertmacro _StdUtils_SHFileMove'
!define StdUtils.SHFileCopy '!insertmacro _StdUtils_SHFileCopy'
