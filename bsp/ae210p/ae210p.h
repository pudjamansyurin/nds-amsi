/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */

#ifndef __AE210P_H__
#define __AE210P_H__

#ifndef __ASSEMBLER__
#include <inttypes.h>
#include <nds32_intrinsic.h>
#endif

/*****************************************************************************
 * System clock
 ****************************************************************************/
#define KHz                     1000
#define MHz                     1000000

#define OSCFREQ                 (20 * MHz)
#define CPUFREQ                 (40 * MHz)
#define HCLKFREQ                (CPUFREQ)
#define PCLKFREQ                (CPUFREQ)
#define UCLKFREQ                (OSCFREQ)

/*****************************************************************************
 * IRQ Vector
 ****************************************************************************/
#define IRQ_RTCPERIOD_VECTOR    0
#define IRQ_RTCALARM_VECTOR     1
#define IRQ_PIT_VECTOR          2
#define IRQ_SPI1_VECTOR         3
#define IRQ_SPI2_VECTOR         4
#define IRQ_I2C_VECTOR          5
#define IRQ_GPIO_VECTOR         6
#define IRQ_UART1_VECTOR        7
#define IRQ_UART2_VECTOR        8
#define IRQ_DMA_VECTOR          9
#define IRQ_BMC_VECTOR          10
#define IRQ_SWI_VECTOR          11
#define IRQ_LDMA_VECTOR         12
#define IRQ_EXTINT0_VECTOR      13
#define IRQ_EXTINT1_VECTOR      14
#define IRQ_EXTINT2_VECTOR      15
#define IRQ_EXTINT3_VECTOR      16
#define IRQ_EXTINT4_VECTOR      17
#define IRQ_EXTINT5_VECTOR      18
#define IRQ_EXTINT6_VECTOR      19
#define IRQ_EXTINT7_VECTOR      20
#define IRQ_EXTINT8_VECTOR      21
#define IRQ_EXTINT9_VECTOR      22
#define IRQ_EXTINT10_VECTOR     23
#define IRQ_EXTINT11_VECTOR     24
#define IRQ_EXTINT12_VECTOR     25
#define IRQ_EXTINT13_VECTOR     26
#define IRQ_EXTINT14_VECTOR     27
#define IRQ_EXTINT15_VECTOR     28
#define IRQ_EXTINT16_VECTOR     29
#define IRQ_EXTINT17_VECTOR     30
#define IRQ_EXTINT18_VECTOR     31

#ifndef __ASSEMBLER__

/*****************************************************************************
 * Device Specific Peripheral Registers structures
 ****************************************************************************/

#define __I                     volatile const	/* 'read only' permissions      */
#define __O                     volatile        /* 'write only' permissions     */
#define __IO                    volatile        /* 'read / write' permissions   */

/*****************************************************************************
 * DMAC - AE210P
 ****************************************************************************/
typedef struct {
	__IO uint32_t  CTRL;                    // DMA Channel Control Register
	__IO uint32_t  SRCADDR;                 // DMA Channel Source Address Register
	__IO uint32_t  DSTADDR;                 // DMA Channel Destination Address Register
	__IO uint32_t  TRANSIZE;                // DMA Channel Transfer Size Register
	__IO uint32_t  LLP;                     // DMA Channel Linked List Pointer Register
} DMA_CHANNEL_REG;

typedef struct {
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register */
	     unsigned int RESERVED0[3];         /* 0x04 ~ 0x0C Reserved */
	__I  unsigned int DMACFG;               /* 0x10 DMA Configure Register */
	     unsigned int RESERVED1[3];         /* 0x14 ~ 0x1C Reserved */
	__IO unsigned int DMACTRL;              /* 0x20 DMA Control Register */
	     unsigned int RESERVED2[3];         /* 0x24 ~ 0x2C Reserved */
	__IO unsigned int INTSTATUS;            /* 0x30 Interrupt Status Register */
	     unsigned int RESERVED3[3];         /* 0x34 ~ 0x3C Reserved */
	__IO unsigned int CHABORT;              /* 0x40 DMA Control Register */
	DMA_CHANNEL_REG   CHANNEL[8];           /* 0x44 ~ 0x54 Channel #n Registers */
} AE210P_DMA_RegDef;

/*****************************************************************************
 * UARTx - AE210P
 ****************************************************************************/
typedef struct {
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register */
	     unsigned int RESERVED0[3];         /* 0x04 ~ 0x0C Reserved */
	__I  unsigned int CFG;                  /* 0x10 Hardware Configure Register */
	__IO unsigned int OSCR;                 /* 0x14 Over Sample Control Register */
	     unsigned int RESERVED1[2];         /* 0x18 ~ 0x1C Reserved */
	union {
		__IO unsigned int RBR;          /* 0x20 Receiver Buffer Register */
		__O  unsigned int THR;          /* 0x20 Transmitter Holding Register */
		__IO unsigned int DLL;          /* 0x20 Divisor Latch LSB */
	};
	union {
		__IO unsigned int IER;          /* 0x24 Interrupt Enable Register */
		__IO unsigned int DLM;          /* 0x24 Divisor Latch MSB */
	};
	union {
		__IO unsigned int IIR;          /* 0x28 Interrupt Identification Register */
		__O  unsigned int FCR;          /* 0x28 FIFO Control Register */
	};
	__IO unsigned int LCR;                  /* 0x2C Line Control Register */
	__IO unsigned int MCR;                  /* 0x30 Modem Control Register */
	__IO unsigned int LSR;                  /* 0x34 Line Status Register */
	__IO unsigned int MSR;                  /* 0x38 Modem Status Register */
	__IO unsigned int SCR;                  /* 0x3C Scratch Register */
} AE210P_UART_RegDef;

