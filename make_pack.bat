@echo off
REM -------------------------------------------------------------------------
set "PATH_7ZIP=E:\7-Zip"
set "PATH_MSVC=D:\Microsoft Visual Studio 10.0\VC"
REM -------------------------------------------------------------------------
call "%PATH_MSVC%\vcvarsall.bat" x86
if "%VCINSTALLDIR%"=="" (
	pause
	exit
)
REM -------------------------------------------------------------------------
set "ISO_DATE="
if not exist "%~dp0\Contrib\StdUtils\utils\Date.exe" GOTO:EOF
for /F "tokens=1,2 delims=:" %%a in ('"%~dp0\Contrib\StdUtils\utils\Date.exe" +ISODATE:%%Y-%%m-%%d') do (
	if "%%a"=="ISODATE" set "ISO_DATE=%%b"
)
if "%ISO_DATE%"=="" (
	pause
	exit
)
REM -------------------------------------------------------------------------
if exist "%~dp0\StdUtils.%ISO_DATE%.zip" (
	attrib -r "%~dp0\StdUtils.%ISO_DATE%.zip"
	del "%~dp0\StdUtils.%ISO_DATE%.zip"
)
if exist "%~dp0\StdUtils.%ISO_DATE%.zip" (
	pause
	exit
)
REM -------------------------------------------------------------------------
MSBuild.exe /property:Configuration=Release_ANSI    /property:Platform=Win32 /target:Rebuild /verbosity:normal "%~dp0\Contrib\StdUtils\StdUtils.sln"
if not "%ERRORLEVEL%"=="0" (
	pause
	exit
)
REM -------------------------------------------------------------------------
MSBuild.exe /property:Configuration=Release_Unicode /property:Platform=Win32 /target:Rebuild /verbosity:normal "%~dp0\Contrib\StdUtils\StdUtils.sln"
if not "%ERRORLEVEL%"=="0" (
	pause
	exit
)
REM -------------------------------------------------------------------------
echo Built on %DATE%, at %TIME%. > "%~dp0\BUILD.tag"
"%PATH_7ZIP%\7z.exe" a -tzip -r -xr@%~dp0\make_pack.lst "%~dp0\StdUtils.%ISO_DATE%.zip" "%~dp0\*.*"
attrib +r "%~dp0\StdUtils.%ISO_DATE%.zip" 
del "%~dp0\BUILD.tag"
REM -------------------------------------------------------------------------
pause
