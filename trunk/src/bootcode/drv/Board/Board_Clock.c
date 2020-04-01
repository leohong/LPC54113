// ==============================================================================
// FILE NAME: BOARD_CLOCK.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 28/11/2013, Leo Create
// --------------------
// ==============================================================================


#include "Board_Clock.h"

/* System configuration variables used by chip driver */
//const DWORD ExtRateIn = 0;
//const DWORD OscRateIn = 12000000;

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Structure for initial base clock states */
struct CLK_BASE_STATES
{
    CHIP_CGU_BASE_CLK_T clk;    /* Base clock */
    CHIP_CGU_CLKIN_T clkin;	    /* Base clock source, see UM for allowable souorces per base clock */
    bool autoblock_enab;	    /* Set to true to enable autoblocking on frequency change */
    bool powerdn;			    /* Set to true if the base clock is initially powered down */
};

static const struct CLK_BASE_STATES InitClkStates[] =
{
    /* Base clock */
    //                  /* Base clock source, see UM for allowable souorces per base clock */
    //                  //          /* Set to true to enable autoblocking on frequency change */
    //                  //          //      /* Set to true if the base clock is initially powered down */
    {CLK_BASE_SAFE,     CLKIN_IRC,  true,   false},
    {CLK_BASE_APB1,     CLKIN_IRC,  true,   false},
    {CLK_BASE_APB3,     CLKIN_IRC,  true,   false},
#if defined(CHIP_LPC43XX)
    {CLK_BASE_PERIPH,   CLKIN_IRC,  true,   false},
#endif
    {CLK_BASE_SSP0,     CLKIN_IRC,  true,   false},
    {CLK_BASE_SSP1,     CLKIN_IRC,  true,   false},
    {CLK_BASE_UART0,    CLKIN_IRC,  true,   false},
    {CLK_BASE_UART1,    CLKIN_IRC,  true,   false},
    {CLK_BASE_UART2,    CLKIN_IRC,  true,   false},
    {CLK_BASE_UART3,    CLKIN_IRC,  true,   false},
};

// ==============================================================================
// FUNCTION NAME: BOARD_CLOCK_INIT
// DESCRIPTION:
//
//
// Params:
// eBOARD_CLOCK eClock:
//
// Returns:
//
//
// modification history
// --------------------
// 28/11/2013, Leo Create
// --------------------
// ==============================================================================
#if 0
void Board_Clock_Init(eBOARD_CLOCK eClock)
{
    int i = 0;
    CHIP_CGU_CLKIN_T sClockIn = CLKIN_IRC;
    DWORD dwClockFreq = CGU_IRC_FREQ;

    switch(eClock)
    {
        case eBOARD_CLOCK_12MHZ:
            sClockIn = CLKIN_IRC;
            dwClockFreq = CGU_IRC_FREQ;
            break;

        case eBOARD_CLOCK_204MHZ:
            sClockIn = CLKIN_CRYSTAL;
            dwClockFreq = MAX_CLOCK_FREQ;
            break;

        default:
            sClockIn = CLKIN_IRC;
            dwClockFreq = CGU_IRC_FREQ;
            break;
    }

    Chip_SetupCoreClock(sClockIn, dwClockFreq, FALSE);

    if(CLKIN_CRYSTAL == sClockIn)
    {
        sClockIn = CLKIN_MAINPLL;
    }

    /* Setup system base clocks and initial states. This won't enable and
       disable individual clocks, but sets up the base clock sources for
       each individual peripheral clock. */
    for(i = 0; i < (sizeof(InitClkStates) / sizeof(InitClkStates[0])); i++)
    {
        Chip_Clock_SetBaseClock(InitClkStates[i].clk,
                                sClockIn,
                                InitClkStates[i].autoblock_enab,
                                InitClkStates[i].powerdn);
    }

    /* Update SystemCoreClock variable */
    SystemCoreClockUpdate();
}
#else
void Board_Clock_Init(eBOARD_CLOCK eClock)
{
    int i = 0x10000;
    int iDelayTime = 3000;
    CHIP_CGU_CLKIN_T sClockIn = CLKIN_IRC;

    __disable_irq();

    /* Switch main system clocking to IRC */
    Chip_Clock_SetBaseClock(CLK_BASE_MX, CLKIN_IRC, true, false);

    switch(eClock)
    {
        case eBOARD_CLOCK_12MHZ:
#if 0
            /* Enables the crystal oscillator */
            Chip_Clock_EnableCrystal();

            /* Wait 250 us to Enables the crystal oscillator */
            for(i = 0; i < iDelayTime; i++);

            /* Switch main system clocking to crystal */
            Chip_Clock_SetBaseClock(CLK_BASE_MX, CLKIN_CRYSTAL, true, false);

            sClockIn = CLKIN_CRYSTAL;

#else
            Chip_Clock_DisableCrystal();
            Chip_Clock_DisableMainPLL();
            sClockIn = CLKIN_IRC;
#endif /* 0 */
            break;

        case eBOARD_CLOCK_204MHZ:
        {
            /* Enables the crystal oscillator */
            Chip_Clock_EnableCrystal();

            /* Wait 250 us to Enables the crystal oscillator */
            for(i = 0; i < iDelayTime; i++);

            /* Switch main system clocking to crystal */
            Chip_Clock_SetBaseClock(CLK_BASE_MX, CLKIN_CRYSTAL, true, false);

            /* Setup PLL for 100MHz and switch main system clocking */
            Chip_Clock_SetupMainPLLHz(CLKIN_CRYSTAL, CRYSTAL_MAIN_FREQ_IN, 100 * 1000000, 100 * 1000000);

            /* Wait to PLL locked */
            while(CGU_PLL_LOCKED != Chip_Clock_MainPLLLocked());

            Chip_Clock_SetBaseClock(CLK_BASE_MX, CLKIN_MAINPLL, true, false);

            /* Wait 50 us to Connect to PLL */
            for(i = 0; i < iDelayTime; i++);

            /* Setup PLL for maximum clock */
            Chip_Clock_SetupMainPLLHz(CLKIN_CRYSTAL, CRYSTAL_MAIN_FREQ_IN, MAX_CLOCK_FREQ, MAX_CLOCK_FREQ);

            /* Wait to PLL locked */
            while(CGU_PLL_LOCKED != Chip_Clock_MainPLLLocked());

            sClockIn = CLKIN_MAINPLL;
        }
        break;

        default:
            sClockIn = CLKIN_IRC;
            break;
    }

    /* Setup system base clocks and initial states. This won't enable and
       disable individual clocks, but sets up the base clock sources for
       each individual peripheral clock. */
    for(i = 0; i < (sizeof(InitClkStates) / sizeof(InitClkStates[0])); i++)
    {
        Chip_Clock_SetBaseClock(InitClkStates[i].clk,
                                sClockIn,
                                InitClkStates[i].autoblock_enab,
                                InitClkStates[i].powerdn);
    }

    /* Update SystemCoreClock variable */
    SystemCoreClockUpdate();

    __enable_irq();
}
#endif /* 1 */

