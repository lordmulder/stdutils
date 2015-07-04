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
	StrCpy $R1 0
FetchNextArg:
	${StdUtils.RawParameter} $R0 $R1 "!$#@%&?"
	StrCmp "$R0" "!$#@%&?" NoMoreArgs
	DetailPrint 'Command-line token #$R1 is "$R0"'
	IntOp $R1 $R1 + 1
	Goto FetchNextArg
NoMoreArgs:
	DetailPrint 'No more command-line tokens!'
	DetailPrint "----"
SectionEnd

Section
	${StdUtils.GetAllParameters} $R0 0
	DetailPrint "Complete command-line: '$R0'"

	${StdUtils.GetAllParameters} $R0 1
	DetailPrint "Truncated command-line: '$R0'"
	
	DetailPrint "----"
SectionEnd
