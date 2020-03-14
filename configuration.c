#include <configuration.h>

/******************************************************************************************************/
void wait(int time)
{
    volatile int tmp;

    for (tmp = 0; tmp < 10800 * time; tmp++)
        ; // ~ 1ms
}

void ConfigureADC(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    // Wait for the ADC0 module to be ready
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
        ;
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1))
        ;

    // ADC0, samplesequencer0, trigger processor, priority0
    // ADC1, samplesequencer1, trigger processor, priority1
    // ADC2, samplesequencer2, trigger processor, priority2
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 1);
    ADCSequenceConfigure(ADC1_BASE, 2, ADC_TRIGGER_PROCESSOR, 2);

    // Samplesequencer0, left side of array
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ROW_1_L); // sequence0,step0
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ROW_2_L); // sequence0,step1
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ROW_3_L); // sequence0,step2
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ROW_4_L); // sequence0,step3
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ROW_5_L); // sequence0,step4
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ROW_6_L); // sequence0,step5
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ROW_7_L); // sequence0,step6
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ROW_8_L); // sequence0,step7

    // Sample sequencer 1, upper right side of the array
    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ROW_1_R); // sequence1,step0
    ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ROW_2_R); // sequence1,step1
    ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ROW_3_R); // sequence1,step2
    ADCSequenceStepConfigure(ADC1_BASE, 1, 3, ROW_4_R); // sequence1,step3

    // Sample sequencer 1, lower right side of the array
    ADCSequenceStepConfigure(ADC1_BASE, 2, 0, ROW_5_R); // sequence2,step0
    ADCSequenceStepConfigure(ADC1_BASE, 2, 1, ROW_6_R); // sequence2,step1
    ADCSequenceStepConfigure(ADC1_BASE, 2, 2, ROW_7_R); // sequence2,step2
    ADCSequenceStepConfigure(ADC1_BASE, 2, 3, ROW_8_R |  // sequence2,step3
                                     ADC_CTL_IE | ADC_CTL_END);	// incl.interrupt

    // Enable ADC
    ADCSequenceEnable(ADC0_BASE, 0); // ADC0 for sample sequencer0
    ADCSequenceEnable(ADC1_BASE, 1); // ADC1 for sample sequencer1
    ADCSequenceEnable(ADC1_BASE, 2);  // ADC1 for sample sequencer2

    // Configure for DMA
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    uDMAEnable();
    uDMAControlBaseSet(puiDMAcontroltable);

    ADCSequenceDMAEnable(ADC0_BASE, 0);
    ADCSequenceDMAEnable(ADC1_BASE, 1);
    ADCSequenceDMAEnable(ADC1_BASE, 2);

    uDMAChannelAttributeDisable(
            UDMA_CHANNEL_ADC0,
            UDMA_ATTR_ALTSELECT | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);
    uDMAChannelAttributeDisable(
            UDMA_CHANNEL_ADC1,
            UDMA_ATTR_ALTSELECT | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);
    uDMAChannelAttributeEnable(UDMA_CHANNEL_ADC0, UDMA_ATTR_USEBURST);
    uDMAChannelAttributeEnable(UDMA_CHANNEL_ADC1, UDMA_ATTR_USEBURST);

    uDMAChannelControlSet(
            UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
            UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_128);
    uDMAChannelControlSet(
            UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
            UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_128);

    uDMAChannelControlSet(
            UDMA_CHANNEL_ADC1 | UDMA_PRI_SELECT,
            UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_128);
    uDMAChannelControlSet(
            UDMA_CHANNEL_ADC1 | UDMA_ALT_SELECT,
            UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_128);

//    IntEnable(INT_ADC0SS0);
//    ADCIntEnableEx(ADC0_BASE, ADC_INT_DMA_SS0); // Enables ADC interrupt source due to DMA on ADC sample sequence 0
    uDMAChannelEnable(UDMA_CHANNEL_ADC0); // Enables DMA channel so it can perform transfers
