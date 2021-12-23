#include <SPI.h>

#define CAN_2515
// #define CAN_2518FD

// Set SPI CS Pin according to your hardware

#if defined(SEEED_WIO_TERMINAL) && defined(CAN_2518FD)
// For Wio Terminal w/ MCP2518FD RPi Hat：
// Channel 0 SPI_CS Pin: BCM 8
// Channel 1 SPI_CS Pin: BCM 7
// Interupt Pin: BCM25
const int SPI_CS_PIN  = BCM8;
const int CAN_INT_PIN = BCM25;
#else
// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN1 = 8;
const int SPI_CS_PIN2 = 10;
const int CAN_INT_PIN = 2;
#endif


#ifdef CAN_2518FD
#include "mcp2518fd_can.h"
mcp2518fd CAN1(SPI_CS_PIN1); // Set CS pin
mcp2518fd CAN2(SPI_CS_PIN2); // Set CS pin
#endif

#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN1(SPI_CS_PIN1); // Set CS pin
mcp2515_can CAN2(SPI_CS_PIN2); // Set CS pin
#endif

void setup() {
    SERIAL_PORT_MONITOR.begin(2000000);
    while(!Serial){};
    // init can bus : baudrate = 500k
    while (CAN_OK != CAN1.begin(CAN_500KBPS, MCP_16MHz)) {             
        SERIAL_PORT_MONITOR.println("CAN1 init fail, retry...");
        delay(100);
    }

    while (CAN_OK != CAN2.begin(CAN_500KBPS, MCP_16MHz)) {             
        SERIAL_PORT_MONITOR.println("CAN2 init fail, retry...");
        delay(100);
    }
    
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}

void mon(unsigned long canId, String direction, unsigned char *buffer, unsigned char length) {
    SERIAL_PORT_MONITOR.print(direction);
    SERIAL_PORT_MONITOR.print(length, HEX);
    SERIAL_PORT_MONITOR.print("\t");
    SERIAL_PORT_MONITOR.print(canId, HEX);
    SERIAL_PORT_MONITOR.print("\t");
    
    for (int i = 0; i < length; i++) { // print the data
        SERIAL_PORT_MONITOR.print(buffer[i], HEX);
        SERIAL_PORT_MONITOR.print("\t");
    }
    SERIAL_PORT_MONITOR.println();
}

void mon2(unsigned long canId, String direction, unsigned char *buffer, unsigned char length) {
     SERIAL_PORT_MONITOR.println(buffer[7], HEX);
    //SERIAL_PORT_MONITOR.println();
}


unsigned char len1 = 0;
unsigned char buf1[8];
unsigned char len2 = 0;
unsigned char buf2[8];
unsigned long can1Id;
unsigned long can2Id;

void loop() {
    if (CAN_MSGAVAIL == CAN1.checkReceive()) {         // check if data coming
        CAN1.readMsgBuf(&len1, buf1);    // read data,  len: data length, buf: data buf
        can1Id = CAN1.getCanId();
        CAN2.sendMsgBuf(can1Id, 0, len1, buf1);
        //mon2(can1Id, "1->2 ", buf1, len1);
    }

    if (CAN_MSGAVAIL == CAN2.checkReceive()) {         // check if data coming
        CAN2.readMsgBuf(&len2, buf2);    // read data,  len: data length, buf: data buf
        can2Id = CAN2.getCanId();
        CAN1.sendMsgBuf(can2Id, 0, len2, buf2);
        //mon2(can2Id, "2->1 ", buf2, len2);
    }
    //unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 0, 0};
    //delay(500);       
    //CAN2.sendMsgBuf(0x00, 0, 8, stmp, 5, false, true);
}
