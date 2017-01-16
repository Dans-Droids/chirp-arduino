# Chirpino for Spark Core

Version 1.1, January 2016

## Overview

**Chirpino** is a program for Arduinos and other microcontrollers to let you create, play and store lists of chirps; this document describes the version for Spark Core. **Chirpino** lets you store playlists of chirp codes and create chirps for text messages and URLs so they can be picked up by anyone with a phone or other device running a chirp-enabled app. You can use its functionality as written or you can use it as an example to help you create your own programs exploring the world of chirps.

**Chirpino** uses the Spark Core's external flash to store chirp playlists even when the power is off. By default, each of the four playlists has room for up to ten chirp codes. When new chirp codes are written to a full playlist they overwrite the oldest ones so in each playlist only the most recently added ten are stored.

Playlists hold settings such as play order, audio preferences, and timing information for sequenced or timed play. 

This document includes the following sections:

* **Getting Started** - required hardware and software, obtaining your API key, and how to run the program
* **Usage** - the command line interface to **Chirpino**; a large number of very simple commands
* **Scripting** - making lists of chirps and/or command sequences available on the web to update your playlists

In addition, two other documents are available:


* **ChirpinoSing.pdf** - full documentation on the **ChirpinoSing** sound output library and its example programs. **ChirpinoSing** was written as an Arduino library, a port of which is bundled here together with the main **Chirpino for Spark Core** source as the files **Beak**, **PortamentoBeak** and **Synth**.


## Getting started

### Required hardware

This version of **Chirpino** is written for the Spark Core.

You'll need a small speaker or an earphone for sound output, or you can open up a 3.5mm audio jack and connect to a larger audio device. Use **pin 10** (A0) and **ground** (GND). Although an amplifier and filtering circuitry might improve the power and quality of the sound output, we have had good results from just connecting either an earphone or an old radio AUX-in via an audio jack wired directly to the pins. In both these cases our sound output is probably technically outside the range expected by the device, so connect at your own risk. The **ChirpinoSing** library documentation describes the sound output and the synthesis process in more detail.

You can control the Spark Core by sending commands from **Chirpino**'s extensive command set using serial-over-usb when it's connected by usb to a computer. You can also use use Spark cloud commands to control Chirpino when it's on wi fi.

### Software dependencies

**Chirpino** for Spark Core contains all the source files that you need.

### Running the program

Ensure your Spark Core is connected and selected, then compile and upload the program. Once the upload is complete open up a serial monitor (the Arduino one works well; set the options at the bottom of the window to 115200 baud and Newlines). You should soon see some messages. Now type a plus sign followed by a few words into the text input box (eg `+hello world`) at the top of the monitor and hit return. If all goes well you should see more messages and hear the new chirp you have just created played out loud through your connected audio device.

## Usage

Interaction with **Chirpino** is by text commands over a serial monitor (set it to 115200 baud, newlines).

Commands are generally one or two punctuation characters. Parameters immediately follow the lead character without a space. Even though many commands are just single characters you always need to press return or hit the send button to issue the command.


### Add existing chirp codes

If you have an 18-character-long chirp code then simply type or paste it in to add it to your current playlist, eg

	ntdb982ilj6etj6e3l

Codes added like this don't play automatically; use the `!` command (below) to play your newly added code. 

#### Playing chirps

New chirps are played when they are created. Chirpino keeps track of the current playlist and the current chirp. When you add a chirp it becomes the current one.

`!` play the current chirp

`]` play the next chirp within the playlist (this is a more recent one, except where the sequence wraps round to the oldest again)

`[` play the previous chirp (one added earlier, except where the sequence wraps around to the newest one)

`)` play the next chirp but chain the playlists together (so the first chirp of the next playlist is played once the current playlist is completed)

`(` play the previous chirp, but chain the playlists (in reverse order)

`,` play the next chirp in autoplay order (see below)

`!number` select & play the chirp at this index position. Eg

	!4

plays the chirp at the fifth position (counting from 0) in the current playlist.

`!code` plays the given 18 character chirp code (containing only digits and/or letters 'a' to 'v') **without** storing it. Eg

	!oscu7ih85kdq6hl3il


