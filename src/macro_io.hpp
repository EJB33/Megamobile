// MegaMobile by EJB3 - macro files and clipboard.
// Macros are saved as plain text in the mod's save folder, inside "macros", with the extension .mmacro.
#pragma once

#include <Geode/Geode.hpp>

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>
#include <vector>

#include "state.hpp"

namespace mm {

    inline std::filesystem::path macrosDir() {
        return geode::Mod::get()->getSaveDir() / "macros";
    }

    // File names (not full paths) of the saved macros, sorted.
    inline std::vector<std::string> listMacroFiles() {
        std::vector<std::string> out;
        std::error_code ec;
        for (auto const& entry : std::filesystem::directory_iterator(macrosDir(), ec)) {
            std::error_code ec2;
            if (entry.is_regular_file(ec2) && entry.path().extension() == ".mmacro") {
                out.push_back(entry.path().filename().string());
            }
        }
        std::sort(out.begin(), out.end());
        return out;
    }

    // macro_20261104_203015.mmacro
    inline std::string newMacroFileName() {
        const std::time_t now = std::time(nullptr);
        char buf[48] = {0};
        std::strftime(buf, sizeof(buf), "macro_%Y%m%d_%H%M%S.mmacro", std::localtime(&now));
        return buf;
    }

    inline bool saveMacroFile(std::string const& name, std::string const& text) {
        std::error_code ec;
        std::filesystem::create_directories(macrosDir(), ec);
        std::ofstream out(macrosDir() / name, std::ios::trunc);
        if (!out) return false;
        out << text;
        return out.good();
    }

    inline bool readMacroFile(std::string const& name, std::string& text) {
        std::ifstream in(macrosDir() / name);
        if (!in) return false;
        text.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        return true;
    }

} // namespace mm
