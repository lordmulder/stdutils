Caption "StdUtils GetRealOSVersion"

!addincludedir  "..\..\Include"

!ifdef NSIS_UNICODE
	!addplugindir "..\..\Plugins\Release_Unicode"
	OutFile "OSVersion-Unicode.exe"
!else
	!addplugindir "..\..\Plugins\Release_ANSI"
	OutFile "OSVersion-ANSI.exe"
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
# GetRealOSVersion
# -----------------------------------------

Section
	${StdUtils.GetRealOSVersion} $1 $2 $3
	${StdUtils.GetRealOSBuildNo} $4
	DetailPrint "Windows NT v$1.$2, ServicePack $3, Build $4"
	
	${StdUtils.GetOSEdition} $1
	DetailPrint "Edition: $1"
	
	${StdUtils.GetRealOSName} $1
	DetailPrint "Friendly name: $\"$1$\""
	
	DetailPrint "--------------"
SectionEnd
