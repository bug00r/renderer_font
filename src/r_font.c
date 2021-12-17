#include "r_font.h"

void rfont_init(rf_ctx_t* ctx, rf_provider_t *provider)
{
    ctx->provider = provider;
    ctx->glyps = provider->get();
}

void rfont_cleanup(rf_ctx_t *ctx) 
{
    ctx->provider->free(&ctx->glyps);
    ctx->provider = NULL;
}

/*_color = (unsigned char)interpolate_lin(_color, renderer->max_z, 0.f, renderer->min_z, 255.f);*/
void rfont_raster(rf_ctx_t const * ctx, vec2_t* _charPos, unsigned long charcode, rf_bbox_t* _charBbox, RASTER_FONT_FUNC rFunc, void *data)
{
    rf_glyph_container_t *glyphs = ctx->glyps;
    rf_glyph_t *glyph = glyphs->get(charcode);
    rf_bbox_t* glyphBbox = &glyph->bbox;

    vec2_t* charPos = _charPos;
    rf_bbox_t* charBbox = _charBbox;

    /* based on negative glyph outlines the wanted positive charBbox has to corrected to negative delta like letter "g" 
       Sum: We calculate the negative delta for the charBbox based on the glyphBbox
    */
    rf_bbox_t alignedCharBox = {
        (long)interpolate_lin((float)glyphBbox->xMin, 0.f, 0.f, (float)charBbox->xMax, (float)glyphBbox->xMax)/* xMin */,
        (long)interpolate_lin((float)glyphBbox->yMin, 0.f, 0.f, (float)charBbox->yMax, (float)glyphBbox->yMax)/* yMin */,
        charBbox->xMax/* xMax */,
        charBbox->yMax/* yMax */
    };

    /* raster reference point for intersection computing */
    vec2_t rasterRef = { 
        glyphBbox->xMax + (( (float)glyphBbox->xMax - (float)glyphBbox->xMin ) * .5f ), 
        glyphBbox->yMin + (( (float)glyphBbox->yMax - (float)glyphBbox->yMin ) * .5f )
    };

    for (long deltaScrY = charBbox->yMin; deltaScrY < charBbox->yMax; ++deltaScrY )
    {
            /* screenresult y */
            long curY = _charPos->y + deltaScrY;
            for (long deltaScrX = charBbox->xMin; deltaScrX < charBbox->xMax; ++deltaScrX )
            {
                /* screenresult x */
                long curX = _charPos->x + deltaScrX;

                //interpolate_lin(_color, renderer->max_z, 0.f, renderer->min_z, 255.f);

            }
    }
}
