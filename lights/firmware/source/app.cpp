#include "mbed-drivers/mbed.h"
#include "minar/minar.h"
#include "core-util/FunctionPointer.h"
#include "ble/BLE.h"
#include "eddystone/EddystoneService.h"
#include "LightService.h"
#include "ChainableLED/ChainableLED.h"         // Driver for the LED

using namespace mbed::util;

static DigitalOut statusLed(LED1);
static ChainableLED rgbLed(D6, D7, 1);   // Declare the LED (it's chainable!)

static LightService* lightServicePtr;

// https://preview.c9users.io/janjongboom1/codefest-ru/lights/web/index.html
static const char defaultUrl[] = "https://goo.gl/uCJ9d0";
static char beaconName[] = "Light System";
static uint16_t uuid16_list[] = { LightService::SERVICE_UUID };

static const PowerLevels_t defaultAdvPowerLevels = {-47, -33, -21, -13};
static const PowerLevels_t radioPowerLevels      = {-30, -16, -4, 4};

static void blinky(void) {
    statusLed = !statusLed;
}

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    // hmm.. is there UNUSED() in mbed?

    BLE::Instance().gap().startAdvertising(); // restart advertising
}

void onDataWrittenCallback(const GattWriteCallbackParams *params) {
    printf("Write came in\r\n");

    if ((params->handle == lightServicePtr->getColorCharHandle())) {
        uint32_t colors;
        memcpy(&colors, params->data, 4);

        uint8_t red = (colors >> 0) & 0xff;
        uint8_t green = (colors >> 8) & 0xff;
        uint8_t blue = (colors >> 16) & 0xff;

        rgbLed.setColorRGB(0, red, green, blue);

        printf("Wrote to lightServicePtr! %li\r\n", colors);
    }
}

void onBleInitError(BLE &ble, ble_error_t error)
{
    (void)ble;
    (void)error;
   /* Initialization error handling should go here */
}

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    // blinky when BLE init is completed
    minar::Scheduler::postCallback(blinky).period(minar::milliseconds(500));

    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        onBleInitError(ble, error);
        return;
    }

    if (ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }

    ble.gap().onDisconnection(disconnectionCallback);
    ble.gattServer().onDataWritten(onDataWrittenCallback);

    lightServicePtr = new LightService(ble, 0);

    auto eddyServicePtr = new EddystoneService(ble, defaultAdvPowerLevels, radioPowerLevels, 0);
    eddyServicePtr->setURLData(defaultUrl);
    eddyServicePtr->setNormalFrameData(beaconName, strlen(beaconName), uuid16_list, sizeof(uuid16_list));

    eddyServicePtr->setUIDFrameAdvertisingInterval(0);
    eddyServicePtr->setTLMFrameAdvertisingInterval(0);
    eddyServicePtr->setURLFrameAdvertisingInterval(500);
    eddyServicePtr->setNormalFrameAdvertisingInterval(500);

    eddyServicePtr->startBeaconService();
}

void app_start(int, char**) {
    rgbLed.setColorRGB(0, 0, 0xff, 0);

    // init the BLE stack
    BLE::Instance().init(bleInitComplete);
}
