#include "r_font.h"

#include "debug.h"

#include <ctype.h>
#include <assert.h>

/*

typedef struct {
    rf_bbox_t bbox;
    dl_list_t *outlines;
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

void __rf_test_render_func(long const * const x, long const * const y, void *data)
{
    printf("r:= x: %ld y: %ld\n", *x, *y);
}

static const rf_bbox_t bbox_1 = {0l, 0l, 512l, 768l};
static const rf_bbox_t bbox_2 = {-40l, -150l, 512l, 768l};

static rf_glyph_t __rf_test_glyph = { bbox_1, NULL};
static rf_glyph_t __rf_test_glyph_neg = { bbox_2, NULL};

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

void __rf_test_glcon_free(rf_glyph_container_t** container) 
{

}

static void test_r_font_raster_dummy() 
{
    DEBUG_LOG(">>>\n");

    rf_provider_t provider = { __rf_test_glcon_get, __rf_test_glcon_free};

    rf_ctx_t rf_ctx;
    rfont_init(&rf_ctx, &provider);

    vec2_t charPos = {30.f, 30.f};
    rf_bbox_t charBbox = {0L,0L, 40L, 40L};

    DEBUG_LOG("Test positive aligned glyph\n");

    rfont_raster(&rf_ctx, &charPos, 0L, &charBbox, __rf_test_render_func, NULL);

    DEBUG_LOG("Test negative aligned glyph\n");

    rfont_raster(&rf_ctx, &charPos, 1L, &charBbox, __rf_test_render_func, NULL);

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