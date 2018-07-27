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

# Dependancies
- [`libcurl`](https://curl.haxx.se/libcurl/) - libcurl
- [`libcurses`](http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)

# Building Swamp RAT
```bash
./configure.sh
make
```

# Goals
- To detect network traffic one would need your servers private key for `ssl` decryption as well as advanced `suricata` rules `lua` scripts to undo the `xor` encryption.
- `ssl` fingerprinting could still be used but you can just generate a new random certificate

# TODO
- SSL Communications
- Keylogging
- Webcam Snapshot
