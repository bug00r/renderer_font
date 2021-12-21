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
    __rf_test_buffer_ctx_t *ctx = data;
    //printf("r:= x: %ld y: %ld\n", *x, *y);
    if ( (*x < 0 ) || ( *y < 0) || ( *y >= ctx->height) || ( *x >= ctx->width) ) return;
    
    ctx->buffer[(*y * ctx->width) + *x] = 1L;
}

/* GLYPH 1 TEST */
static const rf_bbox_t bbox_1 = {0l, 0l, 100l, 100l};
static const size_t cntOutlinePts1_1 = 5;
static const size_t cntOutlinePts1_2 = 5;
static vec2_t outlinePts1_1[5] = {
    {0.f, 0.f}, {0.f, 100.f}, {100.f, 100.f}, {100.f, 0.f}, {0.f, 0.f},
};
static vec2_t outlinePts1_2[5] = {
    {10.f, 10.f}, {90.f, 10.f}, {90.f, 90.f}, {10.f, 90.f}, {10.f, 10.f} 
};

static const size_t cntOutlines1 = 2;
static rf_outlines_t glyph1_outlines[2] = { 
    { &outlinePts1_1[0], cntOutlinePts1_1},  
    { &outlinePts1_2[0], cntOutlinePts1_2}
};

/* GLYPH 2 TEST */

static const rf_bbox_t bbox_2 = {0l, 0l, 100l, 100l};
static const size_t cntOutlinePts2_1 = 5;
static const size_t cntOutlinePts2_2 = 5;
static const size_t cntOutlinePts2_3 = 5;
static vec2_t outlinePts2_1[5] = {
    {0.f, 0.f}, {0.f, 100.f}, {100.f, 100.f}, {100.f, 0.f}, {0.f, 0.f}
};
static vec2_t outlinePts2_2[5] = {
    {10.f, 10.f}, {90.f, 10.f}, {90.f, 25.f}, {10.f, 25.f}, {10.f, 10.f}
};
static vec2_t outlinePts2_3[5] = {
    {10.f, 35.f}, {90.f, 35.f}, {90.f, 90.f}, {10.f, 90.f}, {10.f, 35.f} 
};

static const size_t cntOutlines2 = 3;
static rf_outlines_t glyph2_outlines[3] = { 
    { &outlinePts2_1[0], cntOutlinePts2_1},  
    { &outlinePts2_2[0], cntOutlinePts2_2},
    { &outlinePts2_3[0], cntOutlinePts2_3}
};

/* the ! Glyph */
static const rf_bbox_t bbox_3 = {131l, 0l, 298l, 1088l};
static const size_t cntOutlinePts3_1 = 7;
static const size_t cntOutlinePts3_2 = 5;

static vec2_t outlinePts3_1[7] = {
    {298.f, 842.f}, {260.f, 256.f}, {172.f, 256.f}, {131.f, 842.f}, 
    {131.f, 1088.f}, {298.f, 1088.f}, {298.f, 842.f}
};

static vec2_t outlinePts3_2[5] = {
    {290.f, 0.f}, {137.f, 0.f}, {137.f, 128.f}, {290.f, 128.f}, {290.f, 0.f} 
};

static const size_t cntOutlines3 = 2;
static rf_outlines_t glyph3_outlines[2] = { 
    { &outlinePts3_1[0], cntOutlinePts3_1},  
    { &outlinePts3_2[0], cntOutlinePts3_2}
};

/* the E Glyph */
/*BBOX: xmin/ymin xmax/ymax: 122/0 943/1088
move_to (x/y): 943/0
line_to (x/y): 122/0
line_to (x/y): 122/1088
line_to (x/y): 917/1088
line_to (x/y): 917/960
line_to (x/y): 267/960
line_to (x/y): 267/640
line_to (x/y): 875/640
line_to (x/y): 875/512
line_to (x/y): 267/512
line_to (x/y): 267/128
line_to (x/y): 943/128
line_to (x/y): 943/0
*/
static const rf_bbox_t bbox_4 = {34, 0, 905, 1088};
static const size_t cntOutlinePts4_1 = 13;

static vec2_t outlinePts4_1[13] = {
    {943.f, 0.f}, 
    {122.f, 0.f}, 
    {122.f, 1088.f}, 
    {917.f, 1088.f}, 
    {917.f, 960.f}, 
    {267.f, 960.f}, 
    {267.f, 640.f}, 
    {875.f, 640.f}, 
    {875.f, 512.f},
    {267.f, 512.f}, 
    {267.f, 128.f}, 
    {943.f, 128.f}, 
    {943.f, 0.f}
};

