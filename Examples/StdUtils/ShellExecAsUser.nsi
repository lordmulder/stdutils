Caption "StdUtils Test-Suite"

!addincludedir  "..\..\Include"

!ifdef NSIS_UNICODE
	!addplugindir "..\..\Plugins\Release_Unicode"
	OutFile "ShellExecAsUser-Unicode.exe"
!else
	!addplugindir "..\..\Plugins\Release_ANSI"
	OutFile "ShellExecAsUser-ANSI.exe"
!endif

!include 'StdUtils.nsh'

RequestExecutionLevel admin
ShowInstDetails show

Section
	${StdUtils.ExecShellAsUser} $0 "$SYSDIR\mspaint.exe" "open" ""
	DetailPrint "Result: $0"
SectionEnd
