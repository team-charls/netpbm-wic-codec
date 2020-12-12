# Netpbm Windows Imaging Component Codec

This Windows Imaging Component (WIC) codec makes it possible to decode (.pgm) files with Windows applications that can leverage WIC codecs. It makes it possible to view .pgm encoded images in Windows PhotoViewer, Windows Explorer and import JPEG-LS images in Microsoft Office documents.

It is an experimental C++20 implementation based on the <https://github.com/chausner/PnmWicImageCodec> project.

## Introduction

The Windows Imaging Component (WIC) is a built-in codec framework of Windows that makes it possible to create independent native image codecs that can be used by a large set of applications. WIC is implemented using COM technology. Image codecs for many popular formats are already pre-installed on Windows.

### Status

This project is the experimental development phase:

- Visual Studio 2019 16.9.0 Preview 2.0 is needed to build the project.
- No installer with pre-built binaries is available, manually building and registering is required.
- Only 8 bits per component .pgm (graysscale) images can be decoded.
- All .pgm files are considered single frame (the Netpbm standard also allows multi-frame)

### Supported Operation Systems

The Netpbm WIC codec supports the following Windows operating systems:

- Windows 10
- Windows Server 2016

### Applications that can use the Netpbm WIC codec

The following application have been validated to work with the Netpbm WIC codec:

- Windows Explorer Thumbnail cache. This functionality allows Windows Explorer to show previews of images.
- Windows Photo Viewer.  
 Note: On clean installations of Windows 10 this component is installed but not registered. An Registry file called restore-windows-photo-viewer.reg is provided to restore this functionality. The Registry Editor can be used to add import this .reg file.
- WIC Explorer (sample application from Microsoft). An updated version of this application can be found at <https://github.com/vbaderks/WICExplorer>
- ZackViewer <https://github.com/peirick/ZackViewer>. This viewer can also be used to convert from one image encoding format to another.
- Microsoft Office applications like Word, Excel, Powerpoint. These applications can, when the JPEG-LS codec is installed, import JPEG-LS images in their documents.

#### Windows 10 Microsoft Photos Application not supported

The standard Windows 10 Microsoft Photos application cannot be used at this moment as it is limited to the WIC codecs that are pre-installed on Windows or are provided by Microsoft in the Microsoft Store.
Microsoft currently does not make it possible to create WIC codecs that can be uploaded to the Microsoft Store.

## WIC Codec Identity

The following table provides the codec identification information:

|Property||
|---|---|
|Formal Name|Netpbm Format|
|File Name Extension|.pgm|
|MIME type| image/x-pgm|

The following table lists the GUIDs used to identify the native Netpbm codec components:

|Component|Friendly Name|GUID
|---|---|---|
|Container Format|GUID_ContainerFormatNetPbm|70ab66f5-cd48-43a1-aa29-10131b7f4ff1
|Decoder|CLSID_NetPbmDecoder|06891bbe-cc02-4bb2-9cf0-303fc4e668c3|

The following table lists the pixel formats that can be decoded:

|GUID|Component Count|Bits per Sample|Remark
|---|---|---|---|
|GUID_WICPixelFormat8bppGray|1|8|

## Build Instructions

1. Clone this repro
1. Use Visual Studio 2019 16.9.0 Preview 2.0 or newer and open the netpbm-wic-codec.sln. Batch build all projects.  
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
