@REM Script by balcanar @ https://www.computerbase.de/forum/threads/datum-in-format-jjjjmmtt-2-stellig-addieren.1456014/
@REM Angepasst von Luke Röper, <Luke.Roeper@augsburg.de>, Stadt Augsburg

@echo off

IF "%1"=="" goto :fehler
IF "%2"=="" goto :fehler
IF "%3"=="" goto :fehler

set jahr=%3%
set monat=%2%
set "tag=%1%"
@REM echo %jahr%.%monat%.%tag%

IF %tag%==10 goto :weiter
IF %tag%==20 goto :weiter
IF %tag%==30 goto :weiter
set "tagX=%tag%"
set tagx=%tagx:0=%
set /A tag=%tagX%
:weiter
@REM echo.

@REM pause

set /a Schaltjahr=%jahr%%%4
if %jahr:~-2%==00 set /a Schaltjahr+=%Jahr:~,2%%%4

set korrektur=false

IF %tag%==28 IF %Schaltjahr% neq 0 if %monat%==02 set monat=03& set tag=1& set korrektur=true
IF %tag%==29 IF %monat%==02 set monat=03& set tag=1& set korrektur=true

IF not %tag%==30 goto :notag30
IF %monat%==04 set monat=05& set tag=1& set korrektur=true
IF %monat%==06 set monat=07& set tag=1& set korrektur=true
IF %monat%==09 set monat=10& set tag=1& set korrektur=true
IF %monat%==11 set monat=12& set tag=1& set korrektur=true
:notag30

IF not %tag%==31 goto :notag31
IF %monat%==01 set monat=02& set tag=1& set korrektur=true
IF %monat%==03 set monat=04& set tag=1& set korrektur=true
IF %monat%==05 set monat=06& set tag=1& set korrektur=true
IF %monat%==08 set monat=09& set tag=1& set korrektur=true
IF %monat%==07 set monat=08& set tag=1& set korrektur=true
IF %monat%==10 set monat=11& set tag=1& set korrektur=true
IF %monat%==12 set monat=01& set tag=1& set /a jahr+=1& set korrektur=true
:notag31

IF "%korrektur%"=="true" goto :fertig
set /a tag+=1

:fertig
set "tagX=0%tag%"
set "tagX=%tagx:~-2%"
echo %jahr%%monat%%tagx% > dateplusone.temp
goto :beenden

:fehler
echo ParamFehler
goto :beenden

:beenden
@REM pause