/* DriverLib Includes */
#include "driverlib.h"
/* Standard Includes */
#include <stdint.h>
#include "../inc/SysTick.h"
#include "../inc/Clock.h"
#include <math.h>
/*
 * Sound Following Robot Project
 * EEC10 - Professor Hooman Rashtian
 * Abdallah Hashem
 * SID: 916000709
 * Hardware used: MSP42P, RSLK MAZE, microphone circuit
 */


/////////////You will define variables here//////////////////
uint8_t S1 = 1; // button initially set to high

uint32_t Size;
uint32_t I;
uint16_t c;
uint16_t cc;
uint16_t i;

uint16_t o = 0;
uint16_t p = 0;
uint16_t t = 0;

float avgmax1;
float avgmax2;

float rms1_old;
float rms2_old;
float rmsdiff;
float rmsrel;

float offset;
float offset_old;


int32_t INPUT_P4_7[1024];
float REAL_INPUT_P4_7[1024];
float max1[10]= {0};  // find 10 local maximums in the array of Real_Input
float max2[10]= {0};  // find 10 local maximums in the array of Real_Input

int32_t INPUT_P4_6[1024];
float REAL_INPUT_P4_6[1024];

float x1[1024];
float y1[1024];

float x2[1024];
float y2[1024];

float z1[1024];
float z2[1024];

float ratio1on2;
float ratio2on1;


float alpha;
float alpha2;

//Rms
float rawrms1;
float rawrms2;
float rms1;
float rms2;

uint8_t DIRECTION;

#define FORWARD    1
#define BACKWARD   2
#define LEFT       3
#define RIGHT      4
#define STOP       5
#define ROTATE_180 6
#define SUPERFORWARD 7
uint8_t MODE;

#define SAMPLING_MODE    1
#define RUNNING_MODE     2
#define DANCE_MODE       3

/////////////////////////////////////////////////////////////

#define PERIOD   100

/* Timer_A UpMode Configuration Parameter */
const Timer_A_UpModeConfig upConfig =
{
 TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source 24MHz
 TIMER_A_CLOCKSOURCE_DIVIDER_12,          // SMCLK/12 = 1MHz Timer clock
 PERIOD,                                 // a period of 100 timer clocks => 10 KHz Frequency
 TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
 TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
 TIMER_A_DO_CLEAR                        // Clear value
};


/* Application Defines */
#define TIMER_PERIOD 15000  // 10 ms PWM Period
#define DUTY_CYCLE1 0
#define DUTY_CYCLE2 0



/* Timer_A UpDown Configuration Parameter */
Timer_A_UpDownModeConfig upDownConfig =
{
 TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock SOurce
 TIMER_A_CLOCKSOURCE_DIVIDER_4,          // SMCLK/1 = 1.5MHz
 TIMER_PERIOD,                           // 15000 period
 TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
 TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
 TIMER_A_DO_CLEAR                        // Clear value

};

/* Timer_A Compare Configuration Parameter  (PWM1) */
Timer_A_CompareModeConfig compareConfig_PWM1 =
{
 TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
 TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
 TIMER_A_OUTPUTMODE_TOGGLE_RESET,              // Toggle output but
 DUTY_CYCLE1
};

/* Timer_A Compare Configuration Parameter (PWM2) */
Timer_A_CompareModeConfig compareConfig_PWM2 =
{
 TIMER_A_CAPTURECOMPARE_REGISTER_2,          // Use CCR2
 TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
 TIMER_A_OUTPUTMODE_TOGGLE_RESET,              // Toggle output but
 DUTY_CYCLE2
};

/////////////////////////////////////////////////////////////


void TimerA2_Init(void);
void PWM_Init12(void);
void PWM_Init12(void);
void PWM_duty1(uint16_t duty1, Timer_A_CompareModeConfig* data);
void PWM_duty2(uint16_t duty1, Timer_A_CompareModeConfig* data);
void MotorInit(void);
void motor_forward(uint16_t leftDuty, uint16_t rightDuty);
void motor_right(uint16_t leftDuty, uint16_t rightDuty);
void motor_left(uint16_t leftDuty, uint16_t rightDuty);
void motor_backward(uint16_t leftDuty, uint16_t rightDuty);
void motor_stop(void);
void ADC_Ch67_Init(void);


//////////////////////// MAIN FUNCTION /////////////////////////////////////8
void LED_Init(void){

    // Port 2 Configuration: make P2.2-P2.0 out
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,PIN_ALL16);
}

int main(void)

