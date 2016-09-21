Caption "StdUtils PathUtils"

!addincludedir  "..\..\Include"

!ifdef NSIS_UNICODE
	!addplugindir "..\..\Plugins\Release_Unicode"
	OutFile "PathUtils-Unicode.exe"
!else
	!addplugindir "..\..\Plugins\Release_ANSI"
	OutFile "PathUtils-ANSI.exe"
!endif

!include 'StdUtils.nsh'

RequestExecutionLevel user
ShowInstDetails show

# -----------------------------------------
# GetLibVersion
# -----------------------------------------

Section
	${StdUtils.GetLibVersion} $1 $2
	DetailPrint "Testing StdUtils library v$1"
	DetailPrint "Library built: $2"
	
	DetailPrint "--------------"
SectionEnd

# -----------------------------------------
# NormalizePath
# -----------------------------------------

!macro TestNormalizePath input
	${StdUtils.NormalizePath} $0 '${input}'
	DetailPrint 'Normalize: "${input}" -> "$0"'
!macroend

Section
	!insertmacro TestNormalizePath "C:\"
	!insertmacro TestNormalizePath "C:\Test"
	!insertmacro TestNormalizePath "C:\Test\"
	!insertmacro TestNormalizePath "C:\Test\."
	!insertmacro TestNormalizePath "C:\Test\Foo"
	!insertmacro TestNormalizePath "C:\Test\\\Foo"
	!insertmacro TestNormalizePath "C:\Test\.\Foo"
	!insertmacro TestNormalizePath "C:\Test\Foo\.."
	!insertmacro TestNormalizePath "C:\Test\Foo\..\Bar\"
	!insertmacro TestNormalizePath "C:\Test\.\Foo\.\Bar\Example"
	!insertmacro TestNormalizePath "C:\Test\/\Foo\/\Bar\Example"
	!insertmacro TestNormalizePath "C:\Test/Foo\\Bar"
	!insertmacro TestNormalizePath "C:\Foo\\Bar/Test\..\Test\."
	
	DetailPrint "--------------"
SectionEnd

# -----------------------------------------
# GeParentPath
# -----------------------------------------

!macro TestGetParentPath input
	${StdUtils.GetParentPath} $0 '${input}'
	DetailPrint 'GetParent: "${input}" -> "$0"'
!macroend

Section
	!insertmacro TestGetParentPath "C:\"
	!insertmacro TestGetParentPath "C:\Test"
	!insertmacro TestGetParentPath "C:\Test\"
	!insertmacro TestGetParentPath "C:\Test\."
	!insertmacro TestGetParentPath "C:\Test\Foo"
	!insertmacro TestGetParentPath "C:\Test\\\Foo"
	!insertmacro TestGetParentPath "C:\Test\.\Foo"
	!insertmacro TestGetParentPath "C:\Test\Foo\.."
	!insertmacro TestGetParentPath "C:\Test\Foo\..\Bar\"
	!insertmacro TestGetParentPath "C:\Test\.\Foo\.\Bar\Example"
	!insertmacro TestGetParentPath "C:\Test\/\Foo\/\Bar\Example"
	!insertmacro TestGetParentPath "C:\Test/Foo\\Bar"
	
	DetailPrint "--------------"
SectionEnd



Section
	StrCpy $0 "C:\Foo\\Bar/Test\.\..\Test\."
	${StdUtils.NormalizePath} $1 "$0"
	DetailPrint 'NormalizePath: "$0" -> "$1"'
SectionEnd

Section
	StrCpy $0 "C:\Foo\Bar\Test\Honk\Sponk"
Loop:
	${StdUtils.GetParentPath} $1 "$0"
	DetailPrint 'GetParentPath: "$0" -> "$1"'
	StrCpy $0 "$1"
	StrCmp "$0" "" 0 Loop
SectionEnd
