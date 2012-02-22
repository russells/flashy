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
	int on_signal;
	int off_signal;
};


void colours_init(void);


extern struct Colour red;
extern struct Colour green;
extern struct Colour blue;


/* For both the ATtiny85V and ATtiny43U, these outputs are in PORT B. */
#ifdef _AVR_IOTNX5_H_
# define RED_BIT   4		/** OC1B.  Bit 4 for ATtiny85V. */
# define GREEN_BIT 0		/** OC0A.  Bit 0 for ATtiny85V. */
# define BLUE_BIT  1		/** OC0B.  Bit 1 for ATtiny85V. */
#else
#ifdef _AVR_IOTN43U_H_
# define RED_BIT   5		/** Bit 5 for ATtiny43U. */
# define GREEN_BIT 1		/** Bit 1 for ATtiny43U. */
# define BLUE_BIT  2		/** Bit 2 for ATtiny43U. */
#else
# error Unknown AVR type
#endif
#endif

#define FLASH_MAX_INC 20	/** Maximum increment step while flashing.
				    Setting this higher will give more short
				    flashes. */

#endif
