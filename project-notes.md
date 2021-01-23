# Hardware

## An overview

![](/assets/images/robot-schematic.png)

This robot has a fairly trivial design, but it is the most complex IOT-type circuit I've ever attempted, and involved a lot of modules that were new to me. To summarize -- I wanted a robot with:

* High-level programmability
    * Embedded microcontrollers are all well and good,but multithreaded systems with full OSes give the most options for building complex software with potential for intelligent behavior.
* Extensible control system (sensors and etc can be added)
    * This is provded by having a generic atmel chip board (arduino nano clone), and having spare pins that could later be connected to other sensors & devices. It is worth noting that this approach allows for wasting quite a bit of computation cycles if it improves development time -- by having different microcontrollers dedicated to different subsystems. It also prevents needing other multiplexing solutions. Crude, yet scalable. That said, I did try to maintain good code quality regardless.
* Phone or phone-like (ARM-architecture, USB serial interface) control board.
    * Sadly, despite many attempts to get my 'phone robot' idea to work -- I ran into many roadblocks both with the Android OS and Android development. I will be revisiting these challenges in my next project.
* Adaptability to variations in power supply voltage -- maintaining consistent voltage performance as the batteries ran down.
    * My solution to this was buck converters -- see the Components section for more details.
* Compact, modular hardware design
    * Most of the work is done on a single custom control board that combines serial processing, motor control, and voltage regulation into a single unit. See Components for details. 


## Components

First, I'd like to begin with a moment of silence for the (many) L9110S H bridge modules destroyed in the process of me learning to control them. Oddly enough, these units require that the pins be HIGH by default -- the lower the effective voltage (via PWM), the faster your motors move. It is very odd. And there were other complications -- but first, some background.

The purpose of an H-bridge is to allow a load (usually a DC motor) to be powered with either polarity. Of course, changing polarity on a DC motor changes the direction of rotation. This makes H-bridges a common component in many consumer products. If you have an RC car, it probably has an H bridge of some sort.

The modules I chose -- generic L9110S boards ordered straight from China, are a very cheap and cost effective means of approaching this problem, and even more importantly, are very compact.

One of my goals in this project was to find ways of mineaturizing homebrew DC motor control systems to the point where their space costs on the device were trivial -- and I succeeded in this.

![](/assets/images/L9110S.jpg)


Sadly, there is no protection circuitry to prevent shorting inside the H-bridge. To understand this, please observe this simplified schematic from Wikipedia:

![](/assets/images/1200px-H_bridge.svg.png)

_CC BY-SA 3.0 licensed, by user Cyril BUTTAY_

As you can see, the simplified model of an H-bridge allows motors to spin in either direction by activating -- e.g., either S1 & S4, or, S2 & S3. One of the common features of such modules in practice, is that they also include protection circuitry to prevent activating -- e.g., both S1 & S2, which would result in a short that would quickly exceed the current rating of the switching elements.

Had I known there was no such protection circuitry, I would have at _least_ opted for some polyfuses in series with the H-bridge module VIN. In, fact, I would have likely also bought a generic NAND gate chip, and configured it to physically prevent this failure mode. I am unsure why these modules are not designed with this feature from the getgo, as it seems very trivial.

As it is, software errors alone can destroy the H-bridge module. This is, of course, unacceptable -- but the process of constructing my prototype taught me how to correct it, so I suppose it's still a win! :)

_Side note, as I bought these units from China for 1 USD/unit, it is entirely possible that they are merely factory rejects -- which would explain a lot._

## Buck converters

[[INSERT BUCK CONVERTER PHOTO HERE]]

I used a generic "3A mini" buck converter I found on eBay. By tuning them to different voltages, I was able to seperate the loads somewhat and meet their needs quite easily. I then simply used a much higher rail voltage than any components needed, and the buck converters convert the excess voltage to help meet current requirements -- for instance the heavy requirements needed for the Jetson Nano board.

## Control board

Since switching this project from Lego to physical parts, one of the things I was inspired by was the Lego RCX.

![](/assets/images/lego-mindstorms-rcx.jpg)

This thing pretty much was my childhood. It included power regulation, programmability, motor outputs, and sensor inputs -- and I decided, in the course of this project, that I'd try to approximate that elegance with cheapo home parts.

![](/assets/images/control-board-prototype.jpg)

Yyyeah... sorry about the lighting. My desk arrangement is still a WIP.

But hopefully the important elements are still visible here. Motors themselves, programmable microcontroller, H bridge, and buck converters. And, after a few hours of playing with assembly techniques, adhesives, and perfboards...

[[INSERT CONTROL BOARD PHOTO HERE]]

