/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __BLE_LIGHT_SERVICE_H__
#define __BLE_LIGHT_SERVICE_H__

class LightService {
public:
    const static uint16_t SERVICE_UUID        = 0x7997;
    const static uint16_t COLOR_CHAR_UUID     = 0xA432;

    LightService(BLEDevice &_ble, uint32_t initialValue) :
        ble(_ble), colorChar(COLOR_CHAR_UUID, &initialValue)
    {
        GattCharacteristic *charTable[] = { &colorChar };
        GattService         ledService(SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.addService(ledService);
    }

    GattAttribute::Handle_t getColorCharHandle() const
    {
        return colorChar.getValueHandle();
    }

private:
    BLEDevice                             &ble;
    ReadWriteGattCharacteristic<uint32_t> colorChar;
};

#endif /* #ifndef __BLE_LIGHT_SERVICE_H__ */