#### Playlists

You can select the current playlist by its index `0` to `3` or you can step forward or back through them. Most commands apply only to the selected playlist.

`@number` selects the playlist with the given index. Eg

	@0
	
selects the first playlist

`}` selects the next playlist (wrapping back to 0 where appropriate)

`{` selects the preceding playlist (wrapping round to 3 where appropriate)


#### Clearing

`~` clears the current playlist's chirp codes

`~number` clears a combination of items in the current playlist: add `1` to clear chirp codes, `2` to clear settings, `4` to clear script url. Eg

	~6
	
(2 + 4) leaves the chirp codes intact but clears the settings (including sound and timer preferences) and the script url.

`~#` clears the current playlist's script url (same as `~4`)

`~!` clears everything in **ALL** playlists, effectively resetting the playlist storage


#### Audio

`:number` set the volume to the given value (0 to 255). Very low values are not only quieter but result in lower quality sound output.

`:` set the default volume (the maximum, 255)

`:P` use Portamento chirps (these slide between tones)

`:N` use plain (Non-portamento) chirps (these change tones abruptly)

`:M` Mute or unMute sounds (toggle)

Whereas `L` and `P` (and volume) affect the stored settings for the current playlist, `M` simply turns ALL sound output on or off and does not store the setting. The letters here aren't case-sensitive ('`p`, `n` and `m` are fine).

You can combine chirp style and volume. Enter the letter before the volume value. Eg

	:p128

sets portamento at half volume (numerically, if not in loudness).


#### Autoplay

A whole playlist (or the next few chirps - see `|`) can be played instead of just a single chirp. This autoplaying can be triggered by a `*` command or can be set to occur at particular times.

##### Intervals

`.number` sets the time in seconds between the start of successive chirps during autoplay. Egs

	.20

plays the chirps at 20 second intervals.

	.

sets the default interval of 10 seconds.

##### Orders

Choose from a number of autoplay **orders** (stored in the playlist's settings):

`.F` forwards (the default); start with the oldest and play each in turn until the most recently added chirp

`.B` backwards; start with the most recent and play each in turn until the oldest

`.S` shuffle; play each chirp once in a random order

`.R` random; play chirps entirely at random perhaps repeating some & omitting others

Setting the autoplay order moves the play position to be the first as appropriate for the sequence (eg B sets it to be the last chirp in the playlist, F picks the first chirp). These letters aren't case-sensitive.

##### Limits

`|number` (pipe) limits the number of chirps played during autoplay to number, eg

	|2
	
results in only two chirps being played each autoplay.

`|` resets to the default of playing the whole playlist

##### Immediate autoplay

`*`	Autoplays the current playlist immediately

Hitting return or issuing any new command stops this immediate autoplay. Immediate autoplay does not trigger automatic repeats.

#### Triggers

Triggers allow you to associate an event with a short script. Chirpino comes with two pins set up as 'buttons'; connect the pin to ground briefly to 'press' the 'button'. A third trigger is an inactivity trigger; if you set a non-zero time in seconds it will trigger whenever no chirp has played for this length of time.

The button tagged `l` (for left) is mapped to the chained previous command `(`. Button `r` is mapped to chained next `)`. The inactivity trigger, tagged `z` is also mapped to `)` so will play the next chirp whenever the unit has been ignored (don't worry: it's off by default).

`=tag` lets you fire triggers from scripts or the command line. Eg

	=r
	
is equivalent to 'pressing' the right button.

`-seconds` lets you set the inactivity timer; use 0 to turn it off. Eg

	-120
	
will cause another chirp to be played every two minutes.

Triggers are most useful when you add your own. Each needs a tag and a script (each of which could, as above, be simply a single character). They have an update method that's called every time around the main loop (so very frequently), and an action method that can be called by update when the conditions are right. You could check sensors, track the time, perform calculations or whatever you like really. Take a look in Triggers.cpp; they're pretty simple.

***

This document is part of the [Chirp developer resources](http://developer.chirp.io).

All content copyright &copy; Asio Ltd, 2013-2017. All rights reserved.
