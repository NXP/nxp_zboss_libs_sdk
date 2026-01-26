# NXP Zboss SDK for Linux on i.MX

The NXP Zboss SDK for Linux provides pre-built Zigbee R23 libraries and example source codes.<br>

## Pre-requirements
1. What describes this documentation can be tested on i.MX target with [Standard i.MX images](https://www.nxp.com/design/design-center/software/embedded-software/i-mx-software/embedded-linux-for-i-mx-applications-processors:IMXLINUX) for Zigbee and Openthread with ot-daemon<br>
To use OT Border Router as described in Dual-Pan mode, a "meta-nxp-connectivity" i.MX image is required<br>
Either [meta-nxp-connectivity](https://github.com/nxp-imx/meta-nxp-connectivity/tree/master) image to build or a [prebuilt image](https://www.nxp.com/design/design-center/development-boards-and-designs/MPU-LINUX-MATTER-DEV-PLATFORM)

2. i.MX Yocto SDK is required to build examples and develop new applications, see [meta-nxp-connectivity](https://github.com/nxp-imx/meta-nxp-connectivity/tree/master?tab=readme-ov-file#how-to-build-openthread-border-router-and-openthread-daemon-with-yocto-sdk) to build it

## Build
See [CMake build instructions](CMake_build_guide.md) 

## Learn more
To learn more, see [README.txt](README-linux.txt)
