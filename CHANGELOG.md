xrick Release History
---------------------

### Version 021212 / December 12, 2002:

* Reworked the build system so xrick can easily be built on Unix boxes as well as on non-Unix boxes (do not rely on symbolic links, etc.).
* Use function keys instead of numeric keys for various game controls, as some non-US keyboard require that SHIFT is pressed for numbers (A. Majorel).
* Fixed a potential buffer overflow in command-line parameters handling (A. Majorel).
* Joystick is (hopefully) OK now (S. Chauveau).
* Implemented all sounds: music, effects, etc.
* The sound was horrible on Windows due to a poorly dimensionned sound buffer. Fixed.
* Re-ripped all sounds. This time I used STeem with debugger to save each sound as a WAV file, then Audacity to filter an annoying static noise produced by STeem, then SOX to re-sample.

### Version 010808 / August 8, 2001:

* Alpha sound support, Basic sounds are here, including the Waaaaaa of Death, but music is not implemented. The quality is far from being perfect -- it's alpha.
* Finished Hall of Fame ("congratulations" banner, etc.)
* Added an "xrick" splash screen
* Various minor bugfixes
* New command line options: -nosound to disable sound, and -vol <vol> to set volume to <vol>
* Man page (thanks to André Majorel)
* Cheats now an option that can be turned off at compile time
* Updated -help text
* Add "500" animation when capturing bonus
* Add a switch so that zombies (everything that has been killed) can not be hidden by foreground tiles
* Discovered that initial values for Hall of Fame differ on Atari and PC -- fixed
* Still no feedback about joystick support. However, by comparing w/other SDL apps, it seems I had up/down swapped -- fixed. But it remains alpha
* Disabled "pause of focus" for BeOS -- it seems there are problems w/focus management on BeOS and the game would always go to pause

### Version 010721 / July 21, 2001:

* Plenty of code cleanup.
* Implement hall of fame (ongoing, need to improve presentation)
* Alpha joystick support (I have no joystick and therefore can not test that it works), please report bugs.
* Hide the mouse cursor on the screen
* Automagically determine fullscreen zoom level (no more problem w/missing 320x200 setup under X)
* Pause when window is de-activated (i.e. focus was lost). Must press P to play again (i.e. gaining focus does not automatically restart the game). Also added a "pause" indicator to make it clear when the game is paused.
* Got rid of tiles bank #1 for ST, as it was a copy of bank #0 (for IBM the two banks differ)
* Reworked the sprites display system, wich was rather dirty and was the cause of several display bugs
* Packaging: I have been playing with automake/autoconf, but it's going to take some time until I can do what I want with it. I wonder if it's not a bit of an overkill: I'll probably stick with the current system for a while.
* Fix various things: scroller period set to 24, game over screen for ST graphics, adjust positions for ST graphics, proper ST bomb animation, IBM graphics for map 2 and 3, ...
* Bug fix: connection problem between submaps 0x11 and 0x12
* Bug fix: map 3, when dogs run fast enough, "expose" cheat mode leaves a dirty trail
* Bug fix: map 4, the rocket traps leave a trail of old sprites which only slowly get cleared
* Bug fix: submap 47, wrong sprites sequence (bug in the original IBM PC version of the game. replaced by a valid sequence)

### Version 010710 / July 10, 2001:

* Code cleanup (so I can figure out what I am doing)
* Now compiles and runs on Windows under Cygwin (with SDL 1.2.1)
* New "config" system (yes I'd rather use automake/autoconf but I need to figure out how to do it)
* Fix bug in "trainer" cheat mode (bombs and bullets always stay at six but lives would go down to zero and then xrick segfaulted)
* Make it possible to exit the game (via ESC) from everywhere, including the introduction, game over, etc. screens
* Properly lock and unlock SDL surfaces
* New parameters: -fullscreen, -zoom (default is 2), -speed (replaces -p)
* Got rid of "sprites planes" for PC-CGA graphics
* Add support for arrow keys in addition to original keys
* Make it possible to close the window (and thus abort the game)
* Fix bug w/dynamite sticks remaining lethal forever after they'd exploded
* Fix bug w/scrolling having a small delay at the end
* Fix bug w/cheat infos appearing during intro, hall of fame, etc.
* Keys config via -keys command line option
* Allow jump to any map/submap via -map and -submap command line options
* Added an icon for Windows

### Version 010315 / March 15th, 2001:

* First public release.
