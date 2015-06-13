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
 * $LastChangedRevision$
 */
 

/** @defgroup ble_uart_project_template ble_uart_project_template
@{
@ingroup projects
@brief Empty project that can be used as a template for new projects.

@details
This project is a firmware template for new projects. 
The project will run correctly in its current state.
It can send data on the UART TX characteristic
It can receive data on the UART RX characterisitc.
With this project you have a starting point for adding your own application functionality.

The following instructions describe the steps to be made on the Windows PC:

 -# Install the Master Control Panel on your computer. Connect the Master Emulator 
    (nRF2739) and make sure the hardware drivers are installed.

-# You can use the nRF UART app in the Apple iOS app store with this UART template app

-# You can send data from the Arduino serial monitor, maximum length of a string is 19 bytes
   Set the line ending to "Newline" in the Serial monitor (The newline is also sent over the air

 *
 * Click on the "Serial Monitor" button on the Arduino IDE to reset the Arduino and start the application.
 * The setup() function is called first and is called only once for each reset of the Arduino.
 * The loop() function as the name implies is called in a loop.
 *
 * The setup() and loop() function are called in this way.
 * main() 
 *  {
 *   setup(); 
 *   while(1)
 *   {
 *     loop();
 *   }
 * }
 *    
 */


#include <SPI.h>
//#include <avr/pgmspace.h>
#undef PROGMEM
//#define PROGMEM __attribute__(( section(".progmem.data") ))
#define PROGMEM
#define PSTR(x) (x)

#include <ble_system.h>
#include <lib_aci.h>
#include <aci_setup.h>

/**
Put the nRF8001 setup in the RAM of the nRF8001.
*/
#include "services.h"
/**
Include the services_lock.h to put the setup in the OTP memory of the nRF8001.
This would mean that the setup cannot be changed once put in.
However this removes the need to do the setup of the nRF8001 on every reset.
*/


#ifdef SERVICES_PIPE_TYPE_MAPPING_CONTENT
    static services_pipe_type_mapping_t
        services_pipe_type_mapping[NUMBER_OF_PIPES] = SERVICES_PIPE_TYPE_MAPPING_CONTENT;
#else
    #define NUMBER_OF_PIPES 0
    static services_pipe_type_mapping_t * services_pipe_type_mapping = NULL;
#endif

/*
Total credit should be extracted from the ACI Device Started event.
Since the Bluetooth shield v1.1 does not reset the nRF8001 when the Arduino is reset the
ACI Device Started Event is not generated occasionally.
This issue should be fixed in shield v1.2. This issue does not exist for the 
nRF2740, nRF2741 modules or any other module that connects the nRF8001 reset line to the mcu
or resets the nRF8001 when the mcu is reset
*/
#define ACI_CREDIT_TOTAL_SHIELD_V1_1 	2


/* Store the setup for the nRF8001 in the flash of the AVR to save on RAM */
static hal_aci_data_t setup_msgs[NB_SETUP_MESSAGES] PROGMEM = SETUP_MESSAGES_CONTENT;



// aci_struct that will contain 
// total initial credits
// current credit
// current state of the aci (setup/standby/active/sleep)
// open remote pipe pending
// close remote pipe pending
// Current pipe available bitmap
// Current pipe closed bitmap
// Current connection interval, slave latency and link supervision timeout
// Current State of the the GATT client (Service Discovery)
// Status of the bond (R) Peer address
static struct aci_state_t aci_state;

/*
Temporary buffers for sending ACI commands
*/
static hal_aci_evt_t  aci_data;
//static hal_aci_data_t aci_cmd;

/*
Timing change state variable
*/
static bool timing_change_done = false;

/*
Used to test the UART TX characteristic notification
*/
static uint8_t uart_buffer[20];
static uint8_t uart_buffer_len = 0;

/*
Initialize the radio_ack. This is the ack received for every transmitted packet.
*/
//static bool radio_ack_pending = false;

