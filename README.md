# ProcessScanner

This program is just a wrapper over the windows API to make it more convenient for reading and writing at other processes memory at runtime and also do a little bit of pattern matching in memory chucks. Just like [CheatEngine]() does.
Though this is much simpler, and is missing a lot of useful features, it can still be quite handy to have it for doing a little script to "hack" into some program's memory.

# Usage

As the code for it is very small only reading the header file will give you undestanding of what you can do, and there is also the DS4 example which uses the library to change the DS4 controller's light color by alterating its memory which at the time was not a feature of the [DS4Window app]()

If you have DS4 app and want to see if the example still working just compile it with any C++ compiler on windows.

```console
$ (cc) ProcessScanner.cpp DS4_app_Example.cpp -o example
```

It should be already to go, just remember to set the starting color of the controller as R=65 G=0 B=0 so that the application can try to find it by pattern matching.
