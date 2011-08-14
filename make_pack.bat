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
if exist "%~dp0\_pack.zip" (
	attrib -r "%~dp0\_pack.zip"
	del "%~dp0\_pack.zip"
)
if exist "%~dp0\_pack.zip" (
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
"%PATH_7ZIP%\7z.exe" a -tzip -r -xr@%~dp0\make_pack.lst "%~dp0\_pack.zip" "%~dp0\*.*"
attrib +r "%~dp0\_pack.zip"
REM -------------------------------------------------------------------------
pause
