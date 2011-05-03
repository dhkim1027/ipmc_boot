#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "irq.h"
#include "clksys.h"

#define clksys_enable( _oscSel ) 	( OSC.CTRL |= (_oscSel) )
#define clksys_is_ready( _oscSel ) 	( OSC.STATUS & (_oscSel) )

uint8_t clksys_disable( uint8_t oscSel )
{
    OSC.CTRL &= ~oscSel;
    uint8_t clkEnabled = OSC.CTRL & oscSel;
    return clkEnabled;
}

void CCPWrite( volatile uint8_t * address, uint8_t value )
{
	AVR_ENTER_CRITICAL_REGION( );
    volatile uint8_t * tmpAddr = address;
    asm volatile(
        "movw r30,  %0"       "\n\t"
        "ldi  r16,  %2"       "\n\t"
        "out   %3, r16"       "\n\t"
        "st     Z,  %1"       "\n\t"
        :
        : "r" (tmpAddr), "r" (value), "M" (CCP_IOREG_gc), "i" (&CCP)
        : "r16", "r30", "r31"
        );
    AVR_LEAVE_CRITICAL_REGION( );
}

void clksys_prescalers_cfg( CLK_PSADIV_t PSAfactor,
                               CLK_PSBCDIV_t PSBCfactor )
{
    uint8_t PSconfig = (uint8_t) PSAfactor | PSBCfactor;
    CCPWrite( &CLK.PSCTRL, PSconfig );
}


void clksys_xosc_Cfg( OSC_FRQRANGE_t freqRange,
                         uint8_t lowPower32kHz,
                         OSC_XOSCSEL_t xoscModeSelection )
{
    OSC.XOSCCTRL = (uint8_t) freqRange |
                   ( lowPower32kHz ? OSC_X32KLPM_bm : 0 ) |
                   xoscModeSelection;
}

uint8_t clksys_main_clk_src_Sel( CLK_SCLKSEL_t clockSource )
{
    uint8_t clkCtrl = ( CLK.CTRL & ~CLK_SCLKSEL_gm ) | clockSource;
	CCPWrite( &CLK.CTRL, clkCtrl );
	//CLK.CTRL = clkCtrl;    
    clkCtrl = ( CLK.CTRL & clockSource );
    return clkCtrl;
}

void sysclk_init(void)
{
	//http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=79486
	
	// External Clock Config
	clksys_xosc_Cfg(0, 0, OSC_XOSCSEL_EXTCLK_gc);
	// External clock Enable
	clksys_enable(OSC_XOSCEN_bm);
	// Wait 
	while(clksys_is_ready(OSC_XOSCRDY_bm) == 0);
	// Main Clock Source Select
	clksys_main_clk_src_Sel(CLK_SCLKSEL_XOSC_gc);
	// Prescaler
	clksys_prescalers_cfg(CONFIG_SYSCLK_PSADIV, CONFIG_SYSCLK_PSBCDIV);
	//clksys_prescalers_cfg(CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc);
}
