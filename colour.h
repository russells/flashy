#ifndef colour_h_INCLUDED
#define colour_h_INCLUDED

#include "qpn_port.h"

struct Colour {
	QActive super;
	/** The bit in PORTB that is connected to our timer output. */
	uint8_t bit;
	/** Inverse of bit, so we can mask it down easier. */
	uint8_t notbit;
	/** Address of the PWM counter for this colour. */
	volatile uint8_t *pwmaddr;
	/** The increment for the brightness.  Higher values mean shorter
	    flashes. */
	uint8_t inc;
	/** Maximum value of the PWM brightness. */
	uint8_t max;
};


void colours_init(void);


extern struct Colour red;
extern struct Colour green;
extern struct Colour blue;


#define RED_BIT   4		/** Bit in PORTB for red channel. */
#define GREEN_BIT 0		/** Bit in PORTB for green channel. */
#define BLUE_BIT  1		/** Bit in PORTB for blue channel. */

#define FLASH_MAX_INC 20	/** Maximum increment step while flashing.
				    Setting this higher will give more short
				    flashes. */

#endif
