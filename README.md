<!--
  SPDX-FileCopyrightText: © 2020 Team CharLS
  SPDX-License-Identifier: BSD-3-Clause
-->

# Netpbm Windows Imaging Component Codec

[![Build and test](https://github.com/vbaderks/netpbm-wic-codec/actions/workflows/msbuild.yml/badge.svg)](https://github.com/vbaderks/netpbm-wic-codec/actions/workflows/msbuild.yml)
[![REUSE status](https://api.reuse.software/badge/git.fsfe.org/reuse/api)](https://api.reuse.software/info/git.fsfe.org/reuse/api)

This Windows Imaging Component (WIC) codec makes it possible to decode .pgm and .ppm files with Windows applications that can leverage WIC codecs.
It makes it possible to view Netpbm encoded images in Windows PhotoViewer, Windows Explorer (including thumbnails)
and import these images in Microsoft Office documents.

It is a C++ 23 implementation originally based on the [PNM WIC Image Codec project](https://github.com/chausner/PnmWicImageCodec).
It leverages the C++/WinRT framework to implement the classic COM components required for a WIC codec and uses Wix v5 for the installer.

## Introduction

Netpbm is the name of an open source package of graphics programs and a programming library.
It defines several graphics formats that are not compressed but are easy to exchange between different
platforms:

|Type                  |Extension|Magic number     |Color model                             |
|----------------------|---------|-----------------|----------------------------------------|
|Portable BitMap       |.pbm     |P1,P4            | 0-1 (white & black)                    |
|Portable GrayMap      |.pgm     |P2,P5            | 0-255, 0-65535 * 1 channel (gray scale)|
|Portable Pixel Map    |.ppm     |P3,P6            | 0-255, 0-65535 * 3 channels (RGB)      |
|Portable AnyMap       |.pnm     |P1,P2,P3,P4,P5,P6|Several                                 |
|Portable Arbitrary Map|.pam     |P7               |Several                                 |

### Color Model \ Color Space

The Netpbm format only supports to store the color model but not the actual color space.
Several color spaces are used in practice, for example: BT.709 and sRGB.
As no color space information is available the codec doesn't provide it through its COM interface.
It is up to to displaying application to select the matching color space. For RGB images this is typical sRGB on Windows.

### WIC

The Windows Imaging Component (WIC) is a built-in codec framework of Windows that makes it possible
to create independent native image codecs that can be used by a large set of applications.
WIC is implemented using COM technology. Image codecs for many popular formats are already pre-installed on Windows.

## Installing

### Requirements

- Windows 11 or Windows 10 (version 22H2).
- x86, x64 or ARM64 processor

### Via GitHub with EXE

Go to the [releases](https://github.com/team-charls/netpbm-wic-codec/releases) page and click on
Assets at the bottom to show the files available in the release.
Please use the appropriate installer that matches your machine's architecture.

> [!NOTE]
> Microsoft Defender SmartScreen may show a warning about an unrecognised app when running the installer. Click on "More Info" + "Run anyway" to continue the installation.  
The installer and the DLL are signed, but Defender SmartScreen requires an EV code signing certificate, which is only available to commercial organisations.

## Applications that can use the Netpbm WIC codec

The following application have been validated to work with the Netpbm WIC codec:

- Windows Explorer Thumbnail cache. This functionality allows Windows Explorer to show previews of images.
- Windows Photo Viewer.  
 Note: On clean installations of Windows 10 this component is installed but not registered. An registry file called restore-windows-photo-viewer.reg
 is provided to restore this functionality. The standard Windows Registry Editor can be used to add import this .reg file.
- WIC Explorer (sample application from Microsoft). An updated version of this application can be found at <https://github.com/vbaderks/WICExplorer>
- ZackViewer <https://github.com/peirick/ZackViewer>. This viewer can also be used to convert from one image encoding format to another.
- Microsoft Office applications like Word, Excel, PowerPoint. These applications can, when the NetPbm codec is installed, import .pgm images in their documents. The 32 bit version of Office
requires that the x86 version of the codec is installed.

#### Windows 11\10 Microsoft Photos Application not supported

The standard Windows 11\10 Microsoft Photos application cannot be used at this moment as it is limited to the
WIC codecs that are pre-installed on Windows or are provided by Microsoft in the Microsoft Store.
Microsoft currently does not make it possible to create WIC codecs that can be uploaded to the Microsoft Store.

## WIC Codec Identity

The following table provides the codec identification information:

|Property           |             |
|-------------------|-------------|
|Formal Name        |Netpbm Format|
|File Name Extension|.pgm, .ppm   |
|MIME type          | image/x-pgm |

The following table lists the GUIDs used to identify the native Netpbm codec components:

|Component        |GUID                                |
|-----------------|------------------------------------|
|Container Format |70ab66f5-cd48-43a1-aa29-10131b7f4ff1|
|Decoder Class ID |06891bbe-cc02-4bb2-9cf0-303fc4e668c3|

The following table lists the formats that can be decoded:

|Magic|Component Count|Bits per Sample|WIC Pixel Format GUID       |
|----:|--------------:|--------------:|----------------------------|
|P5   |              1|              2|GUID_WICPixelFormat2bppGray |
|P5   |              1|              4|GUID_WICPixelFormat4bppGray |
|P5   |              1|              8|GUID_WICPixelFormat8bppGray |
|P5   |              1|      10,12,16*|GUID_WICPixelFormat16bppGray|
|P6   |              3|              8|GUID_WICPixelFormat24bppRGB |
|P6   |              3|             16|GUID_WICPixelFormat48bppRGB |

Note *: monochrome images with 10 or 12 bits per sample will be upscaled to 16 bits per sample.

## Manual Build Instructions

1. Clone this repro
1. Use Visual Studio 2022 17.11 or newer and open the netpbm-wic-codec.sln. Batch build all projects.
1. Or use a Developer Command Prompt and run use MSBuild in the root of the cloned repository.

### Installation

1. Open a command prompt with elevated rights
1. Navigate to folder with the netpbm-wic-codec.dll
1. Execute:

```shell
regsvr32 netpbm-wic-codec.dll
```

### Uninstall

1. Open a command prompt with elevated rights
1. Navigate to folder with the netpbm-wic-codec.dll
1. Execute:

```shell
regsvr32 -u netpbm-wic-codec.dll
```

### Building and code signing

A command file is available to build and sign the WIC DLL and the setup application.  
Instructions:

* Open a Visual Studio Developer Command Prompt
* Go the root of the cloned repository
* Ensure a code signing certificate is available
* Execute the command `create-signed-builds.cmd certificate-thumb-print time-stamp-url`  
 Note: the certificate thumbprint and time stamp URL arguments are depending on the used code signing certificate.

 The WIC DLL and the installer will be signed for the release builds of x86, x64 and ARM64.
