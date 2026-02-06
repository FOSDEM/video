/* 
* i2c_port_address_scanner
* Scans ports D0 to D7 on an ESP8266 and searches for I2C device. based on the original code
* available on Arduino.cc and later improved by user Krodal and Nick Gammon (www.gammon.com.au/forum/?id=10896)
* D8 throws exceptions thus it has been left out
*
*/

#include <Wire.h>

#define PIN_SDA 0
#define PIN_SCK 2


/* Registers from the switch datasheet for MII access */
#define MII_BMCR 0
#define MII_BMSR 1
#define MII_PHY1 2
#define MII_PHY2 3
#define MII_AN 4
#define MII_LPA 5
#define MII_ANE 6
#define MII_ANPT 7
#define MII_ANPN 8
#define MII_GBEC 9
#define MII_GBES 10
#define MII_EXTS 15

// BMCR register
#define MII_BMCR_RESET (1<<15)
#define MII_BMCR_LOOPBACK (1<<14)
#define MII_BMCR_SPEED_SELECT1 (1<<13)
#define MII_BMCR_AUTONEG_ENABLE (1<<12)
#define MII_BMCR_POWER_DOWN (1<<11)
#define MII_BMCR_ISOLATE (1<<10)
#define MII_BMCR_AUTONEG_RESTART (1<<9)
#define MII_BMCR_DUPLEX_MODE (1<<8)
#define MII_BMCR_COLLISION_TEST (1<<7)
#define MII_BMCR_SPEED_SELECT2 (1<<6)

// BMSR register
#define MII_BMSR_100BaseT4 (1<<15)
#define MII_BMSR_100BaseTXFD (1<<14)
#define MII_BMSR_100BaseTXHD (1<<13)
#define MII_BMSR_10BaseTFD (1<<12)
#define MII_BMSR_10BaseTHD (1<<11)
#define MII_BMSR_100BaseT2FD (1<<10)
#define MII_BMSR_100BaseT2HD (1<<9)
#define MII_BMSR_EXTENDED_STATUS (1<<8)
#define MII_BMSR_PREAMBLE_SUPRESS (1<<6)
#define MII_BMSR_AUTONEG_COMPLETE (1<<5)
#define MII_BMSR_REMOTE_FAULT (1<<4)
#define MII_BMSR_AUTONEG_CAP (1<<3)
#define MII_BMSR_LINK_STATUS (1<<2)
#define MII_BMSR_JABBER_DETECT (1<<1)
#define MII_BMSR_EXTENDED_CAP (1<<0)

// AN register
#define MII_AN_NEXT_PAGE (1<<15)
#define MII_AN_ACK (1<<14)
#define MII_AN_REMOTE_FAULT (1<<13)
#define MII_AN_PAUSE (1<<10)
#define MII_AN_100BaseT4 (1<<9)
#define MII_AN_100BaseTXFD (1<<8)
#define MII_AN_100BaseTXHD (1<<7)
#define MII_AN_10BaseTFD (1<<6)
#define MII_AN_10BaseTHD (1<<5)

// LPA register
#define MII_LPA_NEXT_PAGE (1<<15)
#define MII_LPA_ACK (1<<14)
#define MII_LPA_REMOTE_FAULT (1<<13)
#define MII_LPA_ASYM_PAUSE (1<<11)
#define MII_LPA_PAUSE (1<<10)
#define MII_LPA_100BaseT4 (1<<9)
#define MII_LPA_100BaseTXFD (1<<8)
#define MII_LPA_100BaseTXHD (1<<7)
#define MII_LPA_10BaseTFD (1<<6)
#define MII_LPA_10BaseTHD (1<<5)

// GBEC register
#define MII_GBEC_TEST_MODE (1<<15)
#define MII_GBEC_MASTER_MANUAL (1<<12)
#define MII_GBEC_MASTER (1<<11)
#define MII_GBEC_PORT_TYPE (1<<10)
#define MII_GBEC_1000BaseTFD (1<<9)
#define MII_GBEC_1000BaseTHD (1<<8)

