/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#ifndef __SPI_AE210P_H
#define __SPI_AE210P_H

#include "ae210p.h"
#include "../include/Driver_SPI.h"
#include "dma_ae210p.h"


/* SPI ID revision register */
#define SPI_IDREV_MINOR_BIT           (0)
#define SPI_IDREV_MINOR_MASK          (0xf << 0)
#define SPI_IDREV_MAJOR_BIT           (4)
#define SPI_IDREV_MAJOR_MASK          (0xf << 4)

/* SPI transfer format register */
#define SPI_CPHA                      (1UL << 0)
#define SPI_CPOL                      (1UL << 1)
#define SPI_SLAVE                     (1UL << 2)
#define SPI_LSB                       (1UL << 3)
#define SPI_MERGE                     (1UL << 7)
#define DATA_BITS(data_bits)          ((data_bits - 1) << 8)

/* SPI transfer control register */
#define SPI_TRANSCTRL_SLV_DATA_ONLY   (0x1 << 31)

// RD/WR transfer count
#define RD_TRANCNT(num)               ((num - 1) << 0)
#define WR_TRANCNT(num)               ((num - 1) << 12)

// SPI transfer mode
#define SPI_TRANSMODE_WRnRD           (0x0 << 24)
#define SPI_TRANSMODE_WRONLY          (0x1 << 24)
#define SPI_TRANSMODE_RDONLY          (0x2 << 24)
#define SPI_TRANSMODE_WR_RD           (0x3 << 24)
#define SPI_TRANSMODE_RD_WR           (0x4 << 24)
#define SPI_TRANSMODE_WR_DMY_RD       (0x5 << 24)
#define SPI_TRANSMODE_RD_DMY_WR       (0x6 << 24)
#define SPI_TRANSMODE_NONEDATA        (0x7 << 24)
#define SPI_TRANSMODE_DMY_WR          (0x8 << 24)
#define SPI_TRANSMODE_DMY_RD          (0x9 << 24)

/* SPI control register */
#define SPIRST                        (1UL << 0)
#define RXFIFORST                     (1UL << 1)
#define TXFIFORST                     (1UL << 2)
#define RXDMAEN                       (1UL << 3)
#define TXDMAEN                       (1UL << 4)
#define RXTHRES(num)                  (num << 8)
#define TXTHRES(num)                  (num << 16)

#define THRES_MASK_FIFO_16            (0x1f)
#define THRES_MASK_FIFO_128           (0xff)
#define RXTHRES_OFFSET                (8)
#define TXTHRES_OFFSET                (16)

/* SPI interrupt status register */
/* SPI interrupt enable register */
#define SPI_RXFIFOOORINT              (1UL << 0)
#define SPI_TXFIFOOURINT              (1UL << 1)
#define SPI_RXFIFOINT                 (1UL << 2)
#define SPI_TXFIFOINT                 (1UL << 3)
#define SPI_ENDINT                    (1UL << 4)
#define SPI_SLVCMD                    (1UL << 5)

// SPI flags
#define SPI_FLAG_INITIALIZED          (1UL << 0)     // SPI initialized
#define SPI_FLAG_POWERED              (1UL << 1)     // SPI powered on
#define SPI_FLAG_CONFIGURED           (1UL << 2)     // SPI configured
#define SPI_FLAG_DATA_LOST            (1UL << 3)     // SPI data lost occurred
#define SPI_FLAG_MODE_FAULT           (1UL << 4)     // SPI mode fault occurred

// SPI transfer information (Run-time)
typedef struct _SPI_TRANSFER_INFO {
	uint8_t			transfer_op;   // transfer operation: send/recv/transfer
	uint8_t			*rx_buf;       // pointer to in data buffer
	uint8_t			*tx_buf;       // pointer to out data buffer
	uint8_t			*tx_buf_limit; // pointer to the end of in data buffer
	uint32_t		rx_cnt;        // number of data received
	uint32_t		tx_cnt;        // number of data sent
	uint8_t			txfifo_refill; // The size of data SPI TX ISR refills one time.
	uint8_t			dma_tx_complete; // DMA driver have handled it's completion, or SPI driver needs to help to close DMA channel.
	uint8_t			dma_rx_complete; // DMA driver have handled it's completion, or SPI driver needs to help to close DMA channel.
} SPI_TRANSFER_INFO;

// SPI status
typedef struct _SPI_STATUS {
	uint8_t			busy;         // transmitter/receiver busy flag
	uint8_t			data_lost;    // data lost: receiver overflow/transmit underflow (cleared on start of transfer operation)
	uint8_t			mode_fault;   // Mode fault detected; optional (cleared on start of transfer operation)
} SPI_STATUS;

// SPI information (Run-time)
typedef struct _SPI_INFO {
	NDS_SPI_SignalEvent_t	cb_event;      // event callback
	SPI_STATUS		status;        // SPI status flags
	SPI_TRANSFER_INFO	xfer;          // SPI transfer information
	uint8_t			flags;         // SPI driver flags
	uint32_t		mode;          // SPI mode
	uint8_t			txfifo_size;   // SPI HW TXFIFO size
	uint8_t			data_bits;     // the size of one unit SPI data (1 ~ 32, defaults: 8 bits)
	uint32_t                data_num;      // num of the transfer data(use in auto-size)
	uint32_t                block_num;     // num of the transfer block(use in auto-size)
} SPI_INFO;

// SPI DMA
typedef const struct _SPI_DMA {
	uint8_t			channel;     // DMA channel
	uint8_t			reqsel;      // DMA request selection
	DMA_SignalEvent_t	cb_event;    // DMA event callback
} SPI_DMA;

// SPI resources definitions
typedef struct {
	NDS_SPI_CAPABILITIES	capabilities; // capabilities
	AE210P_SPI_RegDef	*reg;         // pointer to SPI peripheral
	uint32_t		irq_num;      // SPI IRQ number
	SPI_DMA			*dma_tx;      // SPI TX DMA
	SPI_DMA			*dma_rx;      // SPI RX DMA
	SPI_INFO		*info;        // SPI run-time information
} const SPI_RESOURCES;

#endif /* __SPI_AE210P_H */
