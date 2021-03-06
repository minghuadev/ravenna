#include "../ravenna_defs.h"
#include "i2c_io.h"

//extern void write_i2c_slave(uint32_t slave_addr, uint32_t word_addr, uint32_t data);
//extern uint32_t read_i2c_slave_byte(uint32_t slave_addr, uint32_t word_addr);
//extern void read_i2c_slave_bytes(uint32_t slave_addr, uint32_t word_addr, uint32_t *data, int len);
//extern void i2c_init();
//extern void i2c_start();
//extern void i2c_stop();
//extern uint32_t i2c_write(volatile uint32_t data);
//extern uint32_t i2c_read(bool ack);

#define RTC_I2C_ADDR (uint32_t) 0xA2 // RTC PCF8563
#define RTC_I2C_REG (uint32_t) 0x02 // RTC PCF8563

//#define RTC_I2C_ADDR (uint32_t) 0xDE // RTC MCP79410
//#define RTC_I2C_REG (uint32_t) 0x00 // RTC MCP79410

#define EEPROM_I2C_ADDR (uint32_t) 0xA0 // EEPROM 24LC64


//#define RTC_I2C_ADDR (uint32_t)0xD0 // RTC DS3231

#define BCD_DIGIT0(x) (x & (uint32_t)0x000F)
#define BCD_DIGIT1(x) ((x >> 4) & (uint32_t)0x000F)

// --------------------------------------------------------
// Firmware routines
// --------------------------------------------------------

// Copy the flash worker function to SRAM so that the SPI can be
// managed without having to read program instructions from it.

void flashio(uint32_t *data, int len, uint8_t wrencmd)
{
	uint32_t func[&flashio_worker_end - &flashio_worker_begin];

	uint32_t *src_ptr = &flashio_worker_begin;
	uint32_t *dst_ptr = func;

	while (src_ptr != &flashio_worker_end)
		*(dst_ptr++) = *(src_ptr++);

	((void(*)(uint32_t*, uint32_t, uint32_t))func)(data, len, wrencmd);
}

//--------------------------------------------------------------
// NOTE: Volatile write *only* works with command 01, making the
// above routing non-functional.  Must write all four registers
// status, config1, config2, and config3 at once.
//--------------------------------------------------------------
// (NOTE: Forces quad/ddr modes off, since function runs in single data pin mode)
// (NOTE: Also sets quad mode flag, so run this before entering quad mode)
//--------------------------------------------------------------

void set_flash_latency(uint8_t value)
{
	reg_spictrl = (reg_spictrl & ~0x007f0000) | ((value & 15) << 16);

	uint32_t buffer_wr[2] = {0x01000260, ((0x70 | value) << 24)};
	flashio(buffer_wr, 5, 0x50);
}

//--------------------------------------------------------------

void putchar(char c)
{
	if (c == '\n')
		putchar('\r');
	reg_uart_data = c & 0xff;
}

void print(const char *p)
{
	while (*p)
		putchar(*(p++));
}

void print_ln(const char *p)
{
	for (int i=0; i<20; i++)
	{
	    if (*p)
		    putchar(*(p++));
        else
		    putchar(' ');
    }
}

void clear()
{
    reg_uart_data = 0x7c;
    reg_uart_data = 0x2d;
}

void home()
{
    reg_uart_data = 254;
    reg_uart_data = 0x02;
}

void print_hex(uint32_t v, int digits)
{
	for (int i = digits - 1; i >= 0; i--) {
		char c = "0123456789abcdef"[(v >> (4*i)) & 15];
		putchar(c);
	}
}

void print_dec(uint32_t v)
{
	if (v >= 2000) {
		print("OVER");
		return;
	}
	else if (v >= 1000) { putchar('1'); v -= 1000; }
	else putchar(' ');

	if 	(v >= 900) { putchar('9'); v -= 900; }
	else if	(v >= 800) { putchar('8'); v -= 800; }
	else if	(v >= 700) { putchar('7'); v -= 700; }
	else if	(v >= 600) { putchar('6'); v -= 600; }
	else if	(v >= 500) { putchar('5'); v -= 500; }
	else if	(v >= 400) { putchar('4'); v -= 400; }
	else if	(v >= 300) { putchar('3'); v -= 300; }
	else if	(v >= 200) { putchar('2'); v -= 200; }
	else if	(v >= 100) { putchar('1'); v -= 100; }
	else putchar('0');
		
	if 	(v >= 90) { putchar('9'); v -= 90; }
	else if	(v >= 80) { putchar('8'); v -= 80; }
	else if	(v >= 70) { putchar('7'); v -= 70; }
	else if	(v >= 60) { putchar('6'); v -= 60; }
	else if	(v >= 50) { putchar('5'); v -= 50; }
	else if	(v >= 40) { putchar('4'); v -= 40; }
	else if	(v >= 30) { putchar('3'); v -= 30; }
	else if	(v >= 20) { putchar('2'); v -= 20; }
	else if	(v >= 10) { putchar('1'); v -= 10; }
	else putchar('0');

	if 	(v >= 9) { putchar('9'); v -= 9; }
	else if	(v >= 8) { putchar('8'); v -= 8; }
	else if	(v >= 7) { putchar('7'); v -= 7; }
	else if	(v >= 6) { putchar('6'); v -= 6; }
	else if	(v >= 5) { putchar('5'); v -= 5; }
	else if	(v >= 4) { putchar('4'); v -= 4; }
	else if	(v >= 3) { putchar('3'); v -= 3; }
	else if	(v >= 2) { putchar('2'); v -= 2; }
	else if	(v >= 1) { putchar('1'); v -= 1; }
	else putchar('0');
}

