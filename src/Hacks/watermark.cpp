#include "watermark.h"

#include "../ATGUI/texture.h"
#include "../settings.h"

#include "../Resources/logo.h"

bool Settings::Watermark::enabled = false;

void Watermark::Paint()
{
        static Texture logo(logo_rgba, logo_width, logo_height);

        if (!Settings::Watermark::enabled || !engine->IsInGame())
                return;

        if (!logo.IsValid())
                return;

        logo.Draw(0, 0, logo_width, logo_height);
}
