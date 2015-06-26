Caption "StdUtils Test-Suite"

!addincludedir  "..\..\Include"

!ifdef NSIS_UNICODE
	!addplugindir "..\..\Plugins\Release_Unicode"
	OutFile "HashFunctions-Unicode.exe"
!else
	!addplugindir "..\..\Plugins\Release_ANSI"
	OutFile "HashFunctions-ANSI.exe"
!endif

!include 'StdUtils.nsh'

RequestExecutionLevel user
ShowInstDetails show

!define TestData "The quick brown fox jumps over the lazy dog"
!define TestFile "$WINDIR\Explorer.exe"

Section
	${StdUtils.HashText} $0 "SHA3-224" ""
	DetailPrint 'SHA3-224("") = "$0"'
	
	${StdUtils.HashText} $0 "SHA3-256" ""
	DetailPrint 'SHA3-256("") = "$0"'

	${StdUtils.HashText} $0 "SHA3-384" ""
	DetailPrint 'SHA3-384("") = "$0"'
	
	${StdUtils.HashText} $0 "SHA3-512" ""
	DetailPrint 'SHA3-512("") = "$0"'
SectionEnd

Section
	${StdUtils.HashText} $0 "SHA3-224" "${TestData}"
	DetailPrint 'SHA3-224("${TestData}") = "$0"'
	
	${StdUtils.HashText} $0 "SHA3-256" "${TestData}"
	DetailPrint 'SHA3-256("${TestData}") = "$0"'

	${StdUtils.HashText} $0 "SHA3-384" "${TestData}"
	DetailPrint 'SHA3-384("${TestData}") = "$0"'
	
	${StdUtils.HashText} $0 "SHA3-512" "${TestData}"
	DetailPrint 'SHA3-512("${TestData}") = "$0"'
SectionEnd

Section
	${StdUtils.HashFile} $0 "SHA3-224" "${TestFile}"
	DetailPrint 'SHA3-224(${TestFile}) = "$0"'
	
	${StdUtils.HashFile} $0 "SHA3-256" "${TestFile}"
	DetailPrint 'SHA3-256(${TestFile}) = "$0"'

	${StdUtils.HashFile} $0 "SHA3-384" "${TestFile}"
	DetailPrint 'SHA3-384(${TestFile}) = "$0"'
	
	${StdUtils.HashFile} $0 "SHA3-512" "${TestFile}"
	DetailPrint 'SHA3-512(${TestFile}) = "$0"'
SectionEnd
