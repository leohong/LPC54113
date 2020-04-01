// ===============================================================================
// FILE NAME: Board_Watchdog.c
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2015/03/19, Leo Create
// --------------------
// ===============================================================================


#include "Board.h"
#include "Board_SPIFI.h"
#include "Board_Gpio.h"

#define SPIFLASH_BASE_ADDRESS (0x14000000)
#define TEST_BUFFSIZE (4096)
#define TICKRATE_HZ1 (1000)	/* 1000 ticks per second I.e 1 mSec / tick */

static DWORD lmem[21];

SPIFI_HANDLE_T *pSpifi;
SPIFI_ERR_T errCode;

static DWORD CalculateDivider(DWORD baseClock, DWORD target)
{
	DWORD divider = (baseClock / target);

	/* If there is a remainder then increment the dividor so that the resultant
	   clock is not over the target */
	if(baseClock % target) {
		++divider;
	}
	return divider;
}


static SPIFI_HANDLE_T *initializeSpifi(void)
{
	DWORD memSize;
	SPIFI_HANDLE_T *pReturnVal;

	/* Initialize LPCSPIFILIB library, reset the interface */
	spifiInit(LPC_SPIFI_BASE, true);

	/* register support for the family(s) we may want to work with
	     (only 1 is required) */
    spifiRegisterFamily(spifi_REG_FAMILY_CommonCommandSet);

    /* Enumerate the list of supported devices */
    {
        SPIFI_DEV_ENUMERATOR_T ctx;
        const char * devName = spifiDevEnumerateName(&ctx, 1);

        while (devName) {
            devName = spifiDevEnumerateName(&ctx, 0);
        }
    }

	/* Get required memory for detected device, this may vary per device family */
	memSize = spifiGetHandleMemSize(LPC_SPIFI_BASE);
	if (memSize == 0) {
		/* No device detected, error */
		//fatalError("spifiGetHandleMemSize", SPIFI_ERR_GEN);
	}

	/* Initialize and detect a device and get device context */
	/* NOTE: Since we don't have malloc enabled we are just supplying
	     a chunk of memory that we know is large enough. It would be
	     better to use malloc if it is available. */
	pReturnVal = spifiInitDevice(&lmem, sizeof(lmem), LPC_SPIFI_BASE, SPIFLASH_BASE_ADDRESS);
	if (pReturnVal == NULL) {
		//fatalError("spifiInitDevice", SPIFI_ERR_GEN);
	}
	return pReturnVal;
}


void Board_SPIFI_Init(void)
{
	DWORD spifiBaseClockRate;
	DWORD maxSpifiClock;
	//WORD  libVersion;

	/* Report the library version to start with */
	//libVersion = spifiGetLibVersion();

	///* Setup SPIFI FLASH pin muxing (QUAD) */
	//Chip_SCU_SetPinMuxing(spifipinmuxing, sizeof(spifipinmuxing) / sizeof(PINMUX_GRP_T));

	/* SPIFI base clock will be based on the main PLL rate and a divider */
	spifiBaseClockRate = Chip_Clock_GetClockInputHz(CLKIN_MAINPLL);
	//LOG_MSG(eDB_MSK_LIST_ASSERT, "spifiBaseClockRate %d\r\n", spifiBaseClockRate); /// debug

	/* Setup SPIFI clock to run around 1Mhz. Use divider E for this, as it allows
	   higher divider values up to 256 maximum) */
	//Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, CalculateDivider(spifiBaseClockRate, 1000000));
	//Chip_Clock_SetBaseClock(CLK_BASE_SPIFI, CLKIN_IDIVE, true, false);
	//DEBUGOUT("SPIFI clock rate %d\r\n", Chip_Clock_GetClockInputHz(CLKIN_IDIVE));

	/* Initialize the spifi library. This registers the device family and detects the part */
	pSpifi = initializeSpifi();

	/* Get some info needed for the application */
	maxSpifiClock = spifiDevGetInfo(pSpifi, SPIFI_INFO_MAXCLOCK);

	/* Get info */

	/* Setup SPIFI clock to at the maximum interface rate the detected device
	   can use. This should be done after device init. */
	Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, CalculateDivider(spifiBaseClockRate, maxSpifiClock));

	//DEBUGOUT("SPIFI final Rate    = %d\r\n", Chip_Clock_GetClockInputHz(CLKIN_IDIVE));
	//DEBUGOUT("\r\n");

	/* Enter memMode */
	//spifiDevSetMemMode(pSpifi, true);
    spifiDevDeInit(pSpifi);

}

