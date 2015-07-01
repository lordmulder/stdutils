Caption "StdUtils Test-Suite"

!addincludedir  "..\..\Include"

!ifdef NSIS_UNICODE
	!addplugindir "..\..\Plugins\Release_Unicode"
	OutFile "GetParameters-Unicode.exe"
!else
	!addplugindir "..\..\Plugins\Release_ANSI"
	OutFile "GetParameters-ANSI.exe"
!endif

!include 'StdUtils.nsh'

RequestExecutionLevel user
ShowInstDetails show

Section
	${StdUtils.TestParameter} $R0 "Foobar"
	StrCmp "$R0" "true" 0 +3
	DetailPrint 'Command-line parameter /Foobar is specified!'
	Goto +2
	DetailPrint 'Command-line parameter /Foobar is *not* specified!'
	
	${StdUtils.GetParameter} $R0 "Foobar" "<MyDefault>"
	DetailPrint 'Value of command-line parameter /Foobar is: "$R0"'
	
	DetailPrint "----"
SectionEnd

Section
	${StdUtils.TestParameter} $R0 "Foobar"
	StrCmp "$R0" "true" 0 +3
	DetailPrint 'Command-line parameter /Foobar is specified!'
	Goto +2
	DetailPrint 'Command-line parameter /Foobar is *not* specified!'
	
	${StdUtils.GetParameter} $R0 "Foobar" "<MyDefault>"
	DetailPrint 'Value of command-line parameter /Foobar is: "$R0"'
	
	DetailPrint "----"
SectionEnd

Section
	${StdUtils.GetAllParameters} $R0 0
	DetailPrint "Complete command-line: '$R0'"

	${StdUtils.GetAllParameters} $R0 1
	DetailPrint "Truncated command-line: '$R0'"
	
	DetailPrint "----"
SectionEnd
