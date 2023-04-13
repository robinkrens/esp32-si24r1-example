#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "libsi24.h"

#define PIN_NUM_MISO 35
#define PIN_NUM_MOSI 32
#define PIN_NUM_CLK 12
#define PIN_NUM_CS 33

// #define CONFIG_SI24_SEND_MODE 1
#define CONFIG_SI24_RECV_MODE 1

static spi_device_handle_t spi;

static void spi_init()
{
	spi_bus_config_t buscfg = {
		.miso_io_num = PIN_NUM_MISO,
		.mosi_io_num = PIN_NUM_MOSI,
		.sclk_io_num = PIN_NUM_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 24,
	};

	spi_device_interface_config_t icfg = {
		.command_bits = 0,
		.address_bits = 0,
		.dummy_bits = 0,
		.mode = 0,
		.clock_speed_hz = 2000000,
		.spics_io_num = PIN_NUM_CS,
		.queue_size = 1,
	};

	spi_bus_initialize(SPI3_HOST, &buscfg, 0);
	spi_bus_add_device(SPI3_HOST, &icfg, &spi);
}

static int spi_w_r(unsigned char *data, size_t sz)
{
	unsigned len = sz * 8;
	unsigned char tmpbuf[sz];

	spi_transaction_t wr_tx = {
		.length = len,
		.rxlength = len,
		.tx_buffer = data,
		.rx_buffer = tmpbuf
	};


	spi_device_polling_transmit(spi, &wr_tx);
	memcpy(data, tmpbuf, sz);

	return 0;
}

static void ce(unsigned val)
{
	(void) val;
}

static void eh(si24_t *si, si24_event_t * e)
{
	(void) si;

	switch(e->type) {
		case EV_TX_COMPLETE:
			printf("SENT SUCCESFUL\n");
			break;
		case EV_RX_COMPLETE:
			printf("RECV COMPLETE\n");
			break;
		case EV_RX_EMPTY:
			printf("NO NEW DATA\n");
			break;
		case EV_ERR_TIMEOUT:
			printf("TIMEOUT\n");
			break;
		case EV_TX_FULL:
			printf("TX FULL\n");
			si24_reset(si);
			break;
		case EV_ERR_MAX_RETRIES:
			printf("MAX RETRIES\n");
			break;
		default:
			printf("EVENT: %x\n", e->type);
			break;
	}
}

void app_main(void)
{

	spi_init();

	unsigned char buf[] = "THIS IS A WIRELESS TEST [1]";

	si24_ioctl_t ctl = {
		.write_and_read = spi_w_r,
		.chip_enable = ce,
	};

	const si24_opts_t opts = {
#ifdef CONFIG_SI24_RECV_MODE
		.mode = RECV_MODE,
#elif CONFIG_SI24_SEND_MODE
		.mode = SEND_MODE,
#endif
		.enable_ack = 1,
		.enable_crc = 1,
		.enable_dynpd = 1,
		.crc = TWO_BYTE,
		.ioctl = &ctl,
		.speed = MBPS2,
		.txpwr = PLUS4DB,
		.payload = 5,
		.timeout = 10,
		.retries = 10,
		.mac_addr = {0xAB, 0xCD, 0xEF, 0x01, 0x02}
	};

	struct si24_t * si = si24_init(&opts, eh);

#ifdef CONFIG_SI24_RECV_MODE
	unsigned char recv_buf[32];
        while(1) {
                  int read = si24_recv(si, recv_buf, 32);
                  for (int i = 0; i < read; ++i) {
                          printf("%c", recv_buf[i]);
                  }
                  printf("\n");
                  vTaskDelay(250 / portTICK_PERIOD_MS);
	}

#elif CONFIG_SI24_SEND_MODE
	while(1) {
		int bytes_sent = si24_send(si, buf, sizeof(buf));
		while(bytes_sent != sizeof(buf)) {
			if (bytes_sent == -1)
				break;
			bytes_sent += si24_send(si, buf + bytes_sent, sizeof(buf) - bytes_sent);
			vTaskDelay(250 / portTICK_PERIOD_MS);
		}
		buf[25] += 1;
		if (buf[25] > '9')
			buf[25] = '0';
		vTaskDelay(250 / portTICK_PERIOD_MS);
	}
#endif
}
