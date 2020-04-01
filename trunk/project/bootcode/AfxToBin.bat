SET CORE=%1
SET FileName=%2

rd /q/s ..\..\FwFiles\%FileName%\
md ..\..\FwFiles\%FileName%\

copy .\Objects\CORE_%CORE%\%FileName%.hex ..\..\FwFiles\%FileName%\%FileName%.hex
copy .\Objects\CORE_%CORE%\%FileName%.axf ..\..\FwFiles\%FileName%\%FileName%.axf
copy .\Objects\CORE_%CORE%\%FileName%.htm ..\..\FwFiles\%FileName%\%FileName%.htm
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output  ..\..\FwFiles\%FileName%\%FileName%.bin .\Objects\CORE_%CORE%\%FileName%.axf
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output  ..\..\src\Configuration_Files\%FileName%.bin .\Objects\CORE_%CORE%\%FileName%.axf
