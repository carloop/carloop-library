/* Transmit and receive messages through SocketCAN on Linux
 *
 * Uses the Serial port to send/receive CAN messages in an ASCII
 * protocol called SLCAN.
 *
 * Summary of commands a computer can send to the Carloop:
 *
 * O\r  Opens the CAN connection
 * C\r  Close the CAN connection
 * Sn\r Sets the CAN bit rate (defaults to 500 kbit/s)
 *      This takes effect the next time the CAN is opened.
 *
 *      The following values are available:
 *      S0 10 kbit/s
 *      S1 20 kbit/s
 *      S2 50 kbit/s
 *      S3 100 kbit/s
 *      S4 125 kbit/s
 *      S5 250 kbit/s
 *      S6 500 kbit/s
 *      S7 800 kbit/s
 *      S8 1 Mbit/s
 * t<id><len><data>\n Transmits a CAN message
 *      <id> must be 3 character hex number
 *      <len> must be a 1 character hex number
 *      <data> must be the data byte values in hex
 *
 *      Example:
 *      t1234aabbccdd\n transmits message 0x123 with data 0xaabbccdd
 *
 * Where \r stands for ASCII character 0x0a, new line.
 *
 * When the Carloop receives a CAN message it prints it to the Serial
 * port in the t<id><len><data> format
 *
 * Copyright 2016 Julien Vanier
 *
 * Distributed under the MIT license. See LICENSE.txt for more details.
 */

#include "application.h"
#include "carloop.h"

SYSTEM_THREAD(ENABLED);

void receiveMessages();
void printReceivedMessage(const CANMessage &message);
void parseInput(char c);
void openCAN();
void closeCAN();
void changeCANSpeed(const char *buf, unsigned n);
void transmitMessage(const char *buf, unsigned n);

Carloop<CarloopRevision2> carloop;

const char NEW_LINE = '\r';
char inputBuffer[40];
unsigned inputPos = 0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  if (carloop.can().isEnabled()) {
    receiveMessages();
  }
}

void receiveMessages()
{
  CANMessage message;
  while(carloop.can().receive(message))
  {
    printReceivedMessage(message);
  }
}

void printReceivedMessage(const CANMessage &message)
{
  Serial.printf("t%03x%d", message.id, message.len);
  for(auto i = 0; i < message.len; i++) {
    Serial.printf("%02x", message.data[i]);
  }
  Serial.write(NEW_LINE);
}

void serialEvent()
{
    parseInput(Serial.read());
}

void parseInput(char c)
{
  if (inputPos < sizeof(inputBuffer))
  {
    inputBuffer[inputPos] = c;
    inputPos++;
  }

  if (c == NEW_LINE)
  {
    switch (inputBuffer[0])
    {
      case 'O':
        openCAN();
        break;
      case 'C':
        closeCAN();
        break;
      case 'S':
        changeCANSpeed(&inputBuffer[1], inputPos - 2);
        break;
      case 't':
        transmitMessage(&inputBuffer[1], inputPos - 2);
        break;
    }

    inputPos = 0;
  }
}
bool opened()
{
  return carloop.can().isEnabled();
}

void openCAN()
{
  if (opened())
  {
    return;
  }

  carloop.enableCAN();
}

void closeCAN()
{
  if (!opened())
  {
    return;
  }

  carloop.disableCAN();
}

void changeCANSpeed(const char *buf, unsigned n)
{
  if (n == 0)
  {
    return;
  }

  unsigned speed = 0;
  switch (buf[0])
  {
    case '0': speed = 10000; break;
    case '1': speed = 20000; break;
    case '2': speed = 50000; break;
    case '3': speed = 100000; break;
    case '4': speed = 125000; break;
    case '5': speed = 250000; break;
    case '6': speed = 500000; break;
    case '7': speed = 800000; break;
    case '8': speed = 1000000; break;
    default: return;
  }

  carloop.setCANSpeed(speed);
}

unsigned hex2int(char c)
{
  if (c >= '0' && c <= '9')
  {
    return c - '0';
  }
  else if (c >= 'a' && c <= 'f')
  {
    return c - 'a' + 10;
  }
  else if (c >= 'A' && c <= 'F')
  {
    return c - 'A' + 10;
  }

  return 0;
}



void transmitMessage(const char *buf, unsigned n)
{
  if (!opened())
  {
    return;
  }

  CANMessage message;

  if (n < 4) {
    return;
  }

  message.id = (hex2int(buf[0]) << 8) | (hex2int(buf[1]) << 4) | hex2int(buf[2]);
  message.len = hex2int(buf[3]);
  buf += 4;
  n -= 4;

  if (message.len > 8)
  {
    return;
  }

  for (unsigned i = 0; i < message.len && n >= 2; i++, buf += 2, n -= 2)
  {
    message.data[i] = (hex2int(buf[0]) << 4) | hex2int(buf[1]);
  }

  carloop.can().transmit(message);
}

