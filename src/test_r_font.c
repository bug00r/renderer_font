#include "r_font.h"

#include "debug.h"

#include <ctype.h>
#include <assert.h>

#include "font_provider_default.h"
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
    long width;
    long height;
    vec2_t charPos;
} __rf_test_buffer_ctx_t;

static void __rf_test_render_func(long const * const x, long const * const y, void *data)
{
    __rf_test_buffer_ctx_t *ctx = data;
    vec2_t *charPos = &ctx->charPos;

    //printf("raw r:= x: %ld y: %ld\n", *x, *y);

    long used_x = charPos->x + *x;
    long used_y = ctx->height - *y - charPos->y;

    //printf("used=> r:= x: %ld y: %ld\n", used_x, used_y);
    if ( (used_x < 0 ) || ( used_y < 0) || ( used_y >= ctx->height) || ( used_x >= ctx->width) ) return;
    
    ctx->buffer[(used_y * ctx->width) + used_x] = 1;
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

static rf_outlines_t glyph1_outlines[] = { 
    { &outlinePts1_1[0], cntOutlinePts1_1},  
    { &outlinePts1_2[0], cntOutlinePts1_2},
    /* Senitinel */
    { NULL, 0 },
};

//static rf_glyph_t __rf_test_glyph = { bbox_1, &glyph1_outlines[0]};

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

static rf_outlines_t glyph2_outlines[] = { 
    { &outlinePts2_1[0], cntOutlinePts2_1},  
    { &outlinePts2_2[0], cntOutlinePts2_2},
    { &outlinePts2_3[0], cntOutlinePts2_3},
    /* Senitinel */
    { NULL, 0 },
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

static rf_outlines_t glyph3_outlines[] = { 
    { &outlinePts3_1[0], cntOutlinePts3_1},  
    { &outlinePts3_2[0], cntOutlinePts3_2},
    /* Senitinel */
    { NULL, 0 },
};

/* the E Glyph */
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

static rf_outlines_t glyph4_outlines[] = { 
    { &outlinePts4_1[0], cntOutlinePts4_1},
    /* Senitinel */
    { NULL, 0 },
};

/* glyph ( */

static const rf_bbox_t bbox_5 = {92, -320, 456, 1088};
static const size_t cntOutlinePts5_1 = 24;

static vec2_t outlinePts5_1[24] = {
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

static rf_outlines_t glyph5_outlines[] = { 
    { &outlinePts5_1[0], cntOutlinePts5_1},
        /* Senitinel */
    { NULL, 0 },
};

/* Glyph @ */
static const rf_bbox_t bbox_6 = {86, -320, 1507, 1088};
static const size_t cntOutlinePts6_1 = 103;
static vec2_t outlinePts6_1[103] = {
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
    {1507.f,     0.f}
};
static const size_t cntOutlinePts6_2 = 25;
static vec2_t outlinePts6_2[25] = {
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

static rf_outlines_t glyph6_outlines[] = { 
    { &outlinePts6_1[0], cntOutlinePts6_1},  
    { &outlinePts6_2[0], cntOutlinePts6_2},
    /* Senitinel */
    { NULL, 0 },
};

static rf_glyph_t __rf_test_glyph = { bbox_1, &glyph1_outlines[0]};
static rf_glyph_t __rf_test_glyph_neg = { bbox_2, &glyph2_outlines[0]};
static rf_glyph_t __rf_test_glyph_exclamation_mark = { bbox_3, &glyph3_outlines[0]};
static rf_glyph_t __rf_test_glyph_upper_e = { bbox_4, &glyph4_outlines[0]};
static rf_glyph_t __rf_test_glyph_left_parenthesis_ = { bbox_5, &glyph5_outlines[0]};
static rf_glyph_t __rf_test_glyph_commercial_add = { bbox_6, &glyph6_outlines[0]};

rf_glyph_t* __rf_test_glyph_get( unsigned long charcode ) 
{
    switch (charcode)
    {
        case 0: return &__rf_test_glyph; break;
        case 1: return &__rf_test_glyph_neg; break;
        case 2: return &__rf_test_glyph_exclamation_mark; break;
        case 3: return &__rf_test_glyph_upper_e; break;
        case 4: return &__rf_test_glyph_left_parenthesis_; break;
        case 5: return &__rf_test_glyph_commercial_add; break;
        default: return &__rf_test_glyph; break;
    }
    
}

/*
static const rf_bbox_t bbox_1 = {0l, 0l, 100l, 100l};
static const rf_bbox_t bbox_2 = {0l, 0l, 100l, 100l};
static const rf_bbox_t bbox_3 = {131l, 0l, 298l, 1088l};
static const rf_bbox_t bbox_4 = {34, 0, 905, 1088};
static const rf_bbox_t bbox_5 = {92, -320, 456, 1088};
static const rf_bbox_t bbox_6 = {86, -320, 1507, 1088};
*/

static rf_glyph_container_t __rf_test_glc = {NULL, __rf_test_glyph_get, {0, -320, 1507, 1088}};

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
    for ( size_t i = 0; i < max; ++i  ) {
        buffCtx->buffer[i] = 0;
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

static int __rf_test_data = 42;
void __rf_test_init_fn(void *_provider, void *init_data)
{
    rf_provider_t *provider = (rf_provider_t *)_provider;
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

    rf_provider_init_t provider_init;
    provider_init.get_init_fn = __rf_test_get_init_fn;
    provider_init.init_data = &__rf_test_data;

    rf_provider_t provider = { &provider_init, __rf_test_glcon_get, __rf_test_glcon_free};

    rf_ctx_t rf_ctx;
    rfont_init(&rf_ctx, &provider);

    size_t width = 40;
    size_t height = 40;

    rf_bbox_t charBbox = {0, 0, 30, 30};

    long buffer[1600];

    __rf_test_buffer_ctx_t buffCtx = {buffer, width, height, (vec2_t){5.f, 10.f}};   

    __rf_test_clearBuffer(&buffCtx);
    rfont_raster(&rf_ctx, 5, &charBbox, __rf_test_render_func, &buffCtx);

    __rf_test_printBuffer(&buffCtx);

    DEBUG_LOG("<<<\n");
}

static void test_r_font_raster_default_provider() 
{
    DEBUG_LOG(">>>\n");

    rf_provider_t* provider = get_default_provider();

    rf_ctx_t rf_ctx;
    rfont_init(&rf_ctx, provider);

    size_t width = 40;
    size_t height = 40;

    rf_bbox_t charBbox = {0, 0, 25, 25};

    long buffer[1600];

    __rf_test_buffer_ctx_t buffCtx = {buffer, width, height, (vec2_t){0.f, 7.f}};   

    __rf_test_clearBuffer(&buffCtx);
    rfont_raster(&rf_ctx, 220, &charBbox, __rf_test_render_func, &buffCtx);
    
    buffCtx.charPos = (vec2_t){20.f, 7.f};
    rfont_raster(&rf_ctx, 252, &charBbox, __rf_test_render_func, &buffCtx);
    
    printf("\n\n");
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

    test_r_font_raster_default_provider();

    DEBUG_LOG("<< renderer font tests:\n");
	return 0;
}