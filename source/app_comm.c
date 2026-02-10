
/**
 ********************************************************************************
 * @file    app.c
 * @author  
 * @date    
 * @brief   
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include <string.h>
#include "uart.h"
#include "app_comm.h"
#include "system.h"

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#define MAX_RX_PACKET_BUFFER_LEN 20u
#define TX_BUFFER_COLLECTION_SIZE 20u
#define TX_BUFFER_SIZE_PER_PACKET 37u // 1(STX) + 1(QM) + 1(dataLen) + 1(command) + 32(data) + 1(ETX)

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
uint8_t _rxBuffer[UART_RX_BUFFER_SIZE];

circular_buf_t RxBuffer =
{ 
      .buffer = _rxBuffer,
      .head = 0u,
      .tail = 0u,
      .max = UART_RX_BUFFER_SIZE /*max size of the buffer*/
};

packet_t rxPacketBuffer[MAX_RX_PACKET_BUFFER_LEN];
uint8_t rxPacketBufferHead = 0u;
uint8_t rxPacketBufferTail = 0u;

uint8_t* txBufferCollection[TX_BUFFER_COLLECTION_SIZE] = {0};
uint8_t txBuffer[TX_BUFFER_SIZE_PER_PACKET * TX_BUFFER_COLLECTION_SIZE] = {0};
uint8_t txBufferCollectionHead = 0u;

/************************************
 * STATIC VARIABLES
 ************************************/

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
bool packet_circ_buf_put(packet_t *packet);
bool packet_circ_buf_get(packet_t *packet);
uint8_t packet_circ_buf_getCount(void);

/************************************
 * STATIC FUNCTIONS
 ************************************/
bool packet_circ_buf_put(packet_t *packet)
{
    if(packet_circ_buf_getCount() >= MAX_RX_PACKET_BUFFER_LEN)
    {
        // Buffer full
        return false;
    }

    rxPacketBuffer[rxPacketBufferHead] = *packet;
    rxPacketBufferHead++;
    if(rxPacketBufferHead >= MAX_RX_PACKET_BUFFER_LEN)
    {
        rxPacketBufferHead = 0u;
    }

    return true;
}

bool packet_circ_buf_get(packet_t *packet)
{
    if(packet_circ_buf_getCount() == 0)
    {
        // Buffer empty
        return false;
    }

    *packet = rxPacketBuffer[rxPacketBufferTail];
    rxPacketBufferTail++;
    if(rxPacketBufferTail >= MAX_RX_PACKET_BUFFER_LEN)
    {
        rxPacketBufferTail = 0u;
    }
    return true;
}

uint8_t packet_circ_buf_getCount(void)
{
    if(rxPacketBufferTail <= rxPacketBufferHead)
    {
        return (rxPacketBufferHead - rxPacketBufferTail);
    }
    else
    {
        return(MAX_RX_PACKET_BUFFER_LEN - (rxPacketBufferTail - rxPacketBufferHead));
    }
}


/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void APP_COMM_Init(void)
{
    UART_Init(&RxBuffer);
    SYSTEM_Init();
    
    // Initialise transmit buffer collection
    for(uint8_t i = 0u; i < TX_BUFFER_COLLECTION_SIZE; i++)
    {
        txBufferCollection[i] = &(txBuffer[i * TX_BUFFER_SIZE_PER_PACKET]);
    }

}

void APP_COMM_Handler(void)
{
    static uint8_t packetDataHead = 0u;
    static uint8_t packetDataLen = 0u;
    static packet_t rxPacket;

    // ========= RECEIVE HANDLER =========
    // Turn off interrupts while accessing the buffer

    while(1)
    {
        uint8_t byte;
        __disable_irq();
        if(UART_ReadByte(&byte) == false)
        {
            __enable_irq();
            break;
        }
        __enable_irq();
        // Process byte here

        switch(packetDataHead)
        {
            case 0:
                // STX
                if(byte == STX)
                {
                    packetDataHead++;
                }
                break;
            case 1:
                // QM
                if(byte == QM)
                {
                    packetDataHead++;
                }
                else
                {
                    packetDataHead = 0u;
                }
                break;
            case 2:
                if(packetDataLen > MAX_PACKET_DATA_LEN)
                {
                    // Error, data length too long
                    packetDataHead = 0u;
                    break;
                }
                packetDataLen = byte;
                packetDataHead++;
                break;
            default:
                if(packetDataHead == 3)
                {
                    rxPacket.packetCommand = byte;
                }
                else if(packetDataHead == (4 + packetDataLen))
                {
                    // ETX
                    if(byte == ETX)
                    {
                        // Packet complete
                        rxPacket.dataLen = packetDataLen;
                        // Store packet in buffer
                        if(!packet_circ_buf_put(&rxPacket))
                        {
                            // Packet buffer full, packet lost
                            // TODO
                            
                        }
                    }
                    // Reset for next packet
                    packetDataHead = 0u;
                    packetDataLen = 0u;
                    break;
                }
                else
                {
                    rxPacket.data[packetDataHead - 4] = byte;
                }
                packetDataHead++;
                break;
        }
    }
}

bool APP_COMM_GetPacket(packet_t *packet)
{
    return packet_circ_buf_get(packet);
}

bool APP_COMM_SendPacket(action_e action, uint8_t *pData, uint8_t dataLen)
{
    uint8_t *txBuffer = txBufferCollection[txBufferCollectionHead];

    txBuffer[0] = STX;
    txBuffer[1] = QM;
    txBuffer[2] = dataLen;
    txBuffer[3] = action;
    memcpy(&txBuffer[4], pData, dataLen);
    txBuffer[4 + dataLen] = ETX;

    if(UART_Transmit(txBuffer, dataLen + 5u))
    {
        txBufferCollectionHead = (txBufferCollectionHead + 1u) % TX_BUFFER_COLLECTION_SIZE;
        return true;
    }
    return false;
}
