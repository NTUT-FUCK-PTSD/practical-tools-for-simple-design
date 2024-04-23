#include "Util/SpriteSheet.hpp"

namespace Util {
SpriteSheet::SpriteSheet(const std::string &filepath)
    : m_Image(std::make_unique<Image>(filepath)) {
    m_OriginRect.x = 0;
    m_OriginRect.y = 0;
    m_OriginRect.w = m_Image->GetSize().x;
    m_OriginRect.h = m_Image->GetSize().y;
    m_DisplayRect = m_OriginRect;
}

void SpriteSheet::SetImage(const std::string &filepath) {
    m_Image->SetImage(filepath);
    m_OriginRect.x = 0;
    m_OriginRect.y = 0;
    m_OriginRect.w = m_Image->GetSize().x;
    m_OriginRect.h = m_Image->GetSize().y;
    m_DisplayRect = m_OriginRect;
}

void SpriteSheet::Draw(const Core::Matrices &data) {
    m_Image->Draw(data);
}

glm::vec2 SpriteSheet::GetSize() const {
    return m_Image->GetSize();
}

void SpriteSheet::RestDrawRect() {
    SetDrawRect(m_OriginRect);
    m_DisplayRect = m_OriginRect;
}

void SpriteSheet::SetDrawRect(const SDL_Rect displayRect) {
    // can't just simplely use SDL_SetClipRect, because we use opengl to render
    // and I'm not sure this way if cost a lot performance?
    SDL_Surface &originSDLSurface = m_Image->GetSDLSurface();

    if (displayRect.h + displayRect.y > originSDLSurface.h ||
        displayRect.w + displayRect.x > originSDLSurface.w) {
        LOG_DEBUG("SetDrawRect OverRange");
        return;
    }

    auto OriginalBlendMode = SDL_BlendMode::SDL_BLENDMODE_BLEND;
    SDL_GetSurfaceBlendMode(&originSDLSurface, &OriginalBlendMode);
    SDL_SetSurfaceAlphaMod(&originSDLSurface, m_ColorMod.a);
    SDL_SetSurfaceColorMod(&originSDLSurface, m_ColorMod.r, m_ColorMod.g,
                           m_ColorMod.b);
    SDL_SetSurfaceBlendMode(&originSDLSurface,
                            SDL_BlendMode::SDL_BLENDMODE_NONE);

    auto targetSurface =
        std::unique_ptr<SDL_Surface, std::function<void(SDL_Surface *)>>{
            SDL_CreateRGBSurfaceWithFormat(
                0, displayRect.w, displayRect.h,
                originSDLSurface.format->BitsPerPixel,
                originSDLSurface.format->format),
            SDL_FreeSurface,
        };

    int isCopyWork = SDL_BlitSurface(&originSDLSurface, &displayRect,
                                     targetSurface.get(), NULL);

    SDL_SetSurfaceAlphaMod(&originSDLSurface, 255);
    SDL_SetSurfaceColorMod(&originSDLSurface, 255, 255, 255);
    SDL_SetSurfaceBlendMode(&originSDLSurface, OriginalBlendMode);

    if (isCopyWork != 0) {
        LOG_ERROR("{}", SDL_GetError());
        return;
    }

    m_DisplayRect = displayRect;
    m_Image->UpdateTextureData(*targetSurface.get());
}

void SpriteSheet::SetAlpha(const Uint8 alpha) {
    if (alpha == m_ColorMod.a) {
        return;
    }
    m_ColorMod.a = alpha;
    SetDrawRect(m_DisplayRect);
}

void SpriteSheet::SetColorMod(const SDL_Color color) {
    if (color.r == m_ColorMod.r && color.g == m_ColorMod.g &&
        color.b == m_ColorMod.b && color.a == m_ColorMod.a) {
        return;
    }
    m_ColorMod = color;
    SetDrawRect(m_DisplayRect);
}

} // namespace Util
