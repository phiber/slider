/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision: 4808 $
 */ 

/** @file
@brief Implementation of the ACI transport layer module
*/

#include <SPI.h>
#include "hal_platform.h"
#include "hal/hal_aci_tl.h"
#include "hal/hal_io.h"
#include "ble_system.h"
//#include <avr/sleep.h>
#undef PSTR
#define PSTR(x) (x)

static void           m_print_aci_data(hal_aci_data_t *p_data);



static hal_aci_data_t received_data;
static uint8_t        spi_readwrite(uint8_t aci_byte);
static bool           aci_debug_print;



aci_queue_t    aci_tx_q;
aci_queue_t    aci_rx_q;

static void m_aci_q_init(aci_queue_t *aci_q)
{
  uint8_t loop;
  
  aci_debug_print = false;
  aci_q->head = 0;
  aci_q->tail = 0;
  for(loop=0; loop<ACI_QUEUE_SIZE; loop++)
  {
    aci_tx_q.aci_data[loop].buffer[0] = 0x00;
    aci_tx_q.aci_data[loop].buffer[1] = 0x00;
  }
}

void hal_aci_debug_print(bool state)
{
	aci_debug_print = state;
}

static bool m_aci_q_enqueue(aci_queue_t *aci_q, hal_aci_data_t *p_data)
{
  const uint8_t next = (aci_q->tail + 1) % ACI_QUEUE_SIZE;
  const uint8_t length = p_data->buffer[0];
  
  if (next == aci_q->head)
  {
    /* full queue */
    return false;
  }
  aci_q->aci_data[aci_q->tail].status_byte = 0;
  
  memcpy((uint8_t *)&(aci_q->aci_data[aci_q->tail].buffer[0]), (uint8_t *)&p_data->buffer[0], length + 1);
  aci_q->tail = next;
  
  return true;
}

//@comment after a port have test for the queue states, esp. the full and the empty states
static bool m_aci_q_dequeue(aci_queue_t *aci_q, hal_aci_data_t *p_data)
{
  if (aci_q->head == aci_q->tail)
  {
    /* empty queue */
    return false;
  }
  
  memcpy((uint8_t *)p_data, (uint8_t *)&(aci_q->aci_data[aci_q->head]), sizeof(hal_aci_data_t));
  aci_q->head = (aci_q->head + 1) % ACI_QUEUE_SIZE;
  
  return true;
}

bool m_aci_q_is_empty(aci_queue_t *aci_q)
{
  return (aci_q->head == aci_q->tail);
}

bool m_aci_q_is_full(aci_queue_t *aci_q)
{
  uint8_t next;
  bool state;
  
  next = (aci_q->tail + 1) % ACI_QUEUE_SIZE;  
  
  if (next == aci_q->head)
  {
    state = true;
  }
  else
  {
    state = false;
  }
    
  return state;
}



void m_print_aci_data(hal_aci_data_t *p_data)
{
  const uint8_t length = p_data->buffer[0];
  uint8_t i;
  Serial.print(length, DEC);
  Serial.print(" :");
  for (i=0; i<=length; i++)
  {
    Serial.print(p_data->buffer[i], HEX);
    Serial.print(F(", "));
  }
  Serial.println(F(""));
}


void m_rdy_line_handle(void)
{
  hal_aci_data_t *p_aci_data;
  
  /* Receive or transmit data
  */
  p_aci_data = hal_aci_tl_poll_get();
  
  /* Check if we received data
  */
  if (p_aci_data->buffer[0] > 0)
  {
    if (!m_aci_q_enqueue(&aci_rx_q, p_aci_data))
    {
      /* Receive Buffer full.
         Should never happen.
         Spin in a while loop.
         */
       Serial.println(F("Enqueue Error. Arduino in while loop."));
       while(1);
    }   
  }
}

bool hal_aci_tl_event_get(hal_aci_data_t *p_aci_data)
{
  bool was_full = m_aci_q_is_full(&aci_rx_q);
  
  if (m_aci_q_dequeue(&aci_rx_q, p_aci_data))
  {
    if (true == aci_debug_print)
    {
      Serial.print(" E");
      m_print_aci_data(p_aci_data);
    }
    
    if (was_full)
    {
    }
    return true;
  }
  else
  {
    return false;
  }
}

