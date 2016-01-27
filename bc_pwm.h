/*
 * bc_pwm.h
 *
 * Created: 07/01/2016 08:53:04
 *  Author: CheikhnaAhmedTijane
 */ 


#ifndef BC_PWM_H_
#define BC_PWM_H_

#define PERIOD 500

static int pwm_clock_init(
pwm_channel_t pwm_channel_instance,
uint32_t clock_frequency
)
;

/*static void gpio_init(
void
)
;*/

static void pwm_handling (
void
)
;

void set_pwm(
pwm_channel_t pwm_channel_instance,
uint32_t clock_frequency
)
;



#endif /* BC_PWM_H_ */