/*
Description:

In this template we are using the BTLE as a UART and can send and receive packets.
The maximum size of a packet is 20 bytes.
When a command it received a response(s) are transmitted back.
Since the response is done using a Notification the peer must have opened it(subscribed to it) before any packet is transmitted.
The pipe for the UART_TX becomes available once the peer opens it.
See section 20.4.1 -> Opening a Transmit pipe 
In the master control panel, clicking Enable Services will open all the pipes on the nRF8001.

The ACI Evt Data Credit provides the radio level ack of a transmitted packet.
*/


void setup(void)
{ 
  Serial.begin(115200); //Also select the "Newline" to allow the Serial Monitior to send data back to the phone/master application.
  //Wait until the serial port is available (useful only for the leonardo)
  while(!Serial)
  {}  
  Serial.println(F("Arduino setup"));
  
  /**
  Point ACI data structures to the the setup data that the nRFgo studio generated for the nRF8001
  */   
  if (NULL != services_pipe_type_mapping)
  {
    aci_state.aci_setup_info.services_pipe_type_mapping = &services_pipe_type_mapping[0];
  }
  else
  {
    aci_state.aci_setup_info.services_pipe_type_mapping = NULL;
  }
  aci_state.aci_setup_info.number_of_pipes    = NUMBER_OF_PIPES;
  aci_state.aci_setup_info.setup_msgs         = setup_msgs;
  aci_state.aci_setup_info.num_setup_msgs     = NB_SETUP_MESSAGES;

  /*
  Workaround only for the Bluetooth low energy shield v1.1
  */
  aci_state.data_credit_total                 = ACI_CREDIT_TOTAL_SHIELD_V1_1;

  /* We initialize the data structures required to setup the nRF8001
  */
  lib_aci_init(&aci_state);
  
  /*
  Debug printing. This will print all the ACI Commands and ACI events
  on the Serial port.
  */
  lib_aci_debug_print(false);
  
  /*
  The Bluetooth low energy Arduino shield v1.1 requires about 100ms to reset.
  This is not required for the nRF2740, nRF2741 modules
  */
  delay(100);
  
  /*
  Send the soft reset command to the nRF8001 to get the nRF8001 to a known state.
  We do this since the Bluetooth low energy v1.1 has not exposed the RESET line of
  the nRF8001 through the Arduino lines.
  */
  lib_aci_radio_reset();
  
  while (1)
  {
    /*Wait for the command response of the radio reset command.
    as the nRF8001 will be in either SETUP or STANDBY after the ACI Reset Radio is processed
	*/	
    if (true == lib_aci_event_get(&aci_state, &aci_data))
    {
      aci_evt_t * aci_evt;      
      aci_evt = &aci_data.evt;
	  
      if (ACI_EVT_CMD_RSP == aci_evt->evt_opcode)
      {
            if (ACI_STATUS_ERROR_DEVICE_STATE_INVALID == aci_evt->params.cmd_rsp.cmd_status) //in SETUP
            {
              Serial.println(F("Do setup"));
              if (ACI_STATUS_TRANSACTION_COMPLETE != do_aci_setup(&aci_state))
              {
                Serial.println(F("Error in ACI Setup"));
              }              
            }
            else if (ACI_STATUS_SUCCESS == aci_evt->params.cmd_rsp.cmd_status) //We are now in STANDBY
            {
              /*Looking for an phone by sending radio advertisements
              When an phone connects to us we will get an ACI_EVT_CONNECTED event from the nRF8001
			  */
              lib_aci_connect(180/* in seconds */, 0x0050 /* advertising interval 50ms*/);
              Serial.println(F("Advertising started"));              
            }
          /*
          When we get the command response event we should break out of
          the while loop as the soft reset is complete.
          We should not break when other events are received.
          */
          break;
      }
      else
      {
        /*
        The ACI Device Started may arrive if the nRF8001 is reset.
        We are looking for the command response event for the ACI Radio Reset command
        so we extract any data that we want from the ACI events and continue to
        be in the while loop
         */
        if (ACI_EVT_DEVICE_STARTED == aci_evt->evt_opcode)
        {
            aci_state.data_credit_total = aci_evt->params.device_started.credit_available;            
        }
        else
        {
            Serial.print(F("Discarding ACI Event 0x"));
            Serial.println(aci_evt->evt_opcode, HEX);
        }
      }	  
    }
  }
}

