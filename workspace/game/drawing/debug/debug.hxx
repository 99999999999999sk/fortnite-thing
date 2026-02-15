namespace debug {
    enum e_debug_type {
        DEBUG_TYPE_PERFORMANCE,
        DEBUG_TYPE_ENTITIES,
        DEBUG_TYPE_EVENTS
    };

    struct debug_info_t {
        std::string id;
        std::string header;
        std::vector<std::pair<std::string, std::string>> entries;
        std::unordered_map<std::string, float> metrics;
        std::vector<std::tuple<std::string, ImColor, std::string>> colored_entries;
        e_debug_type type;
        float timestamp;
    };

    class c_debug {
    public:
        void draw( debug_info_t& info ) {
            auto draw_list = ImGui::GetForegroundDrawList( );

            if ( info.id.empty( ) ) {
                info.id = std::to_string( static_cast< int >( info.type ) ) + "_" + info.header;
            }

            auto& state = m_box_states[ info.id ];
            const float box_height = calculate_box_height( info );
            const float box_width = state.custom_width > 0 ? state.custom_width : config::debug::box_width;

            if ( !state.initialized ) {
                state.position = get_initial_position( info.type, box_height );
                state.height = box_height;
                state.initialized = true;
            }

            state.height = box_height;
            const float header_height = 30.0f;
            const float entry_padding = 5.0f;

            ImVec2& overlay_pos = state.position;
            const ImVec2 mouse_pos = ImGui::GetMousePos( );
            const ImRect header_rect( overlay_pos, ImVec2( overlay_pos.x + box_width, overlay_pos.y + header_height ) );

            ImVec2 bg_min = overlay_pos;
            ImVec2 bg_max = ImVec2( overlay_pos.x + box_width, overlay_pos.y + box_height );

            draw_list->AddRectFilled( bg_min, bg_max, config::debug::bg_color, 5.0f );
            draw_list->AddRect( bg_min, bg_max, config::debug::border_color, 5.0f, 0, 1.5f );

            draw_header( draw_list, overlay_pos, box_width, header_height, info );

            float current_y = overlay_pos.y + header_height + entry_padding;
            draw_content( draw_list, overlay_pos, current_y, info, box_width );
        }

        void reset_all( ) {
            m_box_states.clear( );
            m_drag_states.clear( );
            m_resize_states.clear( );
            reset_positioning( );
        }

        void remove_box( const std::string& id ) {
            m_box_states.erase( id );
            m_drag_states.erase( id );
            m_resize_states.erase( id );
        }

        bool has_box( const std::string& id ) const {
            return m_box_states.find( id ) != m_box_states.end( );
        }

    private:
        struct box_state {
            ImVec2 position;
            float height = 0.0f;
            float custom_width = 0.0f;
            bool pinned = false;
            bool initialized = false;
        };

        struct drag_state {
            bool dragging = false;
            ImVec2 drag_offset = ImVec2( 0, 0 );
        };

        struct resize_state {
            bool resizing = false;
        };

        std::unordered_map<std::string, box_state> m_box_states;
        std::unordered_map<std::string, drag_state> m_drag_states;
        std::unordered_map<std::string, resize_state> m_resize_states;

        float calculate_box_height( const debug_info_t& info ) {
            const float base_height = 30.0f;
            const float padding = 15.0f;
            const float min_content_height = 20.0f;
            float content_height = 0.0f;

            switch ( info.type ) {
            case DEBUG_TYPE_PERFORMANCE:
                content_height = max( min_content_height, static_cast< float >( info.metrics.size( ) ) * 25.0f );
                break;
            case DEBUG_TYPE_ENTITIES:
                content_height = max( min_content_height, static_cast< float >( info.entries.size( ) ) * 32.0f );
                break;
            case DEBUG_TYPE_EVENTS:
                content_height = max( min_content_height, static_cast< float >( info.colored_entries.size( ) ) * 20.0f );
                break;
            default:
                content_height = max( min_content_height, static_cast< float >( info.entries.size( ) ) * 22.0f );
            }

            return base_height + content_height + padding;
        }

        ImVec2 get_initial_position( e_debug_type type, float box_height ) {
            static std::unordered_map<e_debug_type, float> type_offsets;
            static std::unordered_map<e_debug_type, int> type_counts;
            static float global_offset = config::debug::start_y;

            if ( static_cast< int >( type ) == -1 && box_height == -1.0f ) {
                type_offsets.clear( );
                type_counts.clear( );
                global_offset = config::debug::start_y;
                return ImVec2( 0, 0 );
            }

            if ( global_offset > ImGui::GetIO( ).DisplaySize.y * 0.8f ) {
                type_offsets.clear( );
                type_counts.clear( );
                global_offset = config::debug::start_y;
            }

            if ( type_offsets.find( type ) == type_offsets.end( ) ) {
                type_offsets[ type ] = global_offset;
                type_counts[ type ] = 0;
                global_offset += box_height + config::debug::box_spacing;
            }

            int box_count = type_counts[ type ]++;
            float x_offset = 20.0f;
            float y_offset = type_offsets[ type ];
            const float estimated_box_width = config::debug::box_width;

            if ( x_offset + estimated_box_width > ImGui::GetIO( ).DisplaySize.x * 0.9f ) {
                x_offset = 20.0f + ( static_cast< int >( type ) * 15.0f );
                y_offset += ( box_count / 3 ) * ( box_height + 10.0f );
            }

            return ImVec2( x_offset, y_offset );
        }

        void reset_positioning( ) {
            get_initial_position( static_cast< e_debug_type >( -1 ), -1.0f );
        }

        void draw_generic_info( ImDrawList* draw_list, ImVec2 pos, float& y, debug_info_t& info, float box_width ) {
            const float entry_height = 20.0f;
            const float value_offset = 120.0f;

            for ( const auto& [label, value] : info.entries ) {
                draw_list->AddText( ImVec2( pos.x + 10, y ), config::debug::text_color, label.c_str( ) );
                draw_list->AddText( ImVec2( pos.x + value_offset, y ), config::debug::text_secondary_color, value.c_str( ) );
                y += entry_height + 2;
            }

            if ( info.entries.empty( ) ) {
                draw_list->AddText( ImVec2( pos.x + 10, y ), config::debug::text_secondary_color, "No debug information available" );
                y += entry_height;
            }
        }

        void draw_performance_info( ImDrawList* draw_list, ImVec2 pos, float& y, debug_info_t& info, float box_width ) {
            const float bar_width = box_width - 20.0f;
            const float bar_height = 8.0f;

            for ( const auto& [label, value] : info.metrics ) {
                draw_list->AddText( ImVec2( pos.x + 10, y ), config::debug::text_color, label.c_str( ) );

                std::string value_str = std::to_string( ( int )value );
                ImVec2 value_size = ImGui::CalcTextSize( value_str.c_str( ) );
                draw_list->AddText( ImVec2( pos.x + box_width - value_size.x - 10, y ), config::debug::text_color, value_str.c_str( ) );

                y += 25.0f;
            }
        }

        void draw_entity_info( ImDrawList* draw_list, ImVec2 pos, float& y, debug_info_t& info, float box_width ) {
            for ( const auto& [label, value] : info.entries ) {
                const float entry_height = 30.0f;

                draw_list->AddRectFilled( ImVec2( pos.x + 2, y ), ImVec2( pos.x + box_width - 2, y + entry_height ), config::debug::entry_bg_color, 3.0f );
                draw_list->AddCircleFilled( ImVec2( pos.x + 15, y + entry_height / 2 ), 4.0f, config::debug::accent_color );
                draw_list->AddText( ImVec2( pos.x + 25, y + 5 ), config::debug::text_color, label.c_str( ) );
                draw_list->AddText( ImVec2( pos.x + 25, y + 18 ), config::debug::text_secondary_color, value.c_str( ) );

                y += entry_height + 2;
            }
        }

        void draw_event_info( ImDrawList* draw_list, ImVec2 pos, float& y, debug_info_t& info, float box_width ) {
            for ( const auto& [text, color, timestamp_str] : info.colored_entries ) {
                float entry_timestamp = info.timestamp;

                try {
                    entry_timestamp = std::stof( timestamp_str );
                }
                catch ( ... ) {
                    entry_timestamp = info.timestamp;
                }

                float fade_time = ImGui::GetTime( ) - entry_timestamp;
                float alpha = std::clamp( 1.0f - ( fade_time / config::debug::event_timeout ), 0.0f, 1.0f );

                draw_list->AddText( ImVec2( pos.x + 10, y ), ImColor( color.Value.x, color.Value.y, color.Value.z, alpha * color.Value.w ), text.c_str( ) );

                y += 20.0f;
            }
        }

        void draw_header( ImDrawList* draw_list, const ImVec2& overlay_pos, float box_width, float header_height, debug_info_t& info ) {
            ImVec2 header_min = overlay_pos;
            ImVec2 header_max = ImVec2( overlay_pos.x + box_width, overlay_pos.y + header_height );

            draw_list->AddRectFilledMultiColor( header_min, header_max, config::debug::header_bg_top, config::debug::header_bg_top, config::debug::header_bg_bottom, config::debug::header_bg_bottom );
            draw_list->AddRect( header_min, header_max, config::debug::border_color, 5.0f, ImDrawFlags_RoundCornersTop, 1.0f );

            std::string header_text = info.header;
            if ( info.type == DEBUG_TYPE_EVENTS ) {
                float time_since = ImGui::GetTime( ) - info.timestamp;
                float alpha = std::clamp( 1.0f - ( time_since / config::debug::event_timeout ), 0.0f, 1.0f );

                draw_list->AddRectFilled( header_min, header_max, ImColor( config::debug::header_bg_top.Value.x, config::debug::header_bg_top.Value.y, config::debug::header_bg_top.Value.z, alpha * config::debug::header_bg_top.Value.w ), 5.0f );
            }

            ImVec2 header_text_size = ImGui::CalcTextSize( header_text.c_str( ) );
            ImVec2 header_text_pos = ImVec2( overlay_pos.x + ( box_width - header_text_size.x ) / 2, overlay_pos.y + ( header_height - header_text_size.y ) / 2 );
            draw_list->AddText( header_text_pos, config::debug::header_color, header_text.c_str( ) );
        }

        void draw_content( ImDrawList* draw_list, const ImVec2& pos, float& y, debug_info_t& info, float box_width ) {
            switch ( info.type ) {
            case DEBUG_TYPE_PERFORMANCE:
                draw_performance_info( draw_list, pos, y, info, box_width );
                break;
            case DEBUG_TYPE_ENTITIES:
                draw_entity_info( draw_list, pos, y, info, box_width );
                break;
            case DEBUG_TYPE_EVENTS:
                draw_event_info( draw_list, pos, y, info, box_width );
                break;
            default:
                draw_generic_info( draw_list, pos, y, info, box_width );
            }
        }
    };
}
