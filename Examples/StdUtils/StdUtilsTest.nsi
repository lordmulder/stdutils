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
	${StdUtils.ScanStr} $0 "Der Test sagt %d ist toll!" "Der Test sagt 571 ist toll!" 42
	DetailPrint "ScanStr: $0"
	${StdUtils.ScanStr} $0 "Der Hund sagt %d ist toll!" "Der Test sagt 571 ist toll!" 42
	DetailPrint "ScanStr: $0"
SectionEnd

Section
	${StdUtils.ScanStr2} $0 $1 "Der Test sagt %d sowie %d ist toll!" "Der Test sagt 571 sowie 831 ist toll!" 42 43
	DetailPrint "ScanStr2: $0, $1"
	${StdUtils.ScanStr2} $0 $1 "Der Test sagt %d sowie %d ist toll!" "Der Test sagt 571 horch 831 ist toll!" 42 43
	DetailPrint "ScanStr2: $0, $1"
	${StdUtils.ScanStr2} $0 $1 "Der Test sagt %d sowie %d ist toll!" "Der Hund sagt 571 horch 831 ist toll!" 42 43
	DetailPrint "ScanStr2: $0, $1"
SectionEnd

Section
	${StdUtils.ScanStr3} $0 $1 $2 "Der Test sagt %d sowie %d ist toll! Und %d." "Der Test sagt 571 sowie 831 ist toll! Und 325" 42 43 44
	DetailPrint "ScanStr3: $0, $1, $2"
	${StdUtils.ScanStr3} $0 $1 $2 "Der Test sagt %d sowie %d ist toll! Und %d." "Der Test sagt 571 sowie 831 ist toll! OMG 325" 42 43 44
	DetailPrint "ScanStr3: $0, $1, $2"
	${StdUtils.ScanStr3} $0 $1 $2 "Der Test sagt %d sowie %d ist toll! Und %d." "Der Test sagt 571 horch 831 ist toll! OMG 325" 42 43 44
	DetailPrint "ScanStr3: $0, $1, $2"
	${StdUtils.ScanStr3} $0 $1 $2 "Der Test sagt %d sowie %d ist toll! Und %d." "Der Hund sagt 571 horch 831 ist toll! OMG 325" 42 43 44
	DetailPrint "ScanStr3: $0, $1, $2"
SectionEnd

Section
	InitPluginsDir
	SetOutPath "$PLUGINSDIR\TestDirA"
	File "${NSISDIR}\Contrib\Graphics\Checks\*.*"
	SetOutPath "$PLUGINSDIR\TestDirA\SubDir"
	File "${NSISDIR}\Contrib\Graphics\Header\*.*"
	CreateDirectory "$PLUGINSDIR\SubDirX"
	CreateDirectory "$PLUGINSDIR\SubDirY"
	
	${StdUtils.SHFileCopy} $0 "$PLUGINSDIR\TestDirA" "$PLUGINSDIR\SubDirX\TestDirB" $HWNDPARENT
	DetailPrint "SHFileCopy: $0"
	${StdUtils.SHFileMove} $0 "$PLUGINSDIR\TestDirA" "$PLUGINSDIR\SubDirY\TestDirC" $HWNDPARENT
	DetailPrint "SHFileMove: $0"
	ExecShell "explore" "$PLUGINSDIR"
SectionEnd

Section
	${StdUtils.SHFileCopy} $0 "$PLUGINSDIR\TestDirXYZ" "$PLUGINSDIR\SubDirX\TestDirZ" $HWNDPARENT
	DetailPrint "SHFileCopy: $0"
	
	${StdUtils.SetVerbose} 1
	${StdUtils.SHFileCopy} $0 "$PLUGINSDIR\TestDirXYZ" "$PLUGINSDIR\SubDirX\TestDirZ" $HWNDPARENT
	DetailPrint "SHFileCopy: $0"
	
	${StdUtils.SetVerbose} 0
	${StdUtils.SHFileCopy} $0 "$PLUGINSDIR\TestDirXYZ" "$PLUGINSDIR\SubDirX\TestDirZ" $HWNDPARENT
	DetailPrint "SHFileCopy: $0"
SectionEnd

Section
	${StdUtils.Unload}
SectionEnd
