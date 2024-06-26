#include "r_font.h"

#include "debug.h"

#include <ctype.h>
#include <assert.h>

#include "font_provider_default.h"

typedef struct {
    long *buffer;
    long width;
    long height;
    Vec2 charPos;
} __rf_test_buffer_ctx_t;

static void __rf_test_render_func(float const * const x, float const * const y, void *data)
{
    __rf_test_buffer_ctx_t *ctx = data;
    Vec2 *charPos = &ctx->charPos;

    long used_x = charPos->x + *x;
    long used_y = ctx->height - *y - charPos->y;

    if ( (used_x < 0 ) || ( used_y < 0) || ( used_y >= ctx->height) || ( used_x >= ctx->width) ) return;
    
    ctx->buffer[(used_y * ctx->width) + used_x] = 1;
}

/* GLYPH 1 TEST */
/*static const RFBBox bbox_1 = {0l, 0l, 100l, 100l};
static const size_t cntOutlinePts1_1 = 5;
static const size_t cntOutlinePts1_2 = 5;
static Vec2 outlinePts1_1[5] = {
    {0.f, 0.f}, {0.f, 100.f}, {100.f, 100.f}, {100.f, 0.f}, {0.f, 0.f},
};
static Vec2 outlinePts1_2[5] = {
    {10.f, 10.f}, {90.f, 10.f}, {90.f, 90.f}, {10.f, 90.f}, {10.f, 10.f} 
};

static rf_outlines_t glyph1_outlines[] = { 
    { &outlinePts1_1[0], cntOutlinePts1_1},  
    { &outlinePts1_2[0], cntOutlinePts1_2},
    { NULL, 0 },
};
*/

//oPts[charcode in hex]
static Vec2 oPts1[] = {
    {0.f, 0.f}, {0.f, 100.f}, {100.f, 100.f}, {100.f, 0.f}, {0.f, 0.f},{10.f, 10.f}, {90.f, 10.f}, {90.f, 90.f}, {10.f, 90.f}, {10.f, 10.f} 
};

static RFGlyph __rf_test_glyph = {{0l, 0l, 100l, 100l}, 10, &oPts1[0]};

/* GLYPH 2 TEST */

/*static const RFBBox bbox_2 = {0l, 0l, 100l, 100l};
static const size_t cntOutlinePts2_1 = 5;
static const size_t cntOutlinePts2_2 = 5;
static const size_t cntOutlinePts2_3 = 5;
static Vec2 outlinePts2_1[5] = {
    {0.f, 0.f}, {0.f, 100.f}, {100.f, 100.f}, {100.f, 0.f}, {0.f, 0.f}
};
static Vec2 outlinePts2_2[5] = {
    {10.f, 10.f}, {90.f, 10.f}, {90.f, 25.f}, {10.f, 25.f}, {10.f, 10.f}
};
static Vec2 outlinePts2_3[5] = {
    {10.f, 35.f}, {90.f, 35.f}, {90.f, 90.f}, {10.f, 90.f}, {10.f, 35.f} 
};

static rf_outlines_t glyph2_outlines[] = { 
    { &outlinePts2_1[0], cntOutlinePts2_1},  
    { &outlinePts2_2[0], cntOutlinePts2_2},
    { &outlinePts2_3[0], cntOutlinePts2_3},
   
    { NULL, 0 },
};
*/

//oPts[charcode in hex]
static Vec2 oPts2[] = {
    {298.f, 842.f}, {260.f, 256.f}, {172.f, 256.f}, {131.f, 842.f}, 
    {131.f, 1088.f}, {298.f, 1088.f}, {298.f, 842.f}, {290.f, 0.f}, 
    {137.f, 0.f}, {137.f, 128.f}, {290.f, 128.f}, {290.f, 0.f} 
};

static RFGlyph __rf_test_glyph_exclamation_mark = {{131l, 0l, 298l, 1088l}, 12, &oPts2[0]};

/* the E Glyph */

