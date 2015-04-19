/**
 * Arduino Wrapper for BTstack
 */
#pragma once

#include "att.h"
#include <btstack/utils.h> 
#include "gatt_client.h"
#include "hci.h"
#include <stdint.h>

typedef enum BLEStatus {
	BLE_STATUS_OK,
	BLE_STATUS_DONE,	// e.g. for service or characteristic discovery done
	BLE_STATUS_CONNECTION_TIMEOUT,
	BLE_STATUS_CONNECTION_ERROR,
	BLE_STATUS_OTHER_ERROR
} BLEStatus;

typedef void (*btstack_packet_handler_t) (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

class UUID {
private:
	uint8_t uuid[16];
public:
	UUID();
	UUID(const uint8_t uuid[16]);
	UUID(const char * uuidStr);
	const char * getUuidString()    const;
	const char * getUuid128String() const;
	const uint8_t * getUuid(void)   const;
	bool matches(UUID *uuid)        const;
};

typedef enum BD_ADDR_TYPE {
	PUBLIC_ADDRESS = 0,
	PRIVAT_ADDRESS
} BD_ADDR_TYPE;

class BD_ADDR {
private:
	uint8_t address[6];
	BD_ADDR_TYPE address_type;
public:
	BD_ADDR();
	BD_ADDR(const char * address_string, BD_ADDR_TYPE address_type = PUBLIC_ADDRESS);
	BD_ADDR(const uint8_t address[6], BD_ADDR_TYPE address_type = PUBLIC_ADDRESS);
	const uint8_t * getAddress();
	const char * getAddressString();
	BD_ADDR_TYPE getAddressType();
};

class BLEAdvertisement {
private:
	uint8_t advertising_event_type;
	uint8_t rssi;
	uint8_t data_length;
	uint8_t data[10 + LE_ADVERTISING_DATA_SIZE];
	BD_ADDR  bd_addr;
public:
	BLEAdvertisement(uint8_t * event_packet);
	BD_ADDR * getBdAddr();
	BD_ADDR_TYPE getBdAddrType();
	int getRssi();
	bool containsService(UUID * service);
	bool nameHasPrefix(const char * namePrefix);
	const uint8_t * getAdvData();  
};

class BLECharacteristic {
private:
	le_characteristic_t characteristic;
	UUID uuid;
public:
	BLECharacteristic();
	BLECharacteristic(le_characteristic_t characteristic);
	const UUID * getUUID();
	bool matches(UUID * uuid);
	bool isValueHandle(uint16_t value_handle);
	const le_characteristic_t * getCharacteristic();
};

class BLEService {
private:
	le_service_t service;
	UUID uuid;
public:
	BLEService();
	BLEService(le_service_t service);
	const UUID * getUUID();
	bool matches(UUID * uuid);
	const le_service_t * getService();
};

class BLEDevice {
private:
	uint16_t handle;
public:
	BLEDevice();
	BLEDevice(uint16_t handle);
	uint16_t getHandle();

	// discovery of services and characteristics
	int discoverGATTServices();
	int discoverCharacteristicsForService(BLEService * service);

	// read/write
	int  readCharacteristic(BLECharacteristic * characteristic);
	int  writeCharacteristic(BLECharacteristic * characteristic, uint8_t * data, uint16_t size);
	int  writeCharacteristicWithoutResponse(BLECharacteristic * characteristic, uint8_t * data, uint16_t size);

	// subscribe/unsubscribe
	int subscribeForNotifications(BLECharacteristic * characteristic);
	int unsubscribeFromNotifications(BLECharacteristic * characteristic);	
};

class BTstackManager {
public:
	BTstackManager(void);
	void setup(void);
	void loop(void);

// @deprecated
	void registerPacketHandler(btstack_packet_handler_t packet_handler);
// @deprecated
	void setClientMode();

	void setAdvData(uint16_t size, const uint8_t * data);
	void setPublicBdAddr(bd_addr_t addr);

	void bleStartScanning();
	void bleStopScanning();

	// connection management
	void bleConnect(BD_ADDR_TYPE address_type, const uint8_t address[6], int timeout_ms);
	void bleConnect(BD_ADDR_TYPE address_type, const char * address, int timeout_ms);
	void bleConnect(BD_ADDR * address, int timeout_ms);
	void bleConnect(BLEAdvertisement * advertisement, int timeout_ms);
	void bleDisconnect(BLEDevice * device);

	// discovery of services and characteristics
	int discoverGATTServices(BLEDevice * device);
	int discoverCharacteristicsForService(BLEDevice * peripheral, BLEService * service);

	// read/write
	int  readCharacteristic(BLEDevice * device, BLECharacteristic * characteristic);
	int  writeCharacteristic(BLEDevice * device, BLECharacteristic * characteristic, uint8_t * data, uint16_t size);
	int  writeCharacteristicWithoutResponse(BLEDevice * device, BLECharacteristic * characteristic, uint8_t * data, uint16_t size);

	// subscribe/unsubscribe
	int subscribeForNotifications(BLEDevice * device, BLECharacteristic * characteristic);
	int unsubscribeFromNotifications(BLEDevice * device, BLECharacteristic * characteristic);

	// Callbacks
	void setBLEAdvertisementCallback(void (*)(BLEAdvertisement * bleAdvertisement));
	void setBLEDeviceConnectedCallback(void (*)(BLEStatus status, BLEDevice * device));
	void setBLEDeviceDisconnectedCallback(void (*)(BLEDevice * device));
	void setGATTServiceDiscoveredCallback(void (*)(BLEStatus status, BLEDevice * device, BLEService * bleService));
	void setGATTCharacteristicDiscoveredCallback(void (*)(BLEStatus status, BLEDevice * device, BLECharacteristic * characteristic));
	void setGATTCharacteristicReadCallback(void (*)(BLEStatus status, BLEDevice * device, uint8_t * value, uint16_t length));
	void setGATTCharacteristicNotificationCallback(void (*)(BLEDevice * device, uint16_t value_handle, uint8_t* value, uint16_t length));
	void setGATTDoneCallback(void (*)(BLEStatus status, BLEDevice * device));

	void setGATTCharacteristicWrittenCallback(void (*)(BLEStatus status, BLEDevice * device));
	void setGATTCharacteristicSubscribedCallback(void (*)(BLEStatus status, BLEDevice * device));
	void setGATTCharacteristicUnsubscribedCallback(void (*)(BLEStatus status, BLEDevice * device));
};

 extern BTstackManager BTstack;