// GBES register
#define MII_GBES_MASTER_FAULT (1<<15)
#define MII_GBES_MASTER (1<<14)
#define MII_GBES_LOCAL_STATUS (1<<13)
#define MII_GBES_REMOTE_STATUS (1<<12)
#define MII_GBES_1000BaseTFD (1<<11)
#define MII_GBES_1000BaseTHD (1<<10)
#define MII_GBES_IDLE_ERROR_COUNT (127)


/* Registers from the rtl8365mb linux drivers */
#define REG_CHIP_ID 0x1300
#define REG_CHIP_VER 0x1301
#define REG_MAGIC 0x13c2
#define REG_MAGIC_VAL 0x0249
#define REG_PHY_BASE 0x2000
#define REG_PHY_OCP_ADDR_PHYREG_BASE 0xA400

/* Code from https://github.com/McMCCRU/Arduino_RTL8367C/blob/master/rtl8367c.ino for SMI access*/

#define CLK_DURATION(clk)	delayMicroseconds(clk)
#define DELAY			3
#define ack_timer		10

/* smi */
void _smi_start()
{
	/* change GPIO pin to Output only */
	pinMode(PIN_SCK, OUTPUT);
	pinMode(PIN_SDA, OUTPUT);

	/* Initial state: SCK: 0, SDA: 1 */
	digitalWrite(PIN_SCK, LOW);
	digitalWrite(PIN_SDA, HIGH);
	CLK_DURATION(DELAY);

	/* CLK 1: 0 -> 1, 1 -> 0 */
	digitalWrite(PIN_SCK, HIGH);
	CLK_DURATION(DELAY);
	digitalWrite(PIN_SCK, LOW);
	CLK_DURATION(DELAY);

	/* CLK 2: */
	digitalWrite(PIN_SCK, HIGH);
	CLK_DURATION(DELAY);
	digitalWrite(PIN_SDA, LOW);
	CLK_DURATION(DELAY);
	digitalWrite(PIN_SCK, LOW);
	CLK_DURATION(DELAY);
	digitalWrite(PIN_SDA, HIGH);
}

void _smi_writeBit(uint16_t signal, uint32_t bitLen)
{
	for ( ; bitLen > 0; bitLen--) {
		CLK_DURATION(DELAY);

		/* prepare data */
		if (signal & (1UL << (bitLen - 1)))
			digitalWrite(PIN_SDA, HIGH);
		else
			digitalWrite(PIN_SDA, LOW);
		CLK_DURATION(DELAY);

		/* clocking */
		digitalWrite(PIN_SCK, HIGH);
		CLK_DURATION(DELAY);
		digitalWrite(PIN_SCK, LOW);
	}
}

void _smi_readBit(uint32_t bitLen, uint32_t *rData)
{
	unsigned long u;

	/* change GPIO pin to Input only */
	pinMode(PIN_SDA, INPUT);

	for (*rData = 0; bitLen > 0; bitLen--) {
		CLK_DURATION(DELAY);

		/* clocking */
		digitalWrite(PIN_SCK, HIGH);
		CLK_DURATION(DELAY);
		u = (digitalRead(PIN_SDA) == HIGH) ? 1 : 0;
		digitalWrite(PIN_SCK, LOW);

		*rData |= (u << (bitLen - 1));
	}

	/* change GPIO pin to Output only */
	pinMode(PIN_SDA, OUTPUT);
}

