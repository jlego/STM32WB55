#include "stm32wbxx.h"

static int uart_initialized = 0;

void UartLogger_Init(void) {
  if (uart_initialized) return;

  // Enable GPIOA clock
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
  
  // Enable USART1 clock (APB2)
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  
  // Configure PA9 as USART1_TX (Alternate Function, Push-Pull)
  // AFRL for pins 0-7, AFRH for pins 8-15
  // PA9: clear bits 4-7 in AFRH, set to AF7 (0111)
  GPIOA->AFR[1] &= ~(0xF << ((9-8)*4));  // Clear AF9
  GPIOA->AFR[1] |= (0x7 << ((9-8)*4));   // Set AF7
  
  // PA9: Mode = Alternate Function (10), Output type = Push-Pull (0), Speed = High (10), Pull = No Pull (00)
  GPIOA->MODER &= ~(0x3 << (9*2));       // Clear mode
  GPIOA->MODER |= (0x2 << (9*2));        // Set AF mode
  GPIOA->OTYPER &= ~(1 << 9);            // Push-pull
  GPIOA->OSPEEDR &= ~(0x3 << (9*2));     // Clear speed
  GPIOA->OSPEEDR |= (0x2 << (9*2));      // High speed
  GPIOA->PUPDR &= ~(0x3 << (9*2));       // No pull-up/pull-down
  
  // Configure USART1: 115200 baud, 8N1, TX only
  // Assuming PCLK2 = 64MHz (SystemCoreClock / 1)
  // BRR = PCLK2 / BaudRate = 64000000 / 115200 = 555 (0x22B)
  uint32_t pclk2_freq = SystemCoreClock; // Assuming APB2 divider = 1
  uint32_t brr = pclk2_freq / 115200;
  USART1->BRR = brr;
  
  // Enable USART1, TX, TE (Transmitter Enable)
  USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
  
  uart_initialized = 1;
}

void UartLogger_Send(const uint8_t* data, uint16_t size) {
  if (!uart_initialized) return;
  
  for (uint16_t i = 0; i < size; i++) {
    // Wait until TXE (Transmit Data Register Empty) is set
    while (!(USART1->ISR & USART_ISR_TXE)) {}
    USART1->TDR = data[i];
  }
  // Wait until TC (Transmission Complete) is set
  while (!(USART1->ISR & USART_ISR_TC)) {}
}

int UartLogger_IsInitialized(void) {
  return uart_initialized;
}