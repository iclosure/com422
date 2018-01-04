@echo off

echo buildex.bat: build bat file

set BuildEvent=%1
set ProjectDir=%2
set SourceDir=%3
set TargetName=%4
set ConfigType=%5
set SystemName=%6
set PlatformName=%7
set ConfigTypeFolder=%8
set TargetFolder=%9

set SymbDirectory=%SourceDir%
set ServerPath=\\CLOSURE\symbols
set SymbVersion=1.0

if "%BuildEvent%" == "-pre" (

	echo %BuildEvent%
	
) else if "%BuildEvent%" == "-post" (

	if exist "%ProjectDir%\..\%ConfigTypeFolder%\crdcomprop.dll" (
		copy "%ProjectDir%\..\%ConfigTypeFolder%\crdcomprop.dll" "%TargetFolder%"
	) else if exist "%ProjectDir%\..\Unicode_Debug\crdcomprop.dll" (
		copy "%ProjectDir%\..\Unicode_Debug\crdcomprop.dll" "%TargetFolder%"
	) else if exist "%ProjectDir%\..\Unicode_Release\crdcomprop.dll" (
		copy "%ProjectDir%\..\Unicode_Release\crdcomprop.dll" "%TargetFolder%"
	)

	echo ----------------------------
	rem %1 i386 directory; %2 symbol server path; %3 version
	echo symbols server path: %ServerPath%

	if exist %ServerPath% (
		if exist "%WDKPATH%\Debuggers\symstore.exe" (
			"%WDKPATH%\Debuggers\symstore.exe" add /r /f %SymbDirectory% /s %ServerPath% /t "closure.dev" /v "%SymbVersion%"
		)

		if exist "C:\Program Files\Debugging Tools for Windows (x86)\symstore.exe" (
			"C:\Program Files\Debugging Tools for Windows (x86)\symstore.exe" add /r /f %SymbDirectory% /s %ServerPath% /t "closure.dev" /v "%SymbVersion%"
		)

		if exist "C:\Program Files\Debugging Tools for Windows (x64)\symstore.exe" (
			"C:\Program Files\Debugging Tools for Windows (x64)\symstore.exe" add /r /f %SymbDirectory% /s %ServerPath% /t "closure.dev" /v "%SymbVersion%"
		)
	) else (
		echo Server path %ServerPath% is not exist!
	)

	echo ----------------------------

) else (

	echo not supported BuildEvent ( '%BuildEvent%' )
	goto exit
	
)

goto exit

:exit
exit /b 0
