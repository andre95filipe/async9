# Electronic Systems - Sistemas Eletrónicos


## Asynchronous communication system 

### Description
* This system is composed of several nodes connected in master/slave architecture via UART;
* Each node uses a MAX485 differencial tranceiver;
* Each node has it's own address;
* The communication protocol uses 9 bits:
    * 8 bits for tghe message to transmit;
    * 1 bit to inform if it's an address or data (1 or 0 respectively);
* Master controls the LED's of each slave by pressing a button linked to a certain slave address;
* The main circuit is composed of:
    * 1 ATmega 328p (and necessary peripheral components);
    * 1 MAX485;
    * 1 LED and resistor;
    * 4 DIP switches to select the 4 bit address;
    * 2 buttons to control the 2 slaves.


### Firmware implementation
`void asynch9_init()`:
* Setup the baud rate;
* Set master as transmiter and slave as receiver;
* Set slave in multiprocessor mode;
* Set 9 bit frame.
* Doesn't return.

`uint8_t send_addr(uint8_t toSend)`:
* Wait for the transmit buffer to be clear;
* Set 9th bit to 1 to inform it is an address;
* Send target slave address set in *toSend*;
* Returns 1 if successful;

`uint8_t send_data(uint8_t toSend)`:
* Wait for the transmit buffer to be clear;
* Set 9th bit to 0 to inform it is data;
* Send target slave the new LED state.
* Return 1 if successful;

`uint8_t get_data()`:
* Check if it has any transmition errors. Return 0 if it has errors;
* Check the 9th bit:
    * If 1, means is address:
        * If it is the slaves address, turn off multiprocessor mode. If not, turn on multiprocessor mode.
    * If 0, means is data:
        * If data is 0, turn off LED. If data is 1, turn on LED;
* Returns 0 if successful;

`void setup()`:
* Set address pins and buttons pins as inputs with internal pullup resistors;
* Set LED and Write Enable (WREN) pins as outputs;
* Set WREN to 1 if master and 0 if slave;

`void loop()`:
* Runs infinitely, separating the code for the master and for the slave:
    * Master:
        * If button 1 changed state:
            * Send address if last "contacted" slave was a different slave. If it was the same slave, don't send address;
            * Send data (state of the button);
        * If button 2 changed state:
            * Send address if last "contacted" slave was a different slave. If it was the same slave, don't send address;
            * Send data (state of the button);
    * Slave:
        * Check if receive buffer is full. If it is, get data.  


### Firmware test

* Clone this repository with `git clone https://git.fe.up.pt/up201808899/asynch9_a2_b01.git`;
* Compile the *main.cpp* file with PlatformIO to get *.hex* file;
* Flash the *.hex* file to the master and slaves.

### Schematic

![Schematic](https://image.easyeda.com/histories/f59e69290e9a444aad2a27f319af673b.png)

### PCB layout

![Pcb](https://image.easyeda.com/histories/460c1efc10da4049b38d38f9de71851e.png)