void hal_aci_tl_init()
{
  received_data.buffer[0] = 0;

  SPI.begin          (DUE_SPI_CSN);
  SPI.setBitOrder    (DUE_SPI_CSN, LSBFIRST);
  SPI.setClockDivider(DUE_SPI_CSN, 28); /* This will run the SPI at 3MHz assuming a 84MHz clock to the mcu*/
  SPI.setDataMode    (DUE_SPI_CSN, SPI_MODE0);


  
  /* initialize aci cmd queue */
  m_aci_q_init(&aci_tx_q);  
  m_aci_q_init(&aci_rx_q);

  //Configure the IO lines
  
  /*
  Since the reset line of the nRF8001 is not connected to the Arduino
  when using the Bluetooth low energy shield v1.1
  the code to use the reset line below are commented
  */
  
  //pinMode(HAL_IO_RADIO_RESET, OUTPUT);
  pinMode(HAL_IO_RADIO_RDY,   INPUT_PULLUP);
  pinMode(HAL_IO_RADIO_REQN,  OUTPUT);

  /*
  Avoid this for the DUE
  */
  //pinMode(SS,                 OUTPUT);
  
  //digitalWrite(HAL_IO_RADIO_RESET, 1);
  //delay(100);
  //digitalWrite(HAL_IO_RADIO_RESET, 0);
  //digitalWrite(HAL_IO_RADIO_RESET, 1);
  
  //digitalWrite(SCK,  0);
  //digitalWrite(MOSI, 0);
  /*
  Avoid this for the DUE
  */
  //digitalWrite(SS,   1);
  digitalWrite(HAL_IO_RADIO_REQN,   1);

  
}

bool hal_aci_tl_send(hal_aci_data_t *p_aci_cmd)
{
  const uint8_t length = p_aci_cmd->buffer[0];
  bool ret_val = false;

  if (length > HAL_ACI_MAX_LENGTH)
  {
    return false;
  }
  else
  {
    if (m_aci_q_enqueue(&aci_tx_q, p_aci_cmd))
    {
      ret_val = true;
	  if (true == aci_debug_print)
	  {
		Serial.print("C");
		m_print_aci_data(p_aci_cmd);
	  }
	  
	  HAL_IO_SET_STATE(HAL_IO_RADIO_REQN, 0);	  
    }
  }
  
  return ret_val;
}

static uint8_t spi_readwrite(const uint8_t aci_byte)
{
	return SPI.transfer(DUE_SPI_CSN, aci_byte, SPI_CONTINUE);
}

static uint8_t spi_readwrite_deselect(const uint8_t aci_byte)
{
	return SPI.transfer(DUE_SPI_CSN, aci_byte, SPI_LAST);
}

hal_aci_data_t * hal_aci_tl_poll_get(void)
{
  uint8_t byte_cnt;
  uint8_t byte_sent_cnt;
  uint8_t max_bytes;
  hal_aci_data_t data_to_send;

  digitalWrite(HAL_IO_RADIO_REQN, 0);
  
  // Receive from queue
  if (m_aci_q_dequeue(&aci_tx_q, &data_to_send) == false)
  {
    /* queue was empty, nothing to send */
    data_to_send.status_byte = 0;
    data_to_send.buffer[0] = 0;
  }
  
  //Change this if your mcu has DMA for the master SPI
  
  // Send length, receive header
  byte_sent_cnt = 0;
  received_data.status_byte = spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  // Send first byte, receive length from slave
  received_data.buffer[0]   = spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  
  if (0 == data_to_send.buffer[0])
  {
    max_bytes = received_data.buffer[0];
  }
  else
  {
    // Set the maximum to the biggest size. One command byte is already sent
    max_bytes = (received_data.buffer[0] > (data_to_send.buffer[0] - 1)) 
      ? received_data.buffer[0] : (data_to_send.buffer[0] - 1);
  }

  if (max_bytes > HAL_ACI_MAX_LENGTH)
  {
    max_bytes = HAL_ACI_MAX_LENGTH;
  }

  // Transmit/receive the rest of the packet 
  for (byte_cnt = 0; byte_cnt < max_bytes-1; byte_cnt++)
  {
    received_data.buffer[byte_cnt+1] =  spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  }
  
  /*
  When the length of the ACI command is only 1 and there is no ACI event i.e. <length><command opcode>
  We are clocking out an extra byte to allow the DUE to de-select SPI
  The extra byte should be ignored by the nRF8001 as the length byte is correct.
  */
  received_data.buffer[byte_cnt+1] =  spi_readwrite_deselect(data_to_send.buffer[byte_sent_cnt++]);
  digitalWrite(HAL_IO_RADIO_REQN, 1);
  //RDYN should follow the REQN line in approx 100ns
  
  
  if (false == m_aci_q_is_empty(&aci_tx_q))
  {
    //Lower the REQN line to start a new ACI transaction         
    digitalWrite(HAL_IO_RADIO_REQN, 0); 
  }
  
  /* valid Rx available or transmit finished*/
  return (&received_data);
}





void m_aci_q_flush(void)
{
  noInterrupts();
  /* re-initialize aci cmd queue and aci event queue to flush them*/
  m_aci_q_init(&aci_tx_q);
  m_aci_q_init(&aci_rx_q);
  interrupts();
}