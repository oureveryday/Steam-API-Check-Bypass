VersionShim
===========
A Dynamic-Link Library which exports the same methods as version.dll (winver.h) in addition to chainloading additional user defined libraries.

Usage
-----
Rename VersionShim.dll to `version.dll` and place it in the target process's search location,
then create a file named `libraries.txt` in the same directory and list the paths to all the libraries you'd like to load,
each on a new line.

You can optionally select the target process(es) by listing its path or filename, each on a new line preceded by `*`. These must come before the library paths.

Lines beginning with `#` will be skipped.

libraries.txt *MUST* be encoded in UTF-8.

License
-------
This project is subject to the terms of the [Mozilla Public License, v. 2.0](./LICENSE).