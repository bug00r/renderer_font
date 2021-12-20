#include "r_font.h"

#ifdef debug
#include <stdio.h>
//Debug output
void __rfont_bbox_print(const char *label, rf_bbox_t *bbox) {
    printf("%s: xmin: %ld ymin: %ld xmax: %ld ymax: %ld\n", label, bbox->xMin, bbox->yMin 
                                                                 , bbox->xMax, bbox->yMax);
}  

#endif

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
    if ( rFunc == NULL || ctx == NULL || _charBbox == NULL ) return;

    rf_glyph_container_t *glyphs = ctx->glyps;
    rf_glyph_t *glyph = glyphs->get(charcode);
    rf_bbox_t* glyphBbox = &glyph->bbox;

    vec2_t* charPos = _charPos;
    rf_bbox_t* charBbox = _charBbox;

    #ifdef debug
        __rfont_bbox_print("(IN)char Bbox", charBbox);
        __rfont_bbox_print("(IN)glyph Bbox", glyphBbox);
    #endif

    /* TO CONVERTER */
    vec2_t lenGlyph = { ((float)glyphBbox->xMax - (float)glyphBbox->xMin), ((float)glyphBbox->yMax - (float)glyphBbox->yMin) };
    vec2_t lenChar = { ((float)charBbox->xMax - (float)charBbox->xMin), ((float)charBbox->yMax - (float)charBbox->yMin) };
    /* TO CONVERTER */
    float xRatioGlyph = lenGlyph.x / lenGlyph.y;
    float xRatioChar = lenChar.x / lenChar.y;
    float xRatioDiff = xRatioGlyph - xRatioChar;
    long alignedXMax = (float)charBbox->xMax + ((float)charBbox->xMax * xRatioDiff);

    /* ( glyphBbox->xMin < 0 ? ( glyphBbox->xMin / lenGlyph.x ) 
       ( glyphBbox->yMin < 0 ? ( glyphBbox->yMin / lenGlyph.y )

       TO CONVERTER
     */
    float xOffsetChar = ( glyphBbox->xMin < 0 ? ( glyphBbox->xMin / lenGlyph.x ) * lenChar.x : 0 );
    float yOffsetChar = ( glyphBbox->yMin < 0 ? ( glyphBbox->yMin / lenGlyph.y ) * lenChar.y : 0 );

    #ifdef debug
        printf("glyph Ratio: %f char Ratio: %f diff: %f\n", xRatioGlyph, xRatioChar, xRatioDiff);
        printf("old xMAx: %ld aligned xMax: %ld\n", charBbox->xMax, alignedXMax);
        printf("Offset (x/y): %f / %f\n", xOffsetChar, yOffsetChar);
    #endif

    /* based on negative glyph outlines the wanted positive charBbox has to corrected to negative delta like letter "g" 
       Sum: We calculate the negative delta for the charBbox based on the glyphBbox
    */
    rf_bbox_t alignedCharBox = {
        /* xMin */charBbox->xMin + (long)xOffsetChar,
        /* yMin */charBbox->yMin + (long)yOffsetChar,
        /* xMax */alignedXMax + (long)xOffsetChar,
        /* yMax */charBbox->yMax + (long)yOffsetChar
    };

    #ifdef debug
        __rfont_bbox_print("(ALIGNED)char Bbox", &alignedCharBox);
    #endif

    /* raster reference point for intersection computing 
       
       INTO CONVERTER
    */
    vec2_t rasterRef = { 
        glyphBbox->xMax + (( (float)glyphBbox->xMax - (float)glyphBbox->xMin ) * .5f ), 
        glyphBbox->yMin + (( (float)glyphBbox->yMax - (float)glyphBbox->yMin ) * .5f )
    };

    #ifdef debug
        printf("RasterRef: x: %f y: %f\n", rasterRef.x, rasterRef.y);
    #endif

    rf_bbox_t toCheckArea = {
        0L,/* will be set in loop */
        0L,/* will be set in loop */
        rasterRef.x, /* based on algo this is max */
        0L,/* will be set in loop */
    };

    for (long deltaScrY = alignedCharBox.yMin; deltaScrY < alignedCharBox.yMax; ++deltaScrY )
    {
            /* screenresult y */
            long curCharY = deltaScrY;
            for (long deltaScrX = alignedCharBox.xMin; deltaScrX < alignedCharBox.xMax; ++deltaScrX )
            {
                /* screenresult x */
                long curCharX = deltaScrX;

                long curGlyphX = interpolate_lin(curCharX, alignedCharBox.xMin, glyphBbox->xMin, alignedCharBox.xMax, glyphBbox->xMax);
                long curGlyphY = interpolate_lin(curCharY, alignedCharBox.yMin, glyphBbox->yMin, alignedCharBox.yMax, glyphBbox->yMax);
            
                #ifdef debug
                    printf("x/y conv:= char: %ld / %ld glyph: %ld / %ld \n", curCharX, curCharY, curGlyphX, curGlyphY);
                #endif

                /* Here we add intersection logic */
                vec2_t checkRef;
                vec2_t curPoint = { (float)curGlyphX, (float)curGlyphY };
                vec2_sub_dest(&checkRef, &rasterRef, &curPoint);

                toCheckArea.xMin = (long)fmin( (double)curGlyphX, (double)rasterRef.x);
                toCheckArea.yMin = (long)fmin( (double)curGlyphY, (double)rasterRef.y);
                toCheckArea.yMax = (long)fmax( (double)curGlyphY, (double)rasterRef.y);

                #ifdef debug
                    __rfont_bbox_print("(INTERSEC) AREA", &toCheckArea);
                #endif

                long renderX = charPos->x + curCharX;
                long renderY = charPos->y + curCharY;
                //send to render function with offset to cur pos
                rFunc((long const * const )&renderX, (long const * const )&renderY, data);
            }
    }
}
