
#include "SPIDevObj.hpp"

#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "OSConfig.h"
#include "DevIOCTL.h"

#include <sys/ioctl.h>
#include <linux/types.h>
#include <alloca.h>
#include <linux/spi/spidev.h>
#include <cstdlib>
#include <cstring>

using namespace DriverFramework;

#define DIR_READ    0x80
#define DIR_WRITE   0x00

SPIDevObj::~SPIDevObj() = default;

int SPIDevObj::start()
{
	spi = new mraa::Spi(0);

	spi->mode(mraa::SPI_MODE3);
	spi->bitPerWord(8);
	spi->lsbmode(0);

	spi->frequency(1000000); // default 1MHz

	return 0;
}

int SPIDevObj::stop()
{
	delete spi;
	return 0;
}


int SPIDevObj::readReg(DevHandle &h, uint8_t address, uint8_t &val)
{
	SPIDevObj *obj = DevMgr::getDevObjByHandle<SPIDevObj>(h);

	if (obj) {
		return obj->_readReg(address, val);
	}

	return -1;
}

int SPIDevObj::_readReg(uint8_t address, uint8_t &val)
{
	uint8_t tx_buf[2];
	uint8_t rx_buf[2];

	tx_buf[0] = address | DIR_READ;
	tx_buf[1] = 0x00;

	spi->transfer(tx_buf, rx_buf, 2);

	val = rx_buf[1];

	return 0;
}

int SPIDevObj::writeReg(DevHandle &h, uint8_t address, uint8_t val)
{
	SPIDevObj *obj = DevMgr::getDevObjByHandle<SPIDevObj>(h);

	if (obj) {
		return obj->_writeReg(address, val);
	}

	return -1;
}

int SPIDevObj::writeRegVerified(DevHandle &h, uint8_t address, uint8_t val)
{
	SPIDevObj *obj = DevMgr::getDevObjByHandle<SPIDevObj>(h);

	if (obj) {
		int result;
		uint8_t read_val = ~val;
		int retries = 5;

		while (retries) {
			result =  obj->_writeReg(address, val);

			if (result < 0) {
				--retries;
				continue;
			}

			result = obj->_readReg(address, read_val);

			if (result < 0 || read_val != val) {
				--retries;
				continue;
			}
		}

		if (val == read_val) {
			return 0;

		} else {
			DF_LOG_ERR("error: SPI write verify failed: %d", errno);
		}
	}

	return -1;
}

int SPIDevObj::_writeReg(uint8_t address, uint8_t val)
{
	return _writeReg(address, &val, 1);
}

int SPIDevObj::_writeReg(uint8_t address, uint8_t *in_buffer, uint16_t length)
{
	uint8_t tx_buf[length + 1];
	uint8_t rx_buf[length + 1];

	memset(tx_buf, 0, length + 1);

	tx_buf[0] = address | DIR_WRITE;

	if (in_buffer) {
		memcpy(&tx_buf[1], in_buffer, length);
	}
	spi->transfer(tx_buf, rx_buf, length + 1);

	return 0;
}

int SPIDevObj::_modifyReg(uint8_t address, uint8_t clearbits, uint8_t setbits)
{
	int ret;
	uint8_t	val;

	ret = _readReg(address, val);

	if (ret != 0) {
		return ret;
	}

	val &= ~clearbits;
	val |= setbits;

	return _writeReg(address, val);
}

int SPIDevObj::_transfer(uint8_t *write_buffer, uint8_t *read_buffer, uint8_t len)
{
	spi->transfer(write_buffer, read_buffer, len);

	return 0;
}

int SPIDevObj::bulkRead(DevHandle &h, uint8_t address, uint8_t *out_buffer, int length)
{
	SPIDevObj *obj = DevMgr::getDevObjByHandle<SPIDevObj>(h);

	if (obj) {
		return obj->_bulkRead(address, out_buffer, length);
	}

	return -1;
}

int SPIDevObj::_bulkRead(uint8_t address, uint8_t *out_buffer, int length)
{
	uint8_t tx_buf[length + 1];
	uint8_t rx_buf[length + 1];

	memset(tx_buf, 0, length + 1);
	memset(rx_buf, 0, length + 1);

	tx_buf[0] = address | DIR_READ;

	spi->transfer(tx_buf, rx_buf, length + 1);

	memcpy(out_buffer, &rx_buf[1], length);

	return 0;
}

int SPIDevObj::setLoopbackMode(DevHandle &h, bool enable)
{
	/* implement sensor interface via rpi2 spi */
	DF_LOG_ERR("ERROR: attempt to set loopback mode in software fails.");
	return -1;
}

int SPIDevObj::setBusFrequency(DevHandle &h, SPI_FREQUENCY freq_hz)
{
	/* implement sensor interface via rpi2 spi */
	SPIDevObj *obj = DevMgr::getDevObjByHandle<SPIDevObj>(h);

	if (obj) {
		return obj->_setBusFrequency(freq_hz);
	}

	return -1;
}

int SPIDevObj::_setBusFrequency(SPI_FREQUENCY freq_hz)
{
	spi->frequency(freq_hz);

	return 0;
}
