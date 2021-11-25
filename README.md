# Land Rover CarPlay

This is a project to run an usable CarPlay in a Freelander 2 and possible Range Rover Evoque and Range Rover Discovery Sport since they share the same Bosch multimedia.

Here's the list of needed components:

1. [Faatech FT-VR-LR12](https://www.faaftech.com/produtos/interface-de-video/ft-video-free-lr12/) or a similar video interface to have an RCA input
2. Android Box of any kind
  1. [Aquário STV-200](https://aquario.com.br/produto/smart-tv-box-4k/)
	2. [Mi Box S](https://www.mi.com/br/mi-box-s/)
3. [Carlinkit USB Dongle](https://www.carlinkit.com/carplay-usb-dongle-for-android-head-unit.html)
4. Arduino Pro Micro, or similar, to emulate the touch screen

Raspberry PI and Radxa Zero with LineageOS are still pending of testing.

Linux devices are still pending of testing with [Go Carplay](https://github.com/mzyy94/gocarplay).

## Faaftech FT-VR-LR12

This interface allows you to connect any audio/video source (RCA) to your factory screen.

It also captures the touch screen which we can use to drive the Android avoiding the need of an external keyboard/rotary device.

## Aquário STV-200

This is the cheapest option and there are a lot of similar devices like the MXQ Pro 4k which are all the same under the hood.

If you don't care too much about performance, this is the way to go.

## Mi Box S

This is the best option in terms of performance.

Although, it runs Android TV which requires a bit more effort to run standard Android apps.

## Carlinkit USB Dongle

This is a dongle that uses the CarPlay in a way that Apple didn't predict at first, AFAIK.

It connects to your phone, wired or wirelessly, and provides a video output through USB connection to the host machine.

This is a perfect fit for Android devices since the dongle will do all the job and will output a video stream.

_You can also use a WinCE device but they aren't popular anymore._
