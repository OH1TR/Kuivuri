/**
 * \file
 *
 * \brief User board configuration template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

// External oscillator frequency
#define BOARD_XOSC_HZ          12000000

#define BOARD_XOSC_TYPE        XOSC_TYPE_XTAL

// External oscillator startup time
#define BOARD_XOSC_STARTUP_US  500000

#define BOARD_OSC_STARTUP_US  500000


#define BOARD_FREQ_SLCK_XTAL 32768
#define BOARD_FREQ_SLCK_BYPASS BOARD_FREQ_SLCK_XTAL

#define BOARD_FREQ_MAINCK_XTAL 12000000

#define BOARD_FREQ_MAINCK_BYPASS BOARD_FREQ_MAINCK_XTAL

//#define CONF_BOARD_UART_CONSOLE

#define DS1820PIN IOPORT_CREATE_PIN(PIOA, 9)

#define OUTRELAY1PIN IOPORT_CREATE_PIN(PIOB, 18)

#endif // CONF_BOARD_H
