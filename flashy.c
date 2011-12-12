/**
 * @file
 *
 */

#include "flashy.h"
#include "bsp.h"


/** The only active Flashy. */
struct Flashy flashy;


Q_DEFINE_THIS_FILE;

static QState flashyInitial        (struct Flashy *me);
static QState flashyState          (struct Flashy *me);


static QEvent flashyQueue[4];

QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
	{ (QActive *)0              , (QEvent *)0      , 0                        },
	{ (QActive *)(&flashy)      , flashyQueue      , Q_DIM(flashyQueue)       },
};
/* If QF_MAX_ACTIVE is incorrectly defined, the compiler says something like:
   flashy.c:68: error: size of array ‘Q_assert_compile’ is negative
 */
Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);


int main(int argc, char **argv)
{
 startmain:
	BSP_startmain();
	flashy_ctor();
	BSP_init(); /* initialize the Board Support Package */

	QF_run();

	goto startmain;
}

void flashy_ctor(void)
{
	QActive_ctor((QActive *)(&flashy), (QStateHandler)&flashyInitial);
}


static QState flashyInitial(struct Flashy *me)
{
	return Q_TRAN(&flashyState);
}


static QState flashyState(struct Flashy *me)
{
	switch (Q_SIG(me)) {
	case WATCHDOG_SIGNAL:
		BSP_watchdog(me);
		return Q_HANDLED();
	}
	return Q_SUPER(&QHsm_top);
}
