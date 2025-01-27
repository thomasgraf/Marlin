///////////////////////////////////////////////////////////////////////////////
/// \file StorageManager.cpp
///
/// \author Ivan Galvez Junquera
///         Ruy Garcia
///         Victor Andueza
///         Joaquin Herrero
///
/// \brief Management class for the EEPROM memory.
///
/// Copyright (c) 2015 BQ - Mundo Reader S.L.
/// http://www.bq.com
///
/// This file is free software; you can redistribute it and/or modify
/// it under the terms of either the GNU General Public License version 2 or
/// later or the GNU Lesser General Public License version 2.1 or later, both
/// as published by the Free Software Foundation.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////

#include "StorageManager.h"

#include <avr/eeprom.h>
#include "Configuration.h"

#ifdef DOGLCD
	#include "Language.h"
#endif //DOGLCD

namespace eeprom
{
	static uint8_t * const ADDR_ZOFFSET_ZPROBE = (uint8_t *) 232;
	static uint8_t * const ADDR_EMERGENCY_STOP = (uint8_t *) 499;
	static uint8_t * const ADDR_FIRST_POWER_ON = (uint8_t *) 500;
	static uint8_t * const ADDR_AUTOLEVEL      = (uint8_t *) 501;
	static uint8_t * const ADDR_LIGHT          = (uint8_t *) 502;
	static uint8_t * const ADDR_SERIAL         = (uint8_t *) 503;
	static uint8_t * const ADDR_LANGUAGE       = (uint8_t *) 504;
	static uint8_t * const ADDR_BOX_FAN        = (uint8_t *) 505;
	static uint8_t * const ADDR_PROTECTED_ZONE = (uint8_t *) 4071;
	static uint8_t * const ADDR_EEPROM_VERSION = (uint8_t *) 4072;
	static uint8_t * const ADDR_EEPROM_FLAG    = (uint8_t *) 4073;
	static uint8_t * const ADDR_BOARD_FAMILY   = (uint8_t *) 4081;

	StorageManager::StorageManager()
	{ }

	void StorageManager::setEmergency()
	{
		StorageManager::single::instance().writeByte(ADDR_EMERGENCY_STOP, EMERGENCY_STOP_ACTIVE);
	}

	void StorageManager::clearEmergency()
	{
		StorageManager::single::instance().writeByte(ADDR_EMERGENCY_STOP, EMERGENCY_STOP_INACTIVE);
	}

	const uint8_t StorageManager::getEmergency()
	{
		if(StorageManager::single::instance().readByte(ADDR_EMERGENCY_STOP) == EMERGENCY_STOP_ACTIVE)
		{
			return EMERGENCY_STOP_ACTIVE;
		}
		return EMERGENCY_STOP_INACTIVE;
	}

	void StorageManager::setOffset(float value)
	{
		float offset = value;
		StorageManager::single::instance().writeData(ADDR_ZOFFSET_ZPROBE, (uint8_t*)&offset, sizeof(offset));
	}

	float StorageManager::getOffset()
	{
		float offset = 0;
		StorageManager::single::instance().readData(ADDR_ZOFFSET_ZPROBE, (uint8_t*)&offset, sizeof(offset));

		if(offset >= 0.0f && offset <= 10.0f)
		{
			return offset;
		}
		return  Z_PROBE_OFFSET_FROM_EXTRUDER;
	}

	void StorageManager::setInitialized()
	{
		StorageManager::single::instance().writeByte(ADDR_FIRST_POWER_ON, INITIALIZED);
	}

	void StorageManager::setUninitialized()
	{
		StorageManager::single::instance().writeByte(ADDR_FIRST_POWER_ON, UNINITIALIZED);
	}

	bool StorageManager::getInitialized()
	{
		if(StorageManager::single::instance().readByte(ADDR_FIRST_POWER_ON) == INITIALIZED)
		{
			return true;
		}
		return false;
	}

#ifdef DOGLCD

	void StorageManager::setLanguage(uint8_t language)
	{
		StorageManager::single::instance().writeByte(ADDR_LANGUAGE, language);
	}

	const uint8_t StorageManager::getLanguage()
	{
		uint8_t language = StorageManager::single::instance().readByte(ADDR_LANGUAGE);

		if(language < static_cast<uint8_t>(Language::MAX_LANGUAGES))
		{
			return language;
		}
		return static_cast<uint8_t>(Language::EN);
	}

#endif // DOGLCD