This isn't exactly an elegant board -- it's held together with liquid electrical tape, cyanoacrolyte, and solder joints -- but it does actually 'work'. Albeit with some pretty serious flaws that would exclude it from a UL listing, but it's the first prototype, so...

# Software

## CLion, PlatformIO, and coding styles

One of the things I most wanted to learn in this project, was how to use CLion as a substitute for the Arduino IDE, so that I could more closely approximate professional development cycles with my microcontroller projects. In this, I was quite successful. In the course of this project, I've learned that:

* CLion is very much available and usable by students for free. After applying as such, my account was approved and I can now make good use of the same features present in Intelij IDEA, with which I previously became aquainted -- except, this time, in C++!

* Using C++ is actually much easier with intellisense. Since it's a statically typed language, you get more useful notices and warnings, as compared to a dynamically typed language, like Python.

* My experience over the last 3 quarters with intellisense has convinced me of the folly of dynamic typing. The ability to type dynamically results in ambiguities that easily translate into more runtime errors, whereas these errors can be caught in compile time for a statically typed language, and before that if you have intellisense. The saved keystrokes seem hardly worth it now, to me.

* C++ is an old langauge with a lot of syntactic oddities. With intellisense, it becomes much more managable to get warnings and reminders for the pitfalls before you even hit compile. I found my development time speeding up massively. I wish I had been using CLion in my C++ class awhile back.

## Serial connection

I played around with having a serial interface that simply accepted 'WSAD' keystrokes, and it worked surprisingly well, but it's hardly an efficient way of encapsulating the entirety of the movements possible. Each motor has a state of 0-255, and a directional component too -- so we'll need multiple bytes to specify the entire rover state with each update in the final version. I think three bytes would be ideal.

The first byte specifies 0-255 for the left motor. The second specifies 0-255 for the right motor. And the third specifies the directionality for both, encoded in the 1 and 10 column.

Unfortunately, when I tried this, I found that data in the serial API was converted to ASCII data -- e.g., trying to send a byte of `2` results in actually sending `50`, or the number `2` in ASCII. This caused a lot of confusion for me at first, and it took a few dirt-simple diagnostic programs to determine that this is what was happening. E.g.:

```c++
void setup() {
    Serial.begin(9600);

    for(int i=0;i<256;i++) {
        Serial.println((unsigned char)i);
    }
}

void loop() {
    if (Serial.available() > 0) {
        Serial.write(Serial.read());
    }
}
```

and

```python
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
for i in range(256):
    print(ser.read(1))
```

Another thing I considered was to use some equivelent of `(ord('b')-97)/25*255`, so as to encode gradations of max motor speed as a single ASCII character, but I rejected this class of solution since my research indicated that ATMEGA chips could not efficiently perform division or multiplication operations. (They are unpacked to iterated addition/division.)

One unexplored option is to produce compact Python code that produces highly redundant, yet efficient series of C++ checks. It seems a little cheesy, though, and honestly I'd rather just encode to ASCII and decode as short parsable strings.

String manipulation is definitely not a good solution, however, and I had trouble getting [the alternatives](https://forum.arduino.cc/index.php?topic=40910.0) to work for me. I intend to continue exploring this in the future.

What I learned most from the hours of struggling with serial stuff in this project, was to smoothly transition my projects from proof of concept to finished project -- rather than quickly developing an idea based on my previous understanding of serial connections. It turned out that it was surprisingly difficult to seperate what I had guessed to be possible, and what I had actually directly experienced as possible in my previous experience working with arduino projects.

## Android development

This is one of the areas in which I encountered abject failure. After hours of banging my head against the wall with Android UI design, and API, I found two critical problems with my original parameters for this project:

* Android UI design is surprisingly complicated and resistant to my usual learning style
    * I usually like to learn about things from a bottom-up, goal-oriented approach.
* Many Android devices actually do not allow devices to simultaneously charge and take in data. This seems like a pretty absurd limitation, and made it difficult to proceed with the project what with my old second-hand phones.

## Latency

One of my research concerns was exploring the latency & extensibility of the Wifi control approach. To my surprise, a simple serial-over-SSH test case produces extremely good and responsive results, with most hiccups (I believe) being a result of poor back-EMF protection in my circuitry. 

## SSH + arduino

This was extremely easy, by combining my previous systems knowledge & tests together. It is, in fact, the first time I've made a mobile platform with SSH support.

And yet, I learned quite a bit from this process -- including that PlatformIO allows for quick & simple headless serial interface access & firmware development, over any remote shell connection.

Here is the process I used to set up my Ubuntu 18.04 install to function

```bash
sudo apt install python3-pip
sudo pip3 install platformio
pio platform install atmelavr
sudo usermod -aG dialout alex
# log out, then log back in
pio device monitor -e nanoatmega328
```
