// ==============================================================================
// FILE NAME: BOARD_EMC.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 15/05/2013, Leohong written
// --------------------
// ==============================================================================


#include "Board_Emc.h"
#include "utilLogAPI.h"
#include "mem_tests.h"


// 參考data sheet設定saram參數
//STATIC const IP_EMC_STATIC_CONFIG_T SRAM_config =
//	{
//	    0,                                      /*!< Chip select */
//	    EMC_STATIC_CONFIG_MEM_WIDTH_16 |        /*!< Configuration value */
//	    EMC_STATIC_CONFIG_CS_POL_ACTIVE_LOW |
//	    EMC_STATIC_CONFIG_BLS_HIGH,
//
//	    EMC_NANOSECOND(0),                      /*!< Write Enable Wait */
//	    EMC_NANOSECOND(0),                      /*!< Output Enable Wait */
//	    EMC_NANOSECOND(55),                     /*!< Read Wait */
//	    EMC_NANOSECOND(0),                      /*!< Page Access Wait */
//	    EMC_NANOSECOND(38),                     /*!< Write Wait */
//	    EMC_NANOSECOND(45)                      /*!< Turn around wait */
//	};

STATIC const IP_EMC_DYN_CONFIG_T m_sSdram_config =  // SDRAM W9825G6JB-6
{
    EMC_NANOSECOND(64000000 / 8192),	/* Row refresh time                                         ????*/  // 8K Refresh Cycles / 64ms
    0x01,	                            /* Command Delayed                                          ????*/  // Remark: Choose command delay strategy (RD = 0x1) for SDRAM operation
    EMC_NANOSECOND(15),                 /* tRP      Precharge Command Period                        ????*/  // tRP  15
    EMC_NANOSECOND(42),                 /* tRAS     Active to Precharge Command Period              ????*/  // tRAS 42
    EMC_NANOSECOND(72),                 /* tSREX    Self Refresh Exit Time                          ????*/  // tXSR 72
    EMC_CLOCK(0x01),                    /* tAPR     Last Data Out to Active Time                    ????*/
    EMC_CLOCK(0x05),                    /* tDAL     Data In to Active Command Time                  ????*/
    EMC_NANOSECOND(2),                  /* tWR      Write Recovery Time                             ????*/  // tWR  2
    EMC_NANOSECOND(60),                 /* tRC      Active to Active Command Period                 ????*/  // tRC  60
    EMC_NANOSECOND(64),                 /* tRFC     Auto-refresh Period                             ????*/  // tRFC 64
    EMC_NANOSECOND(72),                 /* tXSR     Exit Selt Refresh                               ????*/  // tXSR 72
    EMC_NANOSECOND(2),                  /* tRRD     Active Bank A to Active Bank B Time             ????*/  // tRRD 2tck
    EMC_CLOCK(0x02),                    /* tMRD     Load Mode register command to Active Command    ????*/  // tRSC 2
    {
        {
            EMC_ADDRESS_DYCS0,	/* Keil Board uses DYCS0 for SDRAM */
            3,	/* RAS */

            EMC_DYN_MODE_WBMODE_PROGRAMMED |
            EMC_DYN_MODE_OPMODE_STANDARD |
            EMC_DYN_MODE_CAS_3 |
            EMC_DYN_MODE_BURST_TYPE_SEQUENTIAL |
            EMC_DYN_MODE_BURST_LEN_8,

            EMC_DYN_CONFIG_DATA_BUS_16 |
            //EMC_DYN_CONFIG_LPSDRAM |
            EMC_DYN_CONFIG_16Mx16_4BANKS_13ROWS_9COLS |
            EMC_DYN_CONFIG_MD_SDRAM
        },
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
};

#define CLK0_DELAY 7

// ==============================================================================
// FUNCTION NAME: BOARD_EMC_INIT
// DESCRIPTION:
//
// 初始化EMC controller
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 15/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_EMC_Init(void)
{
#if 0
    Chip_RGU_TriggerReset(RGU_EMC_RST);

    while(Chip_RGU_InReset(RGU_EMC_RST)) {}

    /* Setup EMC Clock Divider for divide by 2 - this is done in both the CCU (clocking)
       and CREG. For frequencies over 120MHz, a divider of 2 must be used. For frequencies
       less than 120MHz, a divider of 1 or 2 is ok. */
#if 1
    Chip_Clock_EnableOpts(CLK_MX_EMC_DIV, ENABLE, ENABLE, 2);
    LPC_CREG->CREG6 |= (1 << 16);
#else
    Chip_Clock_EnableOpts(CLK_MX_EMC, ENABLE, ENABLE, 1);
#endif
    /* Enable EMC clock */
    Chip_Clock_Enable(CLK_MX_EMC);

    /* Init EMC Controller -Enable-LE mode */
    Chip_EMC_Init(ENABLE, 0, 0);

    /* Init EMC Static Controller CS0 */
    Chip_EMC_Static_Init((IP_EMC_STATIC_CONFIG_T *) &SRAM_config);

    /* Enable Buffer for External Flash */
    LPC_EMC->STATICCONFIG0 |= 1 << 19;


#else
    Chip_RGU_TriggerReset(RGU_EMC_RST);

    while(Chip_RGU_InReset(RGU_EMC_RST)) {}

    /* Move all clock delays together */
    LPC_SCU->EMCDELAYCLK = ((CLK0_DELAY) | (CLK0_DELAY << 4) | (CLK0_DELAY << 8) | (CLK0_DELAY << 12));

    /* Setup EMC Clock Divider for divide by 2 - this is done in both the CCU (clocking)
       and CREG. For frequencies over 120MHz, a divider of 2 must be used. For frequencies
       less than 120MHz, a divider of 1 or 2 is ok. */
    Chip_Clock_EnableOpts(CLK_MX_EMC_DIV, ENABLE, ENABLE, 2);
    LPC_CREG->CREG6 |= (1 << 16);

    /* Enable EMC clock */
    Chip_Clock_Enable(CLK_MX_EMC);

    /* Init EMC Controller -Enable-LE mode */
    Chip_EMC_Init(ENABLE, 0, 0);

    /* Init EMC Dynamic Controller */
    Chip_EMC_Dynamic_Init((IP_EMC_DYN_CONFIG_T *) &m_sSdram_config);

    /* Enable Buffer for External Flash */
    //LPC_EMC->DYNAMICCONFIG0 |= EMC_CONFIG_BUFFER_ENABLE;
    Chip_EMC_Dynamic_Enable(1);

    /* Init EMC Static Controller CS0 */
    //Chip_EMC_Static_Init((IP_EMC_STATIC_CONFIG_T *) &SRAM_config);

    /* Enable Buffer for External Flash */
    //LPC_EMC->STATICCONFIG0 |= EMC_CONFIG_BUFFER_ENABLE;
#endif /* 0 */


}

// ==============================================================================
// FUNCTION NAME: BOARD_EMC_DEINIT
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 28/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_EMC_DeInit(void)
{
    /* Init EMC Controller -Enable-LE mode */
    Chip_EMC_Init(DISABLE, 0, 0);

    /* Enable EMC clock */
    Chip_Clock_Disable(CLK_MX_EMC);
}


#ifdef MEM_TEST
#define SRAM_BASE_ADDRESS (uint32_t *) EMC_ADDRESS_DYCS0

#define DRAM_SIZE (32 * 1024 * 1024)

// ==============================================================================
// FUNCTION NAME: BOARD_EMC_TEST
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 15/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_EMC_Test(void)
{
    MEM_TEST_SETUP_T memSetup;
    LOG_MSG(eDB_MSK_LIST_ASSERT, "Memory test Start...\r\n");

    /* Walking 0 test */
    LOG_MSG(eDB_MSK_LIST_ASSERT, "Walking 0 memory test\r\n");
    memSetup.start_addr = SRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;

    if(mem_test_walking0(&memSetup))
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Walking 0 memory test passed\r\n");
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Walking 0 memory test failed at address %p\r\n", memSetup.fail_addr);
        LOG_MSG(eDB_MSK_LIST_ASSERT, " Expected %08x, actual %08x\r\n", memSetup.ex_val, memSetup.is_val);
    }

    /* Walking 1 test */
    LOG_MSG(eDB_MSK_LIST_ASSERT, "Walking 1 memory test\r\n");
    memSetup.start_addr = SRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;

    if(mem_test_walking1(&memSetup))
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Walking 1 memory test passed\r\n");
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Walking 1 memory test failed at address %p\r\n", memSetup.fail_addr);
        LOG_MSG(eDB_MSK_LIST_ASSERT, " Expected %08x, actual %08x\r\n", memSetup.ex_val, memSetup.is_val);
    }

    /* Address test */
    LOG_MSG(eDB_MSK_LIST_ASSERT, "Address test\r\n");
    memSetup.start_addr = SRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;

    if(mem_test_address(&memSetup))
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Address test passed\r\n");
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Address test failed at address %p\r\n", memSetup.fail_addr);
        LOG_MSG(eDB_MSK_LIST_ASSERT, " Expected %08x, actual %08x\r\n", memSetup.ex_val, memSetup.is_val);
    }

    /* Inverse address test */
    LOG_MSG(eDB_MSK_LIST_ASSERT, "Inverse address test\r\n");
    memSetup.start_addr = SRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;

    if(mem_test_invaddress(&memSetup))
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Inverse address test passed\r\n");
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Inverse address test failed at address %p\r\n", memSetup.fail_addr);
        LOG_MSG(eDB_MSK_LIST_ASSERT, " Expected %08x, actual %08x\r\n", memSetup.ex_val, memSetup.is_val);
    }

    /* Pattern test */
    LOG_MSG(eDB_MSK_LIST_ASSERT, "Pattern (0x55/0xAA) test\r\n");
    memSetup.start_addr = SRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;

    if(mem_test_pattern(&memSetup))
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Pattern (0x55/0xAA) test passed\r\n");
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Pattern (0x55/0xAA) test failed at address %p\r\n", memSetup.fail_addr);
        LOG_MSG(eDB_MSK_LIST_ASSERT, " Expected %08x, actual %08x\r\n", memSetup.ex_val, memSetup.is_val);
    }

    /* Seeded pattern test */
    LOG_MSG(eDB_MSK_LIST_ASSERT, "Seeded pattern test\r\n");
    memSetup.start_addr = SRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;

    if(mem_test_pattern_seed(&memSetup, 0x12345678, 0x50005))
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Seeded pattern test passed\r\n");
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Seeded pattern test failed at address %p\r\n", memSetup.fail_addr);
        LOG_MSG(eDB_MSK_LIST_ASSERT, " Expected %08x, actual %08x\r\n", memSetup.ex_val, memSetup.is_val);
    }
}
#endif /* MEM_TEST */


