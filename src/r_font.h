#ifndef R_FONT_H
#define R_FONT_H

#include "dl_list.h"

typedef struct {
    long xMin;
    long yMin;
    long xMax;
    long yMax;
} rf_bbox_t;

typedef struct {
    rf_bbox_t bbox;
    dl_list_t *outlines;
} rf_glyph_t;

/* a Collection of all provided glyphs */
typedef struct {
    void *internals;    /* Internals will be structured by Provider */
} rf_glyph_container_t;

typedef struct {
    rf_glyph_container_t*   (*get)  (void);         /* constructor method for rf_data_t object */
    void                    (*free) (rf_glyph_container_t**);  /* destructor method for rf_data_t object */
} rf_provider_t;

typedef struct {
    rf_glyph_container_t const * const glyps;
    rf_provider_t const * const provider;
} rf_ctx_t;

typedef void (*RASTER_FONT_FUNC)(int32_t const * const x, int32_t const * const y, void *data);


void rfont_init(rf_ctx_t* ctx, rf_provider_t const * const provider);
void rfont_raster(rf_ctx_t const * ctx, unsigned long charcode, rf_bbox_t* charBbox, RASTER_FONT_FUNC rFunc, void *data);

#endif