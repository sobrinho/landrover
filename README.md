# Land Rover CarPlay

This is a project to run an usable CarPlay in a Land Rover Freelander 2 HSE 2013+ and possible Range Rover Evoque and Range Rover Discovery Sport since they share the same Bosch multimedia in some versions.

Here's the list of needed components:

1. [Faatech FT-VR-LR12](https://www.faaftech.com/produtos/interface-de-video/ft-video-free-lr12/) or a similar video interface to have an RCA input
2. DC-DC Buck Converter (Step-Down) from ~12V to 5V
   1. HW-638
   2. XL4005
   3. CPT-UL-1
   4. Or any converter capable of supplying from 3A to 5A
3. Android Box of any kind
   1. [Aquário STV-200](https://aquario.com.br/produto/smart-tv-box-4k/)
   2. [Mi Box S](https://www.mi.com/br/mi-box-s/)
   3. Raspberry PI devices pending of testing
   4. ROCK Pi/Radxa Zero pending of testing
   5. Tinker Board pending of testing
   6. Generic Linux devices are still pending of testing with [Go Carplay](https://github.com/mzyy94/gocarplay)
4. [Carlinkit USB Dongle](https://www.carlinkit.com/carplay-usb-dongle-for-android-head-unit.html)
5. Arduino Pro Micro, or similar, to emulate the touch screen
6. PSU in the case of Raspberry PI or Radxa Zero

## Faaftech FT-VR-LR12

[This interface](https://www.faaftech.com/produtos/interface-de-video/ft-video-free-lr12/) allows you to connect any audio/video source (RCA) to your factory screen.

It also translates the touch screen to serial which we can use to drive the Arduino avoiding the need of an external keyboard/rotary device.

## DC-DC Buck Converter (Step-Down)

We need to convert the car power to the device we are going to use.

While [Tinker Board 2](https://tinker-board.asus.com/product/tinker-board-2s.html) accepts ~12V, other devices usually requires 5V.

You can do this by using the cigar power outlet or the Faaftech Ignition Output connected to a Buck Converter.

While the first one requires you to solder a wire or tap the fuse, the second one blows the fuse around 2A.

## Android Box

Any Android Box will be capable of running Carlinkit Android App as long you can install their APK.

If you are using a standard Android Box, you just need to download and install the APK.

If you are using an Android TV Box, you need to download a file browser first, which is less convenient but not a big deal either.

### Aquário STV-200

This is the [cheapest option](https://aquario.com.br/produto/smart-tv-box-4k/) out there and there are a lot of similar devices like the MXQ Pro 4k which are all the same under the hood.

If you don't care too much about performance, this is the way to go, install the APK and you are set.

Although, keep in mind:

1. Every time you plug your phone, a popup to allow the USB device will pop-up but the Carlinkit app will close it after a couple of seconds and will start the CarPlay regardless (cosmetic issue)
2. You will need to connect the audio from your iPhone to the car through Bluetooth, which is not an issue per see but:
   1. Sound will go through from your iPhone to the car using Bluetooth, e.g.: Spotify, Apple Music, Siri, iMessage, WhatsApp, Telegram, etc
   2. Navigation apps like Apple Maps/Waze/Google Maps/Tom Tom won't be able to use the Bluetooth connection
   3. You can't use the AUX connection either because of the background audio noise from Faaftech
   4. You can workaround this by using an external speaker connected to the STV-200 just for navigation apps
3. While it has an AV output, the image seems to be better using an HDMI2AV converter (your mileage may vary)

Still pending to test custom ROMs like the [Aidan's ROM](https://www.aidansrom.ml) to see if it improves the performance.

### Mi Box S

This is the best option in terms of performance but the drawbacks are:

1. [Mi Box S](https://www.mi.com/br/mi-box-s/) runs Android TV which requires a bit more effort to run standard Android apps (at lest to install a File Browser)
2. It has only one USB port, requiring an USB Hub to be able to connect the dongle and the touch screen device (Arduino) at the same time
   1. You can't use a cheap USB Hub since they aren't capable of providing enough power to the dongle
   2. I'm currently using an Orico (from Aliexpress) with success
3. Also, you will need an external HDMI2AV converter since it doesn't have an AV output

## Carlinkit USB Dongle

This is a dongle that uses the CarPlay in a way that Apple didn't predict at first, AFAIK.

It connects to your phone, wired or wirelessly, and provides a video output through USB connection to the host machine.

This is a perfect fit for Android devices since the dongle will do all the job and will output a video stream.

_You can also use a WinCE device but they aren't popular anymore._

## Arduino Pro Micro (Touch Screen)

To send the touch screen data to the Android device, we need to parse the Faaftech touch data and emulate a HID device.

We can do this using an Arduino connected to the Faaftech serial output and the Android's USB with an Arduino Pro Micro.

You need to connect the Faaftech Touch Data wire to the RX1 on the Arduino board and use the [arduino-touch.ino](/arduino-touch.ino) software.

If you picked the Mi Box S, another option is to use an ESP8266 with the [esp8266-touch.ino](/esp8266-touch.ino) to use Bluetooth and avoid the USB Hub Device.

_If you choose the ESP8266, you need to solder a couple of wires to supply power to the ESP8266._

## PSU

If you are using an Android/Linux Device that can corrupt with forced shutdown, you need a PSU (Power Supply Unit) to trigger a safe shutdown when you turn off the ignition.

Not tested yet but I prototyped a [PSU sofware](/psu.ino) to run in an ATtiny85 to trigger the safe shutdown, forced shutdown and recover from failures.

This is yet to be tested but so far:

1. Connect the IGNITION signal to the PIN 0 (using an optocoupler)
2. Connect the Buck Converter to a Relay connected to the PIN 1 (using an optocoupler)
3. Connect the Rapsberry/Radxa Zero safe shutdown pin to the ATtiny85 PIN 2
4. Connect the 3.3v pin back to the ATtiny85 PIN 3
5. Connect an LED to the PIN 4 with an appropriated resistor

This software isn't tested by myself yet, it's just a prototype, keep this on mind before actually running it.

You will need a software in your device to read the power pin (ATtiny85 PIN 2) and trigger a safe shutdown.

After you shutdown the ignition, the PSU will wait until SHUT_DOWN_DELAY before triggering the safe shutdown.

If the device doesn't shutdown in SHUT_DOWN_TIMEOUT, the energy will be cut-off (to recover from shutdown failures and drain the car battery).

If the device doesn't boot in BOOT_TIMEOUT, the energy wil be cut-off and triggered again (to recover from boot failures).

_PS: Schematics to be designed, software to be reviewed and documentation also to be reviewed._
