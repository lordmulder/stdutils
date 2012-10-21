#############################
# StdUtils plug-in for NSIS #
#############################

#################################################################################
# 
# StdUtils plug-in for NSIS
# Copyright (C) 2004-2012 LoRd_MuldeR <MuldeR2@GMX.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# http://www.gnu.org/licenses/gpl-2.0.txt
#################################################################################


### FUNCTION DECLARTIONS ###

!define StdUtils.Time             '!insertmacro _StdUtils_Time'         #time()
!define StdUtils.Rand             '!insertmacro _StdUtils_Rand'         #rand()
!define StdUtils.RandMax          '!insertmacro _StdUtils_RandMax'      #rand() with maximum
!define StdUtils.RandMinMax       '!insertmacro _StdUtils_RandMinMax'   #rand() with minimum/maximum
!define StdUtils.RandList         '!insertmacro _StdUtils_RandList'     #rand() with list support
!define StdUtils.FormatStr        '!insertmacro _StdUtils_FormatStr'    #sprintf() with one format tag (only %d supported!)
!define StdUtils.FormatStr2       '!insertmacro _StdUtils_FormatStr2'   #sprintf() with two format tags (only %d supported!)
!define StdUtils.FormatStr3       '!insertmacro _StdUtils_FormatStr3'   #sprintf() with three format tags (only %d supported!)
!define StdUtils.ScanStr          '!insertmacro _StdUtils_ScanStr'      #sscanf() with one format tag (only %d supported!)
!define StdUtils.ScanStr2         '!insertmacro _StdUtils_ScanStr2'     #sscanf() with two format tags (only %d supported!)
!define StdUtils.ScanStr3         '!insertmacro _StdUtils_ScanStr3'     #sscanf() with three format tags (only %d supported!)
!define StdUtils.TrimStr          '!insertmacro _StdUtils_TrimStr'      #Remove whitspaces from string (left and right)
!define StdUtils.TrimStrLeft      '!insertmacro _StdUtils_TrimStrLeft'  #Remove whitspaces from string (left side only)
!define StdUtils.TrimStrRight     '!insertmacro _StdUtils_TrimStrRight' #Remove whitspaces from string (right side only)
!define StdUtils.RevStr           '!insertmacro _StdUtils_RevStr'       #Reverse a string
!define StdUtils.SHFileMove       '!insertmacro _StdUtils_SHFileMove'   #SHFileOperation with FO_MOVE
!define StdUtils.SHFileCopy       '!insertmacro _StdUtils_SHFileCopy'   #SHFileOperation with FO_COPY
!define StdUtils.ExecShellAsUser  '!insertmacro _StdUtils_ExecShlUser'  #ShellExecute() with user context (for elevated installers)
!define StdUtils.InvokeShellVerb  '!insertmacro _StdUtils_InvkeShlVrb'  #Invokes a Shell Verb on the specified item
!define StdUtils.ExecShellWait    '!insertmacro _StdUtils_ExecShlWait'  #ShellExecuteEx() with process handle to wait for
!define StdUtils.WaitForProc      '!insertmacro _StdUtils_WaitForProc'  #WaitForSingleObject() to wait for process termination
!define StdUtils.GetParameter     '!insertmacro _StdUtils_GetParameter' #Get the value of a specific commandline paramater
!define StdUtils.GetAllParameters '!insertmacro _StdUtils_GetAllParams' #Get complete command-line, but without executable name
!define StdUtils.SetVerbose       '!insertmacro _StdUtils_SetVerbose'   #Verbose mode (for debugging)


### MACRO DEFINITIONS ###

!macro _StdUtils_Time out
	StdUtils::Time /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_Rand out
	StdUtils::Rand /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_RandMax out max
	push ${max}
	StdUtils::RandMax /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_RandMinMax out min max
	push ${min}
	push ${max}
	StdUtils::RandMinMax /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_RandList count max
	push ${max}
	push ${count}
	StdUtils::RandList /NOUNLOAD
!macroend

!macro _StdUtils_FormatStr out format val
	push '${format}'
	push ${val}
	StdUtils::FormatStr /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_FormatStr2 out format val1 val2
	push '${format}'
	push ${val1}
	push ${val2}
	StdUtils::FormatStr2 /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_FormatStr3 out format val1 val2 val3
	push '${format}'
	push ${val1}
	push ${val2}
	push ${val3}
	StdUtils::FormatStr3 /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_ScanStr out format input default
	push '${format}'
	push '${input}'
	push ${default}
	StdUtils::ScanStr /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_ScanStr2 out1 out2 format input default1 default2
	push '${format}'
	push '${input}'
	push ${default1}
	push ${default2}
	StdUtils::ScanStr2 /NOUNLOAD
	pop ${out1}
	pop ${out2}
!macroend

!macro _StdUtils_ScanStr3 out1 out2 out3 format input default1 default2 default3
	push '${format}'
	push '${input}'
	push ${default1}
	push ${default2}
	push ${default3}
	StdUtils::ScanStr3 /NOUNLOAD
	pop ${out1}
	pop ${out2}
	pop ${out3}
!macroend

!macro _StdUtils_TrimStr var
	push ${var}
	StdUtils::TrimStr /NOUNLOAD
	pop ${var}
!macroend

!macro _StdUtils_TrimStrLeft var
	push ${var}
	StdUtils::TrimStrLeft /NOUNLOAD
	pop ${var}
!macroend

!macro _StdUtils_TrimStrRight var
	push ${var}
	StdUtils::TrimStrRight /NOUNLOAD
	pop ${var}
!macroend

!macro _StdUtils_RevStr var
	push ${var}
	StdUtils::RevStr /NOUNLOAD
	pop ${var}
!macroend

!macro _StdUtils_SHFileMove out from to hwnd
	push '${from}'
	push '${to}'
	push ${hwnd}
	StdUtils::SHFileMove /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_SHFileCopy out from to hwnd
	push '${from}'
	push '${to}'
	push ${hwnd}
	StdUtils::SHFileCopy /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_ExecShlUser out file verb args
	push '${file}'
	push '${verb}'
	push '${args}'
	StdUtils::ExecShellAsUser /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_InvkeShlVrb out path file verb_id
	push "${path}"
	push "${file}"
	push ${verb_id}
	StdUtils::InvokeShellVerb /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_ExecShlWait out file verb args
	push '${file}'
	push '${verb}'
	push '${args}'
	StdUtils::ExecShellWait /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_WaitForProc handle
	push '${handle}'
	StdUtils::WaitForProc /NOUNLOAD
!macroend

!macro _StdUtils_GetParameter out name default
	push '${name}'
	push '${default}'
	StdUtils::GetParameter /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_GetAllParams out truncate
	push '${truncate}'
	StdUtils::GetAllParameters /NOUNLOAD
	pop ${out}
!macroend

!macro _StdUtils_SetVerbose on
	!if "${on}" != "0"
		StdUtils::EnableVerboseMode /NOUNLOAD
	!else
		StdUtils::DisableVerboseMode /NOUNLOAD
	!endif
!macroend


### MAGIC NUMBERS ###

!define StdUtils.Const.ISV_PinToTaskbar 5386
!define StdUtils.Const.ISV_UnpinFromTaskbar 5387