static Vec2 oPts3[] = {
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

static RFGlyph __rf_test_glyph_upper_e = {{34, 0, 905, 1088}, 13, &oPts3[0]};

/* glyph ( */
static Vec2 oPts4[] = {
    {456.f, -320.f},
    {359.f, -320.f},
    {282.f, -213.f},
    {217.f, -100.f},
    {163.f,   19.f},
    {124.f,  141.f},
    {100.f,  263.f},
    { 92.f,  384.f},
    {100.f,  517.f},
    {126.f,  643.f},
    {168.f,  763.f},
    {223.f,  877.f},
    {287.f,  985.f},
    {359.f, 1088.f},
    {456.f, 1088.f},
    {377.f,  951.f},
    {317.f,  826.f},
    {277.f,  712.f},
    {252.f,  604.f},
    {237.f,  495.f},
    {232.f,  385.f},
    {257.f,  149.f},
    {332.f,  -86.f},
    {456.f, -320.f}
};

static RFGlyph __rf_test_glyph_left_parenthesis_ = {{92, -320, 456, 1088}, 24, &oPts4[0]};

/* Glyph @ */
static Vec2 oPts5[] = {
    {1507.f,     0.f},
    {1443.f,   -90.f},
    {1358.f,  -168.f},
    {1253.f,  -232.f},
    {1131.f,  -281.f},
    { 995.f,  -310.f},
    { 845.f,  -320.f},
    { 621.f,  -300.f},
    { 433.f,  -240.f},
    { 283.f,  -140.f},
    { 174.f,    -7.f},
    { 108.f,   152.f},
    {  86.f,   338.f},
    {  96.f,   463.f},
    { 125.f,   586.f},
    { 174.f,   707.f},
    { 241.f,   818.f},
    { 326.f,   911.f},
    { 429.f,   987.f},
    { 548.f,  1043.f},
    { 681.f,  1077.f},
    { 828.f,  1088.f},
    {1010.f,  1069.f},
    {1165.f,  1014.f},
    {1295.f,   921.f},
    {1392.f,   803.f},
    {1451.f,   669.f},
    {1470.f,   521.f},
    {1454.f,   389.f},
    {1407.f,   268.f},
    {1328.f,   157.f},
    {1231.f,    70.f},
    {1128.f,    17.f},
    {1019.f,     0.f},
    { 941.f,    13.f},
    { 893.f,    54.f},
    { 874.f,   121.f},
    { 800.f,    54.f},
    { 723.f,    13.f},
    { 644.f,     0.f},
    { 571.f,    10.f},
    { 507.f,    41.f},
    { 450.f,    92.f},
    { 407.f,   158.f},
    { 381.f,   233.f},
    { 372.f,   317.f},
    { 385.f,   425.f},
    { 425.f,   528.f},
    { 491.f,   625.f},
    { 574.f,   704.f},
    { 664.f,   752.f},
    { 761.f,   768.f},
    { 849.f,   755.f},
    { 925.f,   716.f},
    { 988.f,   652.f},
    {1014.f,   768.f},
    {1148.f,   768.f},
    {1085.f,   484.f},
    {1045.f,   304.f},
    {1027.f,   226.f},
    {1021.f,   202.f},
    {1018.f,   184.f},
    {1017.f,   170.f},
    {1019.f,   158.f},
    {1025.f,   147.f},
    {1034.f,   139.f},
    {1045.f,   133.f},
    {1057.f,   129.f},
    {1069.f,   128.f},
    {1103.f,   134.f},
    {1143.f,   152.f},
    {1189.f,   182.f},
    {1235.f,   222.f},
    {1276.f,   270.f},
    {1311.f,   325.f},
    {1338.f,   386.f},
    {1354.f,   450.f},
    {1359.f,   517.f},
    {1342.f,   639.f},
    {1290.f,   745.f},
    {1204.f,   836.f},
    {1093.f,   905.f},
    { 965.f,   946.f},
    { 822.f,   960.f},
    { 650.f,   940.f},
    { 500.f,   881.f},
    { 372.f,   783.f},
    { 275.f,   657.f},
    { 216.f,   512.f},
    { 197.f,   350.f},
    { 216.f,   188.f},
    { 275.f,    54.f},
    { 372.f,   -52.f},
    { 502.f,  -130.f},
    { 659.f,  -176.f},
    { 843.f,  -192.f},
    { 959.f,  -186.f},
    {1064.f,  -169.f},
    {1160.f,  -141.f},
    {1244.f,  -103.f},
    {1315.f,   -56.f},
    {1372.f,     0.f},
    {1507.f,     0.f},
    { 671.f,   128.f},
    { 737.f,   140.f},
    { 800.f,   174.f},
    { 859.f,   232.f},
    { 907.f,   305.f},
    { 935.f,   385.f},
    { 945.f,   472.f},
    { 939.f,   521.f},
    { 923.f,   563.f},
    { 895.f,   596.f},
    { 859.f,   620.f},
    { 818.f,   635.f},
    { 771.f,   640.f},
    { 700.f,   628.f},
    { 638.f,   592.f},
    { 584.f,   532.f},
    { 543.f,   460.f},
    { 518.f,   388.f},
    { 510.f,   315.f},
    { 515.f,   262.f},
    { 531.f,   216.f},
    { 558.f,   178.f},
    { 592.f,   150.f},
    { 630.f,   134.f},
    { 671.f,   128.f}
};
static RFGlyph __rf_test_glyph_commercial_add = {{86, -320, 1507, 1088}, 128, &oPts5[0]};


RFGlyph* __rf_test_glyph_get( unsigned long charcode ) 
{
    switch (charcode)
    {
        //case 0: return &__rf_test_glyph; break;
        //case 1: return &__rf_test_glyph_neg; break;
        case 2: return &__rf_test_glyph_exclamation_mark; break;
        case 3: return &__rf_test_glyph_upper_e; break;
        case 4: return &__rf_test_glyph_left_parenthesis_; break;
        case 5: return &__rf_test_glyph_commercial_add; break;
        default: return &__rf_test_glyph; break;
    }
    
}

static RFGlyphContainer __rf_test_glc = {NULL, __rf_test_glyph_get, {0, -320, 1507, 1088}};

RFGlyphContainer* __rf_test_glcon_get() 
{
    return &__rf_test_glc;
}

static void __rf_test_glcon_free(RFGlyphContainer** container) 
{
    (void)(container);
}

static void __rf_test_clearBuffer(__rf_test_buffer_ctx_t *_buffCtx)
{
    __rf_test_buffer_ctx_t * buffCtx = _buffCtx;
    size_t max = buffCtx->width * buffCtx->height;
    for ( size_t i = 0; i < max; ++i  ) {
        buffCtx->buffer[i] = 0;
    }
}

#ifdef debug
static void __rf_test_printBuffer(__rf_test_buffer_ctx_t *_buffCtx)
{
    __rf_test_buffer_ctx_t * buffCtx = _buffCtx;
    size_t max = buffCtx->width * buffCtx->height;

    for( size_t i = 0; i < max; ++i )
    {
        if ( (i > 0) && ((i % buffCtx->width) == 0) ) printf("\n");
        printf("%ld ", buffCtx->buffer[i]);
    }
    printf("\n\n");
}
#endif

static int __rf_test_data = 42;
void __rf_test_init_fn(void *_provider, void *init_data)
{
    RFProvider *provider = (RFProvider *)_provider;
    int *data = (int *)init_data;

    assert(provider != NULL);
    assert(*data == __rf_test_data);
}

static INIT_PROVIDER_FN __rf_test_get_init_fn()
{
    return __rf_test_init_fn;
} 

static void test_r_font_raster_dummy() 
{
    DEBUG_LOG(">>>\n");

    RFProviderInit provider_init;
    provider_init.get_init_fn = __rf_test_get_init_fn;
    provider_init.init_data = &__rf_test_data;

    RFProvider provider = { &provider_init, __rf_test_glcon_get, __rf_test_glcon_free};

    RFCtx rf_ctx;
    rfont_init(&rf_ctx, &provider);

    size_t width = 40;
    size_t height = 40;

    float charwidth = 35.f;

    long buffer[1600];

    __rf_test_buffer_ctx_t buffCtx = {buffer, width, height, (Vec2){5.f, 10.f}};   

    __rf_test_clearBuffer(&buffCtx);
    rfont_raster(&rf_ctx, 2, charwidth, __rf_test_render_func, &buffCtx);

    #ifdef debug
    __rf_test_printBuffer(&buffCtx);
    #endif

    __rf_test_clearBuffer(&buffCtx);
    rfont_raster(&rf_ctx, 3, charwidth, __rf_test_render_func, &buffCtx);

    #ifdef debug
    __rf_test_printBuffer(&buffCtx);
    #endif

    __rf_test_clearBuffer(&buffCtx);
    rfont_raster(&rf_ctx, 4, charwidth, __rf_test_render_func, &buffCtx);

    #ifdef debug
    __rf_test_printBuffer(&buffCtx);
    #endif

    __rf_test_clearBuffer(&buffCtx);
    rfont_raster(&rf_ctx, 5, charwidth, __rf_test_render_func, &buffCtx);

    #ifdef debug
    __rf_test_printBuffer(&buffCtx);
    #endif

    DEBUG_LOG("<<<\n");
}

static void test_r_font_raster_default_provider() 
{
    DEBUG_LOG(">>>\n");

    RFProvider* provider = get_default_provider();

    RFCtx rf_ctx;
    rfont_init(&rf_ctx, provider);

    size_t width = 40;
    size_t height = 40;

    float charwidth = 35.f;

    long buffer[1600];

    __rf_test_buffer_ctx_t buffCtx = {buffer, width, height, (Vec2){0.f, 7.f}};   

    __rf_test_clearBuffer(&buffCtx);
    rfont_raster(&rf_ctx, 220, charwidth, __rf_test_render_func, &buffCtx);

    buffCtx.charPos = (Vec2){22.f, 7.f};
    rfont_raster(&rf_ctx, 252, charwidth, __rf_test_render_func, &buffCtx);
    
    #ifdef debug
    DEBUG_LOG("\n\n");
    __rf_test_printBuffer(&buffCtx);
    #endif

    __rf_test_clearBuffer(&buffCtx);
    buffCtx.charPos = (Vec2){0.f, 9.f};
    rfont_raster(&rf_ctx, 71, charwidth, __rf_test_render_func, &buffCtx);

    buffCtx.charPos = (Vec2){22.f, 9.f};
    rfont_raster(&rf_ctx, 103, charwidth, __rf_test_render_func, &buffCtx);
    
    #ifdef debug
    DEBUG_LOG("\n\n");
    __rf_test_printBuffer(&buffCtx);
    #endif

    __rf_test_clearBuffer(&buffCtx);
    buffCtx.charPos = (Vec2){5.f, 10.f};
    rfont_raster(&rf_ctx, 64, charwidth, __rf_test_render_func, &buffCtx);

    #ifdef debug
    DEBUG_LOG("\n\n");
    __rf_test_printBuffer(&buffCtx);
    DEBUG_LOG("\n");
    #endif
    DEBUG_LOG("<<<\n");
}

static void test_r_text_raster_default_provider()
{
    DEBUG_LOG(">>>\n");

    RFProvider* provider = get_default_provider();

    RFCtx rf_ctx;
    rfont_init(&rf_ctx, provider);

    size_t width = 120;
    size_t height = 40;

    float charwidth = 32.f;

    long buffer[4800];

    __rf_test_buffer_ctx_t buffCtx = {buffer, width, height, (Vec2){3.f, 10.f}};   

    __rf_test_clearBuffer(&buffCtx);
    rfont_raster_text(&rf_ctx, (unsigned char const * const)"yg]|ßüöä", charwidth, __rf_test_render_func, &buffCtx);

    #ifdef debug
    DEBUG_LOG("\n");
    __rf_test_printBuffer(&buffCtx);
    #endif

    __rf_test_clearBuffer(&buffCtx);
    rfont_raster_text(&rf_ctx, (unsigned char const * const)"§ÜÖÄ€", charwidth, __rf_test_render_func, &buffCtx);

    #ifdef debug
    DEBUG_LOG("\n");
    __rf_test_printBuffer(&buffCtx);
    #endif

    DEBUG_LOG("<<<\n");
}

int main(int argc, char* argv[])
{
    /* unused */
	(void)(argc);
    (void)(argv);

    DEBUG_LOG(">> Start renderer font tests:\n");

    test_r_font_raster_dummy();

    test_r_font_raster_default_provider();

    test_r_text_raster_default_provider();
 
    DEBUG_LOG("<< renderer font tests:\n");
	return 0;
}