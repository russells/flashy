/**
 * @file
 *
 * @brief Flashy startup and state machine.
 */

#include "flashy.h"
#include "colour.h"
#include "bsp.h"
#include "toggle-pin.h"
#include <stdlib.h>		/* random() */


/** The only active Flashy. */
struct Flashy flashy;


Q_DEFINE_THIS_FILE;

static QState flashyInitial        (struct Flashy *me);
static QState flashyState          (struct Flashy *me);
static QState normalState          (struct Flashy *me);
static QState slowState            (struct Flashy *me);
static QState fastState            (struct Flashy *me);
static QState indicateState        (struct Flashy *me);
static QState indicateRedState     (struct Flashy *me);
static QState indicateGreenState   (struct Flashy *me);
static QState indicateBlueState    (struct Flashy *me);
static QState indicateWhiteState   (struct Flashy *me);


/** This queue needs to be fairly large because flashy gets sent events from
    five different sources: ISR, three Colour state machines, and itself.  They
    can all send events at once, so we need a queue big enough to hold that,
    plus a bit. */
static QEvent flashyQueue[7];
static QEvent redQueue   [4];
static QEvent greenQueue [4];
static QEvent blueQueue  [4];

QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
	{ (QActive *)0              , (QEvent *)0      , 0                        },
	{ (QActive *)(&flashy)      , flashyQueue      , Q_DIM(flashyQueue)       },
	{ (QActive *)(&red)         , redQueue         , Q_DIM(redQueue)          },
	{ (QActive *)(&green)       , greenQueue       , Q_DIM(greenQueue)        },
	{ (QActive *)(&blue)        , blueQueue        , Q_DIM(blueQueue)         },
};
/* If QF_MAX_ACTIVE is incorrectly defined, the compiler says something like:
   flashy.c:68: error: size of array ‘Q_assert_compile’ is negative
 */
Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);


/**
 * Return a random byte.
 *
 * At the moment we merely return the low eight bits of the return value of
 * random().
 *
 * @todo Queue bytes constructed from all the bytes of the unsigned long int
 * returned by random().  Generate the queue in QF_onIdle().  I'm concerned
 * that generating random numbers will take up time when we are reacting to
 * events.  So I'd like to spend that time when we're otherwise idle, and also
 * get four bytes from each call to random(), instead of one.
 *
 * @todo Move all that to its own source file.
 */
static uint8_t
randbyte(void)
{
	return random();
}


int main(int argc, char **argv)
{
 startmain:
	BSP_startmain();
	flashy_ctor();
	colours_init();
	BSP_init(); /* initialize the Board Support Package */

	QF_run();

	goto startmain;
}

void flashy_ctor(void)
{
	QActive_ctor((QActive *)(&flashy), (QStateHandler)&flashyInitial);
	flashy.leds_on = 0;
}


static QState flashyInitial(struct Flashy *me)
{
	return Q_TRAN(&flashyState);
}


/**
 * Top state for the Flashy.
 *
 * Does an initial startup delay, just for effect.
 *
 * Counts LED on and off events so that when we transition from the indicate
 * states to the normal flashing states, the count is correct.
 *
 * Handles the watchdog timeouts by calling the BSP function that resets the
 * watchdog.  By involving the main loop in the watchdog resets, we can be sure
 * that both the ISRs and the QP-nano main loop are still running.
 */
static QState flashyState(struct Flashy *me)
{
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		/* Initial delay from 0.5 to 1.5 seconds. */
		QActive_arm((QActive*)me, 15 + randbyte() % 30);
		return Q_HANDLED();
	case Q_TIMEOUT_SIG:
		/* After the initial delay, do the three flashes. */
		return Q_TRAN(indicateRedState); /** @todo see other note */
	case LED_ON_SIGNAL:
		me->leds_on++;
		return Q_HANDLED();
	case LED_OFF_SIGNAL:
		if (! me->leds_on)
			TOGGLE(2000);
		Q_ASSERT(me->leds_on);
		me->leds_on--;
		return Q_HANDLED();
	case WATCHDOG_SIGNAL:
		BSP_watchdog(me);
		return Q_HANDLED();
	}
	return Q_SUPER(&QHsm_top);
}


/**
 * @todo Get the other states to transition here instead of to
 * indicateRedState(), and have this state immediately do a transition to
 * there.  Ensure that particular transition works.  But before that, ensure
 * that the current code works.
 */
static QState
indicateState(struct Flashy *me)
{
	/* see the @todo
	switch (Q_SIG(me)) {
	case Q_INIT_SIG:
		return Q_INIT(indicateRedState); // May need Q_TRAN() instead.
	}
	*/
	return Q_SUPER(flashyState);
}


#define SHORT_FLASH ((250<<8) | (FLASH_MAX_INC-1))
#define SHORT_FLASH_TIMEOUT 9
#define SHORT_FLASHES_TIMEOUT (SHORT_FLASH_TIMEOUT+15)


