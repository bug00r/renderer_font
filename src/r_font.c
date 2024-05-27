#include "r_font.h"

//#ifdef debug
#include <stdio.h>
//Debug output
void __rfont_bbox_print(const char *label, RFBBox *bbox) {
    printf("%s: xmin: %ld ymin: %ld xmax: %ld ymax: %ld\n", label, bbox->xMin, bbox->yMin 
                                                                 , bbox->xMax, bbox->yMax);
}  

//#endif

void rfont_init(RFCtx* ctx, RFProvider *provider)
{
    ctx->provider = provider;
    ctx->glyps = provider->get();

    RFProviderInit *init = provider->init;
    if ( init != NULL )
    {
        INIT_PROVIDER_FN init_fn =  init->get_init_fn();
        if ( init_fn != NULL )
        {
            init_fn(ctx->provider, init->init_data);
        }
    }
}

void rfont_cleanup(RFCtx *ctx) 
{
    ctx->provider->free(&ctx->glyps);
    ctx->provider = NULL;
}

static bool __r_font_vec2_inside_bbox(Vec2 *vec, RFBBox *bbox)
{ 
    return (vec->x >= bbox->xMin) ||
           (vec->x <= bbox->xMax) ||
           (vec->y >= bbox->yMin) ||
           (vec->y <= bbox->yMax);
}

static bool __r_font_must_check_for_intersection(Vec2 *p1, Vec2 *p2, RFBBox* bbox)
{   
    return __r_font_vec2_inside_bbox(p1, bbox) || __r_font_vec2_inside_bbox(p2, bbox);
}

static int __r_font_compute_intersectionsum(RFGlyph *_glyph, RFBBox *toCheckArea,
                                            Vec2 *curPoint, Vec2 *rasterRef)
{
    RFGlyph *glyph = _glyph;

    Vec2 *outlinePts = glyph->points;
    size_t cntPoints = glyph->cntPoints;
    Vec2 *outlineFirst = NULL;

    int intersectionSum = 0;

    for ( size_t curOutlinePt = 1; curOutlinePt < cntPoints; ++curOutlinePt)
    {
        Vec2 *start = &outlinePts[curOutlinePt-1];
        Vec2 *end = &outlinePts[curOutlinePt];

        if ( outlineFirst == NULL )
        {
            outlineFirst = start;
        } else if (vec2_equals(outlineFirst, start))
        {
            outlineFirst = NULL;
            continue;
        }

        #ifdef debug
            __rfont_bbox_print("(INTERSEC) AREA", toCheckArea);
        #endif
        //intersects?
        if ( __r_font_must_check_for_intersection(start, end, toCheckArea) )
        {
            
            bool intersec = lineseg_intersect(curPoint, rasterRef, start, end);

            #ifdef debug
            printf("\tINSTERSECTION TEST (intersec: %i)\n", intersec);
            #endif

            if ( intersec )
            {
                
                #ifdef debug
                printf("\t\tintersects with pos(negative is left) ");
                #endif                               

                /* computing side */
                Vec3 first = { start->x, start->y, 0.f };
                Vec3 middle = { curPoint->x, curPoint->y, 0.f };
                Vec3 last = { end->x, end->y, 0.f };

                float place = place_of_vec3(&first, &last, &middle);

                #ifdef debug
                printf("f l m pos:= %.2f/%.2f | %.2f/%.2f | %.2f/%.2f | %.2f\n",
                        first.x, first.y, last.x, last.y, middle.x, middle.y, place);
                #endif     

                intersectionSum += ( place >= 0.f ? -1 : 1 ); 
                
                #ifdef debug
                printf("\t\tintersection sum: %i\n", intersectionSum);
                #endif


            }
        }
    }    

    return intersectionSum;
}

typedef struct {
    Vec2 curPos;
    Vec2 lastMax;
} __rf_options_t;

