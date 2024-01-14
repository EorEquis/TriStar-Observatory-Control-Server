# TriStar Obs Control Server

An observatory "control server", that provides access to observatory devices and data via a RESTful API

---

## Purpose

The goal of TOCS is to reduce cabling and app/device complexity.  Ideally, every driver in the observatory could get all the data it needs, and completely control whatever device it is connected to via calls to this system's API.

This is, on a practical level, not achievable.  Some drivers are proprietary and cannot be modified.  Others require physical connections that cannot be changed.  But as much as possible, at least self-developed software and hardware will eventually make use of this system.

## Requirements

* An Arduino Mega is used, due to its expanded memory and GPIO pins.  This is a fairly bloated and busy little device.  Some day I might do more work to trim it down, but today is not that day.  Tomorrow probably isn't either.
* Some form of network connectivity.  The code here presumes an ethernet shield, but there's no reason WiFi wouldn't work if you so desired.
* A Real-Time Clock (RTC) module is necessary as things sit now.  While the date/time isn't currently used by any connected drivers/devices, this will change.  The API assumes an RTC is present, and will not start if one is not detected.
* Any other hardware/connectivity your use case might require.  Here a [Pololu Simple Motor Controller for brushed DC motors](https://www.pololu.com/category/94/pololu-simple-motor-controllers) is used to control a roll-off roof (ROR) motor.
* Some sort of project box to hold the arduino, the SMC, and various connectors, if desired.

## Use in your observatory

* Everything here is specific to my particular use cases, setup, and devices/data.  This code will *absolutely* not work for you out of the box.  It's here as a starting point to give ideas.

## Things failed!  My roof ran past a limit and fell into the swamp, my weather data is wrong, my flat panel doesn't work, the API is sending Satanic messages, or things are otherwise behaving in a non-norminal manner!

* Sucks to be you.  You downloaded someone else's code, free of charge, and trusted your observatory to it.  You get what you get.
* If you're not comfortable with placing such trust in a whole pile of shitty arduino and C# code, making your own tweaks, wiring up your own electronics, and DIY risks in general, this probably isn't the project for you.
* In short, I bear no responsibility for any outcome of your use of this code.  Use at your own risk!  

## I still can't figure out _____.

* I'm happy to try to help.  Just email me.  Please understand this isn't a job or a product or anything...I have a life and a job and all that jazz, so I'll get to it when I can.