//
//    IntEnable(INT_ADC0SS1);
//    ADCIntEnableEx(ADC0_BASE, ADC_INT_DMA_SS1); // Enables ADC interrupt source due to DMA on ADC sample sequence 0
    uDMAChannelEnable(UDMA_CHANNEL_ADC1); // Enables DMA channel so it can perform transfers

}

/*********************************************************************************************/
void ConfigureUART(int SysClock)
{
    // Enable the GPIO Peripheral used by the UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //Enable UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysClock, 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE));

    // Enable the UART interrupt
    UARTFIFOEnable(UART0_BASE);
    IntPrioritySet(INT_UART0, HIGH_PRIORITY);                // set priority
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT | UART_INT_TX);
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
    UARTTxIntModeSet(UART0_BASE, UART_TXINT_MODE_EOT);
    UARTIntRegister(UART0_BASE, UARTIntHandler);

    // Enable DMA for UART
    UARTDMAEnable(UART0_BASE, UART_DMA_RX | UART_DMA_TX);
    // Setup DMA for RX
    uDMAChannelAttributeDisable(UDMA_CHANNEL_UART0RX,
    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
    UDMA_ATTR_HIGH_PRIORITY |
    UDMA_ATTR_REQMASK);
    uDMAChannelControlSet(UDMA_CHANNEL_UART0RX | UDMA_PRI_SELECT,
    UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 |
    UDMA_ARB_4);
    uDMAChannelControlSet(UDMA_CHANNEL_UART0RX | UDMA_ALT_SELECT,
    UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 |
    UDMA_ARB_4);
    //Setup DMA for TX
    uDMAChannelAttributeDisable(UDMA_CHANNEL_UART0TX,
    UDMA_ATTR_ALTSELECT |
    UDMA_ATTR_HIGH_PRIORITY |
    UDMA_ATTR_REQMASK);
    uDMAChannelAttributeEnable(UDMA_CHANNEL_UART0TX, UDMA_ATTR_USEBURST);
    uDMAChannelControlSet(UDMA_CHANNEL_UART0TX | UDMA_PRI_SELECT,
    UDMA_SIZE_8 | UDMA_SRC_INC_8 |
    UDMA_DST_INC_NONE |
    UDMA_ARB_4);
    // Enable DMA for TX and RX
    uDMAChannelEnable(UDMA_CHANNEL_UART0RX);
    uDMAChannelEnable(UDMA_CHANNEL_UART0TX);
    // Enable DMA Interrupt
    UARTIntEnable(UART0_BASE, UART_INT_DMATX | UART_INT_DMARX);
    // Enable UART Interrupt
    IntEnable(INT_UART0);

}

void ConfigureUDMA(void)
{
    // enable the UDMA peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);

    // wait for the UDMA module to be ready
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA))
    {
    }

    // enable the uDMA controller
    uDMAEnable();

    // set the base for the channel control table
    uDMAControlBaseSet(&puiDMAcontroltable);

    // set attribute
    uDMAChannelAttributeDisable(UDMA_CHANNEL_SW, UDMA_ATTR_ALL);

    // set channel control
    uDMAChannelControlSet(
    UDMA_CH8_UART0RX | UDMA_CH9_UART0TX | UDMA_CH9_UART0TX | UDMA_PRI_SELECT,
                          UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE |
                          UDMA_ARB_1);

    uDMAChannelEnable(UDMA_CHANNEL_SW);
    uDMAChannelRequest(UDMA_CHANNEL_SW);

}

/*********************************************************************************************/
/* Configure GPIOPM3 = D; PM2 = C; PM1 = B; PM0 = A; PL3 = nD */
void ConfigureGPIO(void)
{
    // Set Port L  0-4 Multiplexer address output for 8x8 Array
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL))
        ;
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,
    GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);

    // Set Port M Pins 0-7 Output LCD Data
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM))
        ;
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, 0xFF);

    // Onboard LEDs output: Port N (Pins 0-1)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
        ;
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // LCD Control output: Port Q (Pins 0-4)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);  // Clock Port Q
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOQ))
        ;
    GPIOPinTypeGPIOOutput(
            GPIO_PORTQ_BASE,
            GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
    // PF0-4 for touch
}