static void __r_font_raster_raw(__rf_options_t * _options, RFCtx const * ctx, unsigned long charcode, float charwidth, RASTER_FONT_FUNC rFunc, void *data)
{
    if ( rFunc == NULL || ctx == NULL || charwidth <= 0.f ) return;

    RFGlyphContainer *glyphs = ctx->glyps;
    RFGlyph *glyph = glyphs->get(charcode);

    RFBBox* glyphBbox = &glyph->bbox;
    RFBBox* globalBbox = &glyphs->globalBbox;

    __rf_options_t * options = _options; 

    #ifdef debug
        __rfont_bbox_print("(IN)glyph Bbox", glyphBbox);
        __rfont_bbox_print("(IN)global Bbox", globalBbox);
    #endif

    if ( glyphBbox->xMax == 0 && glyphBbox->yMax == 0 && glyphBbox->xMin == 0 && glyphBbox->yMin == 0 )
    {
        options->lastMax.x += (charwidth * .25f);
        return;
    }

    RFGlyphMeta meta;
    rfont_get_meta(ctx, &meta, charcode, charwidth);

    RFBBox *alignedCharBox = &meta.alignedCharBox;
    float xOffsetChar = meta.xOffsetChar;
    float yOffsetChar = meta.yOffsetChar;

    #ifdef debug
        __rfont_bbox_print("\n(ALIGNED)char Bbox", alignedCharBox);
    #endif

    Vec2 rasterRef = { 
        globalBbox->xMax + 1.f, 
        globalBbox->yMax + ((globalBbox->yMax - globalBbox->yMin) * 0.5f)
    };

    #ifdef debug
        printf("RasterRef: x: %.2f y: %.2f\n", rasterRef.x, rasterRef.y);
    #endif

    RFBBox toCheckArea = {
        0L,/* will be set in loop */
        0L,/* will be set in loop */
        rasterRef.x, /* based on algo this is max */
        rasterRef.y,/* will be set in loop */
    };

    long alignedCharBoxYMax = alignedCharBox->yMax - 1;
    long alignedCharBoxXMax = alignedCharBox->xMax - 1;
    
    for (long deltaScrY = alignedCharBox->yMin; deltaScrY < alignedCharBoxYMax /*alignedCharBox.yMax*/; ++deltaScrY)
    {
        /* screenresult y */
        float curGlyphY = interpolate_lin(deltaScrY + 1, alignedCharBox->yMin, glyphBbox->yMin, alignedCharBox->yMax, glyphBbox->yMax);

        Vec2 curPoint;
        curPoint.y = (float)curGlyphY;

        toCheckArea.yMin = curGlyphY;

        for (long deltaScrX = alignedCharBox->xMin; deltaScrX < alignedCharBoxXMax/*alignedCharBox.xMax*/; ++deltaScrX)
        {
            float curGlyphX = interpolate_lin(deltaScrX + 1, alignedCharBox->xMin, glyphBbox->xMin, alignedCharBox->xMax, glyphBbox->xMax);
            
            #ifdef debug
                printf("x/y conv:= char: %ld / %ld glyph: %f / %f \n", deltaScrX, deltaScrY, curGlyphX, curGlyphY);
            #endif

            /* Here we add intersection logic */
            curPoint.x = (float)curGlyphX;

            toCheckArea.xMin = curGlyphX;
            
            int intersectionSum = __r_font_compute_intersectionsum( glyph, &toCheckArea, &curPoint, &rasterRef);

            #ifdef debug
                printf(" CHECK intersection sum: %i\n", intersectionSum);
            #endif

            if ( intersectionSum != 0 ) 
            {
                float renderX = options->curPos.x + (float)deltaScrX + xOffsetChar;//round_f(xOffsetChar);
                float renderY = options->curPos.y + (float)deltaScrY + yOffsetChar + 2;//round_f(yOffsetChar);

                options->lastMax.x = ( options->lastMax.x < renderX ? renderX : options->lastMax.x );
                options->lastMax.y = ( options->lastMax.y < renderY ? renderY : options->lastMax.y );

                rFunc((float const * const )&renderX, (float const * const )&renderY, data);
            }

        }
    }
}

void rfont_raster(RFCtx const * ctx, unsigned long charcode, float charwidth, RASTER_FONT_FUNC rFunc, void *data)
{
    __rf_options_t options;
    options.curPos = (Vec2){0.f, 0.f};
    __r_font_raster_raw(&options, ctx, charcode, charwidth, rFunc, data);
}

static unsigned char __r_font_truncate(char *curChar, unsigned int cntMSB)
{   
    unsigned char truncated = *curChar;
    truncated <<= cntMSB;
    truncated >>= cntMSB;
    return truncated;
}

static unsigned long __r_font_compute_charcode(char *curChar, unsigned int *usedBytes)
{
    unsigned long result = *curChar;

    if (curChar == NULL) return result;

    unsigned char usedChar = *curChar;

    *usedBytes = 1;

    if ( (usedChar >> 5) == 6 )
    {   
        result = 0 | __r_font_truncate(curChar++,3) << 6 | __r_font_truncate(curChar, 2);
        *usedBytes = 2;
    } 
    else if ( (usedChar>> 4) == 14 ) 
    {
        result = 0 | __r_font_truncate(curChar++,4) << 12 | 
                     __r_font_truncate(curChar++,2) << 6  | 
                     __r_font_truncate(curChar, 2);
        *usedBytes = 3;
    }
    else if ( (usedChar >> 3) == 30 ) 
    {
        result = 0 | __r_font_truncate(curChar++,4) << 18 | 
                     __r_font_truncate(curChar++,2) << 12 | 
                     __r_font_truncate(curChar++,2) << 6  | 
                     __r_font_truncate(curChar, 2);
        *usedBytes = 4;
    }

    return result;
}

