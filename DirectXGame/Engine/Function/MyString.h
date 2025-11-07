#pragma once
#include <vector>
#include <string>
#include <filesystem>

// 指定フォルダの指定拡張子ファイルの一覧を取得
std::vector<std::string> SearchFiles(const std::filesystem::path& directory, const std::string& extension = "");

// 指定フォルダのファイル名とフォルダ名の一覧を取得（拡張子フィルタ付き）
std::vector<std::string> SearchFileAndFolderNames(const std::filesystem::path& directory);

// 指定フォルダ以下の指定拡張子ファイルの相対パス一覧を取得
std::vector<std::string> SerchFilePathsAddChild(const std::filesystem::path& directory, const std::string& extension = "");

std::wstring ConvertString(const std::string& str);

std::string ConvertString(const std::wstring& str);
//
//template <typename... Args>
//std::string printString(const std::string& format, Args&&... args) {
//    return fmt::sprintf(format, std::forward<Args>(args)...);
//}