/*********************************************************************************************/
void ConfigureTimer0(int SysClock)
{
    // Configure Timer1 Interr
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Clock Gate enable TIMER0upt
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysClock / 20);      // fires every 50 ms
    TimerEnable(TIMER0_BASE, TIMER_A);
    IntPrioritySet(INT_TIMER0A, HIGH_PRIORITY);                // set priority
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

void ConfigureTimer2(int SysClock)
{

    // Configure Timer2 Interrupt
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2); // Clock Gate enable TIMER2upt
    TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);  // set clock for timer 2
    TimerLoadSet(TIMER2_BASE, TIMER_A, SysClock / 50);      // fires every 20 ms
    TimerIntRegister(TIMER2_BASE, TIMER_A, Timer2IntHandler); // set interrupt vector for timer 2A
    IntPrioritySet(INT_TIMER2A, LOW_PRIORITY);                // set priority
    IntEnable(INT_TIMER2A);                         // enable timer 2A interrupt
    TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER2_BASE, TIMER_A);
}

/******************************************************************************************************/
// LCD Panel initialize:
void ConfigureLCD(void)
{
    short wert;
    int value;

    GPIO_PORTQ_DATA_R = 0x00;
    wait(1);
    GPIO_PORTQ_DATA_R = INITIAL_STATE;      // Initial state
    wait(10);                               //

    GPIO_PORTQ_DATA_R &= ~RST;              // Hardware reset
    wait(1);                                //
    GPIO_PORTQ_DATA_R |= RST;               //
    wait(1);                                //

    write_command(SOFTWARE_RESET);          // Software reset
    wait(10);                               //

    GPIO_PORTQ_DATA_R = INITIAL_STATE;      // Initial state
    wait(10);                               //

    GPIO_PORTQ_DATA_R &= ~RST;              // Hardware reset
    wait(1);                                //
    GPIO_PORTQ_DATA_R |= RST;               //
    wait(1);                                //

    write_command(SOFTWARE_RESET);          // Software reset
    wait(10);

    write_command(SET_PLL_MN);              // Set PLL Freq to 120 MHz
    write_cmd_data(0x24);                   //
    write_cmd_data(0x02);                   //
    write_cmd_data(0x04);                   //

    write_command(START_PLL);               // Start PLL
    write_cmd_data(0x01);                   //
    wait(1);                                //

    write_command(START_PLL);               // Lock PLL
    write_cmd_data(0x03);                   //
    wait(1);                                //

    write_command(SOFTWARE_RESET);          // Software reset
    wait(10);
    /*************************************************************************/

    value = 0x01EFFF;
    write_command(SET_LSHIFT);         // Set LCD Pixel Clock 11,4Mhz (0x01D4C0)
    write_cmd_data(value >> 16);              //
    write_cmd_data(value >> 8);               //
    write_cmd_data(value);                  //
    write_command(SET_LCD_MODE);            // Set LCD Panel mode to:
    write_cmd_data(0x20);                   // ..TFT panel 24bit
    write_cmd_data(0x00);                   // ..TFT mode
    write_cmd_data(0x01);             // Horizontal size 480-1 (aka 479 ;)    HB
    write_cmd_data(0xDF);             // Horizontal size 480-1                LB
    write_cmd_data(0x01);             // Vertical size 272-1   (aka 271 ;)    HB
    write_cmd_data(0x0F);             // Vertical size 272-1                  LB
    write_cmd_data(0x00);                   // even/odd line RGB

    write_command(SET_HORI_PERIOD);         // Set Horizontal period
    write_cmd_data(0x02);             // Set HT total pixel=531               HB
    write_cmd_data(0x13);             // Set HT total pixel=531               LB
    write_cmd_data(0x00);             // Set Horiz.sync pulse start pos = 43  HB
    write_cmd_data(0x2B);             // Set Horiz.sync pulse start pos = 43  LB
    write_cmd_data(0x0A);                   // Set horiz.sync pulse with = 10
    write_cmd_data(0x00);             // Set horiz.Sync pulse start pos= 8    HB
    write_cmd_data(0x08);             // Set horiz.Sync pulse start pos= 8    LB
    write_cmd_data(0x00);                   //

    write_command(SET_VERT_PERIOD);         // Set Vertical Period
    write_cmd_data(0x01);             // Set VT lines = 288                   HB
    write_cmd_data(0x20);             // Set VT lines = 288                   LB
    write_cmd_data(0x00);             // Set VPS = 12                         HB
    write_cmd_data(0x0C);             // Set VPS = 12                         LB
    write_cmd_data(0x0A);                   // Set vert.sync pulse with = 10
    write_cmd_data(0x00);              // Set vert.Sync pulse start pos= 8    HB
    write_cmd_data(0x00);              // Set vert.Sync pulse start pos= 8    LB
    write_cmd_data(0x04);                   //

    write_command(0xF0);                    // Set LCD color data format
    write_cmd_data(0x00);                   // Set pixel data format = 8 bit

    write_command(0x30);                    // Set partial area
    write_cmd_data(0);                      // Start row High
    write_cmd_data(0);                      // Start row Low
    wert = 543;
    write_cmd_data(wert >> 8);              // Stop row High
    write_cmd_data(wert);                   // Stop row Low

    write_command(0x12);                    // enter partial mode

    write_command(0x33);                    // Set scroll area
    wert = 0;
    write_cmd_data(wert >> 8);              // TFA high byte (TFA = 0)
    write_cmd_data(wert);                   // TFA low byte
    wert = 544;
    write_cmd_data(wert >> 8);              // VSA high byte (VSA = 272)
    write_cmd_data(wert);                   // VSA low byte
    wert = 0;
    write_cmd_data(wert >> 8);              // BFA high byte (BFA = 543)
    write_cmd_data(wert);                   // BFA low byte

    write_command(0x29);                    // Set display on
}