{
    Size=1000; // Array of sampling data
    I=Size-1;

    //Set up the button S1
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4 | GPIO_PIN1);

    // Port 2 Configuration for Toggling LED
    LED_Init();

    // Set Microcontroller Clock = 48 MHz
    Clock_Init48MHz();

    PWM_Init12();

    // Systick Configuration
    SysTick_Init();

    // Motor Configuration
    MotorInit();

    /* Sleeping when not in use */

    // Port 5 Configuration: make P6.4 out
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN4);

    // Setup ADC for Channel A6 and A7
    ADC_Ch67_Init();

    // Timer A1 Configuration
    TimerA2_Init();

    // starts to go forward if the sound source is behind
    DIRECTION  = FORWARD;
    MODE  = RUNNING_MODE;

    while (1)
    {
    }

}


//////////////////////// FUNCTIONs /////////////////////////////////////

void TA2_0_IRQHandler(void)
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P6, GPIO_PIN4);

    // IN SAMPLING MODE
    if(MODE == SAMPLING_MODE)
    {

     //       SysTick_Wait10ms(10); // Wait 10ms for the motor to stop

        ADC14_toggleConversionTrigger(); // ask ADC to get data

        while(ADC14_isBusy()){};

        INPUT_P4_7[I] = ADC14_getResult(ADC_MEM1);
        REAL_INPUT_P4_7[I] = (INPUT_P4_7[I] * 3.3) / 16384;
        INPUT_P4_6[I] = ADC14_getResult(ADC_MEM0);
        REAL_INPUT_P4_6[I] = (INPUT_P4_6[I] * 3.3) / 16384;


        if(I == 0)
        {
            I = Size-1;
            S1= GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1); // check if button is pressed to go to Dancing mode
            if (S1 == GPIO_INPUT_PIN_LOW) { // it is a pull up resistor  so it is 0 when pressed
                MODE = DANCE_MODE;
            }
            else {
                MODE= RUNNING_MODE;
            }

            //////////// MAKE DIRECTION DECISION BASED SAMPLING RESULTS HERE ///////////////////////

            //// Audio filters to emit any unwanted noise


            // Saving data coming from input pins to an array
            for (c = 0; c < 1000; c++) {
                x1[c] = REAL_INPUT_P4_6[c];
                x2[c] = REAL_INPUT_P4_7[c];
            }



            alpha = 0.8883647883; // alpha for high pass filter
            alpha2 = 0.653374213; // alpha for low pass filter


            for (c = 1; c < 1000; c++) {
                y1[c] = alpha * y1[c-1] + alpha * (x1[c] - x1[c-1]); // high pass filter for left mic
                z1[c] = alpha2 * ((y1[c] - y1[c-1])/2) + (1-alpha2) * z1[c-1]; // low pass filter for left mic

                y2[c] = alpha * y2[c-1] + alpha * (x2[c] - x2[c-1]);            // high pass filter for right mic
                z2[c] = alpha2 * ((y2[c] - y2[c-1])/2) + (1-alpha2) * z2[c-1];  // low pass filter for right mic


            }
            z1[0]=alpha2*x1[0];
            z2[0]=alpha2*x2[0];

            //// compute the root mean square after filtering the audio
            // this is to be used to set robot direction controls

            rawrms1 = 0;
            rawrms2 = 0;
            rms1 = 0;
            rms2 = 0;


            for( i = 0 ; i< 1000 ; i++) {
                rawrms1 += z1[i]* z1[i];
                rawrms2 += z2[i]* z2[i]; // Pin 4.7 is rms2
            }

            rms1 = sqrt( rawrms1);
            rms2 = sqrt( rawrms2); // rms2 is pin 4.7

            // checking for ratio between both microphones to test if it is in front of it or not
            ratio1on2 = rms1/rms2;

            // compute the relative rms in order to know how far sound source is
            rmsdiff = rms1 - rms2;
            rmsdiff = rmsdiff * 100;
            rmsdiff = (int)rmsdiff;
            rmsrel = abs(rmsdiff/ratio1on2);

            ////  Robot Direction Controlling Codes. Determine robot direction based on left and right microphone data//////


            // robot stops if no sound is playing
            if ((rms1 < 1.2) && (rms2 < 1.2)) {
                DIRECTION = STOP;
            }
            // robot go on high forward speed if it is pretty sure that it is right in front of it (or behind it)
            else if ((ratio1on2 > 0.99) && (ratio1on2 < 1.0)) {
                DIRECTION = SUPERFORWARD;

                if ((rms1_old > rms1) && (rms2_old > rms2))  {
                    DIRECTION = ROTATE_180;
                }
            }
            // robot would go forward if there's a good chance that the sound is coming in front of it
            else if ((ratio1on2 > 0.90) && (ratio1on2 < 1.2)) {
                DIRECTION  = FORWARD;
                // robot will check if the sound becomes further after going forward
                // if the signal becomes weaker then it must be behind it
                if ((rms1_old > rms1) && (rms2_old > rms2))  {
                    DIRECTION = ROTATE_180;
                }
            }
            // if the sound is not in front or behind the robot then it must be to its right or left
            // checks if sound is to the right of robot
            else if (rms1 > rms2) {
                DIRECTION = RIGHT;
            }
            // checks if sound is to the left of robot
            else if (rms2 > rms1) {
                DIRECTION = LEFT;
            }

            // equating old values to rms in order to check if the sound became closer or further as it moves
            rms1_old = rms1;
            rms2_old = rms2;

            // creating a history data to identify the last three movements of the RSLK
            o++;
            p++;
            t++;
            int his1;
            int his2;
            int his3;

            if (o == 2) { // first history
                his1 = DIRECTION;
                o = 0;
            }
            if (p == 3 ) { // second history
                his2 = DIRECTION;
                p = 0;
            }

            if (t == 4) { // third history
                his3 = DIRECTION;
                t = 0;
            }
            /* This is used to prevent the RSLK from falling into an infinate loop where the RSLK keeps moving
             * right and left without endlessly.
             * This alog works as the following
             *  if the first history is a right or left movement
             *  and if the second history is a right or left movement
             *  and his1 is not equal to his2
             *  but his1 is equal to his3
             *  That means the car is in infinate loop, so it will go forward and reset variables
             */
            if (~(his1 == his2) && (his1 == his3) && ((his2 == 3) || (his2 ==4)) && ((his1 == 3) || (his1 == 4))) {
                DIRECTION = FORWARD;
                his1 = 0;
                his2 = 0;
                his3 = 0;
            }

            ////////////////////////////////////////////////////////////////////////////////////
        }

        else
        {
            I--;
        }
    }


    // IN RUNNING MODE
    if(MODE == RUNNING_MODE)
    {

        if(DIRECTION  == FORWARD)
        {
            motor_forward(6000,6000); // Move forward
            SysTick_Wait10ms(100); // Wait 3s for the motor to run
        }

        else if (DIRECTION  == BACKWARD)
        {
            motor_backward(5000,5000); // Move backward
            SysTick_Wait10ms(100); // Wait 1s for the motor to run
        }
        else if (DIRECTION  == LEFT)
        {
            motor_left(2000,2300); // Move forward left
            SysTick_Wait10ms(100); // Wait 1s for the motor to run

        }
        else if (DIRECTION  == RIGHT)
        {
            motor_right(2300,2000); // Move forward right
            SysTick_Wait10ms(100); // Wait 1s for the motor to run

        }
        else if (DIRECTION  == STOP)
        {
            motor_stop(); // Move forward right
            SysTick_Wait10ms(300); // Wait 3s for the motor to run

        }

        else if (DIRECTION  == ROTATE_180)
        {
            motor_right(2500, 2500);
            SysTick_Wait10ms(200); // Wait 2s for the motor to run

            motor_forward(6000,6000); // Move forward
            SysTick_Wait10ms(100); // Wait 3s for the motor to run
        }
        // this function is called when it is certain that the sound is right behind it or in front of it
        else if (DIRECTION == SUPERFORWARD)
        {
            motor_forward(8000,8000);
            SysTick_Wait10ms(300);
        }

        MODE = SAMPLING_MODE;
        motor_stop();
        SysTick_Wait10ms(150);  // Stop 1s to take audio sample without Robot motor noises
    }

    /*
     * The dance mode was a fun thing to do since it is triggered by pressing button located on Pin 1.1
     * the car would be dancing to a preset movement showed below by using the functions.
     * It'll keep dancing until I press the reset button on the MSP
     */
    if (MODE == DANCE_MODE){

        while ((rms1 < 1.2) && (rms2 < 1.2)) {
            motor_stop();
        }
        while(1) {
            motor_right(2000, 1200);
            SysTick_Wait10ms(100);

            motor_left(1200,2000);
            SysTick_Wait10ms(100);

            motor_right(2000, 1200);
            SysTick_Wait10ms(100);

            motor_left(1200,2000);
            SysTick_Wait10ms(100);

            motor_right(3000, 2500);
            SysTick_Wait10ms(100);

        }
    }

    Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
}


