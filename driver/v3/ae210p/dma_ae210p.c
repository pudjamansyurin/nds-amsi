/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include "../../../driver/v3/ae210p/dma_ae210p.h"

#include "ae210p.h"
#include "cache.h"

typedef struct {
	uint32_t           SrcAddr;
	uint32_t           DstAddr;
	uint32_t           Size;
	uint32_t           Cnt;
	DMA_SignalEvent_t  cb_event;
} DMA_Channel_Info;

static uint32_t channel_active = 0U;
static uint32_t init_cnt       = 0U;

static DMA_Channel_Info channel_info[DMA_NUMBER_OF_CHANNELS];
#define DMA_CHANNEL(n)  ((DMA_CHANNEL_REG *)&(AE210P_DMA->CHANNEL[n]))

#define NTC0_BONDER_START		0x00000000
#define NTC0_BONDER_END			0x40000000

#ifdef CFG_CACHE_ENABLE
#define DMA_DCACHE_WB(start, end)	nds32_dma_flush_range(start, end)
#define DMA_DCACHE_INVALID(start, end)	nds32_dma_inv_range(start, end)
#else
#define DMA_DCACHE_WB(start, end)	NULL
#define DMA_DCACHE_INVALID(start, end)	NULL
#endif

/**
  \fn          int32_t set_channel_active_flag (uint8_t ch)
  \brief       Protected set of channel active flag
  \param[in]   ch        Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
__inline static int32_t set_channel_active_flag (uint8_t ch) {
#ifdef __NDS32_ISA_V3__
	uint32_t val;

	do {
		val = __nds32__llw((void *)&channel_active);

		if (val & (1U << ch)) {
			return -1;
		}
	} while (!__nds32__scw((void *)&channel_active, val | (1U << ch)));

	return 0;
#else
	uint8_t gie = (__nds32__mfsr(NDS32_SR_PSW) & 1);

	if (gie) {
		__nds32__setgie_dis();
	}

	if (channel_active & (1U << ch)) {
		if (gie) {
			__nds32__setgie_en();
		}
		return -1;
	}

	channel_active |= (1U << ch);

	if (gie) {
		__nds32__setgie_en();
	}

	return 0;
#endif
}

/**
  \fn          void clear_channel_active_flag (uint8_t ch)
  \brief       Protected clear of channel active flag
  \param[in]   ch        Channel number (0..7)
*/
__inline static void clear_channel_active_flag (uint8_t ch) {
#ifdef __NDS32_ISA_V3__
	while (!__nds32__scw((void *)&channel_active, (__nds32__llw((void *)&channel_active) & ~(1U << ch))));
#else
	uint8_t gie = (__nds32__mfsr(NDS32_SR_PSW) & 1);

	if (gie) {
		__nds32__setgie_dis();
	}

	channel_active &= ~(1U << ch);

	if (gie) {
		__nds32__setgie_en();
	}
#endif
}

