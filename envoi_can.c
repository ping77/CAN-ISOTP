/*
 * envoi_can.cpp
 *
 * Created: 27/11/2015 09:49:53
 *  Author: CheikhnaAhmedTijane
 */ 
#include <asf.h>
#include "can_isotp.h"
#include "arduino_can.h"
#include "reception_can.h"

typedef struct {
	uint8_t uc_tq;      //! CAN_BIT_SYNC + uc_prog + uc_phase1 + uc_phase2 = uc_tq, 8 <= uc_tq <= 25.
	uint8_t uc_prog;    //! Propagation segment, (3-bits + 1), 1~8;
	uint8_t uc_phase1;  //! Phase segment 1, (3-bits + 1), 1~8;
	uint8_t uc_phase2;  //! Phase segment 2, (3-bits + 1), 1~8, CAN_BIT_IPT <= uc_phase2;
	uint8_t uc_sjw;     //! Resynchronization jump width, (2-bits + 1), min(uc_phase1, 4);
	uint8_t uc_sp;      //! Sample point value, 0~100 in percent.
} can_bit_timing_t;

typedef struct {
	uint_fast16_t baud_speed;
	uint_fast16_t presc;
	uint8_t pts;
	uint8_t pbs1;
	uint8_t pbs2;
}manual_baud;

/** Values of bit time register for different baudrates, Sample point = ((1 + uc_prog + uc_phase1) / uc_tq) * 100%. */
const can_bit_timing_t bit_time[] = {
{8,   (2 + 1), (1 + 1), (1 + 1), (2 + 1), 75},
{9,   (1 + 1), (2 + 1), (2 + 1), (1 + 1), 67},
{10,  (2 + 1), (2 + 1), (2 + 1), (2 + 1), 70},
{11,  (3 + 1), (2 + 1), (2 + 1), (3 + 1), 72},
{12,  (2 + 1), (3 + 1), (3 + 1), (3 + 1), 67},
{13,  (3 + 1), (3 + 1), (3 + 1), (3 + 1), 77},
{14,  (3 + 1), (3 + 1), (4 + 1), (3 + 1), 64},
{15,  (3 + 1), (4 + 1), (4 + 1), (3 + 1), 67},
{16,  (4 + 1), (4 + 1), (4 + 1), (3 + 1), 69},
{17,  (5 + 1), (4 + 1), (4 + 1), (3 + 1), 71},
{18,  (4 + 1), (5 + 1), (5 + 1), (3 + 1), 67},
{19,  (5 + 1), (5 + 1), (5 + 1), (3 + 1), 68},
{20,  (6 + 1), (5 + 1), (5 + 1), (3 + 1), 70},
{21,  (7 + 1), (5 + 1), (5 + 1), (3 + 1), 71},
{22,  (6 + 1), (6 + 1), (6 + 1), (3 + 1), 68},
{23,  (7 + 1), (7 + 1), (6 + 1), (3 + 1), 70},
{24,  (6 + 1), (7 + 1), (7 + 1), (3 + 1), 67},
{25,  (7 + 1), (7 + 1), (7 + 1), (3 + 1), 68}
};

//!				Initialization for peripheral CAN
uint32_t inputClock=MCK_FOR_CAN_21 ;									//!	Clock from the master clock: Cf data sheet
Can modCan;															//!	CAN module
uint32_t baudrate=CAN_BPS_500K;										//!	Default Baud rate
unsigned char seg_data[4095];									//!	Data field
//uint8_t length;														//! Data length

static int receive_flow_control(
IsoTpMessage msg,
flow_control fc
)
{
	//	If waiting for fc	return 0
		uint16_t status_linker=0;
	//	if flow control pending
	fc.cts=msg.data[0];
	fc.B_size=msg.data[1];
	fc.ST_min=msg.data[2];
	uint16_t clear_to_send=fc.cts;
	switch(clear_to_send)
	{
		case FC_CTS :
		{
			//	Ready to receive
				status_linker=0;	// Status ok code
		} break;
		case FC_WT :
		{
			//	Wait before sending
			//	Wait for next flow control: goto the beginning of the function
			status_linker=-128;	//	In progress code
		} break;
		case FC_OVFLW :
		{
			//	Over flow in receiver's side
			//	Wait for another flow control: goto the beginning of the function
			status_linker=-100; //	Over flow code
		} break;			
	}
	return status_linker;
}

