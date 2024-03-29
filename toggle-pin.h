#ifndef toggle_pin_h_INCLUDED
#define toggle_pin_h_INCLUDED

#include "cpu-speed.h"

#include <util/delay.h>
#include <avr/wdt.h>


/**
 * @brief The port containing the pin that we toggle to show activity.
 *
 * The first statement in each interrupt handler should be
 * @code
 *    TOGGLE_PORT |= ~ (1 << TOGGLE_PIN);
 * @endcode
 *
 * This will raise the pin, and that pin will stay high until AVR_sleep()
 * lowers it just before we go back to sleep.  Monitoring that pin will give
 * some idea of how much CPU is being used.
 */
#define TOGGLE_PORT PORTB
/**
 * @brief The Data Direction Register for the toggled pin.
 *
 * @see TOGGLE_PORT.
 */
#define TOGGLE_DDR  DDRB
/**
 * @brief The number of the toggled pin.
 *
 * @see TOGGLE_PORT.
 */
#define TOGGLE_PIN  3


/**
 * @brief Set up the toggle pin.
 *
 * Call this at the start of the program.
 */
#define TOGGLE_BEGIN() TOGGLE_DDR |= (1 << TOGGLE_PIN)


/**
 * @brief Make the toggle pin high.
 *
 * Call this at the start of the activity you wish to record.
 */
#define TOGGLE_ON() TOGGLE_PORT |= (1 << TOGGLE_PIN)


/**
 * @brief Make the toggle pin low.
 *
 * Call this at the end of the activity you wish to record.
 */
#define TOGGLE_OFF() TOGGLE_PORT &= (~ (1 << TOGGLE_PIN))


/**
 * @brief Toggle a pin permanently.
 *
 * This turns off interrupts and WDT, and toggles the toggle pin continuously,
 * with a half period equal to n microseconds.  Everything stops when we invoke
 * this, and it should only be used for debugging, or for "impossible"
 * conditions.
 *
 * @note This is the same pin that we monitor for CPU activity (@see
 * TOGGLE_PORT), but for a very different purpose.
 *
 * @todo Also make all the IO pins inputs with pullups off.
 */
#define TOGGLE(n) {						\
		TOGGLE_DDR  |= (1<<2);				\
		TOGGLE_PORT |= (1<<2);				\
		cli();						\
		wdt_disable();					\
		while (1) {					\
			TOGGLE_PORT ^= (1 << TOGGLE_PIN);	\
			_delay_ms(n);				\
		}						\
	}

#endif
