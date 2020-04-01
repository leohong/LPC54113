    ; Instructions, pseudo-instructions, and directives must be preceded by white space, such as a space or a tab
    AREA BOOT_LOADER, CODE, READONLY
    INCBIN .\bootcode.bin
    END
