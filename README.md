# Tântalo

This project introduces a versatile tool for the ESP32 platform that enables the implementation of various Wi-Fi attacks. It provides essential functionality commonly used in Wi-Fi attacks and simplifies the process of creating new attacks. The tool includes features such as capturing PMKIDs from handshakes, capturing and parsing WPA/WPA2/WPA3 handshakes, deauthentication attacks using different methods, denial of service attacks, formatting captured traffic into PCAP format, parsing captured handshakes into HCCAPX files ready for cracking with Hashcat, passive handshake sniffing, an easily extensible framework for new attack implementations, and a management AP for easy configuration on the go. Please note that this project does not include the capability to crack hashes effectively, as the ESP32 is not suitable for that purpose. However, it offers a compact, affordable, and low-power solution for other Wi-Fi attack scenarios.


## Features
- **PMKID capture**
- **WPA/WPA2/WPA3 handshake capture** and parsing
- **Deauthentication attacks** using various methods
- **Denial of Service attacks**
- Formatting captured traffic into **PCAP format**
- Parsing captured handshakes into **HCCAPX file** ready to be cracked by Hashcat
- Passive handshake sniffing
- Easily extensible framework for new attacks implementations
- Management AP for easy configuration on the go using smartphone for example
- And more...

## Usage
1. [Build] and [flash] project onto ESP32 (DevKit or module)
1. Power ESP32
1. Management AP is started automatically after boot
1. Connect to this AP\
By default: 
*SSID:* `ManagementAP` and *password:* `mgmtadmin`
1. In browser open `192.168.4.1` and you should see a web client to configure and control tool like this:

## Build

Project can be built in the usual ESP-IDF way:

```shell
idf.py build
```

Legacy method using `make` is not supported by this project.

## Flash
If you have setup ESP-IDF, the easiest way is to use `idf.py flash`.

In case you don't want to setup whole ESP-IDF, you can use pre-build binaries included in [`build/`](build/) and flash them using [`esptool.py`](https://github.com/espressif/esptool) (requires Python).

Example command (follow instructions in [esptool repo](https://github.com/espressif/esptool)):
```
esptool.py -p /dev/ttyS5 -b 115200 --after hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size detect 0x8000 build/partition_table/partition-table.bin 0x1000 build/bootloader/bootloader.bin 0x10000 build/tantalo.bin
```

On Windows you can use official [Flash Download Tool](https://www.espressif.com/en/support/download/other-tools).

### Components
This project consists of multiple components, that can be reused in other projects. Each component has it's own README with detailed description. Here comes brief description of components:

- [**Main**]component is entry point for this project. All neccessary initialisation steps are done here. Management AP is started and the control is handed to webserver.
- [**Wifi Controller**]component wraps all Wi-Fi related operations. It's used to start AP, connect as STA, scan nearby APs etc. 
- [**Webserver**]component provides web UI to configure attacks. It expects that AP is started and no additional security features like SSL encryption are enabled.
- [**Wi-Fi Stack Libraries Bypasser**]component bypasses Wi-Fi Stack Libraries restriction to send some types of arbitrary 802.11 frames.
- [**Frame Analyzer**] component processes captured frames and provides parsing functionality to other components.
- [**PCAP Serializer**] component serializes captured frames into PCAP binary format and provides it to other components (mostly for webserver/UI)
- [**HCCAPX Serializer**] component serializes captured frames into HCCAPX binary format and provides it to other components (mostly for webserver/UI)


## Contributing
Feel free to contribute. Don't hestitate to refactor current code base. Please stick to Doxygen notation when commenting new functions and files. This project is mainly build for educational and demonstration purposes, so verbose documentation is welcome.

## Disclaimer
This project demonstrates vulnerabilities of Wi-Fi networks and its underlaying 802.11 standard and how ESP32 platform can be utilised to attack on those vulnerable spots. Use responsibly against networks you have permission to attack on.
