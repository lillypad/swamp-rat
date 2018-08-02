[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://github.com/lillypad/swamp-rat/blob/master/LICENSE)

# Swamp RAT

<p align="center">
  <a target="_blank" href="https://www.youtube.com/watch?v=JYyw4ODRndI">
    <img src="https://github.com/lillypad/swamp-rat/raw/master/img/swamp-rat.gif" alt="Swamp RAT"/>
  </a>
</p>


A `linux` RAT that lurks where others do not.

# Purpose

I started this as most free `linux` RATs are done half hazardly in higher level languages.

# Screenshots

<p align="center">
  <img src="https://github.com/lillypad/swamp-rat/raw/master/docs/img/swamp-rat.png" alt="Swamp RAT"/>
</p>

# Dependancies
- [`libcurl`](https://curl.haxx.se/libcurl/)
- [`libcurses`](http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)

# Building Swamp RAT
```bash
./configure.sh
make
```

# Notable Features
- `xor` encrypted packets over `tcp`
- `ncurses` terminal `ui`

# TODO
- SSL Communications
- Webcam Snapshot
- Finish `ncurses` interface
