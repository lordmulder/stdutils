Caption "StdUtils Test-Suite"

!addincludedir  "..\..\Include"

!ifdef NSIS_UNICODE
	!addplugindir "..\..\Plugins\Release_Unicode"
	OutFile "StdUtilsTest-Unicode.exe"
!else
	!addplugindir "..\..\Plugins\Release_ANSI"
	OutFile "StdUtilsTest-ANSI.exe"
!endif

!include 'StdUtils.nsh'

RequestExecutionLevel user
ShowInstDetails show

Section
	${StdUtils.Time} $1
	DetailPrint "Time: $1"
	Sleep 500
	${StdUtils.Time} $1
	DetailPrint "Time: $1"
	Sleep 500
	${StdUtils.Time} $1
	DetailPrint "Time: $1"
SectionEnd

Section
	${StdUtils.Rand} $1
	DetailPrint "Random: $1"
	${StdUtils.Rand} $1
	DetailPrint "Random: $1"
	${StdUtils.Rand} $1
	DetailPrint "Random: $1"
	${StdUtils.Rand} $1
	DetailPrint "Random: $1"
	${StdUtils.Rand} $1
	DetailPrint "Random: $1"
	${StdUtils.Rand} $1
	DetailPrint "Random: $1"
SectionEnd

Section
	${StdUtils.RandMax} $1 42
	DetailPrint "Random Max: $1"
	${StdUtils.RandMax} $1 42
	DetailPrint "Random Max: $1"
	${StdUtils.RandMax} $1 42
	DetailPrint "Random Max: $1"
	${StdUtils.RandMax} $1 42
	DetailPrint "Random Max: $1"
	${StdUtils.RandMax} $1 42
	DetailPrint "Random Max: $1"
	${StdUtils.RandMax} $1 42
	DetailPrint "Random Max: $1"
SectionEnd

Section
	${StdUtils.RandMinMax} $1 -4 -2
	DetailPrint "Random Min/Max: $1"
	${StdUtils.RandMinMax} $1 -4 -2
	DetailPrint "Random Min/Max: $1"
	${StdUtils.RandMinMax} $1 -4 -2
	DetailPrint "Random Min/Max: $1"
	${StdUtils.RandMinMax} $1 -4 -2
	DetailPrint "Random Min/Max: $1"
	${StdUtils.RandMinMax} $1 -4 -2
	DetailPrint "Random Min/Max: $1"
	${StdUtils.RandMinMax} $1 20 21
	DetailPrint "Random Min/Max: $1"
SectionEnd

Section
	${StdUtils.FormatStr} $1 "Hello World is %05d woha!" 89
	DetailPrint "FormatStr: $1"
	${StdUtils.FormatStr2} $1 "Hello World is %05d and %05d woha!" 89 384
	DetailPrint "FormatStr: $1"
	${StdUtils.FormatStr3} $1 "Hello World is %05d and %05d or even %05d woha!" 89 384 2384
	DetailPrint "FormatStr: $1"
	${StdUtils.FormatStr} $1 "Hello World is %09000d." 89
	DetailPrint "FormatStr: $1"
SectionEnd

Section
	${StdUtils.RandList} 50 100
	Pop $1
	StrCmp $1 EOL +3
	DetailPrint "RandList: $1"
	Goto -3
SectionEnd

Section
	InitPluginsDir
	SetOutPath "$PLUGINSDIR\TestDirA"
	File "${NSISDIR}\Contrib\Graphics\Checks\*.*"
	SetOutPath "$PLUGINSDIR\TestDirA\SubDir"
	File "${NSISDIR}\Contrib\Graphics\Header\*.*"
	
	${StdUtils.SHFileCopy} $0 "$PLUGINSDIR\TestDirA" "$PLUGINSDIR\SubDirX\TestDirB" $HWNDPARENT
	DetailPrint "SHFileCopy: $0"
	${StdUtils.SHFileMove} $0 "$PLUGINSDIR\TestDirA" "$PLUGINSDIR\SubDirY\TestDirC" $HWNDPARENT
	DetailPrint "SHFileMove: $0"
	ExecShell "explore" "$PLUGINSDIR"
SectionEnd

Section
	${StdUtils.Unload}
SectionEnd
