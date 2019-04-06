/*
 Name:		MAX7219_LED_Display_Driver.cpp
 Created:	27/03/2019 10:39:27 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 Editor:	http://www.visualmicro.com
 License:	GPL v2
*/

#include "MAX7219_LED_Display_Driver.h"

MAX7219::MAX7219(const uint8_t& dOutPin, const uint8_t& loadPin, const uint8_t& clkPin, const uint8_t& scanLimit, const uint8_t& intensity)
	: m_dataOut(dOutPin), m_load(loadPin), m_clk(clkPin)
{
	pinMode(m_dataOut, OUTPUT);
	pinMode(m_load, OUTPUT);
	pinMode(m_clk, OUTPUT);
	digitalWrite(m_dataOut, LOW);
	digitalWrite(m_load, LOW);
	digitalWrite(m_clk, LOW);
	setScanLimit(scanLimit);
	setIntensity(intensity);
	setBcdDecodeMode(0xFF);//set BCD mode on all digits
	clear();
	setShutdown(false);
}

void MAX7219::sendDigit(const digit& dType, const uint8_t& valueToSend, const bool& withDot)
{
	bool bcdValType = (valueToSend > 0x0F) ? false : true;//check if we have value from 0 to 15 (all possible BCD codes), if yes then use BCD
	uint8_t val = valueToSend;

	if (bcdValType && !isBcdSet(dType)) setBcdDecodeMode(numToBitMask(static_cast<uint8_t>(dType)));//if BCD is not setted, then set BCD mode on given digit because value is in BCD range
	else if (!bcdValType && isBcdSet(dType)) setRawDecodeMode(numToBitMask(static_cast<uint8_t>(dType)));

	if (bcdValType && withDot) val |= 0x80;//enable dot

	sendDataToRegister((static_cast<uint16_t>(dType) << 8) | val);
}

void MAX7219::setIntensity(const uint8_t& intensity)
{
	uint8_t level = 0;

	if (intensity) level = static_cast<uint8_t>(intensity / 0x10);

	sendDataToRegister(static_cast<uint16_t>(0x0A00 | level));
}

void MAX7219::setScanLimit(const uint8_t& scanLimit)
{
	uint8_t limit = scanLimit;

	if (scanLimit > 8) limit = 8;
	else if (scanLimit < 1) limit = 1;

	sendDataToRegister(static_cast<uint16_t>(0x0B00 | (scanLimit - 1)));
}

void MAX7219::sendNumber(const int32_t& numberToSend)
{
	uint32_t number = (numberToSend < 0) ? -numberToSend : numberToSend;
	uint8_t digitsSend = 1;

	if (!number)//check if number is 0
	{
		sendDigit(static_cast<MAX7219::digit>(1), 0);
		return;
	}

	for (uint8_t i = 1; i < 9; i++, digitsSend++)
	{
		sendDigit(static_cast<MAX7219::digit>(i), number % 10);
		number /= 10;
		if (!number) break;//if number ended, then stop sending digits
	}

	if (numberToSend < 0 && digitsSend < 8)
	{
		sendDigit(static_cast<MAX7219::digit>(digitsSend + 1), 0x0A);//send minus sign
	}
}

void MAX7219::sendNumber(const double& numberToSend, const uint8_t& precision)
{
	uint32_t number = (numberToSend < 0.0) ? static_cast<uint32_t>(-numberToSend) : static_cast<uint32_t>(numberToSend);
	uint32_t decimals = static_cast<uint32_t>((((numberToSend < 0.0) ? -numberToSend : numberToSend) - number) * pow(10, precision));
	uint8_t digitsSend = 1;

	if (!decimals)//if all decimal places are 0, then send number of 0 according to precision
	{
		for (uint8_t i = 1; (i < (precision + 1) && i < 9); i++, digitsSend++)//in each iteration check if we have space left for new digit
		{
			sendDigit(static_cast<MAX7219::digit>(i), 0);
		}
	}
	else
	{
		for (uint8_t i = 1; (i < (precision + 1) && i < 9); i++, digitsSend++)
		{
			sendDigit(static_cast<MAX7219::digit>(i), decimals % 10);
			decimals /= 10;
			if (!decimals) break;//if only 0 left then stop sending new digits
		}
	}

	if (precision + 1 > 8) return;//if we don't have space left for number, then all work is done here

	if (!number)//if number to send is 0
	{
		sendDigit(static_cast<MAX7219::digit>(precision + 1), 0, true);//send 0 with dot
	}
	else
	{
		sendDigit(static_cast<MAX7219::digit>(precision + 1), number % 10, true);//send one digit with dot
	}
	digitsSend++;//above we are sending one digit with dot, so count it
	number /= 10;//go to next digit

	for (uint8_t i = precision + 2; i < 9; i++, digitsSend++)
	{
		if (!number) break;
		sendDigit(static_cast<MAX7219::digit>(i), number % 10);
		number /= 10;
	}

	if (numberToSend < 0.0 && digitsSend < 8)//if we have a negative number, send minus sign
	{
		sendDigit(static_cast<MAX7219::digit>(digitsSend + 1), 0x0A);
	}
}

void MAX7219::clear(void)
{
	for (uint8_t i = 1; i < 9; i++)
	{
		sendDigit(static_cast<MAX7219::digit>(i), 0x0F);
	}
}

void MAX7219::fillWithZeros(void)
{
	for (uint8_t i = 1; i < 9; i++)
	{
		sendDigit(static_cast<MAX7219::digit>(i), 0);
	}
}

void MAX7219::setBcdDecodeMode(const uint8_t& decodeMode)
{
	m_digitCoding |= decodeMode;
	sendDataToRegister(static_cast<uint16_t>(0x0900 | m_digitCoding));//1 means BCD enabled on given digit
}

void MAX7219::setRawDecodeMode(const uint8_t& decodeMode)
{
	m_digitCoding &= ~decodeMode;
	sendDataToRegister(static_cast<uint16_t>(0x0900 | m_digitCoding));//0 means BCD disabled on given digit
}

uint8_t MAX7219::numToBitMask(const uint32_t& number)
{
	uint8_t mask = 0;
	uint32_t num = number;

	for (uint8_t i = 0; i < 8; i++)
	{
		if (!num) break;
		mask |= 1 << (num % 10);
		num /= 10;
	}

	return mask;
}

void MAX7219::sendDataToRegister(const uint16_t& dataToSend)
{
	digitalWrite(m_load, LOW);
	digitalWrite(m_clk, LOW);
	digitalWrite(m_dataOut, LOW);
	for (uint16_t i = 0x8000; i > 0; i >>= 1)
	{
		digitalWrite(m_clk, LOW);
		if (dataToSend & i) digitalWrite(m_dataOut, HIGH);
		else digitalWrite(m_dataOut, LOW);
		digitalWrite(m_clk, HIGH);
	}
	digitalWrite(m_clk, LOW);
	digitalWrite(m_load, HIGH);
	digitalWrite(m_dataOut, LOW);
	digitalWrite(m_load, LOW);
}
