namespace radar {
    class c_radar {
    public:
        void draw_background( ) {
            auto draw_list = ImGui::GetForegroundDrawList( );

            auto radar_pos = ImVec2( config::radar::position_x, config::radar::position_y );
            auto radar_size = config::radar::size;

            auto radar_background_color = ImColor( 0, 0, 0, ( int )( config::radar::opacity ) );
            draw_list->AddRectFilled(
                radar_pos,
                ImVec2( radar_pos.x + radar_size, radar_pos.y + radar_size ),
                radar_background_color
            );

            draw_list->AddRect(
                radar_pos,
                ImVec2( radar_pos.x + radar_size, radar_pos.y + radar_size ),
                ImColor( 5, 5, 5, 255 ),
                0.0f,
                0,
                1.0f
            );

            auto radar_center = ImVec2(
                radar_pos.x + radar_size / 2.0f,
                radar_pos.y + radar_size / 2.0f
            );

            float purple_line_y = radar_pos.y - 2.0f;
            draw_list->AddLine(
                ImVec2( radar_pos.x, purple_line_y ),
                ImVec2( radar_pos.x + radar_size, purple_line_y ),
                ImColor( 144, 154, 251, 255 ),
                2.0f
            );

            if ( config::radar::grid ) {
                draw_grid_overlay( draw_list, radar_pos, radar_size );
            }
            else {
                auto line_color = ImColor( 5, 5, 5, 255 );
                draw_list->AddLine(
                    ImVec2( radar_pos.x, radar_center.y ),
                    ImVec2( radar_pos.x + radar_size, radar_center.y ),
                    line_color,
                    1.0f
                );

                draw_list->AddLine(
                    ImVec2( radar_center.x, radar_pos.y ),
                    ImVec2( radar_center.x, radar_pos.y + radar_size ),
                    line_color,
                    1.0f
                );
            }

            float fov = g_world->m_camera_fov;
            float half_fov_radians = ( fov / 2.0f ) * std::numbers::pi / 180.0f;

            float distance_to_top_edge = radar_center.y - radar_pos.y;

            float x_offset = distance_to_top_edge * tan( half_fov_radians );
            auto left_line_end = ImVec2( radar_center.x - x_offset, radar_pos.y );
            auto right_line_end = ImVec2( radar_center.x + x_offset, radar_pos.y );

            left_line_end.x = std::clamp( left_line_end.x, radar_pos.x, radar_pos.x + radar_size );
            right_line_end.x = std::clamp( right_line_end.x, radar_pos.x, radar_pos.x + radar_size );

            ImColor fov_fill_color( 195, 177, 225, ( int )( config::radar::opacity - 65 ) );
            draw_list->AddTriangleFilled(
                radar_center,
                left_line_end,
                right_line_end,
                fov_fill_color
            );

            draw_list->AddLine( radar_center, left_line_end, ImColor( 239, 187, 255, 255 ), 1.0f );
            draw_list->AddLine( radar_center, right_line_end, ImColor( 239, 187, 255, 255 ), 1.0f );

            draw_list->AddCircleFilled(
                radar_center,
                3.0f,
                ImColor( 195, 177, 255, 255 ),
                15
            );
        }

