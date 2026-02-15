namespace enviroment {
    void render( lists::c_loot const pickup ) {
        auto draw_list = ImGui::GetBackgroundDrawList( );

        if ( config::world::enabled ) {
            auto rarity = pickup.m_pickup_rarity;
            auto text_color = g_drawing->get_tier_color( rarity );
            if ( !config::world::loot_tier[ 0 ] && rarity == engine::e_fort_rarity::common ) return;
            if ( !config::world::loot_tier[ 1 ] && rarity == engine::e_fort_rarity::uncommon ) return;
            if ( !config::world::loot_tier[ 2 ] && rarity == engine::e_fort_rarity::rare ) return;
            if ( !config::world::loot_tier[ 3 ] && rarity == engine::e_fort_rarity::epic ) return;
            if ( !config::world::loot_tier[ 4 ] && rarity == engine::e_fort_rarity::legendary ) return;

            if ( config::radar::enabled ) {
                g_radar->render( pickup );
            }

            auto world_location = pickup.m_world_location;
            auto world_screen = pickup.m_world_screen;
            if ( world_screen.in_screen( ) ) {
                auto& pickup_name = pickup.m_pickup_name;
                auto text_format = config::world::show_distance ? std::format( "{} ({:.1f}m)", pickup_name, pickup.m_distance ) :
                    std::format( "{}", pickup_name );

                auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );
                auto text_pos = ImVec2(
                    world_screen.x - ( text_size.x / 2.0f ),
                    world_screen.y
                );

                auto outline_color = ImColor( 0, 0, 0, 255 );
                for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                    for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                        if ( dx == 0.0f && dy == 0.0f ) continue;
                        draw_list->AddText(
                            ImVec2( text_pos.x + dx, text_pos.y + dy ),
                            outline_color,
                            text_format.c_str( )
                        );
                    }
                }

                if ( config::world::mark_pickups ) {
                    ImGui::PushFont( icon_font_big );
                    auto icon_size = ImGui::CalcTextSize( "6" );
                    auto marker_pos = ImVec2(
                        text_pos.x + ( text_size.x / 2.0f ) - ( icon_size.x / 2.0f ),
                        text_pos.y - icon_size.y - 2.0f
                    );
                    draw_list->AddText( marker_pos, text_color, "6" );
                    ImGui::PopFont( );
                }

                draw_list->AddText( text_pos, text_color, text_format.c_str( ) );
            }
        }
    }
}