#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <stdint.h>

#include "Singleton.h"

namespace eeprom
{
	const uint8_t EMERGENCY_STOP_ACTIVE = 0x00;
	const uint8_t EMERGENCY_STOP_INACTIVE = 0xFF;
	const uint8_t INITIALIZED = 0x00;
	const uint8_t UNINITIALIZED = 0xFF;
	const uint8_t LIGHT_OFF = 0x00;
	const uint8_t LIGHT_ON = 0x01;
	const uint8_t LIGHT_AUTO = 0x02;
	const uint8_t AUTOLEVEL_ON = 0x01;
	const uint8_t SERIAL_SCREEN_ON = 0x01;
	const uint8_t EEPROM_ENABLED = 0x00;
	const uint8_t EEPROM_DISABLED = 0x01;

	class StorageManager
	{
		public:
			typedef Singleton<StorageManager> single;

		public:
			StorageManager();

			static void setEmergency();
			static void clearEmergency();
			static const uint8_t getEmergency();

			static void setOffset(float value);
			static float getOffset();

			static void setInitialized();
			static void setUninitialized();
			static bool getInitialized();

			static void setLanguage(uint8_t language);
			static const uint8_t getLanguage();

			static void setLight(uint8_t state);
			static uint8_t getLight();

			static void setAutoLevel(bool state);
			static bool getAutoLevel();

			static void setSerialScreen(bool state);
			static bool getSerialScreen();

			static void eraseEEPROM();
			static const uint8_t getEEPROMVersion();
			static const uint8_t checkEEPROMState();
			
			static const uint8_t getBoardType();

		private:
			void updateEEPROMVersion();
			void setEEPROMState(uint8_t state);

			uint8_t readByte(uint8_t * address);
			void writeByte(uint8_t * address, uint8_t data);

			void readData(uint8_t * address, uint8_t * value, uint8_t size);
			void writeData(uint8_t * address, uint8_t * value, uint8_t size);
	};
}

#endif // STORAGE_MANAGER_H