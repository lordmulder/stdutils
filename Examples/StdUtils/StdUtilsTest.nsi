Caption "StdUtils Test-Suite"

!addincludedir  "..\..\Include"

!ifdef NSIS_UNICODE
	!addplugindir "..\..\Plugins\Release_Unicode"
	OutFile "StdUtilsTest-Unicode.exe"
!else
	!addplugindir "..\..\Plugins\Release_ANSI"
	OutFile "StdUtilsTest-ANSI.exe"
!endif

!macro NextTest
	Section
		DetailPrint "--------------"
	SectionEnd
!macroend

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

!insertmacro NextTest

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
	${StdUtils.Rand} $1
	DetailPrint "Random: $1"
	${StdUtils.Rand} $1
	DetailPrint "Random: $1"
SectionEnd

!insertmacro NextTest

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

!insertmacro NextTest

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
	${StdUtils.RandMinMax} $1 20 21
	DetailPrint "Random Min/Max: $1"
	${StdUtils.RandMinMax} $1 20 21
	DetailPrint "Random Min/Max: $1"
	${StdUtils.RandMinMax} $1 20 21
	DetailPrint "Random Min/Max: $1"
	${StdUtils.RandMinMax} $1 20 21
	DetailPrint "Random Min/Max: $1"
	${StdUtils.RandMinMax} $1 20 21
	DetailPrint "Random Min/Max: $1"
SectionEnd

!insertmacro NextTest

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

!insertmacro NextTest

Section
	${StdUtils.RandList} 50 100
	Pop $1
	StrCmp $1 EOL +3
	DetailPrint "RandList: $1"
	Goto -3
SectionEnd

!insertmacro NextTest

Section
	${StdUtils.ScanStr} $0 "Der Test sagt %d ist toll!" "Der Test sagt 571 ist toll!" 42
	DetailPrint "ScanStr: $0"
	${StdUtils.ScanStr} $0 "Der Hund sagt %d ist toll!" "Der Test sagt 571 ist toll!" 42
	DetailPrint "ScanStr: $0"
SectionEnd

!insertmacro NextTest

Section
	${StdUtils.ScanStr2} $0 $1 "Der Test sagt %d sowie %d ist toll!" "Der Test sagt 571 sowie 831 ist toll!" 42 43
	DetailPrint "ScanStr2: $0, $1"
	${StdUtils.ScanStr2} $0 $1 "Der Test sagt %d sowie %d ist toll!" "Der Test sagt 571 horch 831 ist toll!" 42 43
	DetailPrint "ScanStr2: $0, $1"
	${StdUtils.ScanStr2} $0 $1 "Der Test sagt %d sowie %d ist toll!" "Der Hund sagt 571 horch 831 ist toll!" 42 43
	DetailPrint "ScanStr2: $0, $1"
SectionEnd

!insertmacro NextTest

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

!insertmacro NextTest

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

!insertmacro NextTest

Section
	MessageBox MB_ICONINFORMATION "The next three operations are going to fail!$\nBut only one will be verbose..."

	${StdUtils.SHFileCopy} $0 "$PLUGINSDIR\TestDirXYZ" "$PLUGINSDIR\SubDirX\TestDirZ" $HWNDPARENT
	DetailPrint "SHFileCopy: $0"
	
	${StdUtils.SetVerbose} 1
	${StdUtils.SHFileCopy} $0 "$PLUGINSDIR\TestDirXYZ" "$PLUGINSDIR\SubDirX\TestDirZ" $HWNDPARENT
	DetailPrint "SHFileCopy: $0"
	
	${StdUtils.SetVerbose} 0
	${StdUtils.SHFileCopy} $0 "$PLUGINSDIR\TestDirXYZ" "$PLUGINSDIR\SubDirX\TestDirZ" $HWNDPARENT
	DetailPrint "SHFileCopy: $0"
SectionEnd

!insertmacro NextTest

Section
	StrCpy $1 "        Some Text        "
	DetailPrint "String: '$1'"
	StrCpy $0 $1
	${StdUtils.TrimStr} $0
	DetailPrint "TrimStr: '$0'"
	StrCpy $0 $1
	${StdUtils.TrimStrLeft} $0
	DetailPrint "TrimStrLeft: '$0'"
	StrCpy $0 $1
	${StdUtils.TrimStrRight} $0
	DetailPrint "TrimStrRight: '$0'"
	
	StrCpy $1 "Some Text"
	DetailPrint "String: '$1'"
	StrCpy $0 $1
	${StdUtils.TrimStr} $0
	DetailPrint "TrimStr: '$0'"
	StrCpy $0 $1
	${StdUtils.TrimStrLeft} $0
	DetailPrint "TrimStrLeft: '$0'"
	StrCpy $0 $1
	${StdUtils.TrimStrRight} $0
	DetailPrint "TrimStrRight: '$0'"

	StrCpy $1 ""
	DetailPrint "String: '$1'"
	StrCpy $0 $1
	${StdUtils.TrimStr} $0
	DetailPrint "TrimStr: '$0'"
	StrCpy $0 $1
	${StdUtils.TrimStrLeft} $0
	DetailPrint "TrimStrLeft: '$0'"
	StrCpy $0 $1
	${StdUtils.TrimStrRight} $0
	DetailPrint "TrimStrRight: '$0'"
	
	StrCpy $1 "   "
	DetailPrint "String: '$1'"
	StrCpy $0 $1
	${StdUtils.TrimStr} $0
	DetailPrint "TrimStr: '$0'"
	StrCpy $0 $1
	${StdUtils.TrimStrLeft} $0
	DetailPrint "TrimStrLeft: '$0'"
	StrCpy $0 $1
	${StdUtils.TrimStrRight} $0
	DetailPrint "TrimStrRight: '$0'"

	StrCpy $1 "$\tFoobar$\r$\n"
	DetailPrint "String: '$1'"
	StrCpy $0 $1
	${StdUtils.TrimStr} $0
	DetailPrint "TrimStr: '$0'"
	StrCpy $0 $1
	${StdUtils.TrimStrLeft} $0
	DetailPrint "TrimStrLeft: '$0'"
	StrCpy $0 $1
	${StdUtils.TrimStrRight} $0
	DetailPrint "TrimStrRight: '$0'"
SectionEnd

!insertmacro NextTest

Section
	StrCpy $0 "ABC"
	DetailPrint "String: $0"
	${StdUtils.RevStr} $0
	DetailPrint "RevStr: $0"
	
	StrCpy $0 "ABCD"
	DetailPrint "String: $0"
	${StdUtils.RevStr} $0
	DetailPrint "RevStr: $0"

	StrCpy $0 "Just a very long text with no specific meaning at all!"
	DetailPrint "String: $0"
	${StdUtils.RevStr} $0
	DetailPrint "RevStr: $0"
SectionEnd

!insertmacro NextTest
