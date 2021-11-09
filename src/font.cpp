#include "font.hpp"

#include "common.hpp"
#include "logger.hpp"
#include "settings.hpp"

#include <sstream>
#include <iostream>


Font::Font()
    : font_(nullptr)
{
    if ( TTF_Init() == -1) {
        std::string err_msg = std::string("TTF_Init: ") + TTF_GetError();
        Logger::instance().critical(err_msg);
    }
    Logger::instance().debug("SDL2_TTF: successfully initialized");

    SDL_version compiled;
    const SDL_version *linked = TTF_Linked_Version();
    TTF_VERSION(&compiled);
    log_lib_version(compiled, *linked, "SDL2_TTF");

    if (!FcInit()) {
        Logger::instance().critical("FcInit: cannot initialize fontconfig");
    }
    fc_config_ = FcInitLoadConfigAndFonts();
    Logger::instance().debug("fontconfig: successfully initialized");

    const std::string& font_name = Settings::const_instance().const_font().name;
    const int ptsize = Settings::const_instance().const_font().size;
    set_font(font_name, ptsize);
}

Font::~Font() {
    FcConfigDestroy(fc_config_);

    if (font_) {
        TTF_CloseFont(font_);
    }
    TTF_Quit();

    FcFini();
}

// TODO: Fix finding font file
void Font::set_font(const std::string& name, int ptsize) {
    //does not necessarily has to be a specific name.  You could put anything here and Fontconfig WILL find a font for you
    FcPattern* pat = FcNameParse((const FcChar8*)name.c_str());

    FcConfigSubstitute(fc_config_, pat, FcMatchPattern);    //NECESSARY; it increases the scope of possible fonts
    FcDefaultSubstitute(pat);                               //NECESSARY; it increases the scope of possible fonts

    char* font_file;
    FcResult result;

    FcPattern* font = FcFontMatch(fc_config_, pat, &result);

    if (font)
    {
        //The pointer stored in 'file' is tied to 'font'; therefore, when 'font' is freed, this pointer is freed automatically.
        //If you want to return the filename of the selected font, pass a buffer and copy the file name into that buffer
        FcChar8* file = NULL;

        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch)
        {
            font_file = (char*)file;

            std::string font_path(font_file);
            Logger::instance().debug("Loading font `" + name + "` from " + font_path);
            _load_font(font_path, ptsize);
        } else {
            Logger::instance().critical("Cannot load font by name '" + name + "'");
        }
    }

    FcPatternDestroy(font);     //needs to be called for every pattern created; in this case, 'fontFile' / 'file' is also freed
    FcPatternDestroy(pat);      //needs to be called for every pattern created
}



void Font::_load_font(const std::string& font_filepath, int ptsize) {
    filepath_ = font_filepath;

    if (font_) {
        TTF_CloseFont(font_);
    }

    font_ = TTF_OpenFont(filepath_.c_str(), ptsize);
    if (!font_) {
        Logger::instance().critical(std::string("TTF_OpenFont: ") + TTF_GetError());
    }
}

int Font::width() const {
    if ( !font_ )
        return FONT_CHAR_WIDTH;
    int advance;
    if (TTF_GlyphMetrics(font_, 'G', nullptr, nullptr, nullptr, nullptr, &advance) == -1) {
        return FONT_CHAR_WIDTH;
    } else {
        return advance;
    }
}

int Font::height() const {
    return font_? TTF_FontHeight(font_): FONT_CHAR_HEIGHT;
}

SDL_Texture*  Font::get_glyph_texture(const Glyph& glyph) {
    const uint32_t color = glyph.color();

    auto glyph_it = cache_.find(glyph);
    if ( (glyph_it == cache_.end()) || ( glyph_it->second.find(color) == glyph_it->second.end() ) ) {
        SDL_Surface* glyph_surface = _generate_glyph_surface(glyph);
        SDL_Texture *glyph_texture = SDL_CreateTextureFromSurface(renderer_, glyph_surface);
        cache_[glyph][color] = glyph_texture;
    }
    return cache_[glyph][color];
}

SDL_Surface* Font::_generate_glyph_surface(const Glyph& glyph) {
    SDL_Color sdl_color = { UNWRAP_U64(glyph.color()) };
    const char* text = glyph.c_str();
#if FONT_RENDER_STYLE == FONT_RENDER_SOLID
    SDL_Surface *glyph_surface = TTF_RenderUTF8_Solid(font_, text, sdl_color);
#elif FONT_RENDER_STYLE == FONT_RENDER_SHADED
    SDL_Surface *glyph_surface = TTF_RenderUTF8_Shaded(font_, text, sdl_color, {0,0,0,0});
#elif FONT_RENDER_STYLE == FONT_RENDER_BLENDED
    SDL_Surface *glyph_surface = TTF_RenderUTF8_Blended(font_, text, sdl_color);
#endif
    return glyph_surface;
}
