#include <string>
#include <algorithm>
#include <filesystem>

#ifdef WIN32

#include <codecvt>

#include "windows.h"
#include "winnls.h"
#include "shobjidl.h"
#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"
#include "shobjidl.h"
#include "shellapi.h"
#include <Windows.h>
#include <fstream>
#include <shlobj.h>
#include <KnownFolders.h>
#include <cwchar>


/*============================================================================*/

// CreateLink - Uses the Shell's IShellLink and IPersistFile interfaces
//              to create and store a shortcut to the specified object.
//
// Returns the result of calling the member functions of the interfaces.
//
// Parameters:
// lpszPathObj  - Address of a buffer that contains the path of the object,
//                including the file name.
// lpszPathLink - Address of a buffer that contains the path where the
//                Shell link is to be stored, including the file name.
// lpszDesc     - Address of a buffer that contains a description of the
//                Shell link, stored in the Comment field of the link
//                properties.
//lpszPicture   - picture of shortcut

HRESULT CreateLink(LPCSTR lpszPathObj,
                   LPCWSTR lpszPathLinkDir,
                   LPCSTR lpszDesc,
                   LPCSTR lpszPicture,
                   LPCWSTR name) {
    // Initialize COM for this thread (The shell link is typically STA-friendly)
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    const bool com_inited = SUCCEEDED(hr);

    if (!com_inited && hr != RPC_E_CHANGED_MODE) {
        // COM init failed for real (RPC_E_CHANGED_MODE means COM already inited with different model)
        return hr;
    }

    IShellLinkW *psl = nullptr;
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                          IID_IShellLinkW, (void **) &psl);
    if (SUCCEEDED(hr)) {
        // Prefer wide APIs where possible
        psl->SetPath(std::wstring(lpszPathObj, lpszPathObj + strlen(lpszPathObj)).c_str()); // minimal; see note below
        psl->SetDescription(std::wstring(lpszDesc, lpszDesc + strlen(lpszDesc)).c_str());
        psl->SetIconLocation(std::wstring(lpszPicture, lpszPicture + strlen(lpszPicture)).c_str(), 0);

        IPersistFile *ppf = nullptr;
        hr = psl->QueryInterface(IID_IPersistFile, (void **) &ppf);
        if (SUCCEEDED(hr)) {
            WCHAR wsz[MAX_PATH]{};

            hr = _wmakepath_s(wsz, _MAX_PATH, NULL, lpszPathLinkDir, name, L"lnk");
            if (SUCCEEDED(hr))
                hr = ppf->Save(wsz, TRUE);

            ppf->Release();
        }
        psl->Release();
    }

    if (com_inited) CoUninitialize();
    return hr;
}

//destination:
// 0 - FOLDERID_Programs
// 1 - FOLDERID_Desktop
void shortcut(const std::string &filePath,
              const std::string &picturePath,
              const std::string &name,
              std::size_t destination) {
    // if you need to mutate slashes, make local copies:
    std::string fp = filePath;
    std::string pp = picturePath;

    std::replace(pp.begin(), pp.end(), '/', '\\');
    std::replace(fp.begin(), fp.end(), '/', '\\');

    wchar_t *p = nullptr;
    std::wstring location;
    if (destination == 0) {
        HRESULT hres = SHGetKnownFolderPath(FOLDERID_Programs, 0, NULL, &p); //FOLDERID_Programs
        location = p;
        location += L"\\MinGW-W64\\"; ///dir for shortcut
        std::wstring path = location;
        SHCreateDirectory(nullptr, path.c_str());
    } else if (destination == 1) {
        HRESULT hres = SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &p);
        location = p;
    }
    CoTaskMemFree(p);
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    std::string l = converter.to_bytes(location.c_str());

    std::wstring n{name.begin(), name.end()};

    // std::replace(picturePath.begin(), picturePath.end(), '/', '\\');
    // std::replace(filePath.begin(), filePath.end(), '/', '\\');

    printf("\n\nWanna to create shortcut for:  %s,\n to:  %s,\n  picture: %s,\n\n",
           filePath.c_str(), l.c_str(), picturePath.c_str());

    CreateLink(filePath.c_str(), location.c_str(), "Compiler", picturePath.c_str(), n.c_str());
}

std::size_t directory_delete(const std::string &pathname) {
    std::string p = pathname;
    p.append(2, '\0'); // double-null terminate for SHFileOperation

    SHFILEOPSTRUCTA file{};
    file.wFunc = FO_DELETE;
    file.pFrom = p.c_str();
    file.fFlags = FOF_NO_UI | FOF_NOCONFIRMATION;

    return static_cast<std::size_t>(SHFileOperationA(&file));
}

void showError(const char *error) {
    printf("%s", error);

    int msgboxID = MessageBox(NULL, "Check internet conection. Details?", "Problem", MB_YESNO);

    if (msgboxID == IDYES) {
        MessageBox(
            NULL,
            error,
            "Problem",
            MB_OK
        );
    }
}

#else

//destination:
// 0 - FOLDERID_Programs
// 1 - FOLDERID_Desktop
void shortcut(const std::string &filePath, const std::string &picturePath, const std::string &name,
              std::size_t destination) {
    printf("\n\nWanna to create shortcut for:  %s,\n to:  ?,\n  picture: %sn\n",
           filePath.c_str(), picturePath.c_str());
}

std::size_t directory_delete(const std::string &pathname) {
    return std::filesystem::remove_all(pathname);
}

void showError(const char *error) {
    printf("%s", error);
}
#endif
