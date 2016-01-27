/*
 * pwm.c
 *
 * Created: 06/01/2016 21:54:18
 *  Author: CheikhnaAhmedTijane
 */ 

#include "pwm.h"
#include "pmc.h"
#include "pdc.h"
#include "bc_pwm.h"
#include <asf.h>

/*	Counter for duty buffer values	*/
uint16_t i;
/*	Duty buffer for transfer		*/
uint16_t g_us_duty_buffer[PERIOD+1];
/*	PDC data paquet for transfer	*/
pdc_packet_t g_pdc_tx_packet;
/*	Function that deals with clock initialization	*/
static int pwm_clock_init(
pwm_channel_t pwm_channel_instance,
uint32_t clock_frequency
)
{
	/*	System clocks	*/
//		sysclk_init();
		board_init();
	   /*	Activate PWM peripheral clock 	*/
	   pmc_enable_periph_clk(ID_PWM);		
	   /*	Disable the PWM channel before setting up options	*/
	   pwm_channel_disable(PWM, PWM_CHANNEL_0);	
	   /*	Set up clock parameters		*/
	   pwm_clock_t clock_setting = {
		   .ul_clka = clock_frequency,			/*	!	Using clock A	*/
		   .ul_clkb = 0,
		   .ul_mck = 42000000
	   };
	   /*	Initialize PWM with clock settings	*/
	   pwm_init(PWM, &clock_setting);		
	   /*	Chose a scaler for the PWM generating	*/
	   pwm_channel_instance.ul_prescaler = PWM_CMR_CPRE_CLKA;		
	   /*	PWM period		*/
	   pwm_channel_instance.ul_period = PERIOD;						
	   /*	Set up a duty cycle		*/
	   pwm_channel_instance.ul_duty = 50;							
	   /*	Give the channel	*/
	   pwm_channel_instance.channel = PWM_CHANNEL_0;
	   /*	Initialize the channel	*/				
	   pwm_channel_init(PWM, &pwm_channel_instance);	
	   /*	Enable interrupts forPWM	*/			   
	   pwm_channel_enable_interrupt(PWM, PWM_CHANNEL_0, 0);	

	return 0;	   
}
/*
static void gpio_init(
void
)
{

	pmc_enable_periph_clk(ID_PIOA);
	
	pio_set_peripheral(PIOA,PIO_PERIPH_B,PIO_PA21B_PWML0| PIO_PA8B_PWMH0 );
	
	pio_set_output(PIOA, PIO_PA8B_PWMH0, HIGH, DISABLE, ENABLE);
	pio_set_output(PIOA, PIO_PA21B_PWML0, LOW, DISABLE, ENABLE);
}
*/
/* Function that handles data transmission	*/


static void pwm_handling (
void
)
{
	uint32_t pdc_status = pwm_get_interrupt_status(PWM);

	if ((pdc_status & PWM_PDC_TX_END) == PWM_PDC_TX_END) {
		/* Set up the PDC controller */
		g_pdc_tx_packet.ul_addr = (uint32_t) (&g_us_duty_buffer[0]);
		g_pdc_tx_packet.ul_size = PERIOD+1;

		/* Initialize the PDC transfer */
		pdc_tx_init(PDC_PWM, &g_pdc_tx_packet, 0);

		/* Send the PWM value */
		pdc_enable_transfer(PDC_PWM, PERIPH_PTCR_TXTEN);
	}
}

void set_pwm(
pwm_channel_t pwm_channel_instance,
uint32_t clock_frequency
)
{
	pwm_clock_init(pwm_channel_instance,clock_frequency);
	//gpio_init();
		   /*	Initialize the duty buffer	*/
		   for(i=0;i<PERIOD+1;i++)
		   {
			   g_us_duty_buffer[i]=i;
		   }
		   	/*
	 * Request PDC transfer as soon as the synchronous update period elapses
	 * (comparison unit is ignored).
	 */
	pwm_pdc_set_request_mode(PWM, PWM_PDC_UPDATE_PERIOD_ELAPSED, (1 << 0));

	/* Configure interrupt for PDC transfer */
	/*	Disable PWM interrupt	*/
	NVIC_DisableIRQ(PWM_IRQn);
	/*	Clear all pending PWM interrupts	*/
	NVIC_ClearPendingIRQ(PWM_IRQn);
	/*	Set priority for PWM	*/
	NVIC_SetPriority(PWM_IRQn, 0);
	/*	Enable PWM interrupt	*/
	NVIC_EnableIRQ(PWM_IRQn);
	/*	Enable interrupt and DMA channel	*/
	pwm_pdc_enable_interrupt(PWM, PWM_PDC_TX_END);
	pwm_channel_enable(PWM,PWM_CHANNEL_0);
	/*	Main loop	*/
	while(1)
	{
		pwm_handling();
	}

	
}
