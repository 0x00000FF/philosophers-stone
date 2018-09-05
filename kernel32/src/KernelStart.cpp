// PS64 Kernel Start

#include <Types.hpp>
#include <Constants.hpp>

const void* VGABUFFER    =  (const void*)  0xB8000;
      
DWORD KernelPrints          (const int, const int, const char*, const BYTE = 0x07);
void  KernelMessagePrint    (const int, const int, const char*, const bool);
bool  KernelCheckMemorySize ();
bool  KernelInit64Area      ();
void  KernelStop            ();

void KernelStart() {
    KernelPrints(0, 0, "PHILOSOPHER'S STONE :: DEV ALPHA, 0.1", CON_MAGENTA);
    KernelPrints(0, 1, "=====================================");

    KernelPrints(0, 2, "32Bit Protected Mode SUCCESS!!", CON_LIGHT_GREEN);

    bool isMemoryEnough = KernelCheckMemorySize();
    KernelMessagePrint(0, 3, "Minimum Memory Requirement Check....", isMemoryEnough);
    if (!isMemoryEnough) {
        KernelPrints(0, 4, "* Memory size check failed, PS64 requires 64MB or larger system memory.", CON_LIGHT_RED);
        KernelStop();
    }

    KernelMessagePrint(0, 4, "IA-32e Kernel Area Initialization...", KernelInit64Area());
    KernelMessagePrint(0, 5, "IA-32e Page Table Initialization....", false);

    KernelPrints(0, 8, "SYSTEM FAILURE", _CONSOLE_ATTRIBUTE(CON_WHITE, CON_LIGHT_RED));
    KernelStop();
}

DWORD KernelPrints(
        const int    xPos, 
        const int    yPos, 
        const char*  str,
        const BYTE   color 
     ) {
    DWORD      i      = 0;
    CHARACTER* screen = (CHARACTER*) VGABUFFER;
    
    screen += (yPos * 80) + xPos;
    for (i = 0; str[ i ]; ++i ) {
        screen[ i ].Character = str[ i ];
        screen[ i ].Attribute = color;
    }
    
    return i + 1;
}

void KernelMessagePrint (
        const int    xPos,
        const int    yPos,
        const char*  msg,
        const bool   status
    ) {
    auto printedCount = KernelPrints(xPos, yPos, msg);
    KernelPrints(xPos + printedCount, yPos, "[");
    
    if (status) {
        KernelPrints(xPos + printedCount + 1, yPos, " OKAY ", CON_LIGHT_GREEN);
    } else {
        KernelPrints(xPos + printedCount + 1, yPos, "FAILED", CON_LIGHT_RED);
    }

    KernelPrints(xPos + printedCount + 7, yPos, "]");
}

bool KernelCheckMemorySize() {
    DWORD*  currentAddress = ( DWORD * ) 0x100000;

    while ( ( DWORD ) currentAddress < 0x4000000 ) {
        *currentAddress = 0xABCDEFAB;

        if (*currentAddress ^ 0xABCDEFAB) {
            return false;
        }

        currentAddress += 0x100000 >> 2;
    }

    return true;
}

bool KernelInit64Area() {
    DWORD*  currentAddress = ( DWORD* ) 0x100000;

    while ( ( DWORD ) currentAddress < 0x600000 ) {
        *currentAddress = 0x00;

        if ( *currentAddress ) {
            return false;    
        }

        ++currentAddress;
    }
    
    return true;
}

void KernelStop() {
    while (true) ;
}