/**
  \fn          int32_t dma_initialize (void)
  \brief       Initialize DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_initialize (void) {

	uint32_t ch_num;

	init_cnt++;

	// Check if already initialized
	if (init_cnt > 1U) {
		return 0;
	}

	// Reset DMA
	AE210P_DMA->DMACTRL = 1U;

	// Reset all DMA channels
	for (ch_num = 0U; ch_num < DMA_NUMBER_OF_CHANNELS; ch_num++) {
		DMA_CHANNEL(ch_num)->CTRL     = 0U;
		channel_info[ch_num].SrcAddr  = 0U;
		channel_info[ch_num].DstAddr  = 0U;
		channel_info[ch_num].Size     = 0U;
		channel_info[ch_num].Cnt      = 0U;
	}

	// Clear all DMA interrupt flags
	AE210P_DMA->INTSTATUS = 0xFFFFFF;

	// Clear and Enable DMA IRQ
	__nds32__mtsr(1 << IRQ_DMA_VECTOR, NDS32_SR_INT_PEND2);
	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | (1 << IRQ_DMA_VECTOR), NDS32_SR_INT_MASK2);

	return 0;
}

/**
  \fn          int32_t dma_uninitialize (void)
  \brief       De-initialize DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_uninitialize (void) {

	// Check if DMA is initialized
	if (init_cnt == 0U) {
		return -1;
	}

	init_cnt--;
	if (init_cnt != 0U) {
		return 0;
	}

	// Disable and Clear DMA IRQ
	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~(1 << IRQ_DMA_VECTOR), NDS32_SR_INT_MASK2);
	__nds32__mtsr(1 << IRQ_DMA_VECTOR, NDS32_SR_INT_PEND2);

	return 0;
}


/**
  \fn          int32_t dma_channel_configure (uint8_t              ch,
                                              uint32_t             src_addr,
                                              uint32_t             dst_addr,
                                              uint32_t             size,
                                              uint32_t             control,
                                              uint32_t             config,
                                              DMA_SignalEvent_t    cb_event)
  \brief       Configure DMA channel for next transfer
  \param[in]   ch        Channel number (0..7)
  \param[in]   src_addr  Source address
  \param[in]   dst_addr  Destination address
  \param[in]   size      Amount of data to transfer
  \param[in]   control   Channel control
  \param[in]   config    Channel configuration
  \param[in]   cb_event  Channel callback pointer
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_channel_configure (	uint8_t            ch,
				uint32_t           src_addr,
				uint32_t           dst_addr,
				uint32_t           size,
				uint32_t           control,
				DMA_SignalEvent_t  cb_event) {

	DMA_CHANNEL_REG* dma_ch;

	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS) {
		return -1;
	}

	// Set Channel active flag
	if (set_channel_active_flag (ch) == -1) {
		return -1;
	}

#if (defined(CFG_CACHE_ENABLE) && !defined(CFG_CACHE_WRITETHROUGH))
	// source is SDRAM memory, destination is peripheral
	// writeback the new data in cache to SDRAM memory before master/slave dma-write
	if(	((unsigned long)src_addr >= NTC0_BONDER_START) &&
		((unsigned long)src_addr < NTC0_BONDER_END) &&
		(control & DMA_CH_CTRL_DMODE_HANDSHAKE)) {
		DMA_DCACHE_WB((unsigned long)(src_addr), (unsigned long)(src_addr) + (unsigned long)(size));
	}
#endif

	// Save callback pointer
	channel_info[ch].cb_event = cb_event;

	dma_ch = DMA_CHANNEL(ch);

	// Reset DMA Channel configuration
	dma_ch->CTRL = 0U;

	// Clear DMA interrupts status
	AE210P_DMA->INTSTATUS = (1U << (16+ch));
	AE210P_DMA->INTSTATUS = (1U << (8+ch));
	AE210P_DMA->INTSTATUS = (1U << ch);

	// Link list not supported
	dma_ch->LLP = 0U;

	channel_info[ch].Size = size;
	if (size > 0x3FFFFFU) {
		// Max DMA transfer size = 4M
		size = 0x3FFFFFU;
	}

	dma_ch->TRANSIZE = size;
	// Set Source and Destination address
	dma_ch->SRCADDR = src_addr;
	dma_ch->DSTADDR = dst_addr;

	if (!(control & DMA_CH_CTRL_SRCADDR_FIX)) {
		if (control & DMA_CH_CTRL_SRCADDR_DEC) {
			// Source address decrement
			src_addr -= (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
		} else {
			// Source address increment
			src_addr += (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
		}
	}

	if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
		if (control & DMA_CH_CTRL_DSTADDR_DEC) {
			// Source address decrement
			dst_addr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		} else {
			// Destination address increment
			dst_addr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
	}

	// Save channel information
	channel_info[ch].SrcAddr = src_addr;
	channel_info[ch].DstAddr = dst_addr;
	channel_info[ch].Cnt     = size;
	dma_ch->CTRL = control;

	if ((control & DMA_CH_CTRL_ENABLE) == 0U) {
		// Clear Channel active flag
		clear_channel_active_flag (ch);
	}

	return 0;
}

/**
  \fn          int32_t dma_channel_enable (uint8_t ch)
  \brief       Enable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_channel_enable (uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS) {
		return -1;
	}

	// Set Channel active flag
	if (set_channel_active_flag (ch) == -1) {
		return -1;
	}

	DMA_CHANNEL(ch)->CTRL |= DMA_CH_CTRL_ENABLE;
	return 0;
}

/**
  \fn          int32_t dma_channel_disable (uint8_t ch)
  \brief       Disable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_channel_disable (uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS) {
		return -1;
	}

#if (defined(CFG_CACHE_ENABLE) && !defined(CFG_CACHE_WRITETHROUGH))
	uint32_t src_addr = channel_info[ch].SrcAddr;
	uint32_t dst_addr = channel_info[ch].DstAddr;
	uint32_t control = DMA_CHANNEL(ch)->CTRL;
	uint32_t size = channel_info[ch].Size;

	// power-off action
	// source is SDRAM memory, destination is peripheral
	// writeback the new data in cache to SDRAM memory before master/slave dma-write
	if (!(control & DMA_CH_CTRL_SRCADDR_FIX)) {
		if (control & DMA_CH_CTRL_SRCADDR_DEC) {
			// Source address decrement to complemetal
			src_addr += (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
		} else {
			// Source address increment to complemetal
			src_addr -= (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
		}
	}

	if(	((unsigned long)src_addr >= NTC0_BONDER_START) &&
		((unsigned long)src_addr < NTC0_BONDER_END) &&
		(control & DMA_CH_CTRL_DMODE_HANDSHAKE)) {
		DMA_DCACHE_WB((unsigned long)(src_addr), (unsigned long)(src_addr) + (unsigned long)(size));
	}

	// power-off action or uart/spi abort action
	// destination is SDRAM memory, source is peripheral
	// invalid the old data in cache after slave dma-read
	if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
		if (control & DMA_CH_CTRL_DSTADDR_DEC) {
			// Source address increment to complemetal
			dst_addr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		} else {
			// Destination address decrement to complemetal
			dst_addr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
	}

	if(	((unsigned long)dst_addr >= NTC0_BONDER_START) &&
		((unsigned long)dst_addr < NTC0_BONDER_END) &&
		(control & DMA_CH_CTRL_SMODE_HANDSHAKE)) {
		DMA_DCACHE_INVALID((unsigned long)(dst_addr), (unsigned long)(dst_addr) + (unsigned long)(size));
	}
#endif

	// Clear Channel active flag
	clear_channel_active_flag (ch);

	DMA_CHANNEL(ch)->CTRL &= ~DMA_CH_CTRL_ENABLE;

	return 0;
}

/**
  \fn          int32_t dma_channel_abort (uint8_t ch)
  \brief       Disable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_channel_abort (uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS) {
		return -1;
	}

#if (defined(CFG_CACHE_ENABLE) && !defined(CFG_CACHE_WRITETHROUGH))
	uint32_t src_addr = channel_info[ch].SrcAddr;
	uint32_t dst_addr = channel_info[ch].DstAddr;
	uint32_t control = DMA_CHANNEL(ch)->CTRL;
	uint32_t size = channel_info[ch].Size;

	// power-off action
	// source is SDRAM memory, destination is peripheral
	// writeback the new data in cache to SDRAM memory before master/slave dma-write
	if (!(control & DMA_CH_CTRL_SRCADDR_FIX)) {
		if (control & DMA_CH_CTRL_SRCADDR_DEC) {
			// Source address decrement to complemetal
			src_addr += (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
		} else {
			// Source address increment to complemetal
			src_addr -= (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
		}
	}

	if(	((unsigned long)src_addr >= NTC0_BONDER_START) &&
		((unsigned long)src_addr < NTC0_BONDER_END) &&
		(control & DMA_CH_CTRL_DMODE_HANDSHAKE)) {
		DMA_DCACHE_WB((unsigned long)(src_addr), (unsigned long)(src_addr) + (unsigned long)(size));
	}

	// power-off action or i2c slave dma-rx complete
	// destination is SDRAM memory, source is peripheral
	// invalid the old data in cache after slave dma-read
	if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
		if (control & DMA_CH_CTRL_DSTADDR_DEC) {
			// Source address increment to complemetal
			dst_addr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		} else {
			// Destination address decrement to complemetal
			dst_addr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
	}

	if(	((unsigned long)dst_addr >= NTC0_BONDER_START) &&
		((unsigned long)dst_addr < NTC0_BONDER_END) &&
		(control & DMA_CH_CTRL_SMODE_HANDSHAKE)) {
		DMA_DCACHE_INVALID((unsigned long)(dst_addr), (unsigned long)(dst_addr) + (unsigned long)(size));
	}
#endif

	// Clear Channel active flag
	clear_channel_active_flag (ch);

	// abort DMA ch transfer
	AE210P_DMA->CHABORT = (1U << ch);

	DMA_CHANNEL(ch)->CTRL     = 0U;
	channel_info[ch].SrcAddr  = 0U;
	channel_info[ch].DstAddr  = 0U;
	channel_info[ch].Size     = 0U;
	channel_info[ch].Cnt      = 0U;

	// Clear DMA interrupts status
	AE210P_DMA->INTSTATUS = (1U << (16+ch));
	AE210P_DMA->INTSTATUS = (1U << (8+ch));
	AE210P_DMA->INTSTATUS = (1U << ch);

	return 0;
}

/**
  \fn          uint32_t dma_channel_get_status (uint8_t ch)
  \brief       Check if DMA channel is enabled or disabled
  \param[in]   ch Channel number (0..7)
  \returns     Channel status
   - \b  1: channel enabled
   - \b  0: channel disabled
*/
uint32_t dma_channel_get_status (uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS) {
		return 0U;
	};

	if (channel_active & (1 << ch)) {
		return 1U;
	} else {
		return 0U;
	}
}

