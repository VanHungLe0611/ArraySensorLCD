#include <main.h>

/*********************  # global variables #   ******************/
uint32_t SysClock;
struct TABLE *ttable;
COLOR dotColor = (COLOR) BLUE;

int main(void)

{
    SysClock = SysCtlClockFreqSet(
            (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL
                    | SYSCTL_CFG_VCO_480),
            120000000);

    //Initialize the UART, GPIO, ADC and Timer
    IntMasterDisable();
    ConfigureUDMA();
    ConfigureUART(SysClock);
    ConfigureGPIO();
    ConfigureADC();
    ConfigureTimer0(SysClock);
    ConfigureTimer2(SysClock);
//    ConfigureLCD();
    ConfigureLCD7Inch();

    COLOR background = (COLOR) WHITE;
    write_screen_color(background);
    print_string("HAW Hamburg", 10, 10, (COLOR) BLUE, (COLOR) WHITE);

    write_line_normal(150, 5, 630, 5, (COLOR) RED);
    write_line_normal(150, 5, 150, 475, (COLOR) RED);
    write_line_normal(150, 475, 630, 475, (COLOR) RED);
    write_line_normal(630, 5, 630, 475, (COLOR) RED);

    IntMasterEnable();
    while (1)
        ;
}
