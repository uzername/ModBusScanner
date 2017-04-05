==ModBus Functional Software==

Read data from registers of device, which is connected over RS232 line to RS485 line. Scans data from a defined set of registers in realtime mode. 

User adds registers to lookup table, connects to device, sets timer's timeout. General Timer's timeout defines the time of scanning the whole table of registers. Avoid setting too low time between scanning cycles.

Contains a table with selection of registers which are being monitored. They are being rescanned after a given time limit.

Program was created using QT 5.8, works on Windows, and utilizes QModBusDevice library (QSerialBus).

The program works like this: in cycle we send queries to modbus device, using sendReadRequest standard routine. This query creates a new thread. The program waits for response in an asynchronous manner. A valid response contains address of datastructure, its type and expected value.

Typically sendReadRequest fires up almost immediately. It takes more time to see the response. Once again, due to multithreading nature of sending read request routine, it cannot be strictly predicted when response comes. Adding sleep time between queries probably improves stability. Avoid setting this timeout too high (no bigger than 100ms). If this timeout is 0 then it is not used (special flag is being set). An extra MULTITHREADING FUN comes when this timer is not used (or its value is less than estimated RTT for your RS232 connection) and General Timer's value is low.

===ASCOM===

Device settings: Address::1; Speed::2; Odd::0
Settings on PC: COM9, 9600 baudrate, No Parity

LGPL v3

Grab the release: https://www.dropbox.com/s/33zjue7gdrtcl0b/ModbusListenerFinalBuild.rar?dl=1