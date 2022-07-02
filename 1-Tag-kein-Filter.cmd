@echo off
set /P day=Der Tag (Format XX.XX.XXXX): 

pushd temp-csv-export\
del *.csv
popd

pushd code-source\
LogMaster-MTA.exe -System=0 -ExecMode=X1 -SourceFiles=\\SERVER\SHARE\temp\ -Date=%day% -Time=DAY
ren LogMaster-MTA.?????????.csv LogMaster-MTA.????????.nofilter.csv
popd
move /Y code-source\*.csv temp-csv-export\
)
