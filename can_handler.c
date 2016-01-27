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
#include <can.h>
#include <stdint.h>
#include "can_isotp.h"
#include <pmc.h>
#include "arduino_can.h"
IsoTpMessage msg;	
/*	
**	CAN initializer
**
*/
volatile uint32_t g_ul_recv_status;
void can_initialiser(
can_mb_conf_t can0_mailbox0,
can_mb_conf_t can0_mailbox1
)
{

	//	Activation de l'horloge pour le contrôleur CAN0
	pmc_enable_periph_clk(ID_CAN0);
	//	Fixation d'une vitesse par défaut du bus: 500Kb/s
	can_init(CAN0, MCK_FOR_CAN_21, CAN_BPS_500K);
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

/*
**	Choice handler
**	Fonction d'interactivité avec l'utilisateur	
*/

void choice_handler(
uint16_t our_id
)
{
	switch(our_id)
	{
		case SEND_MESSAGE:
		{
			
		} break;
		case SET_MANUAL_BAUDRATE:
		{
			
		}break;
		case SET_AUTO_BAUDRATE:
		{
			
		}break;
		case LISTEN_ONLY:
		{
			
		}break;
	}
}
