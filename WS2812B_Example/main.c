#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>

#include "WS2812B_Driver\ws2812b.h"

#define SAMPLES 5
int currentSample = 0;
GPIO_Value_Type buttonState;

static int CheckButtonState(int gpioFd)
{

	// Check for a button press
	GPIO_Value_Type newButtonState;
	int result = GPIO_GetValue(gpioFd, &newButtonState);
	if (result != 0) {
		Log_Debug("ERROR: Could not read button GPIO: %s (%d).\n", strerror(errno), errno);
		return;
	}

	// If the button has just been pressed, change the LED blink interval
	// The button has GPIO_Value_Low when pressed and GPIO_Value_High when released
	if (newButtonState != buttonState) {
		if (newButtonState == GPIO_Value_Low) {
			currentSample = (currentSample + 1) % SAMPLES;
			Log_Debug("Sample %d Active", currentSample);
			return 1;
		}
		buttonState = newButtonState;
	}
	return 0;
}

int main(void)
{
    // This minimal Azure Sphere app repeatedly toggles GPIO 9, which is the green channel of RGB
    // LED 1 on the MT3620 RDB.
    // If your device exposes different GPIOs, you might need to change this value. For example,
    // to run the app on a Seeed mini-dev kit, change the GPIO from 9 to 7 in the call to
    // GPIO_OpenAsOutput and in the app_manifest.json to blink its LED. Check with your hardware
    // manufacturer to determine which GPIOs are available.
    // Use this app to test that device and SDK installation succeeded that you can build,
    // deploy, and debug an app with Visual Studio, and that you can deploy an app over the air,
    // per the instructions here: https://docs.microsoft.com/azure-sphere/quickstarts/qs-overview
    //
    // It is NOT recommended to use this as a starting point for developing apps; instead use
    // the extensible samples here: https://github.com/Azure/azure-sphere-samples
    Log_Debug(
        "\nVisit https://github.com/Azure/azure-sphere-samples for extensible samples to use as a "
        "starting point for full applications.\n");

    // Change the GPIO number here and in app_manifest.json if required by your hardware.
    int fd = GPIO_OpenAsOutput(9, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (fd < 0) {
        Log_Debug(
            "Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
            strerror(errno), errno);
        return -1;
    }

	int btnAFd = GPIO_OpenAsInput(12);


	WS_PixelStrip_Init(16, 1);

	const struct timespec sleepTime = { 0, 500000000 };
	const struct timespec sleepTime1 = { 0, 100000000 };
	while (true) {

		if (CheckButtonState(btnAFd))
		{
			WS_PiixelStrip_SetColor(-1, 0, 0, 0);
			WS_PixelStrip_Show();
		}

		switch (currentSample)
		{
		case 1:
			WS_PiixelStrip_SetColor(-1, 0, 0, 255);
			WS_PixelStrip_Show();
			nanosleep(&sleepTime1, NULL);
			WS_PiixelStrip_SetColor(-1, 0, 255, 0);
			WS_PixelStrip_Show();
			nanosleep(&sleepTime1, NULL);
			WS_PiixelStrip_SetColor(-1, 255, 0, 0);
			WS_PixelStrip_Show();
			nanosleep(&sleepTime1, NULL);
			break;

		case 2:
		{
			uint8_t green = 255;
			uint8_t red = 0;
			uint8_t blue = 0;
			uint8_t delta = 1.0 / pixelCount * 255;
			for (int i = 0; i < pixelCount; i++)
			{
				WS_PiixelStrip_SetColor(i, red, green, blue);
				red += delta;
				green -= delta;
				WS_PixelStrip_Show();
				nanosleep(&sleepTime1, NULL);
			}
			for (int i = 0; i < pixelCount; i++)
			{
				WS_PiixelStrip_SetColor(i, red, green, blue);
				red -= delta;
				blue += delta;
				WS_PixelStrip_Show();
				nanosleep(&sleepTime1, NULL);
			}
			for (int i = 0; i < pixelCount; i++)
			{
				WS_PiixelStrip_SetColor(i, red, green, blue);
				blue -= delta;
				green += delta;
				WS_PixelStrip_Show();
				nanosleep(&sleepTime1, NULL);
			}
		}
			break;
		case 3:
			WS_PiixelStrip_SetColor(-1, 255, 255, 255);
			WS_PixelStrip_Show();
			nanosleep(&sleepTime, NULL);
			break;
		default:
			GPIO_SetValue(fd, GPIO_Value_Low);
			nanosleep(&sleepTime, NULL);
			GPIO_SetValue(fd, GPIO_Value_High);
			nanosleep(&sleepTime, NULL);
			break;
		}

    }
}
