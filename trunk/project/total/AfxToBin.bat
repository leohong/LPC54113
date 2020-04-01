SET FileName=%1

rd /q/s ..\..\FwFiles\%FileName%\BinFile\
rd /q/s ..\..\FwFiles\%FileName%\HexFile\
rd /q/s ..\..\FwFiles\%FileName%\AxfFile\

md ..\..\FwFiles\%FileName%\BinFile\
md ..\..\FwFiles\%FileName%\HexFile\
md ..\..\FwFiles\%FileName%\AxfFile\

rem dir  ..\appcode\Objects\%FileName%.hex
COPY ..\appcode\Objects\%FileName%.hex ..\..\FwFiles\%FileName%\HexFile\

rem dir  ..\appcode\Objects\%FileName%.axf
COPY ..\appcode\Objects\%FileName%.axf ..\..\FwFiles\%FileName%\AxfFile\

C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output  ..\..\FwFiles\%FileName%\BinFile\%FileName%.bin ..\appcode\Objects\%FileName%.axf