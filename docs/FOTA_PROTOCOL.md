# FOTA Protocol

## UART: ESP32 Telematic to STM32 Gateway

Default UART: USART1 at 115200 bps, 8N1.

| Direction | Byte | Meaning |
| --- | ---: | --- |
| ESP32 -> Gateway | `0x01` | New update request |
| Gateway -> ESP32 | `0x05` | Request accepted |
| Gateway -> ESP32 | `0x06` | Request refused |
| Gateway -> ESP32 | `0x07` | Send firmware header |
| Gateway -> ESP32 | `0x08` | Header received |
| Gateway -> ESP32 | `0x0C` | Send next firmware packet |
| Gateway -> ESP32 | `0x0A` | Packet received |
| Gateway -> ESP32 | `0x0B` | Last packet received |
| Gateway -> ESP32 | `0x0D` | Download done |

Header format from ESP32 to gateway:

```text
byte 0..3: firmware size, little endian
byte 4   : target node id
```

Firmware data is currently transferred to the gateway in 1024-byte UART packets and staged at `0x0800A800`.

## CAN: STM32 Gateway to STM32 Target Bootloader

Default CAN bus speed: 500 kbps.

The gateway sends UDS-like single-byte service markers, then sends data frames.

| Gateway -> Target | Target -> Gateway | Step |
| ---: | ---: | --- |
| `0x10` | `0x50` | Enter programming/update session |
| `0x34` | `0x73` | Request header transfer |
| 8-byte header | `0x74` | Transfer image size and CRC |
| `0x36` | `0x75` | Request packet transfer |
| 16-byte encrypted block | `0x76` | Transfer encrypted firmware block |
| `0x37` | `0x77` | Finish update |

Header format from gateway to target:

```text
byte 0..3: firmware size, little endian
byte 4..7: firmware CRC, little endian
```

## Security and Integrity

- Payload encryption: AES-128-CBC.
- Integrity check: STM32 hardware CRC over the application image.
- Current limitation: AES key and IV are hard-coded in firmware. This is acceptable only for lab/demo usage.

## Required Production Hardening

- Add transfer sequence numbers and reject duplicate/out-of-order packets.
- Add timeout and retry policy for every blocking UART/CAN wait.
- Authenticate the firmware metadata, not only the payload CRC.
- Store key material outside normal source code and rotate IV per image.
- Persist update failure reason in the flag page or a diagnostics page.
