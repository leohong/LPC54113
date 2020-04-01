set CORE=%1

del ..\..\src\drv\board\chip\lib_lpc_chip_43xx_%CORE%.lib
copy .\keil_output\core_%CORE%\lib_lpc_chip_43xx_%CORE%.lib ..\..\src\drv\board\chip\