void _smi_stop()
{
	CLK_DURATION(DELAY);
	digitalWrite(PIN_SDA, LOW);
	digitalWrite(PIN_SCK, HIGH);
	CLK_DURATION(DELAY);
	digitalWrite(PIN_SDA, HIGH);
	CLK_DURATION(DELAY);
	digitalWrite(PIN_SCK, HIGH);
	CLK_DURATION(DELAY);
	digitalWrite(PIN_SCK, LOW);
	CLK_DURATION(DELAY);
	digitalWrite(PIN_SCK, HIGH);

	/* add a click */
	CLK_DURATION(DELAY);
	digitalWrite(PIN_SCK, LOW);
	CLK_DURATION(DELAY);
	digitalWrite(PIN_SCK, HIGH);

	/* change GPIO pin to Input only */
	pinMode(PIN_SDA, INPUT);
	pinMode(PIN_SCK, INPUT);
}

uint32_t smi_read(uint32_t mAddrs)
{
	uint32_t rawData = 0, rData = 0, ACK = 0;
	uint8_t con;

	_smi_start();				/* Start SMI */
	_smi_writeBit(0x0b, 4);			/* CTRL code: 4'b1011 for RTL8370 */
	_smi_writeBit(0x4, 3);			/* CTRL code: 3'b100 */
	_smi_writeBit(0x1, 1);			/* 1: issue READ command */

	con = 0;
	do {
		con++;
		_smi_readBit(1, &ACK);		/* ACK for issuing READ command */
	} while ((ACK != 0) && (con < ack_timer));

	if (ACK != 0) Serial.println(F("smi_read_0...timeout!"));

	_smi_writeBit((mAddrs & 0xff), 8);	/* Set reg_addr[7:0] */

	con = 0;
	do {
		con++;
		_smi_readBit(1, &ACK);		/* ACK for setting reg_addr[7:0] */
	} while ((ACK != 0) && (con < ack_timer));

	if (ACK != 0) Serial.println(F("smi_read_1...timeout!"));

	_smi_writeBit((mAddrs >> 8), 8);	/* Set reg_addr[15:8] */

	con = 0;
	do {
		con++;
		_smi_readBit(1, &ACK);		/* ACK by RTL836x */
	} while ((ACK != 0) && (con < ack_timer));

	if (ACK != 0) Serial.println(F("smi_read_2...timeout!"));

	_smi_readBit(8, &rawData);		/* Read DATA [7:0] */
	rData = rawData & 0xff;

	_smi_writeBit(0x00, 1);			/* ACK by CPU */
	_smi_readBit(8, &rawData);		/* Read DATA [15: 8] */
	_smi_writeBit(0x01, 1);			/* ACK by CPU */

	rData |= (rawData << 8);

	_smi_stop();

	return rData;
}

void smi_write(uint32_t mAddrs, uint32_t rData)
{
	int8_t con;
	uint32_t ACK = 0;

	_smi_start();				/* Start SMI */
	_smi_writeBit(0x0b, 4);			/* CTRL code: 4'b1011 for RTL836x */
	_smi_writeBit(0x4, 3);			/* CTRL code: 3'b100 */
	_smi_writeBit(0x0, 1);			/* 0: issue WRITE command */

	con = 0;
	do {
		con++;
		_smi_readBit(1, &ACK);		/* ACK for issuing WRITE command */
	} while ((ACK != 0) && (con < ack_timer));

	if (ACK != 0) Serial.println(F("smi_write_0...timeout!"));

	_smi_writeBit((mAddrs & 0xff), 8);	/* Set reg_addr[7:0] */

	con = 0;
	do {
		con++;
		_smi_readBit(1, &ACK);		/* ACK for setting reg_addr[7:0] */
	} while ((ACK != 0) && (con < ack_timer));

	if (ACK != 0) Serial.println(F("smi_write_1...timeout!"));

	_smi_writeBit((mAddrs >> 8), 8);	/* Set reg_addr[15:8] */

	con = 0;
	do {
		con++;
		_smi_readBit(1, &ACK);		/* ACK or setting reg_addr[15:8] */
	} while ((ACK != 0) && (con < ack_timer));

	if (ACK != 0) Serial.println(F("smi_write_2...timeout!"));

	_smi_writeBit(rData & 0xff, 8);		/* Write Data [7:0] out */

	con = 0;
	do {
		con++;
		_smi_readBit(1, &ACK);		/* ACK for writing data [7:0] */
	} while ((ACK != 0) && (con < ack_timer));

	if (ACK != 0) Serial.println(F("smi_write_3...timeout!"));

	_smi_writeBit(rData >> 8, 8);		/* Write Data [15:8] out */

	con = 0;
	do {
		con++;
		_smi_readBit(1, &ACK);		/* ACK for writing data [15:8] */
	} while ((ACK != 0) && (con < ack_timer));

	if (ACK != 0) Serial.println(F("smi_write_4...timeout!"));

	_smi_stop();
}
/* End smi */

