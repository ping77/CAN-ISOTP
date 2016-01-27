/*
 * reception_can.cpp
 *
 * Created: 07/01/2016 10:58:48
 *  Author: CheikhnaAhmedTijane
 */ 
#include <asf.h>
#include <stdint.h>
#include "can_isotp.h"
#include "reception_can.h"

void message_received(
can_mb_conf_t can0_mailbox,
volatile uint32_t g_ul_recv_status
)
{
	uint32_t ul_status;
	ul_status = can_mailbox_get_status(CAN0, 0);
	if ((ul_status & CAN_MSR_MRDY) == CAN_MSR_MRDY) {
		can0_mailbox.ul_mb_idx = 0;
		can0_mailbox.ul_status = ul_status;
		can_mailbox_read(CAN0, &can0_mailbox);
		g_ul_recv_status = 1;
	}
		//	attendre la réception de données
		//while (!g_ul_recv_status) {
			//	Call receive Message here:::!!!
		//};
}


static void send_fc(
uint16_t *size,
flow_control fc
)
{
	if(((*size)<=28) && ((*size)>0))
	{
			fc.cts= (N_PCI_FC+FC_CTS);
			fc.B_size= (*size);
			fc.ST_min= (FC_ENVOI_IMMEDIAT);
			
	}
	else
	{
		fc.cts =(N_PCI_FC+FC_CTS);
		fc.B_size =(FC_MB_CAPACITE);
		fc.ST_min =(FC_ENVOI_IMMEDIAT);
	}	
}
/*
	Fonction de réception d'un single frame
	Elle est sollicitée par la fonction de reception des messages 
	afin d'etre en mesure de récupérer des messages CAN simples (entre 0 et 8 octets)
*/
static int receive_single_frame(
IsoTpMessage msg,
int data[4095]
)
{
	//indication indi;
	uint16_t counter;
	for(counter=0;counter<msg.size;counter++)
	{
		data[counter]=msg.data[counter];
	}
	// Send my data without padding to the software
	return 0;
}
/*
	Fonction pour la reception d'un first frame
	Elle est appellée au niveau de la fonction de reception des messages
	Son role est d'analyser la premiere trame recue lors d'un envoi
	qui depasse 8 octets
*/
static uint16_t receive_first_frame(
int complete_message[4095],
IsoTpMessage msg_in,
uint16_t *data_tracer,
uint16_t *mess_size
)
{
	uint16_t last_length=0;
	flow_control fc;
	for ((*data_tracer)=0;(*data_tracer)<6;(*data_tracer)++)
	{
		complete_message[(*data_tracer)]=msg_in.data[(*data_tracer)];
	}
	last_length =(*data_tracer);
	*mess_size=msg_in.size;
	send_fc(mess_size,fc);
	return last_length;
}

static void receive_consecutive_frame(
int complete_message[4095],
uint8_t *number_interrupt,
IsoTpMessage msg_in,
uint16_t *data_tracer,
uint16_t *mess_size
)	
{
	(*number_interrupt)++;
	if (((*mess_size)-(*data_tracer))>=7)
	{
		uint16_t copy=(*data_tracer);
		for ((*data_tracer)=(*data_tracer);(*data_tracer)<(*data_tracer)+7;(*data_tracer)++)
		{
			complete_message[(*data_tracer)+1]=msg_in.data[(*data_tracer)-copy];
		}
	}
	else
	{
		int16_t copy=(*data_tracer);
		for ((*data_tracer)=(*data_tracer);(*data_tracer)<((*mess_size)-(*data_tracer));(*data_tracer)++)
		{
			complete_message[(*data_tracer)+1]=msg_in.data[(*data_tracer)-copy];
		}
	}
}

/*							
	Fonction de réception d'un message en général
	Les trames de types A et B sont directement gerees par le controleur CAn
	Dans cette partie, nous gerons surtout la reception d'une suite de trames afin
	de reformer un message initial
	Nous développons cette interface en accord avec la norme CAN ISOTP
*/
void receive_message(
IsoTpMessage msg_in,
int complete_message[4095],
uint16_t *last_length
)
{
	uint16_t *mess_size;
	mess_size=NULL;
	uint16_t *data_tracer;
	/*	Variable d'initialisation du conteneur pour la donnée à recevoir	*/
	/*	uint32_t initializer;	*/
	/*	Initialisation du tableau */
	/*
	for(initializer=0;initializer<4096;initializer++)
	{
		complete_message[initializer]=0;
	}*/
	uint32_t pci=msg_in.pci;
	uint8_t *number_interrupt;
	number_interrupt=NULL;
	switch(pci)
	{
		case N_PCI_SF:
		{
			receive_single_frame(
			msg_in,
			complete_message
			);
			last_length=0;
		} break;
		case N_PCI_FF:
		{
			(*last_length)= receive_first_frame(
			complete_message,
			msg_in,
			data_tracer,
			mess_size
			);
			*mess_size=msg_in.size;
		} break;
			//	Consecutive Frame
		case N_PCI_CF :
		{
			flow_control fc;
			receive_consecutive_frame(
			complete_message,
			number_interrupt,
			msg_in,
			last_length,
			mess_size
			);
			if (((*number_interrupt)%(4)) && !((*mess_size)-(*last_length)))
			{
				number_interrupt=NULL;
				uint16_t *size;
				*size=(*mess_size)-(*last_length);
				send_fc(size,fc);
			}
			if((*mess_size)==(*last_length))
			{
				last_length=NULL;
			}
		} break;
	}		
		
}
