@echo off
REM ----------------------------------------------------------------------
set "NSIS_ANSI=E:\NSIS"
set "NSIS_Unicode=E:\NSIS\_Unicode"
REM ----------------------------------------------------------------------
del "%~dp0\StdUtilsTest-ANSI.exe"
if exist "%~dp0\StdUtilsTest-ANSI.exe" (
	pause
	exit
)
del "%~dp0\StdUtilsTest-Unicode.exe"
if exist "%~dp0\StdUtilsTest-Unicode.exe" (
	pause
	exit
)
REM ----------------------------------------------------------------------
for %%i in (StdUtilsTest,ShellExecAsUser,ShellExecWait,GetParameters) do (
	"%NSIS_ANSI%\makensis.exe" "%~dp0\%%i.nsi"
	"%NSIS_Unicode%\makensis.exe" "%~dp0\%%i.nsi"
)
REM ----------------------------------------------------------------------
pause
