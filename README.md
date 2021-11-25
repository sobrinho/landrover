# Land Rover CarPlay

This is a project to run an usable CarPlay in a Land Rover Freelander 2 HSE 2013+ and possible Range Rover Evoque and Range Rover Discovery Sport since they share the same Bosch multimedia in some versions.

Here's the list of needed components:

1. [Faatech FT-VR-LR12](https://www.faaftech.com/produtos/interface-de-video/ft-video-free-lr12/) or a similar video interface to have an RCA input
2. Android Box of any kind
   1. [Aquário STV-200](https://aquario.com.br/produto/smart-tv-box-4k/)
   2. [Mi Box S](https://www.mi.com/br/mi-box-s/)
3. [Carlinkit USB Dongle](https://www.carlinkit.com/carplay-usb-dongle-for-android-head-unit.html)
4. Arduino Pro Micro, or similar, to emulate the touch screen

Raspberry PI and Radxa Zero with LineageOS are still pending of testing.

Other Linux devices, even Raspberry PI and Radxa Zero, are still pending of testing with [Go Carplay](https://github.com/mzyy94/gocarplay).

## Faaftech FT-VR-LR12

This interface allows you to connect any audio/video source (RCA) to your factory screen.

It also captures the touch screen which we can use to drive the Android avoiding the need of an external keyboard/rotary device.

## Android Box

Any Android Box will be capable of running Carlinkit Android App as long you can install their APK.

If you are using a standard Android Box, you just need to download and install the APK.

If you are using an Android TV Box, you need to download a file browser to install the APK, which is less convenient but not a big effort either.

### Aquário STV-200

This is the cheapest option and there are a lot of similar devices like the MXQ Pro 4k which are all the same under the hood.

If you don't care too much about performance, this is the way to go, install the APK and you are good to go.

Although, keep in mind:

1. Every time you plug your phone, a popup to allow the USB device will popup but the Carlinkit app will close it after a couple of seconds and will start the CarPlay (cosmetic issue)
2. You will need to connect the audio from your iPhone to the car through Bluetooth, which is not an issue per see but:
   1. Sound will go through from your iPhone to the car using Bluetooth, e.g.: Spotify, Apple Music, Siri, iMessage, WhatsApp, Telegram, etc
   2. Navigation apps like Apple Maps/Waze/Google Maps/Tom Tom won't be able to use the Bluetooth connection
   3. You can't use the AUX connection either because of the background audio noise from Faaftech
   4. You can workaround this by using an external speaker connected to the STV-200 just for navigation apps
3. While it has an AV output, the image seems to be better using an HDMI2AV converter (your mileage may vary)

Still pending to test custom ROMs like the [Aidan's ROM](https://www.aidansrom.ml) to see if it improves the performance.

### Mi Box S

This is the best option in terms of performance but the drawbacks are:

1. Mi Box S runs Android TV which requires a bit more effort to run standard Android apps
2. It has only one USB port, requiring an USB Hub to be able to connect the dongle and the touch screen device (Arduino) at the same time
   1. You can't use a cheap USB Hub since they aren't capable of providing enough power to the dongle
   2. I'm currently using an Orico (from Aliexpress) with success
3. Also, you will need an external HDMI2AV converter since it doesn't have an AV output

## Carlinkit USB Dongle

This is a dongle that uses the CarPlay in a way that Apple didn't predict at first, AFAIK.

It connects to your phone, wired or wirelessly, and provides a video output through USB connection to the host machine.

This is a perfect fit for Android devices since the dongle will do all the job and will output a video stream.

_You can also use a WinCE device but they aren't popular anymore._
