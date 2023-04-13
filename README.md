# SI24R1 example

## Getting the sources
Do not forget to recursively clone the submodule:
`git clone --recurse-submodules https://github.com/robinkrens/esp32-si24r1-example.git`


## Setup 
Tested with **ESP-IDF v4.2** on a ESP32-WROOM32. 
The libsi24 library is a submodule and used as a component in components/libsi24

You will need two EPS32s, one configured as sender, the other as receiver:

```
   +---------+
   |SI24R1   |       +---------+
   |         |       |SI24R1   |
   +----+----+       |         |
        |            +---+-----+
        | SPI            |
        |                | SPI
+-------+--+            ++---------+
|          |            |          |
|  ESP32   |            |  ESP32   |
|  (RECV)  |            | (SENDER) |
|          |            |          |
+----------+            +----------+
```

### Configuration 
Set `#define CONFIG_SI24_RECV_MODE 1` and `#define CONFIG_SI24_SEND_MODE 1`
to respectivily set as receiver or sender. 

### SPI Pinout
As default the setup is
```  
#define PIN_NUM_MISO 35
#define PIN_NUM_MOSI 32
#define PIN_NUM_CLK 12
#define PIN_NUM_CS 33
```
The chip enable is not used in the example, but could assigned by filling
out the `ce(void)` function.

## Output receiver side
This is the output on the receiver side, quickly after starting the sender.
```
NO NEW DATA

NO NEW DATA

RECV COMPLETE
THIS IS A WIRELESS TEST [1]
RECV COMPLETE
THIS IS A WIRELESS TEST [2]
RECV COMPLETE
THIS IS A WIRELESS TEST [3]
RECV COMPLETE
THIS IS A WIRELESS TEST [4]
RECV COMPLETE
THIS IS A WIRELESS TEST [5]
RECV COMPLETE
THIS IS A WIRELESS TEST [6]
RECV COMPLETE
THIS IS A WIRELESS TEST [7]
RECV COMPLETE
THIS IS A WIRELESS TEST [8]
RECV COMPLETE
THIS IS A WIRELESS TEST [9]
```

## Output sender side
Output on the sender side, quickly after starting the receiver.
```
MAX RETRIES
MAX RETRIES
SENT SUCCESFUL
SENT SUCCESFUL
SENT SUCCESFUL
SENT SUCCESFUL
SENT SUCCESFUL
SENT SUCCESFUL
SENT SUCCESFUL
SENT SUCCESFUL
SENT SUCCESFUL
SENT SUCCESFUL
SENT SUCCESFUL
```
