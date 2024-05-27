#ifndef R_FONT_H
#define R_FONT_H

#include <stdbool.h>

#include "defs.h"
#include "dl_list.h"
#include "utilsmath.h"

typedef struct {
    long xMin;
    long yMin;
    long xMax;
    long yMax;
} RFBBox;

typedef struct {
    RFBBox bbox;
    size_t cntPoints;
    Vec2 *points;
} RFGlyph;

/* a Collection of all provided glyphs */
typedef struct {
    void *internals;    /* Internals will be structured by Provider */
    RFGlyph* (*get)( unsigned long charcode);
    RFBBox globalBbox;
} RFGlyphContainer;

typedef void (*INIT_PROVIDER_FN)(void *provider, void *init_data);

typedef struct {
    void*                   init_data;                          /* forwarded init_data to INIT_PROVIDER_FN*/
    INIT_PROVIDER_FN        (*get_init_fn)(void);               /* getter for init method */  
} RFProviderInit;

typedef struct {
    RFProviderInit*     init;                               /* OPTIONAL init process data */
    RFGlyphContainer*   (*get)  (void);                     /* constructor method for rf_Data object */
    void                    (*free) (RFGlyphContainer**);   /* destructor method for rf_Data object */
} RFProvider;

typedef struct {
    RFGlyphContainer *glyps;
    RFProvider *provider;
} RFCtx;

typedef struct {
    RFBBox alignedCharBox;
    Vec2 lenGlyph;
    Vec2 lenGlobal;
    Vec2 glyphPixel;
    float pixelRatio;
    float xOffsetChar;
    float yOffsetChar;
} RFGlyphMeta;

typedef void (*RASTER_FONT_FUNC)(float const * const x, float const * const y, void *data);

/* inits context and calls providers init method */
void rfont_init(RFCtx *ctx, RFProvider *provider);
void rfont_cleanup(RFCtx *ctx);
void rfont_raster(RFCtx const * ctx, unsigned long charcode, float charwidth, RASTER_FONT_FUNC rFunc, void *data);
void rfont_raster_text(RFCtx const * ctx, unsigned char const * const text, float charwidth, RASTER_FONT_FUNC rFunc, void *data);

void rfont_get_meta(RFCtx const * ctx, RFGlyphMeta* meta, unsigned long charcode, float charwidth);
void rfont_get_meta_str(RFCtx const * ctx, RFGlyphMeta* meta, unsigned char const * const text, float charwidth);

#endif