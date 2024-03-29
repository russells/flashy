/*****************************************************************************
* PELICAN crossing example
* Last Updated for Version: 4.0.01
* Date of the Last Update:  May 02, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#ifndef qpn_port_h
#define qpn_port_h

#define Q_ROM                   PROGMEM
#define Q_ROM_BYTE(rom_var_)    pgm_read_byte_near(&(rom_var_))
#define Q_ROM_PTR(rom_var_)     pgm_read_word_near(&(rom_var_))

#define Q_NFSM
#define Q_PARAM_SIZE            2 /* The flashy event has one extra
				     parameter. */
#define QF_TIMEEVT_CTR_SIZE     2 /* 16 bit time counter for flashy. */

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           4 /* The flashy has these active objects:
				     flashy, red, green, blue. */

                               /* interrupt locking policy for IAR compiler */
#define QF_INT_LOCK()           cli()
#define QF_INT_UNLOCK()         sei()

                            /* interrupt locking policy for interrupt level */
/* #define QF_ISR_NEST */                    /* nesting of ISRs not allowed */

#include <avr/io.h>
#include <avr/interrupt.h>                                   /* cli()/sei() */
#include <avr/pgmspace.h> /* accessing data in the program memory (PROGMEM) */

#include <stdint.h>    /* Exact-width integer types. WG14/N843 C99 Standard */
#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */

#endif                                                        /* qpn_port_h */
