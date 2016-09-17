I've added a few things since my last update.

The first is the ability to "play" a text string as keystrokes on the receiver. So, the keyboards sends the string "this is a message" to the receiver which is then translated into keystrokes. These keystrokes are then sent to the PC as if the user typed "this is a message".

I used this to build a menu system which at the moment is used to set the brightness of the LEDs and the output power of the transmitter. Pressing Func+Esc enters the menu and this is what it currently looks:

[code]
7G wireless
firmware build Jul 30 2013  13:38:18
battery voltage=2.83V
keyboard's been on for 0days 21hour 39min 10sec


what do you want to do?
F1 - change transmitter output power (current 0dBm)
F2 - change LED brightness (current 1)
Esc - exit menu
[/code]
Pressing F1 or F2 then opens a sub-menu which allows changing these values.

The menu shows the current battery voltage. This needs a little tweaking to be more accurate, but it serves it's purpose at the moment. In the next PCB revision, I want to improve the circuit which measures the voltage. I have to dig into the AVR datasheets and look for some examples.

The firmware also has a clock (watch) which starts when the keyboard receives power. That's the "keyboard's been on for..." part of the menu. This clock is used in the sleep system (see below), but can be used as a real clock too.

There is also a LED "driver" thing which allows sequences to be replayed. So, for instance, I can play an LED sequence where 1 LED is on for a while, then 2, then all three. The driver also allows gradual transitions from bright do dim and back, but this still needs some testing, I think there are bugs in it. This can be used for all sorts of indicators.

The bad news: transmitter range sucks! On highest transmitter output power (0dBm) I get a range of 6-8 meters, and on the lowest I get 1-2 meters tops. I've used these modules for other projects, and I've confirmed a range of at least 60 meters. The chip should also be able to send a signal through at least one wall, which is not the case now. This, of course, means there is something wrong. I don't know what causes these, but I'm guessing either the keyboard PCB or my controller PCB are somehow shielding the transmitter. Tests are needed...

I don't do any frequency hopping. The channel is hard-coded at the moment. The transmitter will to re-send the package for for a number of times, or until it gets an acknowledge from the dongle.

The controller does not have any reverse voltage protection. So, reversing the batteries will probably fry the entire circuit. I will have to add this in the next controller revision.

The firmware doesn't do any encryption of the data it sends. This means that it's not too hard to build a circuit that can "listen" and log keystrokes. This can be fixed, of course, but it's not very high on my priority list. I'm not very paranoid...

The firmware puts the keyboard into sleep for a variable duration of time which depends on the time since the last change of the state of the keys. The idea is that while the user is typing, this sleep time will stay relatively short (say, 4ms) and if the user is not typing this interval will gradually increase to a max value (at the moment 71ms). Then, as soon as the user starts typing again, the sleep duration will jump back to the minimal value. Currently the implementaion of this is very crude, and needs more work. I plan to make this configurable so that the user can select if he wants to save power with longer sleeps, or have a more responsive keyboard and drain the batteries quicker. I also thought about making a "deep sleep" mode which puts the keyboard in a state where it will sleep for longer durations (say, 500ms) between matrix scans and wait for a specific key combination to get out of this deep sleep. The idea here is that you want to put the keyboard in deep sleep if you want to transport it, or if you want to lock it. The keyboard could also go into deep sleep if no keys have been pressed for a long time (30 minutes or so) to preserve power.

Because of this sleep system I decided I will not do any debouncing. What's the point of debouncing if I'm already scanning the keyboard matrix every 4ms or longer? Any comments on this?

The source code and the Eagle CAD schematics of the controller PCB are on:

https://code.google.com/p/7g-wireless/

So, that's it for now.