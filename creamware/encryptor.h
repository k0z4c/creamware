#pragma once

#pragma once

#include <iostream>
#include <Windows.h>

#define AES_KEY_SIZE 16
#define IN_CHUNK_SIZE (AES_KEY_SIZE * 10) // a buffer must be a multiple of the key size
#define OUT_CHUNK_SIZE (IN_CHUNK_SIZE * 2) // an output buffer (for encryption) must be twice as big

void encryptFile(const wchar_t* filename, const wchar_t* filename2, bool isDecrypt);