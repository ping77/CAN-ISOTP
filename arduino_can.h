/*
 * arduino_can.h
 *
 * Created: 26/11/2015 11:33:15
 *  Author: CheikhnaAhmedTijane DIOUF
 */ 


#ifndef ARDUINO_CAN_H_
#define ARDUINO_CAN_H_
/*
*
*	Header file for CAN module
*	In this file,we are defining functions protocols and variables
*	that we will use to set the CAN communication
*/

/*									Headers Inclusion														*/
#include <board.h>
#include <stdint.h>
#include <arduino_due_x.h>

/*									Defining variables														*/

/*									Baud rate																*/
#define CAN_BAUDRATE_10K				10000u
#define CAN_BAUDRATE_20K				20000u
#define CAN_BAUDRATE_30K				30000u
#define CAN_BAUDRATE_50K				50000u
#define CAN_BAUDRATE_100K				100000u
#define CAN_BAUDRATE_200K				200000u
#define CAN_BAUDRATE_300K				300000u
#define CAN_BAUDRATE_500Kbps			500000u	//Bit time =2us
#define CAN_BAUDRATE_1Mbps				1000000u
//Supplements : referred in potocole.txt
#define CAN_BAUDRATE_125Kbps			10000u
#define CAN_BAUDRATE_250Kbps			10000u

/*							CAN Pins	&	Controllers																		*/
#define CANRX0									// PA1
#define CANTX0									// PA0
#define CANRX1									// PB15
#define CANTX1									// PB14
/*
Name: CAN_MR: 32bits CAN mode register
Address: 0x400B4000 (0), 0x400B8000 (1)
Access: Read-write
*/
#define CAN_ENABLE						0x0001u	// Activate CAN controller : BIT1
#define CAN_LISTEN_ONLY					0x0004u	// Activate listen-only mode: BIT3

/*
Name: CAN_IER: CAN interrupt Enable
Address: 0x400B4004 (0), 0x400B8004 (1)
Access: Write-only
*/
#define MAILBOX0_INTERRUPT_ENABLE		0x0001u	//	Activate mailbox 0
#define MAILBOX1_INTERRUPT_ENABLE		0x0002u //	Activate mailbox 1
#define MAILBOX2_INTERRUPT_ENABLE		0x0004u //	Activate mailbox 2
#define MAILBOX3_INTERRUPT_ENABLE		0x0008u //	Activate mailbox 3
#define MAILBOX4_INTERRUPT_ENABLE		0x0010u //	Activate mailbox 4
#define MAILBOX5_INTERRUPT_ENABLE		0x0020u //	Activate mailbox 5
#define MAILBOX6_INTERRUPT_ENABLE		0x0040u //	Activate mailbox 6
#define MAILBOX7_INTERRUPT_ENABLE		0x0080u //	Activate mailbox 7
#define MAILBOX_INTERRUPT_ENABLE_ALL	0x00FFu	//	Activate all mailboxes

/*							CAN Clocks								*/
#define MCK_FOR_CAN_42					42000000U			/*	MCK divided by 2: Cf data sheet	SAM3X			*/
#define MCK_FOR_CAN_21					21000000U			/*	MCK divided by 4: Cf data sheet SAM3X			*/


#endif /* ARDUINO_CAN_H_ */
