#include <plib.h>
#include <peripheral/pps.h>
#include "main.h"

/*
 * To enable the buzzer functionality when receiving bell characters,
 * a passive piezo buzzer must be soldered between RPB3 (pin 7) and
 * VSS (pin 8) on the PIC32MX270F256B-I_SP.
 */

// PWM signal (800 Hz, 2% high, 98% low)
#define PWM_FREQ 995 // in Hz  C# =D
#define PWM_HIGH 50   // in %

static volatile _Bool buzzerEnable;

void initBuzzer(_Bool buzEn)
{
    buzzerEnable = buzEn;
    
    const uint32_t period = CLOCKFREQ / 64 / PWM_FREQ;
    const uint32_t oc = period * PWM_HIGH / 100;

    // Configure buzzer on RPB3
    OC1CON = OC_OFF;        // Turn off the OC1 when performing the setup
    OC1R   = oc;            // Initialize primary Compare register
    OC1RS  = oc;            // Initialize secondary Compare register
    OC1CON = OC_PWM_FAULT_PIN_DISABLE; // Configure for PWM mode without Fault pin enabled
    PR2    = period;
    T2CON  = T2_PS_1_64;
    T2CONSET  = T2_ON;      // Enable Timer2
    OUT_PIN_PPS1_RPB3 = OUT_FN_PPS1_OC1; // Output OC1 to RPB3
}


static volatile int BuzzerTimer = 0;

void StartBuzzer(void) {
    if (buzzerEnable) {
        if (BuzzerTimer == 0) {
            BuzzerTimer = 125;
            OC1CONSET = OC_ON; // Enable PWN signal to buzzer
        }
    }
}

void StopBuzzerWhenTimeout(void)
{
    if (BuzzerTimer)
        if (--BuzzerTimer < 25)
            OC1CONCLR = OC_ON; // Disable PWM signal to buzzer
}
