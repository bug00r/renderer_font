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
} rf_bbox_t;

typedef struct {
    rf_bbox_t bbox;
    size_t cntPoints;
    vec2_t *points;
} rf_glyph_t;

/* a Collection of all provided glyphs */
typedef struct {
    void *internals;    /* Internals will be structured by Provider */
    rf_glyph_t* (*get)( unsigned long charcode);
    rf_bbox_t globalBbox;
} rf_glyph_container_t;

typedef void (*INIT_PROVIDER_FN)(void *provider, void *init_data);

typedef struct {
    void*                   init_data;                          /* forwarded init_data to INIT_PROVIDER_FN*/
    INIT_PROVIDER_FN        (*get_init_fn)(void);               /* getter for init method */  
} rf_provider_init_t;

typedef struct {
    rf_provider_init_t*     init;                               /* OPTIONAL init process data */
    rf_glyph_container_t*   (*get)  (void);                     /* constructor method for rf_data_t object */
    void                    (*free) (rf_glyph_container_t**);   /* destructor method for rf_data_t object */
} rf_provider_t;

typedef struct {
    rf_glyph_container_t *glyps;
    rf_provider_t *provider;
} rf_ctx_t;

typedef struct {
    rf_bbox_t alignedCharBox;
    vec2_t lenGlyph;
    vec2_t lenGlobal;
    vec2_t glyphPixel;
    float pixelRatio;
    float xOffsetChar;
    float yOffsetChar;
} rf_glyph_meta_t;

typedef void (*RASTER_FONT_FUNC)(float const * const x, float const * const y, void *data);

/* inits context and calls providers init method */
void rfont_init(rf_ctx_t *ctx, rf_provider_t *provider);
void rfont_cleanup(rf_ctx_t *ctx);
void rfont_raster(rf_ctx_t const * ctx, unsigned long charcode, float charwidth, RASTER_FONT_FUNC rFunc, void *data);
void rfont_raster_text(rf_ctx_t const * ctx, unsigned char const * const text, float charwidth, RASTER_FONT_FUNC rFunc, void *data);

void rfont_get_meta(rf_ctx_t const * ctx, rf_glyph_meta_t* meta, unsigned long charcode, float charwidth);
void rfont_get_meta_str(rf_ctx_t const * ctx, rf_glyph_meta_t* meta, unsigned char const * const text, float charwidth);

#endif