        void render( lists::c_actor player ) {
            auto draw_list = ImGui::GetForegroundDrawList( );

            auto radar_location = player.m_base_location;
            if ( !radar_location.is_valid( ) ) return;

            engine::f_vector2d radar_screen{};
            if ( config::radar::rotation_mode ) {
                rotate_point_with_player_rotation( &radar_screen, radar_location );
            }
            else {
                rotate_point( &radar_screen, radar_location );
            }

            draw_list->AddCircleFilled(
                radar_screen.vec( ),
                3,
                ImColor( 142, 130, 209, 255 ),
                15
            );

            if ( config::radar::line_of_sight ) {
                draw_line_of_sight( draw_list, radar_screen, player );
            }

            if ( config::radar::distance ) {
                auto text_format = std::format( "({:.1f}m)", player.m_distance );
                auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

                auto text_pos = ImVec2(
                    radar_screen.x - ( text_size.x / 2.0f ),
                    radar_screen.y + 2.0
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

                draw_list->AddText( text_pos, ImColor( 142, 130, 209, 255 ), text_format.c_str( ) );
            }
        }

        void render( lists::c_loot pickup ) {
            if ( !config::radar::show_pickups )
                return;

            auto draw_list = ImGui::GetForegroundDrawList( );
            auto render_color = g_drawing->get_tier_color( pickup.m_pickup_rarity );

            auto radar_location = pickup.m_world_location;
            if ( !radar_location.is_valid( ) ) return;

            engine::f_vector2d radar_screen{};
            if ( config::radar::rotation_mode ) {
                rotate_point_with_player_rotation( &radar_screen, radar_location );
            }
            else {
                rotate_point( &radar_screen, radar_location );
            }

            draw_list->AddCircleFilled(
                radar_screen.vec( ),
                3,
                render_color,
                15
            );

            if ( config::radar::distance ) {
                auto text_format = std::format( "({:.1f}m)", pickup.m_distance );
                auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

                auto text_pos = ImVec2(
                    radar_screen.x - ( text_size.x / 2.0f ),
                    radar_screen.y + 2.0
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

                draw_list->AddText( text_pos, render_color, text_format.c_str( ) );
            }
        }

    private:
        void rotate_point( engine::f_vector2d* screen, engine::f_vector origin ) {
            auto delta = origin - g_world->m_camera_location;
            auto angle = atan2( delta.y, delta.x );
            auto distance = sqrt( delta.x * delta.x + delta.y * delta.y ) * 0.01f;

            auto scaled_distance = ( distance / config::radar::range ) * ( config::radar::size / 2.0f );
            scaled_distance = min( scaled_distance, config::radar::size / 2.0f );

            auto x = scaled_distance * cos( angle );
            auto y = scaled_distance * sin( angle );

            auto radar_center = engine::f_vector2d(
                config::radar::position_x + ( config::radar::size / 2.0f ),
                config::radar::position_y + ( config::radar::size / 2.0f )
            );

            *screen = engine::f_vector2d( radar_center.x + x, radar_center.y + y );
        }

        void draw_grid_overlay( ImDrawList* draw_list, ImVec2 radar_pos, float radar_size ) {
            int grid_divisions = config::radar::grid_divisions;
            float grid_spacing = radar_size / grid_divisions;

            ImColor grid_color( 255, 255, 255, 30 );

            for ( int i = 1; i < grid_divisions; i++ ) {
                float x = radar_pos.x + ( i * grid_spacing );
                draw_list->AddLine(
                    ImVec2( x, radar_pos.y ),
                    ImVec2( x, radar_pos.y + radar_size ),
                    grid_color,
                    0.5f
                );
            }

            for ( int i = 1; i < grid_divisions; i++ ) {
                float y = radar_pos.y + ( i * grid_spacing );
                draw_list->AddLine(
                    ImVec2( radar_pos.x, y ),
                    ImVec2( radar_pos.x + radar_size, y ),
                    grid_color,
                    0.5f
                );
            }
        }

        void draw_line_of_sight( ImDrawList* draw_list, engine::f_vector2d player_pos, lists::c_actor player ) {
            float player_yaw = player.m_player_rotation.yaw;
            float our_yaw = g_world->m_player_rotation.yaw;

            float relative_angle;
            if ( config::radar::rotation_mode ) {
                relative_angle = ( player_yaw - our_yaw ) * std::numbers::pi / 180.0f;
            }
            else {
                relative_angle = player_yaw * std::numbers::pi / 180.0f;
            }

            float sight_length = 25.0f;
            float sight_width = 0.3f;

            float center_x = player_pos.x + sight_length * sin( relative_angle );
            float center_y = player_pos.y - sight_length * cos( relative_angle );

            float left_angle = relative_angle - sight_width;
            float right_angle = relative_angle + sight_width;

            float left_x = player_pos.x + sight_length * sin( left_angle );
            float left_y = player_pos.y - sight_length * cos( left_angle );

            float right_x = player_pos.x + sight_length * sin( right_angle );
            float right_y = player_pos.y - sight_length * cos( right_angle );

            ImVec2 player_vec = player_pos.vec( );
            ImVec2 left_end( left_x, left_y );
            ImVec2 right_end( right_x, right_y );

            ImVec2 radar_pos( config::radar::position_x, config::radar::position_y );
            float radar_size = config::radar::size;

            left_end = clamp_to_radar( left_end, radar_pos, radar_size );
            right_end = clamp_to_radar( right_end, radar_pos, radar_size );

            ImColor sight_fill_color( 255, 100, 100, 25 );
            ImColor sight_line_color( 255, 150, 150, 100 );

            if ( config::radar::los_fill ) {
                draw_list->AddTriangleFilled( player_vec, left_end, right_end, sight_fill_color );
            }

            draw_list->AddLine( player_vec, left_end, sight_line_color, 1.0f );
            draw_list->AddLine( player_vec, right_end, sight_line_color, 1.0f );
        }

        ImVec2 clamp_to_radar( ImVec2 point, ImVec2 radar_pos, float radar_size ) {
            return ImVec2(
                std::clamp( point.x, radar_pos.x, radar_pos.x + radar_size ),
                std::clamp( point.y, radar_pos.y, radar_pos.y + radar_size )
            );
        }

        void rotate_point_with_player_rotation( engine::f_vector2d* screen, engine::f_vector origin ) {
            auto player_radians = g_world->m_player_rotation.yaw * std::numbers::pi / 180.0f;

            auto delta = origin - g_world->m_camera_location;
            auto angle = atan2( delta.y, delta.x ) - player_radians;
            auto distance = sqrt( delta.x * delta.x + delta.y * delta.y ) * 0.01f;

            auto scaled_distance = ( distance / config::radar::range ) * ( config::radar::size / 2.0f );
            scaled_distance = min( scaled_distance, config::radar::size / 2.0f );

            auto x = scaled_distance * sin( angle );
            auto y = -scaled_distance * cos( angle );

            auto radar_center = engine::f_vector2d(
                config::radar::position_x + ( config::radar::size / 2.0f ),
                config::radar::position_y + ( config::radar::size / 2.0f )
            );

            *screen = engine::f_vector2d( radar_center.x + x, radar_center.y + y );
        }
    };
}