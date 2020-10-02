#include"christmascard.h"
#include"animation.h"
#include<avr/boot.h>
#include<avr/interrupt.h>
#include<avr/power.h>
#include<avr/io.h>
#include<avr/common.h>
#include<avr/builtins.h>
#include<avr/wdt.h>
#include<avr/sleep.h>
#include<util/delay.h>
#include<assert.h>
#include"ws2811_8.h"

ISR(INT0_vect) {
	cc_reset_for_next_animation();
}

void cc_init_nextanibutton(void){
	/*	Output and enable pin for next button.	*/
	PORTD |= BUTTON_NEXT;
	DDRD |= BUTTON_NEXT;

	/*	Enable interrupt.	*/
	/*	External interrupt invoked on volt drop.	*/
	MCUCR |= (1 << ISC00) | ( 1 << ISC01);
	GICR |= (1 << INT0);
}

void cc_init_ledcontrollers(void){

	/*	Output and enable pin for ws2811.	*/
	DDRB |= WS2811_IN;

	/*	Output rows.	*/
	ROWDPORT |= ROWALL;
}

void cc_init_time2ovf(void){
	/*
	TIMSK |= ( 1 << TOIE2);
	*/
	/*	Enable time overflow and interrupt.	*/
	/*
	ASSR = (1<<AS2);
	TIMSK = (1 << TOIE2);
	TCCR2 |= (1 << CS02);
	TIFR = (1 << TOV2);
	*/

}

void cc_set_row(const uint8_t r){

	/*	Set all rows to sink.	 */
	ROWPORT &= ~ROWALL;
	ROWDPORT |= ROWALL;

	/*	Set specified row to source mode.	*/
	if(r == 0){
		ROWPORT |= ROW0;
	}
	else if(r == 1){
		ROWPORT |= ROW1;
	}
	else if(r == 2){
		ROWPORT |= ROW2;
	}
	else if (r == 3){
		ROWPORT |= ROW3;
	}
}

void cc_set_col(const uint8_t d[9]){

	cli();
	ws2811_send(d, 3, WS2811_PIN);
	sei();
}

void cc_display_next_keyframe(void){
	uint8_t i, j;

	/*	*/
	for(j = 0; j < NUMROWS; j++){
		uint8_t buf[9];

		/*	Fetch row data.	*/
		for (i = 0; i < NUMCOL; i++) {
			buf[i] = cc_get_curr_animation_keyframe(j, i);
		}

		cc_set_col(buf);
		cc_set_row(j);

		//_delay_ms(1000);
		_delay_us(30);
	}
	//_delay_us(15);
}

void cc_select_led_controller(void) {
	DDRC |= (1 << PC6);
	PORTC |= (1 << PC6);
}

void init(void) {
	/*	Disable interrupt intill
	 *	the initialization part is done.*/
	cli();

#if F_CPU == 16000000 && 0
	clock_prescale_set(clock_div_2);
#endif

	/*	Enable all required components of the MCU.	*/
	cc_init_ledcontrollers();

	/*	*/
	cc_init_nextanibutton();

	/*	*/
	cc_init_animation();

	/*	*/
	cc_init_time2ovf();

	/*	*/
	cc_select_led_controller();

	sleep_enable();
	set_sleep_mode(SLEEP_MODE_IDLE);

	sei();
}

int main(void) {

	init();

	/*	main logic.	*/
	while (1) {
		uint8_t i = 0;
		const uint8_t it = 144; /*	How long the display will be lit.	*/

		/*	Refresh the LED n number of times.	*/
		for (i = 0; i < it; i++) {
			cc_display_next_keyframe();

		}

		/*	Change to next animation.*/
		if (cc_next_animation_ready()) {
			cc_reset_for_next_animation();
			continue;
		}

		/*	Next frame.	*/
		cc_next_frame();
	}

	return 0;
}
