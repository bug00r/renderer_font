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

static bool __r_font_vec2_inside_bbox(vec2_t *vec, rf_bbox_t *bbox)
{ 
    return (vec->x >= bbox->xMin) &&
           (vec->x <= bbox->xMax) &&
           (vec->y >= bbox->yMin) &&
           (vec->y <= bbox->yMax);
}

static bool __r_font_must_check_for_intersection(vec2_t *p1, vec2_t *p2, rf_bbox_t* bbox)
{   
    return __r_font_vec2_inside_bbox(p1, bbox) && __r_font_vec2_inside_bbox(p2, bbox);
}

/*_color = (unsigned char)interpolate_lin(_color, renderer->max_z, 0.f, renderer->min_z, 255.f);*/
void rfont_raster(rf_ctx_t const * ctx, vec2_t* _charPos, unsigned long charcode, rf_bbox_t* _charBbox, RASTER_FONT_FUNC rFunc, void *data)
{
    if ( rFunc == NULL || ctx == NULL || _charBbox == NULL ) return;

    rf_glyph_container_t *glyphs = ctx->glyps;
    rf_glyph_t *glyph = glyphs->get(charcode);
    
    if ( glyph->cntOutlines < 2 ) {
        #ifdef debug
            printf("not enough outlines available.");
        #endif
        return;
    }
    
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
            long curGlyphY = interpolate_lin(deltaScrY, alignedCharBox.yMin, glyphBbox->yMin, alignedCharBox.yMax, glyphBbox->yMax);

            if ( curGlyphY < rasterRef.y ) {
                toCheckArea.yMin = curGlyphY;
                toCheckArea.yMax = rasterRef.y;
            } else {
                toCheckArea.yMin = rasterRef.y;
                toCheckArea.yMax = curGlyphY;
            }

            vec2_t curPoint;
            curPoint.y = (float)curGlyphY;

            for (long deltaScrX = alignedCharBox.xMin; deltaScrX < alignedCharBox.xMax; ++deltaScrX )
            {
                long curGlyphX = interpolate_lin(deltaScrX, alignedCharBox.xMin, glyphBbox->xMin, alignedCharBox.xMax, glyphBbox->xMax);
                
                #ifdef debug
                    printf("x/y conv:= char: %ld / %ld glyph: %ld / %ld \n", deltaScrX, deltaScrY, curGlyphX, curGlyphY);
                #endif

                /* Here we add intersection logic */
                curPoint.x = (float)curGlyphX;
                //vec2_t checkRef;
                //vec2_sub_dest(&checkRef, &rasterRef, &curPoint);

                toCheckArea.xMin = curGlyphX;
                
                int intersectionSum = 0;

                for ( size_t curOutline = 0; curOutline < glyph->cntOutlines; ++curOutline)
                {

                    rf_outlines_t *outline = &glyph->outlines[curOutline];
                    
                    for ( size_t curOutlinePt = 1; curOutlinePt < outline->cntPoints; ++curOutlinePt)
                    {

                        vec2_t *start = &outline->points[curOutlinePt-1];
                        vec2_t *end = &outline->points[curOutlinePt];
                        
                        #ifdef debug
                            __rfont_bbox_print("(INTERSEC) AREA", &toCheckArea);
                        #endif

                        //intersects?
                        if ( __r_font_must_check_for_intersection(start, end, &toCheckArea) )
                        {
                            if ( line_intersect_denominator(&curPoint, &rasterRef, start, end) != 0.f)
                            {

                                /* computing side */
                                vec3_t first = { start->x, start->y, 0.f };
                                vec3_t middle = { curPoint.x, curPoint.y, 0.f };
                                vec3_t last = { end->x, end->y, 0.f };

                                float place = place_of_vec3(&first, &last, &middle);

                                intersectionSum += ( place <= 0.f ? -1 : 1 );
                                
                            }
                        }
                    }
                }

                if ( intersectionSum != 0 ) 
                {
                    long renderX = charPos->x + deltaScrX;
                    long renderY = charPos->y + deltaScrY;
                    //send to render function with offset to cur pos
                    rFunc((long const * const )&renderX, (long const * const )&renderY, data);
                }

            }
    }
}
