#include "colour.h"
#include "flashy.h"

struct Colour red;
struct Colour green;
struct Colour blue;


static QState colourInitial(struct Colour *me);
static QState colourState(struct Colour *me);
static QState offState(struct Colour *me);
static QState onState(struct Colour *me);
static QState flashState(struct Colour *me);
static QState flashCountUpState(struct Colour *me);
static QState flashCountDownState(struct Colour *me);


void
colours_init(void)
{
	red.bit = (1 << RED_BIT);
	red.notbit = ~ (1 << RED_BIT);
	red.pwmaddr = &(OCR1B);
	QActive_ctor((QActive*)(&red), (QStateHandler)colourInitial);

	green.bit = (1 << GREEN_BIT);
	green.notbit = ~ (1 << GREEN_BIT);
	green.pwmaddr = &(OCR0A);
	QActive_ctor((QActive*)(&green), (QStateHandler)colourInitial);

	blue.bit = (1 << BLUE_BIT);
	blue.notbit = ~ (1 << BLUE_BIT);
	blue.pwmaddr = &(OCR0B);
	QActive_ctor((QActive*)(&blue), (QStateHandler)colourInitial);
}


static QState
colourInitial(struct Colour *me)
{
	return Q_TRAN(&offState);
}


static QState
colourState(struct Colour *me)
{
	return Q_SUPER(&QHsm_top);
}


/**
 * Don't produce any colour from this channel.
 */
static QState
offState(struct Colour *me)
{
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		*(me->pwmaddr) = 0;
		DDRB &= me->notbit;
		return Q_HANDLED();
	case FLASH_SIGNAL:
		/* Save the flash parameters, then start the flash. */
		me->max = Q_PAR(me) >> 8;
		me->inc = Q_PAR(me) & 0xff;
		return Q_TRAN(flashCountUpState);
	}
	return Q_SUPER(colourState);
}


/**
 * @todo Use Q_INIT_SIG to do an inital transition to flashCountUpState(), so
 * that offState() can transition here without knowing the internals of how the
 * flash is done.
 *
 * @todo Combine onState() and flashState().
 */
static QState
onState(struct Colour *me)
{
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		*(me->pwmaddr) = 0xff; /* This turns off the LED. */
		DDRB |= me->bit;
		return Q_HANDLED();
	}

	return Q_SUPER(colourState);
}


static QState
flashState(struct Colour *me)
{
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		*(me->pwmaddr) = 0x01;
		QActive_arm((QActive*)me, 1);
		return Q_HANDLED();
	}

	return Q_SUPER(onState);
}


static QState
flashCountUpState(struct Colour *me)
{
	uint8_t value;
	uint8_t newvalue;

	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		*(me->pwmaddr) = 0x01;
		me->inc = 1;
		QActive_arm((QActive*)me, 1);
		return Q_HANDLED();
	case Q_TIMEOUT_SIG:
		value = *(me->pwmaddr);
		if (value < FLASH_MAX_INC) {
			newvalue = value + me->inc;
		} else {
			newvalue = value +
				(me->inc) * (2*(value/FLASH_MAX_INC));
		}
		if (newvalue < value) {
			/* Sanity check for wraparound. */
			return Q_TRAN(flashCountDownState);
		} else if (value > me->max || newvalue > me->max) {
			return Q_TRAN(flashCountDownState);
		} else {
			*(me->pwmaddr) = newvalue;
			QActive_arm((QActive*)me, 1);
			return Q_HANDLED();
		}
	}
	return Q_SUPER(flashState);
}


static QState
flashCountDownState(struct Colour *me)
{
	uint8_t value;
	uint8_t newvalue;

	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		QActive_arm((QActive*)me, 1);
		return Q_HANDLED();
	case Q_TIMEOUT_SIG:
		value = *(me->pwmaddr);
		if (value < me->inc) {
			newvalue = 0;
		}
		if (value < FLASH_MAX_INC) {
			newvalue = value - me->inc;
		} else {
			newvalue = value -
				(me->inc)*(value/FLASH_MAX_INC);
		}
		if (newvalue > value) {
			/* Sanity check for wraparound. */
			newvalue = 0;
		}
		if (0 == newvalue) {
			*(me->pwmaddr) = 0xff; /* Turns off the light. */
			return Q_TRAN(offState);
		} else {
			*(me->pwmaddr) = newvalue;
			QActive_arm((QActive*)me, 1);
			return Q_HANDLED();
		}
	}
	return Q_SUPER(flashState);
}