uint32_t smi_read_port(int port, int reg) {
  uint32_t offset = REG_PHY_BASE + (32*port) + reg;

  // write address
  smi_write(0x1f02, offset);

  // Send read command
  smi_write(0x1f00, 1 );

  // Read result
  return smi_read(0x1f04);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  delayMicroseconds(500);
  setvbuf(stdout, NULL, _IONBF, 0);
  Serial.print("\n\n");

  // https://github.com/torvalds/linux/blob/7e90b5c295ec1e47c8ad865429f046970c549a66/drivers/net/dsa/realtek/rtl8365mb.c#L2045
  smi_write(REG_MAGIC, REG_MAGIC_VAL);
  uint32_t id = smi_read(REG_CHIP_ID);
  uint32_t ver = smi_read(REG_CHIP_VER);
  printf("Chip: RTL%x version %d\n", id, ver);
  smi_write(REG_MAGIC, 0);

  for(int port = 0; port < 5; port++) {
    printf("Port %d: ", port);
    uint32_t bmcr = smi_read_port(port, MII_BMCR); // Basic mode control register
    uint32_t bmsr = smi_read_port(port, MII_BMSR); // Basic mode status register
    uint32_t an = smi_read_port(port, MII_AN); // Auto negotiation settings
    uint32_t lpa  = smi_read_port(port, MII_LPA); // Link partner ability
    uint32_t gbec = smi_read_port(port, MII_GBEC);
    uint32_t gbes = smi_read_port(port, MII_GBES);

    bool link = true;
    int speed = 0;
    bool fd = false;
    // https://github.com/torvalds/linux/blob/master/drivers/net/mii.c
    if(bmcr & MII_BMCR_AUTONEG_ENABLE) {
      // Auto negotiation is enabled on the interface
      if(bmsr & MII_BMSR_AUTONEG_COMPLETE) {
        // Auto negotiaten complete

        if ((gbes & MII_GBES_1000BaseTFD) && (gbec & MII_GBEC_1000BaseTFD)) {
          fd = true;
          speed = 1000;
        } else if ((gbes & MII_GBES_1000BaseTHD) && (gbec & MII_GBEC_1000BaseTHD)) {
          fd = false;
          speed = 1000;
        } else if ( (lpa & MII_LPA_100BaseTXFD) && (an & MII_AN_100BaseTXFD )) {
          fd = true;
          speed = 100;
        } else if ((lpa & MII_LPA_100BaseTXHD) && (an & MII_AN_100BaseTXHD)) {
          fd = false;
          speed = 100;
        } else if ((lpa & MII_LPA_10BaseTFD) && (an & MII_AN_10BaseTFD)) {
          fd = true;
          speed = 10;
        } else if ((lpa & MII_LPA_10BaseTHD) && (an & MII_AN_10BaseTHD)) {
          fd = false;
          speed = 10;
        }

      } else {
        // Auto negotiation failed
        link = false;        
      }
    }

    if(!link) {
      printf("No link\n");
      continue;      
    } else {
      printf("%d Mbps ", speed);
      if(fd) {
        printf("Full-duplex\n");
      } else {
        printf("Half-duplex\n");
      }
    }
  }

}

void loop() {
}