////////////////////////////////////////////////////////////////////////////////////


void TimerA2_Init(void){
    /* Configuring Timer_A1 for Up Mode */
    Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);

    /* Enabling interrupts and starting the timer */
    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableInterrupt(INT_TA2_0);
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
    /* Enabling MASTER interrupts */
    Interrupt_setPriority(INT_TA2_0, 0x20);
    Interrupt_enableMaster();

}


void PWM_Init12(void){

    /* Setting P2.4 and P2.5 and peripheral outputs for CCR */
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring Timer_A1 for UpDown Mode and starting */
    Timer_A_configureUpDownMode(TIMER_A0_BASE, &upDownConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UPDOWN_MODE);

    /* Initialize compare registers to generate PWM1 */
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM1);

    /* Initialize compare registers to generate PWM2 */
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM2);
}


void PWM_duty1(uint16_t duty1, Timer_A_CompareModeConfig* data)  // function definition
{
    if(duty1 >= TIMER_PERIOD) return; // bad input
    data->compareValue = duty1; // access a struct member through a pointer using the -> operator
    /* Initialize compare registers to generate PWM1 */
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM1);
}

void PWM_duty2(uint16_t duty2, Timer_A_CompareModeConfig* data)  // function definition
{
    if(duty2 >= TIMER_PERIOD) return; // bad input
    data->compareValue = duty2; // access a struct member through a pointer using the -> operator
    /* Initialize compare registers to generate PWM2 */
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM2);
}




