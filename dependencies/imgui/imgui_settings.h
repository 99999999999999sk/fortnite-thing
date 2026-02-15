#define IMGUI_DEFINE_MATH_OPERATORS
#include <d3d11.h>
#include "imgui_internal.h"

inline bool draw_grid = false;

inline ImVec2 settings_win_pos;

inline ImVec2 win_pos;

inline ImColor child_color = ImColor( 22 , 22 , 24 , 255 );

inline ImColor child_color_title = ImColor( 19 , 19 , 21 , 255 );

inline ImColor child_stroke_color( 1.f , 1.f , 1.f , 0.07f );

inline ImColor main_color( 155 , 150 , 219 , 255 );

inline ImColor background_color( 230 , 233 , 238 , 255 );

inline ImColor second_color( 250 , 253 , 258 , 255 );

inline ImColor third_color( 200 , 203 , 208 , 255 );

inline ImColor text_color_active( 245 , 245 , 245 , 255 );

inline ImColor text_color_default( 245 , 245 , 245 , 45 );

inline ImColor winbg_color( 16 , 16 , 18 , 255 );

inline ImColor stroke_color( 1.f , 1.f , 1.f , 0.15f );

inline ImFont* semibold_font;

inline ImFont* medium_little_font;

inline ImFont* medium_small_font;

inline ImFont* icon_font_small;

inline ImFont* icon_font_regular;

inline ImFont* icon_font_medium;

inline ImFont* icon_font_big;

inline ImVec2 frame_size( 212 , 13 );

inline float anim_speed = 8.f;

static int dnd_counter = 0;

static bool checkbox[ 100 ];
static int slider_int[ 100 ];
static bool child_active[ 100 ];
static int combo[ 10 ];
static int keybind[ 10 ];

static ImVec4 mouse_circle_col;
static ImVec2 mouse_circle_pos;
static float mouse_circle_radius;

inline const char* multicombo_flags[] = { "IN_ATTACK", "IN_JUMP", "IN_DUCK", "IN_RUN", "IN_ZOOM" };

static bool multi[ 10 ][ 5 ];

inline float color_edit[4] = {0};

inline bool image_loaded = false;

inline ID3D11ShaderResourceView* bg = nullptr;
inline ID3D11ShaderResourceView* model_tx = nullptr;

inline ImVec2 center_text( ImVec2 min , ImVec2 max , const char* text )
{
    return min + ( max - min ) / 2 - ImGui::CalcTextSize( text ) / 2;
}

inline ImColor GetColorWithAlpha( ImColor color , float alpha )
{
    return ImColor( color.Value.x , color.Value.y , color.Value.z , alpha );
}

inline ImVec4 ImColorToImVec4( const ImColor& color )
{
    return ImVec4( color.Value.x , color.Value.y , color.Value.z , color.Value.w );
}

inline int rotation_start_index;
inline void ImRotateStart( )
{
    rotation_start_index = ImGui::GetWindowDrawList( )->VtxBuffer.Size;
}

inline ImVec2 ImRotationCenter( )
{
    ImVec2 l( FLT_MAX , FLT_MAX ) , u( -FLT_MAX , -FLT_MAX ); // bounds

    const auto& buf = ImGui::GetWindowDrawList( )->VtxBuffer;
    for ( int i = rotation_start_index; i < buf.Size; i++ )
        l = ImMin( l , buf[ i ].pos ) , u = ImMax( u , buf[ i ].pos );

    return ImVec2( ( l.x + u.x ) / 2 , ( l.y + u.y ) / 2 ); // or use _ClipRectStack?
}


inline void ImRotateEnd( float rad , ImVec2 center = ImRotationCenter( ) )
{
    float s = sin( rad ) , c = cos( rad );
    center = ImRotate( center , s , c ) - center;

    auto& buf = ImGui::GetWindowDrawList( )->VtxBuffer;
    for ( int i = rotation_start_index; i < buf.Size; i++ )
        buf[ i ].pos = ImRotate( buf[ i ].pos , s , c ) - center;
}
