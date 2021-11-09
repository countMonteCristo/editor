#ifndef FONT_HPP_
#define FONT_HPP_

#include <string>
#include <unordered_map>

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

#include <fontconfig/fontconfig.h>

#include "glyph.hpp"
#include "common.hpp"


constexpr int FONT_CHAR_WIDTH = 16;
constexpr int FONT_CHAR_HEIGHT = 25;


class Font {
public:
    Font() ;
    ~Font();

    SDL_Texture* get_glyph_texture(const Glyph& glyph);

    void set_font(const std::string& name, int ptsize);

    void set_renderer(SDL_Renderer* r) { renderer_ = r; }
    int height() const;
    int width() const;
private:
    SDL_Surface* _generate_glyph_surface(const Glyph& glyph);
    void _load_font(const std::string& font_filepath, int ptsize);

private:
    std::string filepath_;
    TTF_Font* font_;
    FcConfig* fc_config_;

    typedef std::unordered_map<uint32_t, SDL_Texture*> colored_map_t;
    std::unordered_map<Glyph, colored_map_t> cache_;

    SDL_Renderer *renderer_;
};


#endif // FONT_HPP_
