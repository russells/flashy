#include <avr/wdt.h>

#include "bsp.h"
#include "flashy.h"
#include "toggle-pin.h"


void QF_onStartup(void)
{

}

/**
 * Called by QF when all event queues are empty.
 *
 * We put the MCU to sleep.
 *
 * @todo Call the random byte generator here.  We should just do a return after
 * calling that, to speed up the return to the event loop without going to
 * sleep.
 */
void QF_onIdle(void)
{
	uint8_t mcucr;

	TOGGLE_OFF();

	/* We must re-enable interrupts as a minimum, or no events will be
	   processed or generated. */

	/* Idle sleep mode.  Any interrupt can wake us. */
	mcucr = MCUCR;
	mcucr &= ~ ((1 << SM1) | (1 << SM0));
	MCUCR = mcucr;

	MCUCR |= (1 << SE);

	/* Don't separate the following two assembly instructions.  See
           Atmel's NOTE03. */
        __asm__ __volatile__ ("sei" "\n\t" :: );
        __asm__ __volatile__ ("sleep" "\n\t" :: );

	MCUCR &= ~(1 << SE);
}


void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line)
{
	TOGGLE(1000);
}


void BSP_watchdog(struct Flashy *me)
{
	wdt_reset();
	WDTCR |= (1 <<WDIE);
}


void BSP_startmain(void)
{

}


void BSP_init(void)
{
	TOGGLE_BEGIN();

	wdt_enable(WDTO_30MS);
	WDTCR |= (1 <<WDIE);

	/* Set up the timers for PWM. */

	/* Timer 0 */
	TCCR0A = (0b10 << COM0A0) |
		(0b10 << COM0B0) |
		(0b11 << WGM00);
	TCCR0B = (0 << WGM02) |
		(0b010 << CS00);
	OCR0A = 0xff;
	OCR0B = 0xff;

	/* Timer 1 */
	TCCR1 = (0b0 << CTC1) |
		(0b1 << PWM1A) |
		(0b00 << COM1A0) |
		(0b0100 << CS10);
	GTCCR = (0b1 << PWM1B) |
		(0b10 << COM1B0);
	OCR1B = 0xff;
}


SIGNAL(WDT_vect)
{
	TOGGLE_ON();
	QActive_postISR((QActive*)(&flashy), WATCHDOG_SIGNAL, 0);
	QF_tick();
}