/*****************************************************************************
 * PIT - AE210P
 ****************************************************************************/
typedef struct {
	__IO uint32_t  CTRL;                    // PIT Channel Control Register
	__IO uint32_t  RELOAD;                  // PIT Channel Reload Register
	__IO uint32_t  COUNTER;                 // PIT Channel Counter Register
	__IO uint32_t  RESERVED[1];
} PIT_CHANNEL_REG;

typedef struct {
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register */
	     unsigned int RESERVED[3];          /* 0x04 ~ 0x0C Reserved */
	__I  unsigned int CFG;                  /* 0x10 Configuration Register */
	__IO unsigned int INTEN;                /* 0x14 Interrupt Enable Register */
	__IO unsigned int INTST;                /* 0x18 Interrupt Status Register */
	__IO unsigned int CHNEN;                /* 0x1C Channel Enable Register */
	PIT_CHANNEL_REG   CHANNEL[4];           /* 0x20 ~ 0x50 Channel #n Registers */
} AE210P_PIT_RegDef;

/*****************************************************************************
 * WDT - AE210P
 ****************************************************************************/
typedef struct {
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register */
	     unsigned int RESERVED[3];          /* 0x04 ~ 0x0C Reserved */
	__IO unsigned int CTRL;                 /* 0x10 Control Register */
	__O  unsigned int RESTART;              /* 0x14 Restart Register */
	__O  unsigned int WREN;                 /* 0x18 Write Enable Register */
	__IO unsigned int ST;                   /* 0x1C Status Register */
} AE210P_WDT_RegDef;

/*****************************************************************************
 * RTC - AE210P
 ****************************************************************************/
typedef struct _AE210P_RTC_RegDef
{
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register */
	     unsigned int RESERVED[3];          /* 0x04 ~ 0x0C Reserved */
	__IO unsigned int CNTR;                 /* 0x10 Counter Register */
	__IO unsigned int ALARM;                /* 0x14 Alarm Register */
	__IO unsigned int CTRL;                 /* 0x18 Control Register */
	__IO unsigned int STATUS;               /* 0x1C Status Register */
} AE210P_RTC_RegDef;

/*****************************************************************************
 * GPIO - AE210P
 ****************************************************************************/
typedef struct {
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register */
	     unsigned int RESERVED0[3];         /* 0x04 ~ 0x0c Reserved */
	__I  unsigned int CFG;                  /* 0x10 Configuration Register */
	     unsigned int RESERVED1[3];         /* 0x14 ~ 0x1c Reserved */
	__I  unsigned int DATAIN;               /* 0x20 Channel Data-in Register */
	__IO unsigned int DATAOUT;              /* 0x24 Channel Data-out Register */
	__IO unsigned int CHANNELDIR;           /* 0x28 Channel Direction Register */
	__O  unsigned int DOUTCLEAR;            /* 0x2c Channel Data-out Clear Register */
	__O  unsigned int DOUTSET;              /* 0x30 Channel Data-out Set Register */
	     unsigned int RESERVED2[3];         /* 0x34 ~ 0x3c Reserved */
	__IO unsigned int PULLEN;               /* 0x40 Pull Enable Register */
	__IO unsigned int PULLTYPE;             /* 0x44 Pull Type Register */
	     unsigned int RESERVED3[2];         /* 0x48 ~ 0x4c Reserved */
	__IO unsigned int INTREN;               /* 0x50 Interrupt Enable Register */
	__IO unsigned int INTRMODE0;            /* 0x54 Interrupt Mode Register (0~7) */
	__IO unsigned int INTRMODE1;            /* 0x58 Interrupt Mode Register (8~15) */
	__IO unsigned int INTRMODE2;            /* 0x5c Interrupt Mode Register (16~23) */
	__IO unsigned int INTRMODE3;            /* 0x60 Interrupt Mode Register (24~31) */
	__IO unsigned int INTRSTATUS;           /* 0x64 Interrupt Status Register */
	     unsigned int RESERVED4[2];         /* 0x68 ~ 0x6c Reserved */
	__IO unsigned int DEBOUNCEEN;           /* 0x70 De-bounce Enable Register */
	__IO unsigned int DEBOUNCECTRL;         /* 0x74 De-bounce Control Register */
} AE210P_GPIO_RegDef;

/*****************************************************************************
 * I2C - AE210P
 ****************************************************************************/