	void StorageManager::setLight(uint8_t state)
	{
		StorageManager::single::instance().writeByte(ADDR_LIGHT, state);
	}

	uint8_t StorageManager::getLight()
	{
		if(StorageManager::single::instance().readByte(ADDR_LIGHT) == LIGHT_OFF)
		{
			return LIGHT_OFF;
		}
		else if(StorageManager::single::instance().readByte(ADDR_LIGHT) == LIGHT_ON)
		{
			return LIGHT_ON;
		}
		return LIGHT_AUTO;
	}

	void StorageManager::setBoxFan(bool state)
	{
		StorageManager::single::instance().writeByte(ADDR_BOX_FAN, state);
	}

	bool StorageManager::getBoxFan()
	{
		if(StorageManager::single::instance().readByte(ADDR_BOX_FAN) == BOX_FAN_DISABLED)
		{
			return false;
		}
		return true;
	}

	void StorageManager::setAutoLevel(bool state)
	{
		StorageManager::single::instance().writeByte(ADDR_AUTOLEVEL, state);
	}

	bool StorageManager::getAutoLevel()
	{
		if(StorageManager::single::instance().readByte(ADDR_AUTOLEVEL) == AUTOLEVEL_ON)
		{
			return true;
		}
		return false;
	}

	void StorageManager::setSerialScreen(bool state)
	{
		StorageManager::single::instance().writeByte(ADDR_SERIAL, state);
	}

	bool StorageManager::getSerialScreen()
	{
		if(StorageManager::single::instance().readByte(ADDR_SERIAL) == SERIAL_SCREEN_ON)
		{
			return true;
		}
		return false;
	}

	void StorageManager::eraseEEPROM()
	{
		uint8_t * address = (uint8_t *) 0;

		StorageManager::single::instance().setEEPROMState(EEPROM_DISABLED);

		while (address <= ADDR_PROTECTED_ZONE)
		{
			StorageManager::single::instance().writeByte(address, 0xFF);
			address++;
		}

		StorageManager::single::instance().updateEEPROMVersion();
		StorageManager::single::instance().setEEPROMState(EEPROM_ENABLED);
	}

	const uint8_t StorageManager::getEEPROMVersion()
	{
		return StorageManager::single::instance().readByte(ADDR_EEPROM_VERSION);
	}

	const uint8_t StorageManager::checkEEPROMState()
	{
		if(StorageManager::single::instance().readByte(ADDR_EEPROM_FLAG) != EEPROM_ENABLED)
		{
			return EEPROM_DISABLED;
		}
		return EEPROM_ENABLED;
	}

	void StorageManager::updateEEPROMVersion()
	{
#ifdef BQ_EEPROM_VERSION
		StorageManager::single::instance().writeByte(ADDR_EEPROM_VERSION, BQ_EEPROM_VERSION);
#endif
	}

	void StorageManager::setEEPROMState(uint8_t state)
	{
		StorageManager::single::instance().writeByte(ADDR_EEPROM_FLAG, state);
	}

	const uint8_t StorageManager::getBoardType()
	{
		char board_type[4];
		StorageManager::single::instance().readData(ADDR_BOARD_FAMILY, (uint8_t*)&board_type, sizeof(board_type)/sizeof(board_type[0]));
		board_type[3] = '\0';

		if(strcmp(board_type, "ZM3") == 0)
		{
			return BOARD_BQ_ZUM_MEGA_3D;
		}
		else if(strcmp(board_type, "CNC") == 0)
		{
			return BOARD_BQ_CNC;
		}
		return BOARD_RAMPS_13_EFB;
	}

	uint8_t StorageManager::readByte(uint8_t * address)
	{
		while ( !eeprom_is_ready() ) {}
		return eeprom_read_byte(address);
	}

	void StorageManager::writeByte(uint8_t * address, uint8_t data)
	{
		while ( !eeprom_is_ready() ) {}
		eeprom_write_byte(address, data);
	}

	void StorageManager::readData(uint8_t * address, uint8_t * value, uint8_t size)
	{
		while(size > 0)
		{
			*value = readByte(address);
			address++;
			value++;
			size--;
		}
	}

	void StorageManager::writeData(uint8_t * address, uint8_t * value, uint8_t size)
	{
		while(size > 0)
		{
			writeByte(address, *value);
			address++;
			value++;
			size--;
		}
	}
}
