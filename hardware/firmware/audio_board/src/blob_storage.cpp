#include "blob_storage.h"

#include <cstdio>
#include <cstring>

#include "debug.h"
#include "storage.h"

int BlobStorage::instances = 0;

BlobStorage::BlobStorage() {
	this->blob.buf[0] = '\0';
	this->eepromDirty = false;
	this->instanceId  = this->instances++;
}

bool BlobStorage::set(const char* buf) {
	if (strlen(buf) + 1 > sizeof(this->blob.buf)) {
		return false;
	}
	strcpy(this->blob.buf, buf);
	this->eepromDirty = true;
	return true;
}

const char* BlobStorage::get() const {
	return this->blob.buf;
}

bool BlobStorage::EepromSave() {
	if (!this->eepromDirty) {
		return false;
	}

	char fname[10];
	sprintf(fname, "blob.%d", this->instanceId);

	this->blob.checksum = 0;
	this->blob.checksum = struct_checksum(this->blob);

	if (!storage_save(fname, &this->blob, sizeof(this->blob))) {
		debug_printf("storage: issue writing\n");
		return false;
	}

	this->eepromDirty = false;
	return true;
}

bool BlobStorage::EepromLoad() {
	char fname[10];
	sprintf(fname, "blob.%d", this->instanceId);

	storage_load(fname, &this->blob, sizeof(this->blob));

	uint8_t checksum = struct_checksum(this->blob);
	if (checksum != 0) {
		debug_printf("storage: invalid checksum on index %d [%02x]\n", this->instanceId, checksum);
		this->blob.buf[0] = '\0';
		this->eepromDirty = true;
		return false;
	}

	debug_printf("blob: Restored %s\n", fname);

	return true;
}
