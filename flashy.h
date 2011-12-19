#ifndef flashy_h_INCLUDED
#define flashy_h_INCLUDED

#include "qpn_port.h"

/* Testing - stop the app and busy loop. */
#define FOREVER for(;;)

enum FlashySignals {
	/**
	 * Sent for timing, and so we can confirm that the event loop is
	 * running.
	 */
	WATCHDOG_SIGNAL = Q_USER_SIG,
	/**
	 * Start a new flash.
	 */
	FLASH_SIGNAL,

	/** The red LED has been turned on. */
	RED_ON_SIGNAL,
	/** The red LED has been turned off. */
	RED_OFF_SIGNAL,
	/** The green LED has been turned on. */
	GREEN_ON_SIGNAL,
	/** The green LED has been turned off. */
	GREEN_OFF_SIGNAL,
	/** The blue LED has been turned on. */
	BLUE_ON_SIGNAL,
	/** The blue LED has been turned off. */
	BLUE_OFF_SIGNAL,

	MAX_PUB_SIG,
	MAX_SIG,
};


/**
 * Create the flashy.
 */
void flashy_ctor(void);


/**
 */
struct Flashy {
	QActive super;
	uint8_t red_on;
	uint8_t green_on;
	uint8_t blue_on;
};


/** The only active Flashy. */
extern struct Flashy flashy;


/**
 * Call this just before calling QActive_post() or QActive_postISR().
 *
 * It checks that there is room in the event queue of the receiving state
 * machine.  QP-nano does this check itself anyway, but the assertion from
 * QP-nano will always appear at the same line in the same file, so we won't
 * know which state machine's queue is full.  If this check is done in user
 * code instead of library code we can tell them apart.
 */
#define fff(o) Q_ASSERT(((QActive*)(o))->nUsed <= Q_ROM_BYTE(QF_active[((QActive*)(o))->prio].end))

#endif