void print_digit(uint32_t v)
{
    v &= (uint32_t)0x000F;

    if (v == 9) { putchar('9'); }
    else if (v == 8) { putchar('8'); }
    else if (v == 7) { putchar('7'); }
    else if (v == 6) { putchar('6'); }
    else if (v == 5) { putchar('5'); }
    else if (v == 4) { putchar('4'); }
    else if (v == 3) { putchar('3'); }
    else if (v == 2) { putchar('2'); }
    else if (v == 1) { putchar('1'); }
    else if (v == 0) { putchar('0'); }
    else putchar('*');
}

char getchar_prompt(char *prompt)
{
	int32_t c = -1;

	uint32_t cycles_begin, cycles_now, cycles;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));

//	reg_leds = ~0;

	if (prompt)
		print(prompt);

	while (c == -1) {
		__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
		cycles = cycles_now - cycles_begin;
		if (cycles > 12000000) {
			if (prompt)
				print(prompt);
			cycles_begin = cycles_now;
//			reg_leds = ~reg_leds;
		}
		c = reg_uart_data;
	}

//	reg_leds = 0;
	return c;
}

char getchar()
{
	return getchar_prompt(0);
}

// ----------------------------------------------------------------------

void cmd_read_flash_regs_print(uint32_t addr, const char *name)
{
    uint32_t buffer[2] = {0x65000000 | addr, 0x0}; //
    flashio(buffer, 6, 0);

    print("0x");
    print_hex(addr, 6);
    print(" ");
    print(name);
    print(" 0x");
    print_hex(buffer[1], 2);
    print("  ");
}

void cmd_echo()
{
	print("Return to menu by sending '!'\n\n");
	char c;
	while ((c = getchar()) != '!') {
		if (c == '\r')
		    putchar('\n');
        else
		    putchar(c);
    }
}

// ----------------------------------------------------------------------

void cmd_read_flash_regs()
{
    cmd_read_flash_regs_print(0x800000, "SR1V");
    cmd_read_flash_regs_print(0x800002, "CR1V");
    cmd_read_flash_regs_print(0x800003, "CR2V");
    cmd_read_flash_regs_print(0x800004, "CR3V");
}

//void rtc_run()
//{
//    write_i2c_slave(RTC_I2C_ADDR, 0x00, 0x00);
//}
//
//void rtc_stop()
//{
//    write_i2c_slave(RTC_I2C_ADDR, 0x00, 0x10);
//}

void read_rtc()
{
    uint32_t data[2];
    unsigned char d;

//    rtc_stop();

//    data = read_i2c_slave_byte(RTC_I2C_ADDR, 0x00); // RTC DS3231

    d = read_i2c_slave_byte(RTC_I2C_ADDR, RTC_I2C_REG);

//    read_i2c_slave_bytes(RTC_I2C_ADDR, RTC_I2C_REG, data, 3);

//    data[0] &= (uint32_t) 0x007F;
//    data[1] &= (uint32_t) 0x007F;
//    data[2] &= (uint32_t) 0x003F;

//    clear();
//    print("\r");
//    print_hex(d,2);
//    print("Time = ");
//    print_digit(BCD_DIGIT1(data[2]));
//    print_digit(BCD_DIGIT0(data[2]));
//    print(":");
//    print_digit(BCD_DIGIT1(data[1]));
//    print_digit(BCD_DIGIT0(data[1]));
//    print(":");
//    print_digit(BCD_DIGIT1(data[0]));
//    print_digit(BCD_DIGIT0(data[0]));
//    print("     ");

//    putchar('A');
//    rtc_run();
}

