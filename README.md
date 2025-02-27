<h1 align="center">TXT Reader for Numworks Calculator</h1>
<p align="center">
    <img alt="Version" src="https://img.shields.io/badge/Version-1.0.0-blue?style=for-the-badge&color=blue">
    <img alt="Stars" src="https://img.shields.io/github/stars/SaltyMold/TXT-Reader?style=for-the-badge&color=magenta">
    <img alt="Forks" src="https://img.shields.io/github/forks/SaltyMold/TXT-Reader?color=cyan&style=for-the-badge&color=purple">
    <img alt="License" src="https://img.shields.io/github/license/SaltyMold/TXT-Reader?style=for-the-badge&color=blue">
    <br>
    <a href="https://github.com/SaltyMold"><img title="Developer" src="https://img.shields.io/badge/Developer-SaltyMold-red?style=flat-square"></a>
    <img alt="Maintained" src="https://img.shields.io/badge/Maintained-No-blue?style=flat-square">
    <img alt="Written In" src="https://img.shields.io/badge/Written%20In-C-yellow?style=flat-square">
</p>

_This app is a [TXT](https://en.wikipedia.org/wiki/Text_file) reader that runs on the [NumWorks calculator](https://www.numworks.com), allowing users to open and read plain text files directly on their device. It provides a simple interface for navigating through text, making it ideal for viewing notes, documents, and other text-based content on the go._

## Install the app

To install this app, you'll need to:
1. Download the latest `TXT-Reader.nwa` file from the [Releases](https://github.com/SaltyMold/TxtReader/releases) page
2. Choose a `txt` file you want to read on your calculator.
3. Connect your NumWorks calculator to your computer using a USB cable.  
4. Head to [my.numworks.com/apps](https://my.numworks.com/apps) to send the `nwa` file on your calculator along the `txt` file.

## How to use the app
| Touche     | Action    |
|------------|----------|
| Back | Quit  |
| Arrow Down | Down |
| Arrow Up | Up |

## Build the app

To build this sample app, you will need to install the [embedded ARM toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain) and [Node.js](https://nodejs.org/en/) 18. The C SDK for Epsilon apps is shipped as an npm module called [nwlink](https://www.npmjs.com/package/nwlink) v0.0.16.

```sh
#debian

sudo apt install -y build-essential git gcc-arm-none-eabi binutils-arm-none-eabi nodejs npm && npm install -g n && sudo n 18 && npm install -g nwlink@0.0.16
git clone https://github.com/SaltyMold/TXT-Reader.git
make clean && make build
```


```sh
#windows

#install msys2 from https://github.com/msys2/msys2-installer/releases/download/2025-02-21/msys2-x86_64-20250221.exe and open the msys2.exe file.
pacman -Syu

#Download the .zip from https://github.com/actions/node-versions/releases/download/18.20.7-13438827950/node-18.20.7-win32-x64.7z, extract it, and set the variable.
npm install -g nwlink@0.0.16
nwlink --version 

pacman -S --noconfirm mingw-w64-x86_64-arm-none-eabi-gcc
arm-none-eabi-gcc --version

pacman -S --noconfirm git
git --version
git clone https://github.com/SaltyMold/TXT-Reader.git

cd TXT-Reader
make clean && make build
```

You should now have a `output/app.nwa` file that you can distribute! Anyone can now install it on their calculator from the [NumWorks online uploader](https://my.numworks.com/apps).

## Build your own app
To build your own app, start by cloning the repository:

```sh
git clone https://github.com/SaltyMold/TXT-Reader.git
```
Inside the project, you'll find eadk.h, which provides essential functions for interacting with the calculator. Modify main.c to implement your own code.
Additionally, make sure to include an icon.png with dimensions 55×56 pixels to serve as your app’s icon. Once your modifications are done, link the app with [nwlink](https://www.npmjs.com/package/nwlink) and enjoy your app!

## Special thanks 

I followed the guide from [epsilon-sample-app-c](https://github.com/numworks/epsilon-sample-app-c) to build this app.
