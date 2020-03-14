/*
 * Interrupt_handler.c
 *
 *  Created on: 13.02.2020
 */
#include <Interrupt_handler.h>
#include <lcd_functions.h>

#define YELLOW 0b100000;    // Debug Output to oscilliscope (my ch1 is blue;)
#define BLUE   0b010000;    // Debug Output to oscilliscope

/*****************************  # global variables #   ****************************/
//Base address pointer to Value Arrays
uint8_t *ucPtr;
int m = 0, n = 0;

/***********************  TIMER 1 interrupt handler   ***********************/
/* Periodicaly send Array Data via UART. This handler triggers the first byte */
void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // some debug outputs:
    GPIO_PORTM_DATA_R ^= YELLOW
    ;
    GPIO_PORTM_DATA_R |= BLUE
    ;
    GPIO_PORTN_DATA_R ^= 0b1;

    ReadArray();
    Computations();
    draw_display();

    m = 0;
    n = 0;
    ucPtr = (uint8_t*) &DiffSinResults[0][0];
    while (n < 16)
    {
        UARTDMAEnable(UART0_BASE, UART_DMA_TX);
        UARTCharPutNonBlocking(UART0_BASE, *(ucPtr++));
        m++;
        n++;
    }
    n = 0;
}

/*********************************************************************************************/
void UARTIntHandler(void)
{
    // Get the interrrupt status.
    uint32_t UIstatus = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, UIstatus);

    /* reset transfer if '0' signal received */
    if (UIstatus & UART_INT_RX) //  || UIstatus & UART_INT_RT)
    {
        // GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, GPIO_PIN_4);
        char receive = UARTCharGet(UART0_BASE);
        while (UARTCharsAvail(UART0_BASE))
        {
            UARTCharGet(UART0_BASE);
        }

        if (receive == '0')
        {
            GPIO_PORTM_DATA_R ^= BLUE
            ;
            GPIO_PORTN_DATA_R |= 0b10;
            m = 256;
            n = 16;
            TimerDisable(TIMER0_BASE, TIMER_A);
            UARTFIFODisable(UART0_BASE);
            SysCtlDelay(10000);
            UARTFIFOEnable(UART0_BASE);
            TimerLoadSet(TIMER0_BASE, TIMER_A, SysClock / 20);
            TimerEnable(TIMER0_BASE, TIMER_A);
        }
        else if (receive == '1')
        {
//            GPIO_PORTM_DATA_R ^= BLUE;
        }
    }

    /* send data while m < 256 (= send sin 0-127 and cos 0-127 = 256 values) */
    if (UIstatus & UART_INT_TX && (m < 256))
    {
//        GPIO_PORTM_DATA_R ^= YELLOW;
        if (m == 128)
        {
            ucPtr = (uint8_t*) &DiffCosResults[0][0];
        }
        while (n < 16)
        {
            UARTDMAEnable(UART0_BASE, UART_DMA_TX);
            UARTCharPutNonBlocking(UART0_BASE, *(ucPtr++));
            m++;
            n++;
        }
        n = 0;
    }
    else
    {
        GPIO_PORTM_DATA_R &= ~BLUE
        ;
    }
}

void Timer2IntHandler(void)
{

//    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

//    ReadArray();
//    Computations();

}