static QState
indicateRedState(struct Flashy *me)
{
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		QActive_post((QActive*)(&red), FLASH_SIGNAL, SHORT_FLASH);
		QActive_arm((QActive*)me, SHORT_FLASH_TIMEOUT);
		return Q_HANDLED();
	case Q_TIMEOUT_SIG:
		return Q_TRAN(indicateGreenState);
	}
	return Q_SUPER(indicateState);
}


static QState
indicateGreenState(struct Flashy *me)
{
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		QActive_post((QActive*)(&green), FLASH_SIGNAL, SHORT_FLASH);
		QActive_arm((QActive*)me, SHORT_FLASH_TIMEOUT);
		return Q_HANDLED();
	case Q_TIMEOUT_SIG:
		return Q_TRAN(indicateBlueState);
	}
	return Q_SUPER(indicateState);
}


static QState
indicateBlueState(struct Flashy *me)
{
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		QActive_post((QActive*)(&blue), FLASH_SIGNAL, SHORT_FLASH);
		QActive_arm((QActive*)me, SHORT_FLASH_TIMEOUT);
		return Q_HANDLED();
	case Q_TIMEOUT_SIG:
		return Q_TRAN(indicateWhiteState);
	}
	return Q_SUPER(indicateState);
}


static QState
indicateWhiteState(struct Flashy *me)
{
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		QActive_post((QActive*)(&red), FLASH_SIGNAL, SHORT_FLASH);
		QActive_post((QActive*)(&blue), FLASH_SIGNAL, SHORT_FLASH);
		QActive_post((QActive*)(&green), FLASH_SIGNAL, SHORT_FLASH);
		QActive_arm((QActive*)me, SHORT_FLASHES_TIMEOUT);
		return Q_HANDLED();
	case Q_TIMEOUT_SIG:
		return Q_TRAN(slowState);
	}
	return Q_SUPER(indicateState);
}


static void
send_random_flash_event(uint8_t maxinc)
{
	uint8_t cols;
	uint8_t max;
	uint8_t inc;

	cols = 0;
	while (! cols) {
		cols = randbyte() % 0b111111;
	}
	max = randbyte();
	/* Ensure that max is at least 10. */
	max = 10 + (randbyte() % 230);
	/* Ensure that inc is at least 1. */
	inc = 1 + (randbyte() % (maxinc-1));
	if (cols & 0b100100) {
		QActive_post((QActive*)(&red), FLASH_SIGNAL, (max<<8)|inc);
	}
	if (cols & 0b010010) {
		QActive_post((QActive*)(&green), FLASH_SIGNAL, (max<<8)|inc);
	}
	if (cols & 0b001001) {
		QActive_post((QActive*)(&blue), FLASH_SIGNAL, (max<<8)|inc);
	}
}


/**
 * A super state for slowState() and fastState().  The main task of this state
 * is to handle the LED on and off counts, and to ensure that when all the LEDs
 * are off, we immediately start another flash.
 *
 * FIXME check that there are no paths that result in us entering here and
 * ending up with a count out of sync.
 */
static QState
normalState(struct Flashy *me)
{
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		return Q_HANDLED();
	case LED_ON_SIGNAL:
		me->leds_on++;
		return Q_HANDLED();
	case LED_OFF_SIGNAL:
		if (! me->leds_on)
			TOGGLE(3000);
		Q_ASSERT(me->leds_on);
		me->leds_on--;
		if (0 == me->leds_on)
			QActive_post((QActive*)me, FLASH_SIGNAL, 0);
		return Q_HANDLED();
	}
	return Q_SUPER(flashyState);
}


static QState
slowState(struct Flashy *me)
{
	uint8_t change;

	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		if (0 == me->leds_on)
			send_random_flash_event(2);
		return Q_HANDLED();
	case Q_TIMEOUT_SIG:
	case FLASH_SIGNAL:
		send_random_flash_event(2);
		change = randbyte();
		if (change > 240) {
			return Q_TRAN(fastState);
		} else if (change >200 && change < 210) {
			return Q_TRAN(indicateRedState); /** @todo see other note */
		} else {
			QActive_arm((QActive*)me, 15 + randbyte() % 30);
			return Q_HANDLED();
		}
	}
	return Q_SUPER(normalState);
}


static QState
fastState(struct Flashy *me)
{
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		QActive_arm((QActive*)me, 1);
		return Q_HANDLED();
	case Q_TIMEOUT_SIG:
	case FLASH_SIGNAL:
		send_random_flash_event(FLASH_MAX_INC);
		if (randbyte() > 240) {
			return Q_TRAN(slowState);
		} else {
			QActive_arm((QActive*)me, 3 + randbyte() % 10);
			return Q_HANDLED();
		}
	}
	return Q_SUPER(normalState);
}
