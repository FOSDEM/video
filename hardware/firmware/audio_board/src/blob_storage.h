#pragma once
#include <cstdint>

#include "config.h"

struct __attribute__((packed)) EepromBlob {
	char    buf[BLOB_SIZE];
	uint8_t checksum;
};

class BlobStorage {
	public:
		BlobStorage();

		bool set(const char* buf);

		const char* get() const;

		bool EepromSave();
		bool EepromLoad();

	private:
		EepromBlob blob;

		static int instances;
		int        instanceId;
		bool       eepromDirty;
};