// ----------------------------------------------------------------------
// Raven demo:  Demonstrate various capabilities of the Raven testboard.
//
// 1. Flash:  Set the flash to various speed modes
// 2. GPIO:   Drive the LEDs in a loop
// 3. UART:   Print text to the UART LCD display
// 4. RC Osc: Enable the 100MHz RC oscillator and output on a GPIO line
// 5. DAC:    Enable the DAC and ramp
// 6. ADC:    Enable both ADCs and periodically read and display the DAC
//	      and bandgap values on the UART display.
// ----------------------------------------------------------------------
// NOTE:  There are two versions of this demo.  Demo 1 runs only up to
//	  2X speed (DSPI + CRM).  The higher speeds cause some timing
//	  trouble, which may be due to reflections on the board wires
//	  for IO2 and IO3.  Not sure of the mechanism.  To keep the
//	  boards running without failure, do not use QSPI modes.

void main()
{
	uint32_t i, j;
	uint8_t data[3];

	int r;

	set_flash_latency(8);

	// Start in standard (1x speed) mode

	// NOTE: Crystal on testboard running at 12.5MHz
	// Internal clock is 8x crystal, or 100MHz
	// Divided by clkdiv is 9.6 kHz
	// So at this crystal rate, use clkdiv = 10417 for 9600 baud.

    // Set UART clock to 9600 baud
//	reg_uart_clkdiv = 6667;   // for 8MHz osc
//	reg_uart_clkdiv = 5000;   // for 6MHz osc

//	rtc_run();

	reg_gpio_enb = 0x0000;
	reg_gpio_data = 0x0001;

//    for (j = 0; j < 70000; j++);
    for (j = 0; j < 1000; j++);

    reg_gpio_data = 0x0008;

	// This should appear on the LCD display 4x20 characters.
//    print("Starting...\n");

//    reg_i2c_config = 0;
//    reg_i2c_data = 0;

    // Enable I2C with prescaler set for 100kb/s (standard mode)
    // core clk / (i2c clk * 5)
    i2c_init(128);
    reg_gpio_data = 0x000b;


//
//    reg_gpio_data = 0x0005;
//
//    print("Press ENTER to continue..\n");
//    while (getchar() != '\r') {}
//
////    cmd_echo();
//
//    print("\n\n");
//
//    reg_gpio_data = 0x000f;

	while (1) {

        // read and display real-time clock
//        read_rtc();
        *(data) = (uint32_t) 0x00;

        write_i2c_slave_byte_eeprom(EEPROM_I2C_ADDR, 0x0150, 0x77);

        reg_gpio_data = 0x0003;

        read_i2c_slave_byte_eeprom(EEPROM_I2C_ADDR, 0x0150, data);

        reg_gpio_data = 0x0f;

//        for (j = 0; j < 100; j++);
//        reg_gpio_data = data[0];
//        for (j = 0; j < 100; j++);
//        reg_gpio_data = data[1];
//        for (j = 0; j < 100; j++);
//        reg_gpio_data = data[2];
//        for (j = 0; j < 100; j++);
//        reg_gpio_data = data[3];
//
//        if ((data[0] == 0x77) || (data[1] == 0x77) || (data[2] == 0x77) || (data[3] == 0x77))
//            reg_gpio_data = 0x0005;
//        else
//            reg_gpio_data = 0x000c;

        // Send command 6, data byte 0xfa
//        r = i2c_send(RTC_I2C_ADDR, 0xfa);
//        data = read_i2c_slave_byte(RTC_I2C_ADDR, 0x02);

//        if (r != 0)
//            reg_gpio_data |= 0x0002;

//        if ((reg_i2c_status & I2C_STAT_AL)  == 1)
//            reg_gpio_data |= 0x0004;
//
//        if ((reg_i2c_status & I2C_STAT_IF)  == 1)
//        {
//            reg_gpio_data |= 0x0008;
//            reg_i2c_command = 0x0001;
//        }

//        print("data = ");
//        print_hex(data, 2);
//        print("  0x");
//        print_hex(reg_i2c_status, 2);
//        print("\n");

//        for (j = 0; j < 70000; j++);

//        reg_gpio_data = 0x0000;

        // Send command 6, data byte 0xfa
//        r = i2c_send(RTC_I2C_ADDR, 0xfa);
//        data = read_i2c_slave_byte(RTC_I2C_ADDR, 0x02);
//        i2c_send( RTC_I2C_ADDR, 0x02 );

//        if (r != 0)
//            reg_gpio_data |= 0x0002;
//
//        if ((reg_i2c_status & I2C_STAT_AL)  == 1)
//            reg_gpio_data |= 0x0004;
//
//        if ((reg_i2c_status & I2C_STAT_IF)  == 1)
//        {
//            reg_gpio_data |= 0x0008;
//            reg_i2c_command = 0x0001;
//        }

//        for (j = 0; j < 70000; j++); // 2 sec
        while (1);

	}
}

