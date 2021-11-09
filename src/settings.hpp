#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include <string>
#include <vector>
#include <cstdint>
#include <libconfig.h++>

#include "logger.hpp"


struct Colors {
    uint32_t selection;
    uint32_t text;
    uint32_t bg;
    uint32_t cursor;
    uint32_t ui;
    uint32_t line_no;

    Colors();
};

struct CursorSettings {
    std::string shape;
    int blinkrate_ms;

    CursorSettings();
};

struct FontSettings {
    int size;
    std::string name;

    FontSettings();
};

struct LogSettings {
    int level;
};

class Settings {
public:
    static Settings& instance();
    static const Settings& const_instance() { return Settings::instance(); }

    explicit Settings(const Settings&) = delete;
    void operator=(const Settings&) = delete;

    void init(const std::string& config_path);

    Colors& colors() { return colors_; }
    const Colors& const_colors() const { return colors_; }

    FontSettings& font() { return font_settings_; }
    const FontSettings& const_font() const { return font_settings_; }

    LogSettings& log() { return log_; }
    const LogSettings& const_log() const { return log_; }

    CursorSettings& cursor() { return cursor_; }
    const CursorSettings& const_cursor() const { return cursor_; }

    std::vector<int>& rulers() { return rulers_; }
    const std::vector<int>& const_rulers() const { return rulers_; }

private:
    bool lookupColor(const libconfig::Setting& setting, const char* name, uint32_t& field);

private:
    explicit Settings();

    Colors colors_;
    FontSettings font_settings_;
    LogSettings log_;
    CursorSettings cursor_;
    std::vector<int> rulers_;

    std::string config_path_;
};


#endif // SETTINGS_HPP_
