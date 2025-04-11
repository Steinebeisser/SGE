**Little Endian**<br>
# SGE SCENE File format

## Overview

## Table of Content

1. [Header](#header)
2. [Section Header](#section-header)
3. [Section Types](#section-types) <br>
   3.1 [SGEREND section](#sgerend-section) <br>
    3.1.1 [SGEREND section data](#section-data)
4. [Extensions](#extensions)

## Header
48 Bytes excluding variable sizes <br>
Total Size is calculated using 48(0x30) + S + A + D + E <br>
S = Scene Name Size <br>
A = Author Name Size <br>
D = Description Size <br>
E = Extension Data Size <br>

| Offset               | Size    | Name                         | Description                       |
|:---------------------|:--------|:-----------------------------|:----------------------------------|
| 0x00                 | 8 Bytes | Magic Number                 | "SGESCNE\0"                       |
| 0x08                 | 2 Bytes | Major version                | 16 bit                            |
| 0x0A                 | 2 Bytes | Minor version                | 16 bit                            |
| 0x0C                 | 2 Bytes | Patch                        | 16 bit                            |
| 0x0E                 | 4 Bytes | Section Count                | 16 Bit                            |
| 0x12                 | 4 Bytes | Scene Name Size              | Populates `S Bytes`               |
| 0x16                 | S Bytes | Scene Name                   | Name to identify, Null Terminated |
| 0x16 + S             | 4 Bytes | Author Name Size             | populates `A Bytes`               |
| 0x1A + S             | A Bytes | Author Name                  | Null Terminated                   |
| 0x1A + S + A         | 8 Bytes | Creation Date Timestamp      | Unix Timestamp                    |
| 0x22 + S + A         | 8 Bytes | Last Modified Date Timestamp | Unix Timestamp                    |
| 0x2A + S + A         | 4 Bytes | Description Size             | Populates `D Bytes`               |
| 0x2E + S + A         | D Bytes | Description                  | Null Terminated                   |
| 0x2E + S + A + D     | 2 Bytes | Header Extension Count       | Extension Count                   |
| 0x30 + S + A + D     | E Bytes | Header Extension Data        | Extension Data                    |
| 0x30 + S + A + D + E | 4 Bytes | Header Checksum              | CRC32 Checksum "0xEDB88320"       |

---

## Section Header
40 Bytes excluding variable sizes <br>
S = Scene Name Size <br>
E = Extension Data Size <br>

| Offset       | Size    | Name                         | Description                    |
|:-------------|:--------|:-----------------------------|:-------------------------------|
| 0x00         | 2 Bytes | Section Type                 | sgerend, light, ...            |
| 0x02         | 8 Bytes | Section Offset               | File offset of section         |
| 0x0A         | 8 Bytes | Data Size                    | Size of Section without header |
| 0x12         | 4 Bytes | Scene Name Size              | Populates `S Bytes`            |
| 0x16         | S Bytes | Section Name                 | Name to identify               |
| 0x16 + S     | 8 Bytes | Creation Date Timestamp      | Unix Timestamp                 |
| 0x1E + S     | 8 Bytes | Last Modified Date Timestamp | Unix Timestamp                 |
| 0x26 + S     | 2 Bytes | Section Extension Count      | Extension Count                |
| 0x28 + S     | E Bytes | Section Extension Data       | Extension Data                 |
| 0x28 + S + E | 4 Bytes | Section Checksum             | CRC32 Checksum                 |

---

## Section Types

| Value | Type     | Description    |
|:------|:---------|:---------------|
| 1     | sgerend  | sgerend object |
| 2     | light    | light          |

---

### SGEREND section

#### Section Data

SSS     = SGEREND source size   = if (include type = 0) { 2 Bytes } else { 4 Bytes }            <br>
SS      = SGEREND source        = Size of SGEREND source                                        <br>
SDS     = Section Data size     = if (addition section count > 0 ) { 4 Byte} else { 0 Byte }    <br>
SD      = Section Data          = Size of Section Data                                          <br>


| Offset                     | Size                        | Name                     | Description                                                              |
|:---------------------------|:----------------------------|:-------------------------|:-------------------------------------------------------------------------|
| +0x00                      | 1 Byte                      | Include Type             | 0 for external file, 1 for embedded                                      |
| 0x01                       | 2 Byte                      | Additional Section Count | If loading SGEREND file option to add section types                      |
| 0x03                       | 2 Byte / 4 Byte if embedded | SGEREND source size      | If the Include Type is embedded increase the Size to 4 Bytes, normally 2 |
| 0x03 + SSS                 | S Bytes                     | SGEREND Source           | Path to SGEREND file or full Data                                        |
| 0x03 + SSS + SS            | if sec count > 0, 4 Byte    | Section Data Size        | Fills up `SD Bytes` and `SDS Bytes`                                      |
| 0x03 + SSS + SS + SDS      | SD Bytes                    | Section Data             |                                                                          |
| 0x03 + SSS + SS + SDS + SD | 2 byte                      | Transformation Flags     | Flags of [transformation flags](#transformation-flags)                   |
| 0x05 + SSS + SS + SDS + SD | Variable                    | Transformations Data     | Data based on flags                                                      |

#### Transformation Flags

| Bit | Type     |
|:----|:---------|
| 1   | Position |
| 2   | Scale    |
| 3   | Rotation |

#### Transformation Data 

| Offset | Size          | Name | Description                                       |
|:-------|:--------------|:-----|:--------------------------------------------------|
| +0x00  | 1 Byte        | Type | Transformation Type based on flag                 |
| 0x01   | Based on Type | Data | Check [Transformation Size](#transformation-size) |

##### Transformation Size

| Type | Format            | Size    | Description |
|:-----|:------------------|:--------|:------------|
| 1    | 3x float32 (vec3) | 12 Byte | Position    |
| 2    | 3x float32 (vec3) | 12 Byte | Scale       |
| 3    | 3x float32 (vec3) | 12 byte | Rotation    |
## Extensions

| Value | Type      | Description                     |
|:------|:----------|:--------------------------------|
| 1     | Animation | Only Viable for SGEREND section |