/*	
	Function that handles padding of a single frame
	It also deals with errors that can occur like wrong size of the message
*/
static int single_frame(
IsoTpMessage msg,
int mess[4095]
)
{
	uint8_t i=0;
	while(mess[i]!='\0')
	{
		msg.data[i]=mess[i];
		i++;
	}		
	msg.size=i;
	if(msg.size<7)
		{
			while(i!=7)
			{
				msg.data[i]=0xA0;
				i++;
			}
				return 0;
		}
	if (msg.size==7)
	{
			return 0;
	}
	else
	return -1;
}
/*
static void create_consecutive_frame(
IsoTpMessage msg,
int last_length,
uint32_t arbitrary_id,
int message[4095],
int block_size
)
{
	int prepare;
	int bs_tester;
	if((last_length>0) && (last_length<8))
	{
		for (prepare=0;prepare<last_length;prepare++)
		{
			msg.data[prepare]=message[prepare+6];
		}
		if((sizeof(msg.data)) <7)
		for (prepare=(sizeof(msg.data));prepare<7;prepare++)
		{
			msg.data[prepare]=0xA0;
		}

	}
	else
	{
		do{
			for (prepare=0;prepare<7;prepare++)
			{
				msg.data[prepare]=message[prepare+6];
				//	call a function that provides sending CAN messages
			}
			last_length-=7;
			bs_tester++;
		}while(bs_tester<block_size);			
	}
}*/

static int send_cf(
IsoTpMessage msg,
int mess[4095],
uint16_t *last_length,
uint16_t *data_size,
uint16_t *identifier
)
{
	
	int counter=0;
	int padding=0;
	msg.pci=(*identifier);
	if (msg.pci==0x2F)
	{
		(*identifier)=0x21;
	}
	msg.size=(sizeof(mess))/(sizeof(int));
	//msg.arb_id=id_arb;
	if (((*data_size)-(*last_length))>=7)
	{
			do
			{
				msg.data[0]=mess[(*last_length)+1];
				counter++;
			} while (counter<6);
			(*last_length)+=7;
	}
	else
	{
		for (counter=((*data_size)-(*last_length));counter<(*last_length);counter++)
		{
			msg.data[0]=mess[(*last_length)+1];
			padding++;
		}
		for (counter=padding;counter<7;counter++)
		{
			msg.data[counter]=0xA0;
		}
		(*last_length)=0;
	}

	return 0;
}

static void send_ff(
IsoTpMessage msg,
int mess[4095],
uint16_t *last_length,
uint16_t *data_size
)
{
	uint8_t counter;
	msg.pci=N_PCI_FF;
	msg.size=(*data_size);
	for (counter=0;counter<6;counter++)
	{
		msg.data[counter]=mess[counter];
	}
	(*last_length)=6;
	
}

/*
** Function providing data handling and transfer
*/
static void send_message(
IsoTpMessage msg,
int mess[4095],
uint16_t *last_length,
uint16_t *data_size,
IsoTpMessage fc_mess
)
{
	uint16_t stat;
	can_mb_conf_t can0_mailbox;
	flow_control flow;
	volatile uint32_t g_ul_recv_status;
	uint16_t *identifier;
	(*identifier)=0x21;
	//Donner l'élément à envoyer
	(*data_size)=sizeof(mess)/sizeof(int);
	if ((*data_size)==0)
	{
		//send error
	}
	if ((*data_size)<=7)
	{
		single_frame(
		msg,
		mess
		);
		msg.pci=N_PCI_SF;
	}
	if ((*data_size)>7)
	{
		if ((*last_length)>=6)
		{
			if(stat==0)
			{
				for (stat=0;stat<flow.B_size;stat++)
				{
					if (flow.ST_min==FC_ENVOI_IMMEDIAT)
					{
						send_cf(
						msg,
						mess,
						last_length,
						data_size,
						identifier
						);
					}
				}
				stat=1;
				message_received(can0_mailbox,
				g_ul_recv_status
				);
				while((g_ul_recv_status)!=1);
				// Mettre un if après le Timeout
				stat=receive_flow_control(fc_mess,
				flow
				);
			}
		}
		else
		{
			send_ff(
			msg,
			mess,
			last_length,
			data_size
			);
			message_received(can0_mailbox,
			g_ul_recv_status
			);
			while((g_ul_recv_status)!=1);
			// Mettre un if après le Timeout
			stat=receive_flow_control(fc_mess,
			flow
			);

		}
	}
}
