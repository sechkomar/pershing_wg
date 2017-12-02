#pragma once
#include <inttypes.h>

#include "common.h"


enum class Result : uint32_t {
	OKEY = 0,
	BAD_COMMAND = 1,
	RESOURCE_NOT_FOUND = 2,
	PATH_NOT_FOUND = 3,
	ACCESS_DENIED = 5,
	NO_RESULT
};

struct ResponseMessage {
	Result result;
	size_t data_length;
	char* data;

	ResponseMessage() : data(nullptr) {};

	ResponseMessage(const Result &res, const size_t &len, const char* const msg) : result(res), data_length(len) {
		//if (data_length != 0) {
		//	data = new char[data_length];
		//	memcpy_s(data, data_length, msg, len);
		//}
		//else {
		//	data = nullptr;
		//}
		copy_string(this->data, len, msg);
	}

	ResponseMessage(const ResponseMessage &msg) {
		set(msg.result, msg.data_length, msg.data);
	}

	ResponseMessage & operator=(const ResponseMessage &msg) {
		set(msg.result, msg.data_length, msg.data);
		return *this;
	}

	void set(Result new_result, size_t new_data_len, const char* new_data) {
		this->result = new_result;
		this->data_length = new_data_len;

		/*if (data != nullptr) {
			delete[] data;
			data = nullptr;
		}

		if (new_data_len != 0) {
			this->data = new char[data_length + 1];
			memcpy_s(this->data, this->data_length, new_data, new_data_len);
			this->data[data_length] = '\0';
		}*/

		copy_string(this->data, new_data_len, new_data);
	}

	~ResponseMessage() {
		if (data != nullptr) {
			delete[] data;
			data = nullptr;
		}
	}
};