/**
  \fn          uint32_t dma_channel_get_count (uint8_t ch)
  \brief       Get number of transferred data
  \param[in]   ch Channel number (0..7)
  \returns     Number of transferred data
*/
uint32_t dma_channel_get_count (uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS) {
		return 0;
	}

	return (channel_info[ch].Cnt - (DMA_CHANNEL(ch)->TRANSIZE & 0x3FFFFF));
}

/**
  \fn          void dma_irq_handler (void)
  \brief       DMA interrupt handler
*/
void dma_irq_handler (void) {

	uint32_t ch, size;

	DMA_CHANNEL_REG * dma_ch;
	for (ch = 0; ch < DMA_NUMBER_OF_CHANNELS; ch++) {
		if ((AE210P_DMA->INTSTATUS & (1U << ch)) == 0) {
			dma_ch = DMA_CHANNEL(ch);

			// Terminal count request interrupt
			if (AE210P_DMA->INTSTATUS & (1U << (16 + ch))) {
				// Clear interrupt flag
				AE210P_DMA->INTSTATUS = (1U << (16 + ch));

				if (channel_info[ch].Cnt != channel_info[ch].Size) {
					// Data waiting to transfer
					uint32_t control;

					size = channel_info[ch].Size - channel_info[ch].Cnt;
					// Max DMA transfer size = 4M
					if (size > 0x3FFFFFU) {
						size = 0x3FFFFFU;
					}

					channel_info[ch].Cnt += size;
					control = dma_ch->CTRL;

					if (!(control & DMA_CH_CTRL_SRCADDR_FIX)) {
						dma_ch->SRCADDR = channel_info[ch].SrcAddr;

						if (control & DMA_CH_CTRL_SRCADDR_DEC) {
							// Source address decrement
							channel_info[ch].SrcAddr -= (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
						} else {
							// Source address increment
							channel_info[ch].SrcAddr += (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
						}
					}
					if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
						dma_ch->DSTADDR = channel_info[ch].DstAddr;

						if (control & DMA_CH_CTRL_DSTADDR_DEC) {
							// Source address decrement
							channel_info[ch].DstAddr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						} else {
							// Destination address increment
							channel_info[ch].DstAddr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						}
					}

					// Set transfer size
					dma_ch->TRANSIZE = size;
					// Enable DMA Channel
					dma_ch->CTRL |= DMA_CH_CTRL_ENABLE;
				} else {
					// All Data has been transferred

#if (defined(CFG_CACHE_ENABLE) && !defined(CFG_CACHE_WRITETHROUGH))
					// destination is SDRAM memory, source is peripheral
					// invalid the old data in cache after master dma-read
					uint32_t dst_addr = channel_info[ch].DstAddr;
					uint32_t control = dma_ch->CTRL;
					uint32_t size = channel_info[ch].Size;

					if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
						if (control & DMA_CH_CTRL_DSTADDR_DEC) {
							// Source address increment to complemetal
							dst_addr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						} else {
							// Destination address decrement to complemetal
							dst_addr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						}
					}

					if(	((unsigned long)dst_addr >= NTC0_BONDER_START) &&
						((unsigned long)dst_addr < NTC0_BONDER_END) &&
						(control & DMA_CH_CTRL_SMODE_HANDSHAKE)) {
						DMA_DCACHE_INVALID((unsigned long)(dst_addr), (unsigned long)(dst_addr) + (unsigned long)(size));
					}
#endif
					// Clear Channel active flag
					clear_channel_active_flag (ch);

					// Signal Event
					if (channel_info[ch].cb_event) {
						channel_info[ch].cb_event(DMA_EVENT_TERMINAL_COUNT_REQUEST);
					}
				}
			} else {
				// DMA error interrupt
				if (AE210P_DMA->INTSTATUS & (1U << ch)) {
					dma_ch->CTRL = 0U;
					// Clear Channel active flag
					clear_channel_active_flag (ch);

					// Clear interrupt flag
					AE210P_DMA->INTSTATUS = (1U << ch);

					// Signal Event
					if (channel_info[ch].cb_event) {
						channel_info[ch].cb_event(DMA_EVENT_ERROR);
					}
				// DMA abort interrupt
				} else if (AE210P_DMA->INTSTATUS & (1U << (8 + ch))) {
					dma_ch->CTRL = 0U;
					// Clear Channel active flag
					clear_channel_active_flag (ch);

					// Clear interrupt flag
					AE210P_DMA->INTSTATUS = (1U << (8 + ch));

					// Signal Event
					if (channel_info[ch].cb_event) {
						channel_info[ch].cb_event(DMA_EVENT_ABORT);
					}
				}
			}
		}
	}
}
