#include "settings.hpp"

#include "logger.hpp"

#include <libconfig.h++>

#include <limits>
#include <sstream>
#include <iostream>


Colors::Colors()
    :
    selection(0x555555FF),
    text(0xFFFFFFFF) ,
    bg(0x00000000),
    cursor(0xFFFFFFFF),
    ui(0x999999FF),
    line_no(0xBBBBBBFF)
{}

FontSettings::FontSettings()
    :
    size(24),
    name("FiraCode")
{}

CursorSettings::CursorSettings()
    :
    shape("rect"),
    blinkrate_ms(1000)
{}


Settings::Settings() {}

void Settings::init(const std::string& config_path) {
    config_path_ = config_path;

    libconfig::Config config;

    try {
        config.readFile(config_path_.c_str());
    } catch (const libconfig::FileIOException&) {
        Logger::instance().critical("libconfig++: cannot load config from file " + config_path_);
    }

    const libconfig::Setting& root = config.getRoot();
    const libconfig::Setting& editor = root.lookup("editor");
    const libconfig::Setting& ui = editor.lookup("ui");

    // load colors
    const libconfig::Setting& colors = ui.lookup("colors");
    lookupColor(colors, "selection", colors_.selection);
    lookupColor(colors, "text", colors_.text);
    lookupColor(colors, "background", colors_.bg);
    lookupColor(colors, "cursor", colors_.cursor);
    lookupColor(colors, "panels", colors_.ui);
    lookupColor(colors, "lines", colors_.line_no);

    // load font settings
    const libconfig::Setting& font = ui.lookup("font");
    font.lookupValue("size", font_settings_.size);
    font.lookupValue("name", font_settings_.name);

    // load cursor settings
    const libconfig::Setting& cursor = ui.lookup("cursor");
    cursor.lookupValue("shape", cursor_.shape);
    cursor.lookupValue("blinkrate", cursor_.blinkrate_ms);

    // load rulers
    const libconfig::Setting &rulers  = ui.lookup("rulers");
    for (int n = 0; n < rulers.getLength(); ++n)
    {
        rulers_.push_back(rulers[n]);
    }

    libconfig::Setting& dev = editor.lookup("dev");

    // load log settings
    libconfig::Setting& log = dev.lookup("log");
    log.lookupValue("level", log_.level);
}

bool Settings::lookupColor(const libconfig::Setting& setting, const char* name, uint32_t& field) {
    long long val;
    if (!setting.lookupValue(name, val)) {
        std::string msg;
        std::stringstream msg_builder(msg);
        msg_builder << "Settings::lookupColor: cannot read property '" << name << "' in " << setting.getPath();
        Logger::instance().warning(msg_builder.str());
        return false;
    }
    field = static_cast<uint32_t>(val);
    return true;
}

Settings& Settings::instance() {
    static Settings instance;
    return instance;
}
