# Netpbm Windows Imaging Component Codec

[![Build and test](https://github.com/vbaderks/netpbm-wic-codec/actions/workflows/msbuild.yml/badge.svg)](https://github.com/vbaderks/netpbm-wic-codec/actions/workflows/msbuild.yml)

This Windows Imaging Component (WIC) codec makes it possible to decode (.pgm) files with Windows applications that can leverage WIC codecs.
It makes it possible to view .pgm encoded images in Windows PhotoViewer, Windows Explorer and import .pgm images in Microsoft Office documents.

It is an experimental C++20 implementation based on the <https://github.com/chausner/PnmWicImageCodec> project.

## Introduction

Netpbm is the name of an open source package of graphics programs and a programming library.
It defines several graphics formats that are not compressed but are easy to exchange between different
platforms:

|Type            |Extension|Magic number|Color space|
|----------------|---------|--------------------------|-----------|
|Portable BitMap |.pbm     |P1,P4       | 0-1 (white & black)|
|Portable GrayMap|.pgm     |P2,P5       | 0-255, 0-65535 * 1 channel (gray scale|
|Portable PixMap |.ppm     |P3,P6       | 0-255, 0-65535 * 3 channels (sRGB)|
|Portable AnyMap |.pnm     |P1,P2,P3,P4,P5,P6|Several|
|Portable Arbitrary Map|.pam|P7       |Several|

### Color space

The AnyMap format provides no support to store the actual color space. The typical convention is to assume grayscale for P2\P5 images and sRGB for P3\P6.

### WIC

The Windows Imaging Component (WIC) is a built-in codec framework of Windows that makes it possible
to create independent native image codecs that can be used by a large set of applications.
WIC is implemented using COM technology. Image codecs for many popular formats are already pre-installed on Windows.

### Status

This project is the development phase:

- Visual Studio 2022 17.4 or newer is needed to build the project.
- No installer with pre-built binaries is available, manually building and registering is required.
- Only .pgm P5 gray scale images can be decoded.
- All .pgm files are considered single frame (the Netpbm standard also allows multi-frame)
- Only a decoder is available.

### Supported Operation Systems

The Netpbm WIC codec supports the following Windows operating systems:

- Windows 11
- Windows 10
- Windows Server 2022
- Windows Server 2019
- Windows Server 2016

### Applications that can use the Netpbm WIC codec

The following application have been validated to work with the Netpbm WIC codec:

- Windows Explorer Thumbnail cache. This functionality allows Windows Explorer to show previews of images.
- Windows Photo Viewer.  
 Note: On clean installations of Windows 10 this component is installed but not registered. An registry file called restore-windows-photo-viewer.reg
 is provided to restore this functionality. The standard Windows Registry Editor can be used to add import this .reg file.
- WIC Explorer (sample application from Microsoft). An updated version of this application can be found at <https://github.com/vbaderks/WICExplorer>
- ZackViewer <https://github.com/peirick/ZackViewer>. This viewer can also be used to convert from one image encoding format to another.
- Microsoft Office applications like Word, Excel, Powerpoint. These applications can, when the NetPbm codec is installed, import .pgm images in their documents.

#### Windows 11\10 Microsoft Photos Application not supported

The standard Windows 11\10 Microsoft Photos application cannot be used at this moment as it is limited to the
WIC codecs that are pre-installed on Windows or are provided by Microsoft in the Microsoft Store.
Microsoft currently does not make it possible to create WIC codecs that can be uploaded to the Microsoft Store.

## WIC Codec Identity

The following table provides the codec identification information:

|Property||
|---|---|
|Formal Name|Netpbm Format|
|File Name Extension|.pgm|
|MIME type| image/x-pgm|

The following table lists the GUIDs used to identify the native Netpbm codec components:

|Component|GUID
|---|---|
|Container Format|70ab66f5-cd48-43a1-aa29-10131b7f4ff1
|Decoder Class ID |06891bbe-cc02-4bb2-9cf0-303fc4e668c3|

The following table lists the encodings that can be decoded:

|Magic|Component Count|Bits per Sample|WIC Pixel Format GUID|
|---|---|---|--|
|P5|1|2|GUID_WICPixelFormat2bppGray
|P5|1|4|GUID_WICPixelFormat4bppGray
|P5|1|8|GUID_WICPixelFormat8bppGray
|P5|1|10,12,16*|GUID_WICPixelFormat16bppGray

Note *: monochrome images with 10 or 12 bits per sample will be upscaled to 16 bits per sample.

## Build Instructions

1. Clone this repro
1. Use Visual Studio 2022 17.4 or newer and open the netpbm-wic-codec.sln. Batch build all projects.
1. Or use a Developer Command Prompt and run use MSbuild in the root of the cloned repository.

## Installation

1. Open a command prompt with elevated rights
1. Navigate to folder with the netpbm-wic-codec.dll
1. Execute:

```shell
regsvr32 netpbm-wic-codec.dll
```

## Uninstall

1. Open a command prompt with elevated rights
1. Navigate to folder with the netpbm-wic-codec.dll
1. Execute:

```shell
regsvr32 -u netpbm-wic-codec.dll
```
