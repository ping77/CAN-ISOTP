/*
 * can_handler.c
 *
 * Created: 07/01/2016 09:49:25
 *  Author: CheikhnaAhmedTijane
 */ 
/*
	!	Structure which tell us about the information that we want to communicate
*/
/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
//#include <can.h>
#include <stdint.h>
#include "can_isotp.h"
//#include <pmc.h>
#include <asf.h>
#include "arduino_can.h"
#include "reception_can.h"
#include "envoi_can.h"

/*	VAriables globales	*/
IsoTpMessage msg;	
int message[4095];
uint16_t *last_length; 
volatile uint32_t g_ul_recv_status;
can_mb_conf_t can_mb0, can_mb1;

/*	
**	CAN initializer
**
*/
void can_initialiser(
can_mb_conf_t can0_mailbox0,
can_mb_conf_t can0_mailbox1
)
{

	//	Activation de l'horloge pour le contrôleur CAN0
	pmc_enable_periph_clk(ID_CAN0);
	//	Fixation d'une vitesse par défaut du bus: 500Kb/s
	//can_init(CAN0, MCK_FOR_CAN_21, CAN_BPS_500K);
	//	Activer le contrôleur can0
	can_enable(CAN0);
	//	Initialisation des structures de transfert
	can_reset_all_mailbox(CAN0);
	
	//	Définir 1 Mailbox pour la réception de données
	can0_mailbox0.ul_mb_idx = 0;
	can0_mailbox0.uc_obj_type = CAN_MB_RX_MODE;
	can0_mailbox0.ul_id_msk = CAN_MAM_MIDvA_Msk | CAN_MAM_MIDvB_Msk;
	can0_mailbox0.ul_id = CAN_MID_MIDvA(0x07);	//	Pour un can standard A
	can_mailbox_init(CAN0, &can0_mailbox0);

	//	Définir 1 Mailbox pour le transfert de données
	can0_mailbox1.ul_mb_idx = 1;
	can0_mailbox1.uc_obj_type = CAN_MB_TX_MODE;
	can0_mailbox1.uc_tx_prio = 15;
	can0_mailbox1.uc_id_ver = 0;
	can0_mailbox1.ul_id_msk = 0;
	can_mailbox_init(CAN0, &can0_mailbox1);
	
	//	ID transmission et longueur de données
	can0_mailbox1.ul_id = CAN_MID_MIDvA(0x07);
	uint32_t i;
	for (i=0;i<sizeof(msg.data);i++);
	{
		can0_mailbox1.ul_datal += (16)*((uint32_t)msg.data[(sizeof(msg.data))-i]);
	}
	can0_mailbox1.ul_datah &=~(can0_mailbox1.ul_datal);
	can0_mailbox1.uc_length = msg.size;
	
	//	Autoriser les interruptions CAN
	can_enable_interrupt(CAN0, CAN_IER_MB0);
	NVIC_EnableIRQ(CAN0_IRQn);
	
	can_mailbox_write(CAN0, &can0_mailbox1);
	can_global_send_transfer_cmd(CAN0, CAN_TCR_MB0); 

}

static bool my_flag_autorize_cdc_transfert = false;
bool my_callback_cdc_enable(void)
{
	my_flag_autorize_cdc_transfert = true;
	return true;
}
void my_callback_cdc_disable(void)
{
	my_flag_autorize_cdc_transfert = false;
}
void task(void)
{
	if (my_flag_autorize_cdc_transfert) {
		udi_cdc_putc('A');
		udi_cdc_getc();
		// Waits and gets a value on CDC line
		//int udi_cdc_getc(void);
		// Reads a RAM buffer on CDC line
		//iram_size_t udi_cdc_read_buf(int* buf, iram_size_t size);
		// Puts a byte on CDC line
		//int udi_cdc_putc(int value);
		// Writes a RAM buffer on CDC line
		//iram_size_t udi_cdc_write_buf(const int* buf, iram_size_t size);
		//bool var=udi_cdc_is_rx_ready(); // verifier si un caractère est reçu

	}
}

void usb_init(
void
)
{
	// Authorize interrupts 
	irq_initialize_vectors();
	cpu_irq_enable();
	// Initialize the sleep manager service 
	sleepmgr_init();
	udc_start();
}
/*
**	Choice handler
**	Fonction d'interactivité avec l'utilisateur	
*/

void choice_handler(
uint16_t our_id
)
{
	uint32_t baudrate=250;	//	Par défaut mais il prendra la valeur communiquée par le Host
	switch(our_id)
	{
		case SEND_MESSAGE:
		{
			receive_message(
			msg,
			message,
			last_length
			);
		} break;
		case SET_AUTO_BAUDRATE:
		{
			can_set_baudrate(
			CAN0,
			MCK_FOR_CAN_42,
			baudrate
			);
		}break;
		case LISTEN_ONLY:
		{
			can_enable_autobaud_listen_mode(CAN0);
		}break;
	}
}