static const size_t cntOutlines4 = 1;
static rf_outlines_t glyph4_outlines[1] = { 
    { &outlinePts4_1[0], cntOutlinePts4_1}
};

/* glyph ( */

static const rf_bbox_t bbox_5 = {92, -320, 456, 1088};
static const size_t cntOutlinePts5_1 = 24;

static vec2_t outlinePts5_1[24] = {
{       456.f, -320.f},
{       359.f, -320.f},
{282.111115f, -213.444458f},
{216.777771f, -100.444450f},
{163.000000f, 18.999985f},
{123.555550f, 141.333344f},
{ 99.888893f, 263.000000f},
{ 92.000000f, 384.000000f},
{100.444443f, 516.777771f},
{125.777786f, 643.111145f},
{168.000000f, 763.000000f},
{223.000000f, 876.888855f},
{286.666687f, 985.222229f},
{359.000000f, 1088.000000f},
{       456.f, 1088.f},
{377.000000f, 951.111145f},
{317.333344f, 825.777771f},
{277.000000f, 712.000000f},
{252.000000f, 604.111084f},
{237.000000f, 495.111115f},
{232.000000f, 385.000000f},
{256.888885f, 149.333344f},
{331.555542f, -85.666672f},
{456.000000f, -320.f}
};

static const size_t cntOutlines5 = 1;
static rf_outlines_t glyph5_outlines[1] = { 
    { &outlinePts5_1[0], cntOutlinePts5_1}
};


static rf_glyph_t __rf_test_glyph = { bbox_1, &glyph1_outlines[0], cntOutlines1};
static rf_glyph_t __rf_test_glyph_neg = { bbox_2, &glyph2_outlines[0], cntOutlines2};
static rf_glyph_t __rf_test_glyph_exclamation_mark = { bbox_3, &glyph3_outlines[0], cntOutlines3};
static rf_glyph_t __rf_test_glyph_upper_e = { bbox_4, &glyph4_outlines[0], cntOutlines4};
static rf_glyph_t __rf_test_glyph_left_parenthesis_ = { bbox_5, &glyph5_outlines[0], cntOutlines5};

rf_glyph_t* __rf_test_glyph_get( unsigned long charcode ) 
{
    switch (charcode)
    {
        case 0: return &__rf_test_glyph; break;
        case 1: return &__rf_test_glyph_neg; break;
        case 2: return &__rf_test_glyph_exclamation_mark; break;
        case 3: return &__rf_test_glyph_upper_e; break;
        case 4: return &__rf_test_glyph_left_parenthesis_; break;
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

    #ifndef debug
        size_t width = 40;
        size_t height = 40;
    #else
        size_t width = 10;
        size_t height = 10;
    #endif
    
    #ifndef debug
    rf_bbox_t charBbox = {0, 0, 35, 35};
    vec2_t charPos = {10.f, 5.f};
    #else
    rf_bbox_t charBbox = {0, 0, 9, 9};
    vec2_t charPos = {0.f, 0.f};
    #endif

    long buffer[1600];

    __rf_test_buffer_ctx_t buffCtx = {buffer, width, height};   

    DEBUG_LOG("Test positive aligned glyph\n");

    __rf_test_clearBuffer(&buffCtx);
    rfont_raster(&rf_ctx, &charPos, 4, &charBbox, __rf_test_render_func, &buffCtx);

    #ifndef debug
    charPos = (vec2_t){30.f, -1.f};
    #else
    charPos = (vec2_t){8.f, -1.f};
    #endif

    rfont_raster(&rf_ctx, &charPos, 2, &charBbox, __rf_test_render_func, &buffCtx);
    __rf_test_printBuffer(&buffCtx);

    /*printf("\n------------\n");
    DEBUG_LOG("Test negative aligned glyph\n");

    __rf_test_clearBuffer(&buffCtx);

    charPos     = (vec2_t){5.f, 0.f};
    #ifndef debug
    charBbox    = (rf_bbox_t){0, 0, 25, 25};
    #else
    charBbox    = (rf_bbox_t){0, 0, 7, 7};
    #endif

    rfont_raster(&rf_ctx, &charPos, 1L, &charBbox, __rf_test_render_func, &buffCtx);
   
    printf("\n------------\n");   
    __rf_test_printBuffer(&buffCtx);
    */
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