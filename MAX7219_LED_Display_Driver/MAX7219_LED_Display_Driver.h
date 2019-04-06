/*
 Name:		MAX7219_LED_Display_Driver.h
 Created:	27/03/2019 10:39:27 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 Editor:	http://www.visualmicro.com
 License:	GPL v2
*/

#ifndef _MAX7219_LED_Display_Driver_h
#define _MAX7219_LED_Display_Driver_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class MAX7219 final
{
public:
	/// <summary>
	/// Constructor for MAX7219 object. By default scan limit is setted to 8, intensity to max, display is cleared and then enabled.
	/// </summary>
	/// <param name="dOutPin">data out pin number</param>
	/// <param name="loadPin">load pin number</param>
	/// <param name="clkPin">clk pin number</param>
	/// <param name="scanLimit">scan limit</param>
	/// <param name="intensity">intensity value</param>
	MAX7219(const uint8_t& dOutPin, const uint8_t& loadPin, const uint8_t& clkPin, const uint8_t& scanLimit = 8, const uint8_t& intensity = 0xFF);

	MAX7219(const MAX7219&) = delete;
	MAX7219& operator=(const MAX7219&) = delete;

	/// <summary>
	/// Destructor for MAX7219 object. Clears display and sets shutdown state.
	/// </summary>
	~MAX7219() { clear(); setShutdown(true); }

public:
	/// <summary>
	/// Possible digits values.
	/// </summary>
	enum class digit : uint8_t { d0 = 0x01, d1, d2, d3, d4, d5, d6, d7 };

	/// <summary>
	/// Possible chars values.
	/// </summary>
	enum class chars : uint8_t { dash = 0x0A, e, h, l, p, blank };

	/// <summary>
	/// Sends one digits to display.
	/// </summary>
	/// <param name="dType">digit number from 1 to 8</param>
	/// <param name="valueToSend">value to send. From 0 to 15 this is interpreted as BCD, and above 15 as raw value</param>
	/// <param name="withDot">true if add a dot to digit, false otherwise</param>
	void sendDigit(const digit& dType, const uint8_t& valueToSend, const bool& withDot = false);

	/// <summary>
	/// Sets intensity.
	/// </summary>
	/// <param name="intensity">intensity value. From 0 to 255, it will be converted to 16 steps</param>
	void setIntensity(const uint8_t& intensity);

	/// <summary>
	/// Sets how many digits will be displayed. If you want to display only 3 digits or less, change Rset value to avoid excessive dissipation of power from digit drivers.
	/// </summary>
	/// <param name="scanLimit">value from 1 to 8</param>
	void setScanLimit(const uint8_t& scanLimit);

	/// <summary>
	/// Enables or disables shutdown state.
	/// </summary>
	/// <param name="shutdown">true to enable, false otherwise</param>
	void setShutdown(const bool& shutdown) { sendDataToRegister(static_cast<uint16_t>(0x0C00 | !shutdown)); }

	/// <summary>
	/// Enables or disables display test state (all segments lit).
	/// </summary>
	/// <param name="displayTest">true to enable, false to disable</param>
	void setDisplayTest(const bool& displayTest) { sendDataToRegister(static_cast<uint16_t>(0x0F00 | displayTest)); }

	/// <summary>
	/// Sends NOP command with optional data byte.
	/// </summary>
	/// <param name="nopData">optional data byte</param>
	void sendNop(const uint8_t& nopData = 0) { sendDataToRegister(static_cast<uint16_t>(nopData)); }//value should be 0 but command will be NOP so value can be anything if needed

	/// <summary>
	/// Displays integer number. Minus sign is added if number is negative.
	/// </summary>
	/// <param name="numberToSend">number to display</param>
	void sendNumber(const int32_t& numberToSend);

	/// <summary>
	/// Displays double number with given precision. If all decimal places are 0, then it will add amout of zeros according to precision.
	/// </summary>
	/// <param name="numberToSend">number to display</param>
	/// <param name="precision">number of decimal places</param>
	void sendNumber(const double& numberToSend, const uint8_t& precision);

	/// <summary>
	/// Displays char on given digit place.
	/// </summary>
	/// <param name="dType">place where char will be displayed</param>
	/// <param name="charToSend">char to display</param>
	/// <param name="addDot">true to add dot to displayed char, false otherwise (default)</param>
	void sendChar(const digit& dType, const chars& charToSend, const bool& addDot = false) { sendDigit(dType, static_cast<uint8_t>(charToSend), addDot); }

	/// <summary>
	/// Clears display by setting blanks on all digits.
	/// </summary>
	void clear(void);

	/// <summary>
	/// Fills all display with zeros.
	/// </summary>
	void fillWithZeros(void);

private:
	/// <summary>
	/// Sets BCD decoding mode on given digits.
	/// </summary>
	/// <param name="decodeMode">bitmask to set. One means BCD decoding mode for given digit</param>
	void setBcdDecodeMode(const uint8_t& decodeMode);

	/// <summary>
	/// Sets raw decoding mode on given digits.
	/// </summary>
	/// <param name="decodeMode">bitmask to set. Zero means raw decoding mode for given digit</param>
	void setRawDecodeMode(const uint8_t& decodeMode);

	/// <summary>
	/// Checks if BCD decoding mode is set for given digits.
	/// </summary>
	/// <param name="digitToCheck">digits to check, ex. 527 will check 5, 2 and 7. Order is not important</param>
	/// <returns>true if BCD decoding mode is enabled for given digits, false otherwise</returns>
	bool isBcdSet(const digit& digitToCheck) { return m_digitCoding & numToBitMask(static_cast<uint8_t>(digitToCheck)); }

	/// <summary>
	/// Creates bitmask for given digits.
	/// </summary>
	/// <param name="number">positions setted as one, ex 428 will set fourth, second and eighth bit to one. Order is not important</param>
	/// <returns>created bitmask for given positions</returns>
	uint8_t numToBitMask(const uint32_t& number);

	/// <summary>
	/// Sends data to register.
	/// </summary>
	/// <param name="dataToSend">data to send</param>
	void sendDataToRegister(const uint16_t& dataToSend);

private:
	/// <summary>
	/// Data out pin.
	/// </summary>
	const uint8_t m_dataOut;

	/// <summary>
	/// Load pin.
	/// </summary>
	const uint8_t m_load;

	/// <summary>
	/// Clk pin.
	/// </summary>
	const uint8_t m_clk;

	/// <summary>
	/// Coding of digits.
	/// </summary>
	uint8_t m_digitCoding = 0xFF;//all digits in BCD
};

#endif