void MotorInit(void){

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6|GPIO_PIN7); // choose P1.6 and P1.7 as outputs

    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6|GPIO_PIN7);   // choose P3.6 and P3.7 as outputs

}


void motor_forward(uint16_t leftDuty, uint16_t rightDuty){

    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6|GPIO_PIN7); // choose P1.6 and P1.7 Low
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6|GPIO_PIN7); // choose P3.6 and P3.7 High
    PWM_duty1(leftDuty, &compareConfig_PWM1);
    PWM_duty2(rightDuty, &compareConfig_PWM2);

}



// ------------Motor_Right------------
// Turn the robot to the right by running the
// left wheel forward and the right wheel
// backward with the given duty cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void motor_right(uint16_t leftDuty, uint16_t rightDuty){




    GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN7); // choose P5.4 and P5.5 Low
    GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN6); // set P5.5 high
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6|GPIO_PIN7); // choose P3.6 and P3.7 High
    PWM_duty1(leftDuty, &compareConfig_PWM1);
    PWM_duty2(rightDuty, &compareConfig_PWM2);

}

// ------------Motor_Left------------
// Turn the robot to the left by running the
// left wheel backward and the right wheel
// forward with the given duty cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void motor_left(uint16_t leftDuty, uint16_t rightDuty){


    GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN6); // choose P5.4 and P5.5 Low
    GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN7); // set P5.5 high
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6|GPIO_PIN7); // choose P3.6 and P3.7 High
    PWM_duty1(leftDuty, &compareConfig_PWM1);
    PWM_duty2(rightDuty, &compareConfig_PWM2);

}

// ------------Motor_Backward------------
// Drive the robot backward by running left and
// right wheels backward with the given duty
// cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void motor_backward(uint16_t leftDuty, uint16_t rightDuty){
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6|GPIO_PIN7); // choose P1.6 and P1.7 Low
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6|GPIO_PIN7); // choose P1.6 and P1.7 High
    PWM_duty1(leftDuty, &compareConfig_PWM1);
    PWM_duty2(rightDuty, &compareConfig_PWM2);

}


void motor_stop(void){

    PWM_duty1(0, &compareConfig_PWM1);
    PWM_duty2(0, &compareConfig_PWM2);

}



void ADC_Ch67_Init(void){

    /* Initializing ADC (MCLK/1/1) */
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1,0);

    /* Configuring GPIOs for Analog In */

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,GPIO_PIN7 | GPIO_PIN6, GPIO_TERTIARY_MODULE_FUNCTION);


    /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM1 (A5 - A6) */
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, false);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A7, false); // A7 for Pin 4.6
    ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A6, false); // A6 for Pin 4.7

    /* Enabling the interrupt when a conversion on channel 1 (end of sequence)
     *  is complete and enabling conversions */
    ADC14_disableInterrupt(ADC_INT1);

    /* Enabling Interrupts */
    Interrupt_disableInterrupt(INT_ADC14);
    //  Interrupt_enableMaster();

    /* Setting up the sample timer to automatically step through the sequence
     * convert.
     */
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    /* Triggering the start of the sample */
    ADC14_enableConversion();

}

///////////////////////////////////////END/////////////////////////////////////////////

