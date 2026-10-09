#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cstdlib>

// Minimal UTF-8 TSV reader: first line is the header, '#' lines are comments.
struct TsvTable {
    std::vector<std::string> header;
    std::vector<std::vector<std::string>> rows;
    std::unordered_map<std::string, int> col;

    static std::vector<std::string> split(const std::string& line, char sep) {
        std::vector<std::string> out;
        std::string cur;
        for (char c : line) {
            if (c == sep) { out.push_back(cur); cur.clear(); }
            else if (c != '\r') cur.push_back(c);
        }
        out.push_back(cur);
        return out;
    }

    bool load(const std::string& path, std::string* err) {
        std::ifstream f(path, std::ios::binary);
        if (!f) { if (err) *err = "cannot open " + path; return false; }
        std::string line;
        if (!std::getline(f, line)) { if (err) *err = "empty " + path; return false; }
        if (line.size() >= 3 && (unsigned char)line[0] == 0xEF) line = line.substr(3);
        header = split(line, '\t');
        for (int i = 0; i < (int)header.size(); ++i) col[header[i]] = i;
        while (std::getline(f, line)) {
            if (line.empty() || line[0] == '#' || line == "\r") continue;
            rows.push_back(split(line, '\t'));
        }
        return true;
    }
    std::string s(size_t r, const char* key) const {
        auto it = col.find(key);
        if (it == col.end() || it->second >= (int)rows[r].size()) return {};
        const std::string& v = rows[r][it->second];
        return v == "-" ? std::string() : v;
    }
    float f(size_t r, const char* key, float def = 0) const {
        std::string v = s(r, key);
        return v.empty() ? def : (float)std::atof(v.c_str());
    }
    int i(size_t r, const char* key, int def = 0) const {
        std::string v = s(r, key);
        return v.empty() ? def : std::atoi(v.c_str());
    }
};