bool uart_tx(uint8_t *buffer, uint8_t buffer_len)
{
	bool status = false;
	
	if (lib_aci_is_pipe_available(&aci_state, PIPE_UART_OVER_BTLE_UART_TX_TX) &&
	    (aci_state.data_credit_available >= 1))
	{
		status = lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, buffer, buffer_len);
		if (status)
		{
			aci_state.data_credit_available--;
		}
	}
	
	return status;
}

void aci_loop()
{
  // We enter the if statement only when there is a ACI event available to be processed
  if (lib_aci_event_get(&aci_state, &aci_data))
  {
    aci_evt_t * aci_evt;
    
    aci_evt = &aci_data.evt;    
    switch(aci_evt->evt_opcode)
    {
        /**
        As soon as you reset the nRF8001 you will get an ACI Device Started Event
        */
      case ACI_EVT_DEVICE_STARTED:
        aci_state.data_credit_total = aci_evt->params.device_started.credit_available;
        switch(aci_evt->params.device_started.device_mode)
        {
          case ACI_DEVICE_SETUP:
            /**
            When the device is in the setup mode
            */
            Serial.println(F("Evt Device Started: Setup"));
            if (ACI_STATUS_TRANSACTION_COMPLETE != do_aci_setup(&aci_state))
            {
              Serial.println(F("Error in ACI Setup"));
            }
            break;
          
          case ACI_DEVICE_STANDBY:
            Serial.println(F("Evt Device Started: Standby"));
            //Looking for an iPhone by sending radio advertisements
            //When an iPhone connects to us we will get an ACI_EVT_CONNECTED event from the nRF8001
            lib_aci_connect(180/* in seconds */, 0x0050 /* advertising interval 50ms*/);
            Serial.println(F("Advertising started"));
            break;
        }
        break; //ACI Device Started Event
        
      case ACI_EVT_CMD_RSP:
        //If an ACI command response event comes with an error -> stop
        if (ACI_STATUS_SUCCESS != aci_evt->params.cmd_rsp.cmd_status)
        {
          //ACI ReadDynamicData and ACI WriteDynamicData will have status codes of
          //TRANSACTION_CONTINUE and TRANSACTION_COMPLETE
          //all other ACI commands will have status code of ACI_STATUS_SCUCCESS for a successful command
          Serial.print(F("ACI Command "));
          Serial.println(aci_evt->params.cmd_rsp.cmd_opcode, HEX);
          Serial.println(F("Evt Cmd respone: Error. Arduino is in an while(1); loop"));
          while (1);
        }
        if (ACI_CMD_GET_DEVICE_VERSION == aci_evt->params.cmd_rsp.cmd_opcode)
        {
          //Store the version and configuration information of the nRF8001 in the Hardware Revision String Characteristic
          lib_aci_set_local_data(&aci_state, PIPE_DEVICE_INFORMATION_HARDWARE_REVISION_STRING_SET, 
            (uint8_t *)&(aci_evt->params.cmd_rsp.params.get_device_version), sizeof(aci_evt_cmd_rsp_params_get_device_version_t));
        }        
        break;
        
      case ACI_EVT_CONNECTED:
        Serial.println(F("Evt Connected"));
        aci_state.data_credit_available = aci_state.data_credit_total;
        
        /*
        Get the device version of the nRF8001 and store it in the Hardware Revision String
        */
        lib_aci_device_version();
        break;
        
      case ACI_EVT_PIPE_STATUS:
        Serial.println(F("Evt Pipe Status"));
        if (lib_aci_is_pipe_available(&aci_state, PIPE_UART_OVER_BTLE_UART_TX_TX) && (false == timing_change_done))
        {
          lib_aci_change_timing_GAP_PPCP(); // change the timing on the link as specified in the nRFgo studio -> nRF8001 conf. -> GAP. 
                                            // Used to increase or decrease bandwidth
          timing_change_done = true;
        }
        break;
        
      case ACI_EVT_TIMING:
        Serial.println(F("Evt link connection interval changed"));
        break;
        
      case ACI_EVT_DISCONNECTED:
        Serial.println(F("Evt Disconnected/Advertising timed out"));
        lib_aci_connect(180/* in seconds */, 0x0100 /* advertising interval 100ms*/);
        Serial.println(F("Advertising started"));        
        break;
        
      case ACI_EVT_DATA_RECEIVED:
        Serial.print(F("UART RX: 0x"));
        Serial.print(aci_evt->params.data_received.rx_data.pipe_number, HEX);
        {
          Serial.print(F(" Data(Hex) : "));
          for(int i=0; i<aci_evt->len - 2; i++)
          {
            Serial.print((char)aci_evt->params.data_received.rx_data.aci_data[i]);
            uart_buffer[i] = aci_evt->params.data_received.rx_data.aci_data[i];
            Serial.print(F(" "));
          }
          uart_buffer_len = aci_evt->len - 2;
        }
        Serial.println(F(""));
        if (lib_aci_is_pipe_available(&aci_state, PIPE_UART_OVER_BTLE_UART_TX_TX))
        {
				/*
				Do this to test the loopback otherwise comment it out
								
				if (!uart_tx(&uart_buffer[0], aci_evt->len - 2))
				{
					Serial.println(F("UART loopback failed"));
				}
				else
				{
					Serial.println(F("UART loopback OK"));
				}
				*/
        }
        break;
   
      case ACI_EVT_DATA_CREDIT:
        aci_state.data_credit_available = aci_state.data_credit_available + aci_evt->params.data_credit.credit;
        break;
      
      case ACI_EVT_PIPE_ERROR:
        /*See the appendix in the nRF8001 Product Specication for details on the error codes
		*/
        Serial.print(F("ACI Evt Pipe Error: Pipe #:"));
        Serial.print(aci_evt->params.pipe_error.pipe_number, DEC);
        Serial.print(F("  Pipe Error Code: 0x"));
        Serial.println(aci_evt->params.pipe_error.error_code, HEX);
                
        /*Increment the credit available as the data packet was not sent.
        The pipe error also represents the Attribute protocol Error Response sent from the peer and that should not be counted 
        for the credit.
		*/
		/*
		@todo The credit is incremented event when there is no connection, this is in-correct
		However since on the ACI CONNECTED event the credit is set correctly this not a major issue
		since credits are usable only when in a connection
		*/
        if (ACI_STATUS_ERROR_PEER_ATT_ERROR != aci_evt->params.pipe_error.error_code)
        {
          aci_state.data_credit_available++;
        }
        break;
   
           
    }
  }
  else
  {
    //Serial.println(F("No ACI Events available"));
    // No event in the ACI Event queue and if there is no event in the ACI command queue the arduino can go to sleep
    // Arduino can go to sleep now
    // Wakeup from sleep from the RDYN line
  }
}

String inputString     = "";     // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

//Hack for the DUE until Arduino fixes SerialEvent
// http://forum.arduino.cc/index.php?topic=135011.0
void serialEventRun(void) {
  if (Serial.available()) serialEvent();
} 

void loop() {

  //Process any ACI commands or events
  aci_loop();
  
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.print(F("Sending: "));
    Serial.println(inputString);
    inputString.toCharArray((char*)uart_buffer,20);
    
    if (inputString.length() > 20)
    {
        uart_buffer_len = 20;
        uart_buffer[19] = '\n';
        Serial.println(F("Serial input truncted"));
    }
    else
    {
        uart_buffer_len = inputString.length();
    }
    
    if (!lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, uart_buffer, uart_buffer_len))
      {
        Serial.println(F("Serial input dropped"));
      }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 Serial Event is NOT compatible with Leonardo, Micro, Esplora
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