void ConfigureLCD7Inch(void)
{

    GPIO_PORTQ_DATA_R = 0x00;
    wait(1);
    GPIO_PORTQ_DATA_R = INITIAL_STATE;      // Initial state
    wait(10);                               //

    GPIO_PORTQ_DATA_R &= ~RST;              // Hardware reset
    wait(1);                                //
    GPIO_PORTQ_DATA_R |= RST;               //
    wait(1);                                //

    write_command(SOFTWARE_RESET);          // Software reset
    wait(10);                               //

    GPIO_PORTQ_DATA_R = INITIAL_STATE;      // Initial state
    wait(10);                               //

    GPIO_PORTQ_DATA_R &= ~RST;              // Hardware reset
    wait(1);                                //
    GPIO_PORTQ_DATA_R |= RST;               //
    wait(1);                                //

    write_command(SOFTWARE_RESET);          // Software reset
    wait(10);

    write_command(SET_PLL_MN);              // Set PLL Freq to 120 MHz
    write_cmd_data(0x24);                   //
    write_cmd_data(0x02);                   //
    write_cmd_data(0x04);                   //

    write_command(START_PLL);               // Start PLL
    write_cmd_data(0x01);                   //
    wait(1);                                //

    write_command(START_PLL);               // Lock PLL
    write_cmd_data(0x03);                   //
    wait(1);                                //

    write_command(SOFTWARE_RESET);          // Software reset
    wait(10);

    //******************************************************************************

    write_command(0xe6);       // pixel clock frequency
    write_cmd_data(0x04); // LCD_FPR = 290985 = 33.300 Mhz Result for 7" Display
    write_cmd_data(0x70);   //
    write_cmd_data(0xA9);   //

    write_command(0xB0);          //SET LCD MODE   SIZE !!
    write_cmd_data(0x19); //19 TFT panel data width - Enable FRC or dithering for color depth enhancement 8 18  1f- 38
    write_cmd_data(0x20); //SET TFT MODE & hsync+Vsync+DEN MODE   20  or 00
    write_cmd_data(0x03); //SET horizontal size=800+1 HightByte   !!!!!!!!!!!!
    write_cmd_data(0x1F);      //SET horizontal size=800+1 LowByte
    write_cmd_data(0x01);      //SET vertical size=480+1 HightByte
    write_cmd_data(0xDF);      //SET vertical size=480+1 LowByte
    write_cmd_data(0x00); //Even line RGB sequence / Odd line RGB sequence RGB

    write_command(0xB4);            // Set Horizontal Period
    write_cmd_data(0x03); //03 High byte of horizontal total period (display + non-display)
    write_cmd_data(0x5E); //51 Low byte of the horizontal total period (display + non-display)
    write_cmd_data(0x00); //High byte of the non-display period between the start of the horizontal sync (LLINE) signal and the first display data.
    write_cmd_data(0x46); //**   // 46 Low byte of the non-display period between the start of the horizontal sync (LLINE) signal and the first display data
    write_cmd_data(0x09);       // Set the vertical sync
//    write_cmd_data(0x00);       //SET Hsync pulse start
//    write_cmd_data(0x00);                                       //00
//    write_cmd_data(0x00); //SET Hsync pulse subpixel start position  //00
    write_cmd_data(0x00);             // Set horiz.Sync pulse start pos= 8    HB
    write_cmd_data(0x08);             // Set horiz.Sync pulse start pos= 8    LB
    write_cmd_data(0x00);                   //
    //   ** too small will give you half a PICTURE !!

    write_command(0xB6);          //Set Vertical Period
    write_cmd_data(0x01); //01 High byte of the vertical total (display + non-display)
    write_cmd_data(0xFE); //F4 Low byte F5 INCREASES SYNC TIME AND BACK PORCH
    write_cmd_data(0x00);      // 00
    write_cmd_data(0x0C); //0C =12 The non-display period in lines between the start of the frame and the first display data in line.
//    write_cmd_data(0x00); //Set the vertical sync pulse width (LFRAME) in lines.
//    write_cmd_data(0x00);      //SET Vsync pulse start position
//    write_cmd_data(0x00);
    write_cmd_data(0x00);              // Set vert.Sync pulse start pos= 8    HB
    write_cmd_data(0x00);              // Set vert.Sync pulse start pos= 8    LB
    write_cmd_data(0x04);                   //

    write_command(0x36);
    write_cmd_data(0x01);

    // PWM signal frequency = PLL clock
    write_command(0xBE);
    write_cmd_data(0x08);
    write_cmd_data(0x80);
    write_cmd_data(0x01);

//    write_command(0xBC);   //Set Post Proc
//    write_cmd_data(0x40); //40 Set the contrast value
//    write_cmd_data(0x80); //80 Set the brightness value
//    write_cmd_data(0x40); //40 Set the saturation value
//    write_cmd_data(0x01);  //1 Enable the postprocessor
//
//    write_command(0xE2);
//    write_cmd_data(60);    //35 PLLclk = REFclk * 36 (360MHz)
//    write_cmd_data(5);     // SYSclk = PLLclk / 3  (120MHz)
//    write_cmd_data(0x54);  // validate M and N      dec 84
//
//    write_command(0x26);  //    Set Gamma Curve
//    //  write_cmd_data(0x01); // Gamma curve selection =0
//    write_cmd_data(0x08);  // Gamma curve selection =3
//
//    write_command(0x0B);          //SET SCAN MODE
//    write_cmd_data(0x00); //SET TFT MODE   top to bottom, left to right normal etc

    write_command(0x0A);
    write_cmd_data(0x1C);         //Power Mode

//    write_command(0x3A);          //SET Pixel Format
//    write_cmd_data(0x50);       //16 bit pixel
//    write_cmd_data(0x60);       //18 bit pixel
//    write_cmd_data(0x70);       //24 bit pixel
    write_command(0xF0); //set pixel data format 8bit
    write_cmd_data(0x00);

//    write_command(0xF0);      //  Set Pixel Data Interface
//    write_cmd_data(0x03); // 16-bit (565 format)   011 16-bit (565 format)
    write_command(0x29);                    // Set display on

}

