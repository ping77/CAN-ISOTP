/*
 * can_isotp.h
 *
 * Created: 06/12/2015 17:16:22
 *  Author: CheikhnaAhmedTijane
 *  Author: TonyKafando
 */ 


#ifndef CAN_ISOTP_H_
#define CAN_ISOTP_H_
/*
	! La norme CAN ISO TP intervient essentiellement au niveau des couches réseau et transport du modèle OSI
	! La segmentation est gérée par la couche transport tandis que la couche réseau se charge de pourvoir
	! des spécifications par rapport aux échanges de données
*/

#define virtual_N_SA				0xFFu					//	encodeur du TX
#define virtual_N_TA				0x00u					//	encodeur du RX
#define virtual_N_AE				0x01u					//	Extenseur d'adresses	
#define virtual_Parameter_Value		0x00u					//	Valeur à donner aux param du protocole
#define SEND_MESSAGE				30
#define SET_MANUAL_BAUDRATE			31
#define SET_AUTO_BAUDRATE			32
#define LISTEN_ONLY					33
#define SF							0						//!	Single Frame
#define FF							1						//!	First Frame
#define CF							2						//!	Consecutive frame
#define	FC							3						//!	Flow Control
/* Flow Status given in FC frame */
#define FC_CTS	0	/* clear to send */
#define FC_WT	1		/* wait */
#define FC_OVFLW	2	/* overflow */
#define N_PCI_SZ 1	/* size of the PCI byte #1 */
#define SF_PCI_SZ4 1	/* size of SingleFrame PCI including 4 bit SF_DL */
#define SF_PCI_SZ8 2	/* size of SingleFrame PCI including 8 bit SF_DL */
#define FF_PCI_SZ12 2	/* size of FirstFrame PCI including 12 bit FF_DL */
#define FF_PCI_SZ32 6	/* size of FirstFrame PCI including 32 bit FF_DL */
#define FC_CONTENT_SZ 3	/* flow control content size in byte (FS/BS/STmin) */
/* N_PCI type values in bits 7-4 of N_PCI bytes */
#define N_PCI_SF	0x00 /* single frame */
#define N_PCI_FF	0x10 /* first frame */
#define N_PCI_CF	0x20 /* consecutive frame */
#define N_PCI_FC	0x30 /* flow control */
#define FC_ENVOI_IMMEDIAT	0x00	/*	Envoyer des que possible les donnees		*/
#define FC_MB_CAPACITE		0x04

/*
	Identificateur de la nouvelle adresse
*/
typedef enum Mtype{
	Diagnostics,
	Remote
	}Mtype;
	
/*
	Encode le modèle de communication utilisé par les communicants 
*/
typedef enum N_TAtype{
	Physical,
	Functional
	}N_TAtype;
/*
	Identificateur d'un paramètre de la couche réseau
*/	
typedef enum Parameter{
	STmin,
	BS
	}Parameter;
/*
	Résultat d'un changement de paramètre
*/	
typedef enum Result_ChangeParameter{
	N_OK, 
	N_RX_ON, 
	N_WRONG_PARAMETER, 
	N_WRONG_VALUE
}Result_ChangeParameter;	

/*
	Enumeration Result_USDT
*/
typedef enum Result_USDT 
{
	OK, 
	N_TIMEOUT_A, 
	N_TIMEOUT_Bs, 
	N_TIMEOUT_Cr, 
	N_WRONG_SN, 
	N_UNEXP_PDU, 
	N_WFT_OVRN
	} Result_USDT;
	
/*
	Définition d'une structure request pour param
	Elle sert à l'envoi de données à provenant des couches supérieures
*/
 typedef struct requestparam
{
	Mtype mt;
	uint_fast16_t N_SA;
	uint_fast16_t N_TA;
	N_TAtype nat;
	uint_fast16_t N_AE;
	Parameter par;
	uint_fast16_t Parameter_Value;
}requestparam;
/*
	Réponses par rapport à une requête param
*/
typedef struct confirmparam
{
	Mtype mt;
	uint_fast16_t N_SA;
	uint_fast16_t N_TA;
	N_TAtype nat;
	uint_fast16_t N_AE; 
	Parameter par;
	Result_ChangeParameter rcp;
}confirmparam;

/*
	Requête pour l'US Data
*/
typedef struct request
{
		Mtype mt;
		uint_fast16_t N_SA;
		uint_fast16_t N_TA;
		N_TAtype nat;
		uint_fast16_t N_AE;
		char MessageData[4095];
		uint16_t Length;
}request;

/*
	Confirmation de la requête pour request
*/
typedef struct confirm
{
		Mtype mt;
		uint_fast16_t N_SA;
		uint_fast16_t N_TA;
		N_TAtype nat;
		uint_fast16_t N_AE;
		Result_USDT r_usdt;
}confirm;

/*
	Définition du first frame indication
*/
typedef struct FF_indication{
		Mtype mt;
		uint_fast16_t N_SA;
		uint_fast16_t N_TA;
		N_TAtype nat;
		uint_fast16_t N_AE;
		uint16_t Length;			
	}FF_indication;
/*
	Définition des indications pour l'US DATA
*/
typedef struct indication{
		Mtype mt;
		uint_fast16_t N_SA;
		uint_fast16_t N_TA;
		N_TAtype nat;
		uint_fast16_t N_AE;
		char MessageData[4095];
		uint16_t Length;	
		Result_USDT r_usdt;
	}indication;


/*
	Définition du service change parameter
*/
typedef struct N_ChangeParameter{
	requestparam requette;
	confirmparam confirmation;
	}N_ChangeParameter;
	
/*
	Définition d'un N_USDATA
*/	
typedef struct N_USData{
	request req;
	confirm conf;
	FF_indication ffind;
	indication eventOccured;
	}N_USData;
/*	IsoTP message	*/
typedef struct IsoTpMessage{
	const uint32_t arb_id;
	uint32_t pci;
	uint16_t size;
	uint8_t data[8];
	} IsoTpMessage;

typedef struct flow_control{
	uint16_t cts;
	uint16_t B_size;
	uint16_t ST_min;
	} flow_control;
#endif /* CAN_ISOTP_H_ */