void rfont_raster_text(RFCtx const * ctx, unsigned char const * const text, float charwidth, RASTER_FONT_FUNC rFunc, void *data)
{
    __rf_options_t options;
    options.curPos = (Vec2){0.f, 0.f};
    char *curChar = (char *)text;

    float hGap = 3.f;

    while( *curChar != '\0' )
    {
        unsigned int usedBytes = 0;
        unsigned long charcode = __r_font_compute_charcode(curChar, &usedBytes);
        __r_font_raster_raw(&options, ctx, charcode, charwidth, rFunc, data);

        options.curPos.x = options.lastMax.x + hGap;
        //printf("usedBytes: %i\n", usedBytes);
        curChar += usedBytes;
    }
}

void rfont_get_meta(RFCtx const * ctx, RFGlyphMeta* _meta, unsigned long charcode, float charwidth)
{
    RFGlyphContainer *glyphs = ctx->glyps;
    RFGlyph *glyph = glyphs->get(charcode);

    RFBBox* glyphBbox = &glyph->bbox;

    RFBBox* globalBbox = &glyphs->globalBbox;

    RFGlyphMeta* meta = _meta;

    meta->lenGlyph = (Vec2){ ((float)glyphBbox->xMax - (float)glyphBbox->xMin), ((float)glyphBbox->yMax - (float)glyphBbox->yMin) };
    meta->lenGlobal = (Vec2){ ((float)globalBbox->xMax - (float)globalBbox->xMin), ((float)globalBbox->yMax - (float)globalBbox->yMin) };

    meta->pixelRatio = ( charwidth / meta->lenGlobal.x);

    meta->glyphPixel = (Vec2){ ceilf(meta->pixelRatio * meta->lenGlyph.x), ceilf(meta->pixelRatio * meta->lenGlyph.y) };
 

    meta->xOffsetChar = ( glyphBbox->xMin != 0 ? (float)glyphBbox->xMin * meta->pixelRatio : 0 );
    meta->yOffsetChar = ( glyphBbox->yMin != 0 ? (float)glyphBbox->yMin * meta->pixelRatio : 0 );

    meta->alignedCharBox = (RFBBox){
        /* xMin */-1,
        /* yMin */-1,
        /* xMax */meta->glyphPixel.x,
        /* yMax */meta->glyphPixel.y,
    };

    if ( glyphBbox->xMax == 0 && glyphBbox->yMax == 0 && glyphBbox->xMin == 0 && glyphBbox->yMin == 0 )
    {
        meta->xOffsetChar += (charwidth * .25f);
    }
}

void rfont_get_meta_str(RFCtx const * ctx, RFGlyphMeta* _meta, unsigned char const * const text, float charwidth)
{
    RFGlyphMeta* globalMeta = _meta;
    RFGlyphMeta localMeta;

    char *curChar = (char *)text;

    int hGap = 3;

    globalMeta->yOffsetChar = 0.f;
    //not in use globalMeta->xOffsetChar
    globalMeta->alignedCharBox = (RFBBox){
        /* xMin */-1,
        /* yMin */-1,
        /* xMax */0,
        /* yMax */0,
    };

    while( *curChar != '\0' )
    {
        rfont_get_meta(ctx, &localMeta, *curChar,charwidth);

        long width = localMeta.alignedCharBox.xMax - localMeta.alignedCharBox.xMin + floorf(fabsf(localMeta.xOffsetChar));
	    long height = localMeta.alignedCharBox.yMax - localMeta.alignedCharBox.yMin + ceilf(fabsf(localMeta.yOffsetChar)); 
        
        globalMeta->alignedCharBox.xMax += width;
        globalMeta->alignedCharBox.xMax += hGap;

        globalMeta->alignedCharBox.yMax = ( globalMeta->alignedCharBox.yMax < height ? height : globalMeta->alignedCharBox.yMax );

        globalMeta->yOffsetChar = ( globalMeta->yOffsetChar > localMeta.yOffsetChar ? localMeta.yOffsetChar : globalMeta->yOffsetChar );

        curChar++;
    }

    globalMeta->alignedCharBox.xMax -= hGap;
}
