#include "r_font.h"

#include "debug.h"

#include <ctype.h>
#include <assert.h>

/*

typedef struct {
    vec2_t *points;
    size_t cntPoints;
} rf_outlines_t;

typedef struct {
    rf_bbox_t bbox;
    rf_outlines_t *outlines;
    size_t cntOutlines;
} rf_glyph_t;

// a Collection of all provided glyphs 
typedef struct {
    void *internals;    // Internals will be structured by Provider
    rf_glyph_t* (*get)( unsigned long charcode);    
} rf_glyph_container_t;


void rfont_init(rf_ctx_t *ctx, rf_provider_t *provider);
void rfont_cleanup(rf_ctx_t *ctx);
void rfont_raster(rf_ctx_t const * ctx, vec2_t* charPos, unsigned long charcode, rf_bbox_t* charBbox, RASTER_FONT_FUNC rFunc, void *data);

typedef struct {
    rf_glyph_container_t*   (*get)  (void);         //constructor method for rf_data_t object
    void                    (*free) (rf_glyph_container_t**);  // destructor method for rf_data_t object
    } rf_provider_t;

*/
typedef struct {
    long *buffer;
    size_t width;
    size_t height;
} __rf_test_buffer_ctx_t;

static void __rf_test_render_func(long const * const x, long const * const y, void *data)
{
    //printf("r:= x: %ld y: %ld\n", *x, *y);
    if ( !(*x > -1 && *y > -1) ) return;
    __rf_test_buffer_ctx_t *ctx = data;
    ctx->buffer[(*y * ctx->width) + *x] = 1L;
}

static const rf_bbox_t bbox_1 = {0l, 0l, 512l, 768l};
static const rf_bbox_t bbox_2 = {-50l, -150l, 512l, 768l};

/* GLYPH 1 TEST */

static const size_t cntOutlinePts1_1 = 5;
static const size_t cntOutlinePts1_2 = 5;
static vec2_t outlinePts1_1[5] = {
    {0.f, 0.f}, {0.f, 100.f}, {100.f, 100.f}, {100.f, 0.f}, {0.f, 0.f},
};
static vec2_t outlinePts1_2[5] = {
    {30.f, 30.f}, {70.f, 30.f}, {70.f, 70.f}, {30.f, 70.f}, {30.f, 30.f} 
};

static const size_t cntOutlines1 = 2;
static rf_outlines_t glyph1_outlines[2] = { 
    { &outlinePts1_1[0], cntOutlinePts1_1},  
    { &outlinePts1_2[0], cntOutlinePts1_2}
};

/* GLYPH 2 TEST */

static const size_t cntOutlinePts2_1 = 5;
static const size_t cntOutlinePts2_2 = 5;
static const size_t cntOutlinePts2_3 = 5;
static vec2_t outlinePts2_1[5] = {
    {0.f, 0.f}, {0.f, 100.f}, {100.f, 100.f}, {100.f, 0.f}, {0.f, 0.f}
};
static vec2_t outlinePts2_2[5] = {
    {30.f, 30.f}, {70.f, 30.f}, {70.f, 45.f}, {30.f, 45.f}, {30.f, 30.f}
};
static vec2_t outlinePts2_3[5] = {
    {30.f, 55.f}, {70.f, 55.f}, {70.f, 70.f}, {30.f, 70.f}, {30.f, 55.f} 
};

static const size_t cntOutlines2 = 3;
static rf_outlines_t glyph2_outlines[3] = { 
    { &outlinePts2_1[0], cntOutlinePts2_1},  
    { &outlinePts2_2[0], cntOutlinePts2_2},
    { &outlinePts2_3[0], cntOutlinePts2_3}
};


static rf_glyph_t __rf_test_glyph = { bbox_1, &glyph1_outlines[0], cntOutlines1};
static rf_glyph_t __rf_test_glyph_neg = { bbox_2, &glyph2_outlines[0], cntOutlines2};

rf_glyph_t* __rf_test_glyph_get( unsigned long charcode ) 
{
    switch (charcode)
    {
        case 0: return &__rf_test_glyph; break;
        case 1: return &__rf_test_glyph_neg; break;
        default:
            break;
    }
    
}

static rf_glyph_container_t __rf_test_glc = {NULL, __rf_test_glyph_get};

rf_glyph_container_t* __rf_test_glcon_get() 
{
    return &__rf_test_glc;
}

static void __rf_test_glcon_free(rf_glyph_container_t** container) 
{

}

static void __rf_test_clearBuffer(__rf_test_buffer_ctx_t *_buffCtx)
{
    __rf_test_buffer_ctx_t * buffCtx = _buffCtx;
    size_t max = buffCtx->width * buffCtx->height;
    for ( size_t i = max-1; i--;  ) {
        buffCtx->buffer[i] = 0L;
    }
}

static void __rf_test_printBuffer(__rf_test_buffer_ctx_t *_buffCtx)
{
    __rf_test_buffer_ctx_t * buffCtx = _buffCtx;
    size_t max = buffCtx->width * buffCtx->height;
    for( size_t i = 0; i < max; ++i )
    {
        if ( (i > 0) && ((i % buffCtx->width) == 0) ) printf("\n");
        printf("%ld ", buffCtx->buffer[i]);
    }
}

static void test_r_font_raster_dummy() 
{
    DEBUG_LOG(">>>\n");

    rf_provider_t provider = { __rf_test_glcon_get, __rf_test_glcon_free};

    rf_ctx_t rf_ctx;
    rfont_init(&rf_ctx, &provider);

    size_t width = 10;
    size_t height = 10;
    vec2_t charPos = {1.f, 1.f};
    rf_bbox_t charBbox = {0, 0, 8, 8};
    long buffer[1600];

    __rf_test_buffer_ctx_t buffCtx = {buffer, width, height};   

    DEBUG_LOG("Test positive aligned glyph\n");

    __rf_test_clearBuffer(&buffCtx);
    rfont_raster(&rf_ctx, &charPos, 0L, &charBbox, __rf_test_render_func, &buffCtx);
    
    printf("\n------------\n");
    __rf_test_printBuffer(&buffCtx);
    
    DEBUG_LOG("Test negative aligned glyph\n");

    __rf_test_clearBuffer(&buffCtx);

    charPos     = (vec2_t){5.f, 2.f};
    charBbox    = (rf_bbox_t){0, 0, 7, 7};

    rfont_raster(&rf_ctx, &charPos, 1L, &charBbox, __rf_test_render_func, &buffCtx);
   
    printf("\n------------\n");   
    __rf_test_printBuffer(&buffCtx);

    DEBUG_LOG("<<<\n");
}

int main(int argc, char* argv[])
{
    /* unused */
	(void)(argc);
    (void)(argv);

    DEBUG_LOG(">> Start renderer font tests:\n");

    test_r_font_raster_dummy();

    DEBUG_LOG("<< renderer font tests:\n");
	return 0;
}