typedef struct _AE210P_I2C_RegDef
{
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register */
	     unsigned int RESERVED[3];          /* 0x04 ~ 0x0C Reserved */
	__I  unsigned int CFG;                  /* 0x10 Configuration Register */
	__IO unsigned int INTEN;                /* 0x14 Interrupt Enable Register */
	__IO unsigned int STATUS;               /* 0x18 Status Register */
	__IO unsigned int ADDR;                 /* 0x1C Address Register */
	__IO unsigned int DATA;                 /* 0x20 Data Register */
	__IO unsigned int CTRL;                 /* 0x24 Control Register */
	__IO unsigned int CMD;                  /* 0x28 Command Register */
	__IO unsigned int SETUP;                /* 0x2C Setup Register */
} AE210P_I2C_RegDef;

/*****************************************************************************
 * SPI - AE210P
 ****************************************************************************/
typedef struct {
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register */
	     unsigned int RESERVED0[3];         /* 0x04 ~ 0x0c Reserved */
	__IO unsigned int TRANSFMT;             /* 0x10 SPI Transfer Format Register */
	__IO unsigned int DIRECTIO;             /* 0x14 SPI Direct IO Control Register */
	     unsigned int RESERVED1[2];         /* 0x18 ~ 0x1c Reserved */
	__IO unsigned int TRANSCTRL;            /* 0x20 SPI Transfer Control Register */
	__IO unsigned int CMD;                  /* 0x24 SPI Command Register */
	__IO unsigned int ADDR;                 /* 0x28 SPI Address Register */
	__IO unsigned int DATA;                 /* 0x2c SPI Data Register */
	__IO unsigned int CTRL;                 /* 0x30 SPI Conrtol Register */
	__I  unsigned int STATUS;               /* 0x34 SPI Status Register */
	__IO unsigned int INTREN;               /* 0x38 SPI Interrupt Enable Register */
	__O  unsigned int INTRST;               /* 0x3c SPI Interrupt Status Register */
	__IO unsigned int TIMING;               /* 0x40 SPI Interface Timing Register */
	     unsigned int RESERVED2[3];         /* 0x44 ~ 0x4c Reserved */
	__IO unsigned int MEMCTRL;              /* 0x50 SPI Memory Access Control Register */
	     unsigned int RESERVED3[3];         /* 0x54 ~ 0x5c Reserved */
	__IO unsigned int SLVST;                /* 0x60 SPI Slave Status Register */
	__I  unsigned int SLVDATACNT;           /* 0x64 SPI Slave Data Count Register */
	     unsigned int RESERVED4[5];         /* 0x68 ~ 0x78 Reserved */
	__I  unsigned int CONFIG;               /* 0x7c Configuration Register */
} AE210P_SPI_RegDef;

/*****************************************************************************
 * Memory Map
 ****************************************************************************/

#ifdef CFG_16MB
#define UNCACHE_MAP(addr)       (addr)
#else
#define UNCACHE_MAP(addr)       ((addr) << 8)
#endif

#define _IO_(addr)              UNCACHE_MAP(addr)

#define EILM_BASE               0x00000000
#define EDLM_BASE               0x00200000
#define SPIMEM_BASE             0x00800000

#define BMC_BASE                _IO_(0x00E00000)
#define DMAC_BASE               _IO_(0x00E0E000)
#define APBBRG_BASE             _IO_(0x00F00000)
#define SMU_BASE                _IO_(0x00F01000)
#define UART1_BASE              _IO_(0x00F02000)
#define UART2_BASE              _IO_(0x00F03000)
#define PIT_BASE                _IO_(0x00F04000)
#define WDT_BASE                _IO_(0x00F05000)
#define RTC_BASE                _IO_(0x00F06000)
#define GPIO_BASE               _IO_(0x00F07000)
#define I2C_BASE                _IO_(0x00F0A000)
#define SPI1_BASE               _IO_(0x00F0B000)
#define SPI2_BASE               _IO_(0x00F0F000)

/*****************************************************************************
 * Peripheral declaration
 ****************************************************************************/
#define AE210P_DMA              ((AE210P_DMA_RegDef *)  DMAC_BASE)
#define AE210P_UART1            ((AE210P_UART_RegDef *) UART1_BASE)
#define AE210P_UART2            ((AE210P_UART_RegDef *) UART2_BASE)
#define AE210P_PIT              ((AE210P_PIT_RegDef *)  PIT_BASE)
#define AE210P_WDT              ((AE210P_WDT_RegDef *)  WDT_BASE)
#define AE210P_RTC              ((AE210P_RTC_RegDef *)  RTC_BASE)
#define AE210P_GPIO             ((AE210P_GPIO_RegDef *) GPIO_BASE)
#define AE210P_I2C              ((AE210P_I2C_RegDef *)  I2C_BASE)
#define AE210P_SPI1             ((AE210P_SPI_RegDef *)  SPI1_BASE)
#define AE210P_SPI2             ((AE210P_SPI_RegDef *)  SPI2_BASE)

#endif	/* __ASSEMBLER__ */

#endif	/* __AE210P_H__ */
