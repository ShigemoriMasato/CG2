#include "MyString.h"
#include <Windows.h>

namespace fs = std::filesystem;

std::vector<std::string> SearchFiles(const fs::path& directory, const std::string& extension) {
    std::vector<std::string> contents;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return {};
    }

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            fs::path relativePath = entry.path().lexically_relative(directory);
            contents.push_back(relativePath.generic_string());
        }
    }

    return contents;
}

std::vector<std::string> SearchFileAndFolderNames(const fs::path& directory) {
    std::vector<std::string> contents;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return {};
    }

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_directory()) {
            contents.push_back(entry.path().filename().generic_string());
        }
    }

    return contents;

}

std::vector<std::string> SerchFilePathsAddChild(const fs::path& directory, const std::string& extension) {
    std::vector<fs::path> filePaths;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return {};
    }

    // 再帰的にファイル収集
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && (extension.empty() || entry.path().extension() == extension)) {
            filePaths.push_back(entry.path());
        }
    }

    // ソート：ディレクトリ構造順＋辞書順
    std::sort(filePaths.begin(), filePaths.end(), [&](const fs::path& a, const fs::path& b) {
        return a.lexically_relative(directory).generic_string() < b.lexically_relative(directory).generic_string();
        });

    // 相対パスに変換
    std::vector<std::string> result;
    for (const auto& path : filePaths) {
        result.push_back(path.lexically_relative(directory).generic_string());
    }

    return result;
}

std::wstring ConvertString(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0) {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

std::string ConvertString(const std::wstring& str) {
    if (str.empty()) {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded == 0) {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}
