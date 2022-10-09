// STM32F103
#include "stm32f10x.h"

// cstd
#include <stdbool.h>

//user
#include "UART.h"

// Local variables
// Data
static uint8_t* transmit_buf;
static uint8_t* receive_buf;
// Number of data
static int to_transmit = 0;
static int to_receive = 0;
// Flags
static volatile bool transmitted = true;
static volatile bool received = true;

// Interrupt
void USART1_IRQHandler(void)
{
    // Transmit data register empty
    if ( ((USART1->SR & USART_SR_TXE) != 0 ) &&((USART1->CR1 & USART_CR1_TXEIE) != 0) ) {
        if (to_transmit != 0) {
            USART1->DR = transmit_buf[0];
            transmit_buf++;
            to_transmit--;
        }
        else
            USART1->CR1 &= ~USART_CR1_TXEIE;    // transmit data register empty interrupt disable
    }
    // Transmission complete
    if ( ((USART1->SR & USART_SR_TC) != 0) &&((USART1->CR1 & USART_CR1_TCIE) != 0) ) {
        transmitted = true;
        USART1->CR1 &= ~USART_CR1_TCIE;         // transmission complete interrupt disable
    }
    // Received data register not empty
    if ((USART1->SR & USART_CR1_RXNEIE) != 0) {
        if (received == true) {
            (void) USART1->DR;  // receive start bit
            return;
        }
        if (to_receive != 0) {
            receive_buf[0] = USART1->DR;
            receive_buf++;
            to_receive--;
        }
        if (to_receive == 0) {
            received = true;
        }
    }
}

void InitUart(void)
{
    USART1->CR1 |= USART_CR1_UE;    // USART enable
    USART1->BRR = USART9600*2;      // set baud rate
    USART1->CR1 |= USART_CR1_TE;    // transmitter enable
    USART1->CR1 |= USART_CR1_RE;    // receiver enable
    // Interrupts
    USART1->CR1 |=  USART_CR1_TXEIE     * 0 |   // transmit data register empty interrupt enable
                    USART_CR1_TCIE      * 0 |   // transmission complete interrupt enable
                    USART_CR1_RXNEIE    * 1;    // received data register not empty interrupt enable
}
void TransmitUart(uint8_t* buf, int num)
{
    transmit_buf = buf;
    to_transmit = num - 1;
    transmitted = false;
    
    USART1->DR = transmit_buf[0];
    transmit_buf++;
    
    USART1->CR1 |= USART_CR1_TXEIE;     // transmit data register empty interrupt enable
    USART1->CR1 |= USART_CR1_TCIE;      // transmission complete interrupt enable
}
void ReceiveUart(uint8_t* buf, int num)
{
    if (num == 0)
        return;
    
    receive_buf = buf;
    to_receive = num;
    received = false;
}
bool TransmittedUart(void)
{
    return transmitted;
}
bool ReceivedUart(void)
{
    return received;
}
void WaitForTransmitUart(void)
{
    while (! transmitted)
        ;
}
void WaitForReceiveUart(void)
{
    while (! received)
        ;
}
