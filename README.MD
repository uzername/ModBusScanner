==ModBus Functional Software==
Read data from registers of device, which is connected over RS232 line to RS485 line.

Contains a table with selection of registers which are being monitored. They are being rescanned after a given time limit.

Program was created using QT 5.8, works on Windows, and utilizes QModBusDevice library (QSerialBus).

===ASCOM===

Device settings: Address::1; Speed::2; Odd::0
Settings on PC: COM9, 9600 baudrate, No Parity

LGPL v3