// ==============================================================================
// FUNCTION NAME: Board_SPIFI_MEMMode_Set
// DESCRIPTION:
//
//
// Params:
// BOOL bEnable:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/11/13, Larry Create
// --------------------
// ==============================================================================
void Board_SPIFI_MEMMode_Set(BOOL bEnable)
{
    if(bEnable)
    {
        spifiDevDeInit(pSpifi);
    }
    else
    {
        spifiDevInit(pSpifi);
    }
}

void Board_SPIFI_Read(unsigned int READ_ADDRESS, void *pvData, DWORD dwSize)
{
	DWORD pageAddress;
	DWORD loopBytes;
	DWORD bytesRemaining;

	/* Get the maximum amount we can write and check against our buffer.
	     If larger, restrict to our buffer size */
	loopBytes = spifiDevGetInfo(pSpifi, SPIFI_INFO_PAGESIZE);
	if (loopBytes > dwSize) {
		loopBytes = dwSize;
	}//256 BYTE

	pageAddress = spifiGetAddrFromBlock(pSpifi, 0);
	bytesRemaining = spifiDevGetInfo(pSpifi, SPIFI_INFO_ERASE_BLOCKSIZE);

	/* Read the sector and validate it using DevRead api*/
	//DEBUGOUT("Verifying Sector 0...\r\n");
	while (bytesRemaining) {
		if (loopBytes > bytesRemaining) {
			loopBytes = bytesRemaining;
		}

		errCode = spifiDevRead(pSpifi, READ_ADDRESS, pvData, loopBytes);
		if (errCode != SPIFI_ERR_NONE) {
			//fatalError("WriteBlock 0", errCode);
		}
		bytesRemaining -= loopBytes;
		pageAddress += loopBytes;
	}

	/* Done, de-init will enter memory mode */
	spifiDevDeInit(pSpifi);
}

// ==============================================================================
// FUNCTION NAME: Board_SPIFI_GetInfo_MAXREADSIZE
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/11/13, Larry Create
// --------------------
// ==============================================================================
UINT32 Board_SPIFI_GetInfo_MaxSize(void)
{
    return spifiDevGetInfo(pSpifi, SPIFI_INFO_MAXREADSIZE);
}

// ==============================================================================
// FUNCTION NAME: Board_SPIFI_GetInfo_BLOCKS_Number
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/11/13, Larry Create
// --------------------
// ==============================================================================
UINT32 Board_SPIFI_GetInfo_BlockNumber(void)
{
    return spifiDevGetInfo(pSpifi, SPIFI_INFO_ERASE_BLOCKS);
}

// ==============================================================================
// FUNCTION NAME: Board_SPIFI_GetInfo_BlockSize
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/11/15, Larry Create
// --------------------
// ==============================================================================
UINT32 Board_SPIFI_GetInfo_BlockSize(void)
{
    return spifiDevGetInfo(pSpifi, SPIFI_INFO_ERASE_BLOCKSIZE);
}

// ==============================================================================
// FUNCTION NAME: Board_SPIFI_GetInfo_Address
// DESCRIPTION:
//
//
// Params:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/11/13, Larry Create
// --------------------
// ==============================================================================
UINT32 Board_SPIFI_GetInfo_Address(UINT32 ulNumberBlocks)
{
    return spifiGetAddrFromBlock(pSpifi, ulNumberBlocks);
}

// ==============================================================================
// FUNCTION NAME: Board_SPIFI_GetInfo_PageSize
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/11/15, Larry Create
// --------------------
// ==============================================================================
UINT32 Board_SPIFI_GetInfo_PageSize(void)
{
    return spifiDevGetInfo(pSpifi, SPIFI_INFO_PAGESIZE);
}

// ==============================================================================
// FUNCTION NAME: Board_SPIFI_GetInfo_EraseBlocks
// DESCRIPTION:
//
//
// Params:
// UINT32 ulFirstBlock:
// UINT32 ulNumberBlocks:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/11/13, Larry Create
// --------------------
// ==============================================================================
UINT8 Board_SPIFI_EraseBlocks(UINT32 ulFirstBlock, UINT32 ulNumberBlocks)
{
    return spifiErase(pSpifi, ulFirstBlock, ulNumberBlocks);
}

// ==============================================================================
// FUNCTION NAME: Board_SPIFI_Write
// DESCRIPTION:
//
//
// Params:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/11/13, Larry Create
// --------------------
// ==============================================================================
UINT8 Board_SPIFI_Write(UINT32 ulAddr, UINT32 ulSize, UINT8 *pucData)
{
    return spifiDevPageProgram(pSpifi, ulAddr, (UINT32*)pucData, ulSize);
}



