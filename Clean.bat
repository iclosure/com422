
@echo off

title 清除工程垃圾文件

echo.
echo clean.bat: Deleting output
echo.

rem ****************************************
rem   variables
rem ****************************************

% 项目目录 %
SET BASE_PATH=%CD%

% 工程文件夹名 %
SET PROJ_NAMES=^
., ^
ComApi, ^
ComDriverBus, ^
ComDriverPort, ^
ComDriverProp, ^
ComTest, ^
ComTool, ^
Setup, ^
package/com422

% 目标文件夹名 %
SET TARGET_FOLDER_NAMES=^
ipch, ^
debug, ^
release, ^
unicode_debug, ^
unicode_release, ^
objchk_wxp_x86, ^
objchk_wlh_x86, ^
objchk_win7_x86, ^
objchk_wxp_x64, ^
objchk_wlh_x64, ^
objchk_win7_x64, ^
objfre_wxp_x86, ^
objfre_wlh_x86, ^
objfre_win7_x86, ^
objfre_wxp_x64, ^
objfre_wlh_x64, ^
objfre_win7_x64, ^
obj, ^
SmartDriver_Debug

% 目标文件后缀名 %
SET TARGET_FILE_SUFFIXS=^
ncb, ^
suo,^
sdf, ^
opensdf, ^
user, ^
map, ^
log, ^
wrn, ^
err, ^
aps

rem ****************************************
rem   Delete working folders
rem ****************************************

echo.
echo clean.bat: Deleting working folders
echo.

for %%I in ( %PROJ_NAMES% ) do (
	if exist %BASE_PATH%\%%I (
		cd %BASE_PATH%\%%I
		for %%J in ( %TARGET_FOLDER_NAMES% ) do (
			if exist %%J (
				echo Delete folder: %cd%\%%J 
				rd /q /s %%J
			)
		)
	)
)

rem ****************************************
rem   Delete files
rem ****************************************

echo.
echo clean.bat: Deleting files
echo.

for %%I in ( %PROJ_NAMES% ) do (
	if exist %BASE_PATH%\%%I (
		cd %BASE_PATH%\%%I
		for %%J in ( %TARGET_FILE_SUFFIXS% ) do ( 
			for %%K in ( *.%%J ) do (
				if exist %%K (
					echo Delete file: %cd%\%%K
					del /a /q /s /f %%K
				)
			)
		)
	)
)

goto exit

:pause
pause

:exit
exit /b 0