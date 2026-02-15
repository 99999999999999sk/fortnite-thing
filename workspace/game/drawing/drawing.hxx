namespace drawing {
    struct find_window_data {
        unsigned long pid;
        std::string class_name;
        std::string window_name;
        HWND hwnd;
    };

    BOOL __stdcall enum_windows_proc( HWND hwnd, LPARAM l_param ) {
        find_window_data* data = ( find_window_data* )l_param;

        DWORD pid = 0;
        GetWindowThreadProcessId( hwnd, &pid );

        if ( pid == data->pid ) {
            char class_name[ 256 ];
            GetClassNameA( hwnd, class_name, sizeof( class_name ) );
            if ( data->class_name == class_name ) {
                char window_name[ 256 ];
                GetWindowTextA( hwnd, window_name, sizeof( window_name ) );
                if ( data->window_name == window_name ) {
                    data->hwnd = hwnd;
                    return false;
                }
            }
        }
        return true;
    }

    HWND find_child_window_from_parent( HWND parent, const char* class_name, const char* window_name ) {
        DWORD pid = 0;
        GetWindowThreadProcessId( parent, &pid );

        if ( pid == 0 )
            return nullptr;

        find_window_data data = { pid, class_name, window_name, nullptr };
        EnumWindows( enum_windows_proc, reinterpret_cast< LPARAM >( &data ) );
        return data.hwnd;
    }

    class c_drawing {
    public:
        bool hijack( ) {
            m_width = GetSystemMetrics( SM_CXSCREEN );
            m_height = GetSystemMetrics( SM_CYSCREEN );

            m_width_center = m_width / 2;
            m_height_center = m_height / 2;

            HWND game_hwnd = FindWindowA( ( "UnrealWindow" ), ( "Fortnite  " ) );

            m_window_handle = find_child_window_from_parent( game_hwnd, "IME", "Default IME" );
            if ( !m_window_handle ) return false;

            RECT rect = get_client_area_and_size( game_hwnd );
            if ( !SetWindowPos( m_window_handle, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER ) )
                return false;

            GetWindowRect( GetDesktopWindow( ), &rect );
            SetWindowLong( m_window_handle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW );

            MARGINS window_margin{ -1 };
            DwmExtendFrameIntoClientArea( m_window_handle, &window_margin );
            SetLayeredWindowAttributes( m_window_handle, 0, 255, LWA_ALPHA );

            UpdateWindow( m_window_handle );
            ShowWindow( m_window_handle, SW_SHOW );
            return true;
        }

        RECT get_client_area_and_size( HWND hwnd ) {
            RECT rect;
            if ( GetClientRect( hwnd, &rect ) ) {
                POINT top_left = { rect.left, rect.top };
                POINT bottom_right = { rect.right, rect.bottom };

                ClientToScreen( hwnd, &top_left );
                ClientToScreen( hwnd, &bottom_right );

                rect.left = top_left.x;
                rect.top = top_left.y;
                rect.right = bottom_right.x;
                rect.bottom = bottom_right.y;
            }
            else {
                rect = { 0, 0, 0, 0 };
            }

            return rect;
        }

        bool setup( ) {
            DXGI_SWAP_CHAIN_DESC sd;
            ZeroMemory( &sd, sizeof( sd ) );
            sd.BufferCount = 2;
            sd.BufferDesc.Width = 0;
            sd.BufferDesc.Height = 0;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = m_window_handle;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;
            sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            D3D_FEATURE_LEVEL feature_level;
            const D3D_FEATURE_LEVEL feature_levels[ 2 ] = {
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_0
            };

            if ( FAILED( D3D11CreateDeviceAndSwapChain(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                0,
                feature_levels,
                2,
                D3D11_SDK_VERSION,
                &sd,
                &m_swap_chain,
                &m_d3d_device,
                &feature_level,
                &m_device_context ) ) ) {
                return false;
            }

            create_render_target( );
            setup_imgui( );
            return true;
        }

        void create_render_target( ) {
            ID3D11Texture2D* back_buffer;
            m_swap_chain->GetBuffer( 0, IID_PPV_ARGS( &back_buffer ) );
            if ( back_buffer ) {
                m_d3d_device->CreateRenderTargetView( back_buffer, nullptr, &m_render_target );
                back_buffer->Release( );
            }
        }

        void setup_imgui( ) {
            IMGUI_CHECKVERSION( );
            ImGui::CreateContext( );
            ImGuiIO& io = ImGui::GetIO( );
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            ImGuiStyle& s = ImGui::GetStyle( );

            auto font_size = 15.0;
            ImFontConfig font_cfg;
            font_cfg.OversampleH = 1;
            font_cfg.OversampleV = 1;
            font_cfg.PixelSnapH = true;

            io.Fonts->AddFontFromMemoryTTF( &InterMedium, sizeof InterMedium, 16, NULL, io.Fonts->GetGlyphRangesCyrillic( ) );

            medium_little_font = io.Fonts->AddFontFromMemoryTTF( &InterMedium, sizeof InterMedium, 11, NULL, io.Fonts->GetGlyphRangesCyrillic( ) );

            medium_small_font = io.Fonts->AddFontFromMemoryTTF( &InterMedium, sizeof InterMedium, 14, NULL, io.Fonts->GetGlyphRangesCyrillic( ) );

            semibold_font = io.Fonts->AddFontFromMemoryTTF( &InterSemiBold, sizeof InterSemiBold, 18, NULL, io.Fonts->GetGlyphRangesCyrillic( ) );

            icon_font_regular = io.Fonts->AddFontFromMemoryTTF( &icomoon, sizeof icomoon, 14, NULL, io.Fonts->GetGlyphRangesCyrillic( ) );

            icon_font_small = io.Fonts->AddFontFromMemoryTTF( &icomoon, sizeof icomoon, 7, NULL, io.Fonts->GetGlyphRangesCyrillic( ) );

            icon_font_medium = io.Fonts->AddFontFromMemoryTTF( &icomoon, sizeof icomoon, 12, NULL, io.Fonts->GetGlyphRangesCyrillic( ) );

            icon_font_big = io.Fonts->AddFontFromMemoryTTF( &icomoon, sizeof icomoon, 17, NULL, io.Fonts->GetGlyphRangesCyrillic( ) );


            D3DX11_IMAGE_LOAD_INFO iInfo;
            ID3DX11ThreadPump* threadPump{ nullptr };

            if ( bg == nullptr && !image_loaded ) {
                D3DX11CreateShaderResourceViewFromMemory( m_d3d_device, background, sizeof( background ), &iInfo, threadPump, &bg, 0 );
                D3DX11CreateShaderResourceViewFromMemory( m_d3d_device, model, sizeof( model ), &iInfo, threadPump, &model_tx, 0 );
                image_loaded = true;
            }

            ImGui::StyleColorsDark( );
            s.FramePadding = ImVec2( 10, 10 );
            s.ItemSpacing = ImVec2( 10, 10 );
            s.FrameRounding = 2.f;
            s.WindowRounding = 10.f;
            s.WindowBorderSize = 0.f;
            s.PopupBorderSize = 0.f;
            s.WindowPadding = ImVec2( 0, 0 );
            s.ChildBorderSize = 1.f;
            s.Colors[ ImGuiCol_WindowBg ] = ImColorToImVec4( winbg_color );
            s.Colors[ ImGuiCol_Border ] = ImVec4( 0.f, 0.f, 0.f, 0.f );
            s.WindowShadowSize = 0;
            s.PopupRounding = 5.f;
            s.ScrollbarSize = 1;
            s.PopupBorderSize = 1.3f;

            ImGui_ImplWin32_Init( m_window_handle );
            ImGui_ImplDX11_Init( m_d3d_device, m_device_context );
        }

        void draw_menu( ) {
            if ( !m_menu_active )
                return;

            ImGui::SetNextWindowSize( ImVec2( 700, 565 ) );
            ImGui::Begin( "General", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus );
            {
                anim_speed = ImGui::GetIO( ).DeltaTime * 8.f;

                auto draw = ImGui::GetWindowDrawList( );
                const auto& p = ImGui::GetWindowPos( );
                win_pos = ImGui::GetWindowPos( );

                const ImVec2& region = ImGui::GetContentRegionMax( );

                draw->AddRectFilled( p, p + ImVec2( 200, region.y - 25 ), ImColor( 14, 14, 16, 255 ), 10.f, ImDrawFlags_RoundCornersTopLeft );
                draw->AddRectFilled( p + ImVec2( 0, 540 ), p + ImVec2( region.x, region.y ), ImColor( 1.f, 1.f, 1.f, 0.03f ), 10.f, ImDrawFlags_RoundCornersBottom );
                draw->AddRectFilled( p + ImVec2( 200, 0 ), p + ImVec2( 200, 540 ), ImColor( 1.f, 1.f, 1.f, 0.03f ), 10.f, ImDrawFlags_RoundCornersBottom );
                draw->AddRectFilled( p + ImVec2( 201, 60 ), p + ImVec2( region.x, 60 ), ImColor( 1.f, 1.f, 1.f, 0.03f ), 10.f, ImDrawFlags_RoundCornersBottom );
                draw->AddText( center_text( p + ImVec2( 0, 540 ), p + ImVec2( region.x, region.y ), "MakFN External for Fortnite" ), stroke_color, "MakFN External for Fortnite" );

                ImGui::PushFont( semibold_font );
                draw->AddText( p + ImVec2( 15, 11 ), stroke_color, "Aimbot" );
                draw->AddText( p + ImVec2( 15, 172 ), stroke_color, "Visuals" );
                draw->AddText( p + ImVec2( 15, 378 ), stroke_color, "Miscellaneous" );

                draw->AddRectFilled( p + ImVec2( 15 + ImGui::CalcTextSize( "Aimbot " ).x, 10 + ImGui::CalcTextSize( "Aimbot " ).y / 2 ), p + ImVec2( 183, 12 + ImGui::CalcTextSize( "Aimbot " ).y / 2 ), ImColor( 1.f, 1.f, 1.f, 0.03f ), 10.f );
                draw->AddRectFilled( p + ImVec2( 15 + ImGui::CalcTextSize( "Visuals " ).x, 171 + ImGui::CalcTextSize( "Visuals " ).y / 2 ), p + ImVec2( 183, 173 + ImGui::CalcTextSize( "Visuals " ).y / 2 ), ImColor( 1.f, 1.f, 1.f, 0.03f ), 10.f );
                draw->AddRectFilled( p + ImVec2( 15 + ImGui::CalcTextSize( "Miscellaneous " ).x, 377 + ImGui::CalcTextSize( "Miscellaneous " ).y / 2 ), p + ImVec2( 183, 379 + ImGui::CalcTextSize( "Miscellaneous " ).y / 2 ), ImColor( 1.f, 1.f, 1.f, 0.03f ), 10.f );
                ImGui::PopFont( );

                ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4, 4 ) );
                static int iTabs = 0;
                ImGui::SetCursorPos( ImVec2( 15, 32 ) );
                ImGui::BeginGroup( );
                ImGui::Tab( "Aimbot", "1", &iTabs, 0 );
                ImGui::Tab( "Triggerbot", "7", &iTabs, 1 );
                ImGui::EndGroup( );

                ImGui::SetCursorPos( ImVec2( 15, 193 ) );
                ImGui::BeginGroup( );
                ImGui::Tab( "Visuals", "2", &iTabs, 2 );
                ImGui::Tab( "World", "5", &iTabs, 3 );
                ImGui::Tab( "Radar", "6", &iTabs, 4 );
                ImGui::EndGroup( );

                ImGui::SetCursorPos( ImVec2( 15, 397 ) );
                ImGui::BeginGroup( );
                ImGui::Tab( "Misc", "4", &iTabs, 5 );
                ImGui::Tab( "Configs", "8", &iTabs, 6 );
                ImGui::Tab( "Exploits", "3", &iTabs, 7 );
                ImGui::EndGroup( );
                ImGui::PopStyleVar( );

                ImGui::SetCursorPos( ImVec2( 212, 10 ) );
                ImGui::BeginGroup( );
                if ( iTabs == 1 ) {
                    ImGui::Text( "Triggerbot" );
                }
                else if ( iTabs == 2 ) {
                    ImGui::Text( "Visuals" );
                }
                else if ( iTabs == 3 ) {
                    ImGui::Text( "World" );
                }
                else if ( iTabs == 4 ) {
                    ImGui::Text( "Radar" );
                }
                else if ( iTabs == 5 ) {
                    ImGui::Text( "Misc" );
                }
                else if ( iTabs == 6 ) {
                    ImGui::Text( "Configs" );
                }
                else if ( iTabs == 7 ) {
                    ImGui::Text( "Exploits" );
                }
                ImGui::EndGroup( );

                ImGui::SetCursorPos( ImVec2( 212, 65 ) );
                switch ( iTabs ) {
                case 0:
                {
                    static int weapon_tab = 0;
                    ImGui::SetCursorPos( ImVec2( 212, 10 ) );
                    ImGui::BeginGroup( );

                    ImGui::SubTab( "Aimbot ", &weapon_tab, 0 ); ImGui::SameLine( );
                    ImGui::SubTab( "Close Aim", &weapon_tab, 1 );

                    ImGui::EndGroup( );
                    ImGui::SetCursorPos( ImVec2( 212, 74 ) );

                    switch ( weapon_tab ) {
                    case 0: {
                        ImVec2 child_size = ImVec2( 480, ImGui::GetWindowHeight( ) - 128 - ImGui::GetStyle( ).WindowPadding.y );
                        ImGui::BeginChild( "Aimbot", child_size, &config::aimbot::enabled, 0, "g" );
                        {
                            ImGui::Columns( 2, "Visuals", false );
                            ImGui::SetColumnWidth( 0, 230 );

                            ImGui::Checkbox( "Target Line", &config::aimbot::target_line[ config::aimbot::weapon_configs ] );
                            ImGui::Checkbox( "Target Dot", &config::aimbot::target_dot[ config::aimbot::weapon_configs ] );

                            ImGui::NextColumn( );

                            ImGui::Checkbox( "FOV Circle", &config::aimbot::fov_circle[ config::aimbot::weapon_configs ] );
                            ImGui::SliderFloat( "FOV Radius", &config::aimbot::fov_radius[ config::aimbot::weapon_configs ], 0, 120 );

                            ImGui::Columns( 1 );

                            ImGui::Spacing( );
                            ImGui::Separator( );
                            ImGui::Spacing( );

                            ImGui::Columns( 2, "AimFeatures", false );
                            ImGui::SetColumnWidth( 0, 230 );

                            ImGui::Checkbox( "Smart Targeting", &config::aimbot::smart_targeting[ config::aimbot::weapon_configs ] );
                            ImGui::Checkbox( "No Target Switch", &config::aimbot::no_target_switch[ config::aimbot::weapon_configs ] );
                            ImGui::Checkbox( "Inverted Vertical Axis", &config::aimbot::inverted_vertical_axis[ config::aimbot::weapon_configs ] );
                            ImGui::Checkbox( "Deadzone", &config::aimbot::deadzone[ config::aimbot::weapon_configs ] );
                            ImGui::SliderFloat( "Restriction", &config::aimbot::restriction[ config::aimbot::weapon_configs ], 0, 25 );
                            ImGui::SliderInt( "Smoothing", &config::aimbot::smoothing[ config::aimbot::weapon_configs ], 0, 50 );

                            ImGui::NextColumn( );

                            ImGui::Checkbox( "Aim Curve", &config::aimbot::aim_curve[ config::aimbot::weapon_configs ] );
                            ImGui::Checkbox( "Aim Acceleration", &config::aimbot::aim_acceleration[ config::aimbot::weapon_configs ] );
                            ImGui::Checkbox( "Aim Between Bones", &config::aimbot::linear_aim[ config::aimbot::weapon_configs ] );
                            const char* aim_prediction[ ]{ "Delta", "Bullet" };
                            ImGui::MultiCombo( "Aim Prediction", config::aimbot::aim_prediction[ config::aimbot::weapon_configs ], aim_prediction, IM_ARRAYSIZE( aim_prediction ) );
                            ImGui::SliderFloat( "Aim Velocity", &config::aimbot::max_velocity[ config::aimbot::weapon_configs ], 1, 200 );
                            ImGui::SliderFloat( "Acceleration", &config::aimbot::acceleration[ config::aimbot::weapon_configs ], 20, 150 );

                            ImGui::Columns( 1 );

                            ImGui::Spacing( );
                            ImGui::Separator( );
                            ImGui::Spacing( );

                            ImGui::Columns( 2, "Misc", false );
                            ImGui::SetColumnWidth( 0, 230 );

                            const char* aim_bone[ ]{ "Head", "Neck", "Chest", "Middle Chest", "Lower Chest" };
                            ImGui::MultiCombo( "Aim Bone", config::aimbot::aim_bone[ config::aimbot::weapon_configs ], aim_bone, IM_ARRAYSIZE( aim_bone ) );
                            ImGui::Keybind( "Aim Key", &config::aimbot::aim_key[ config::aimbot::weapon_configs ] );
                            ImGui::SliderInt( "Aim Distance", &config::aimbot::max_distance[ config::aimbot::weapon_configs ], 50.0f, 500.0f );

                            ImGui::NextColumn( );

                            ImGui::Checkbox( "Only Visible", &config::aimbot::only_visible[ config::aimbot::weapon_configs ] );
                            ImGui::Checkbox( "Ignore Downed", &config::aimbot::ignore_downed[ config::aimbot::weapon_configs ] );
                            ImGui::Checkbox( "Ignore Bots", &config::aimbot::ignore_bots[ config::aimbot::weapon_configs ] );

                            ImGui::Columns( 1 );

                            ImGui::Spacing( );
                            ImGui::Separator( );
                            ImGui::Spacing( );

                            ImGui::Columns( 2, "Configs", false );
                            ImGui::SetColumnWidth( 0, 230 );

                            const char* weapon_configs[ ]{ "General", "Rifles", "Shotguns" , "SMGs", "Pistols", "Snipers" };
                            ImGui::Combo( "Gun Config", &config::aimbot::weapon_configs, weapon_configs, IM_ARRAYSIZE( weapon_configs ) );

                            ImGui::NextColumn( );

                            if ( config::aimbot::weapon_configs )
                                ImGui::Checkbox( "Override General", &config::aimbot::override_general[ config::aimbot::weapon_configs ] );

                            ImGui::Columns( 1 );

                            ImGui::Spacing( );
                            ImGui::Separator( );
                        }
                        ImGui::EndChild( "Aimbot", config::aimbot::enabled );
                    } break;
                    case 1: {
                        ImVec2 child_size = ImVec2( 480, ImGui::GetWindowHeight( ) - 128 - ImGui::GetStyle( ).WindowPadding.y );
                        ImGui::BeginChild( "Close Aim", child_size, &config::aimbot::close_aim, 0, "g" );
                        {
                            const auto max_close_aim = 6;

                            ImGui::Columns( 2, "Filters", false );
                            ImGui::SetColumnWidth( 0, 230 );

                            ImGui::Checkbox( "FOV Circle", &config::aimbot::fov_circle[ max_close_aim ] );
                            ImGui::SliderFloat( "FOV Radius", &config::aimbot::fov_radius[ max_close_aim ], 0, 120 );

                            ImGui::NextColumn( );

                            ImGui::Checkbox( "Shotgun Only", &config::aimbot::shotgun_only );
                            ImGui::Checkbox( "No Target Switch", &config::aimbot::no_target_switch[ max_close_aim ] );

                            ImGui::Columns( 1 );

                            ImGui::Spacing( );
                            ImGui::Separator( );
                            ImGui::Spacing( );

                            ImGui::Columns( 2, "Features", false );
                            ImGui::SetColumnWidth( 0, 230 );

                            ImGui::Checkbox( "Inverted Vertical Axis", &config::aimbot::inverted_vertical_axis[ max_close_aim ] );
                            ImGui::Checkbox( "Deadzone", &config::aimbot::deadzone[ max_close_aim ] );
                            ImGui::SliderFloat( "Restriction", &config::aimbot::restriction[ max_close_aim ], 0, 25 );
                            ImGui::SliderInt( "Smoothing", &config::aimbot::smoothing[ max_close_aim ], 0, 50 );

                            ImGui::NextColumn( );

                            ImGui::Checkbox( "Aim Between Bones", &config::aimbot::linear_aim[ max_close_aim ] );
                            ImGui::Keybind( "Aim Key", &config::aimbot::aim_key[ max_close_aim ] );
                            const char* aim_bone[ ]{ "Head", "Neck", "Chest", "Middle Chest", "Lower Chest" };
                            ImGui::MultiCombo( "Aim Bone", config::aimbot::aim_bone[ max_close_aim ], aim_bone, IM_ARRAYSIZE( aim_bone ) );
                            ImGui::SliderInt( "Aim Distance", &config::aimbot::max_distance[ max_close_aim ], 0, 50 );

                            ImGui::Columns( 1 );

                            ImGui::Spacing( );
                            ImGui::Separator( );
                            ImGui::Spacing( );
                        }
                        ImGui::EndChild( "CloseAim", config::aimbot::close_aim );
                    } break;
                    }

                    break;
                }

                case 1:
                {
                    ImVec2 child_size = ImVec2( 480, ImGui::GetWindowHeight( ) - 130 - ImGui::GetStyle( ).WindowPadding.y );
                    ImGui::BeginChild( "Triggerbot", child_size, &config::triggerbot::enabled, 0, "g" );
                    {

                    }
                    ImGui::EndChild( "Triggerbot", config::triggerbot::enabled );
                    break;
                }

                case 2:
                {
                    ImVec2 child_size = ImVec2( 480, ImGui::GetWindowHeight( ) - 130 - ImGui::GetStyle( ).WindowPadding.y );
                    ImGui::BeginChild( "Player", child_size, &config::visuals::enabled, 0, "v" );
                    {
                        ImGui::Columns( 2, "BoxTypesColumns", false );
                        ImGui::SetColumnWidth( 0, 230 );

                        ImGui::Checkbox( "Full Box", &config::visuals::full_box );
                        ImGui::Checkbox( "3D Box", &config::visuals::threed_box );

                        ImGui::NextColumn( );

                        ImGui::Checkbox( "Corner Box", &config::visuals::corner_box );
                        ImGui::Checkbox( "Filled", &config::visuals::filled );

                        ImGui::Columns( 1 );

                        ImGui::Spacing( );
                        ImGui::Separator( );
                        ImGui::Spacing( );

                        ImGui::Columns( 2, "PlayerInfoColumns", false );
                        ImGui::SetColumnWidth( 0, 230 );

                        ImGui::Checkbox( "Skeleton", &config::visuals::skeleton );
                        ImGui::Checkbox( "Weapon Name", &config::visuals::weapon_name );
                        ImGui::Checkbox( "Season Rank", &config::visuals::season_rank );
                        ImGui::Checkbox( "Level", &config::visuals::level );
                        ImGui::Checkbox( "Movement", &config::visuals::movement );

                        ImGui::NextColumn( );

                        ImGui::Checkbox( "Snaplines", &config::visuals::snaplines );
                        ImGui::Checkbox( "Display Name", &config::visuals::display_name );
                        ImGui::Checkbox( "Platform", &config::visuals::platform );
                        ImGui::Checkbox( "Movement Tracers", &config::visuals::movement_tracers );
                        ImGui::Checkbox( "Kills", &config::visuals::kills );

                        ImGui::Columns( 1 );

                        ImGui::Spacing( );
                        ImGui::Separator( );
                        ImGui::Spacing( );

                        ImGui::Columns( 2, "ExtraFeaturesColumns", false );
                        ImGui::SetColumnWidth( 0, 230 );

                        ImGui::Checkbox( "Wounded", &config::visuals::wounded );
                        ImGui::Checkbox( "View Line", &config::visuals::view_line );
                        ImGui::SliderFloat( "Max Distance", &config::visuals::max_distance, 50.0f, 500.0f );

                        ImGui::NextColumn( );

                        ImGui::Checkbox( "FOV Arrows", &config::visuals::fov_arrows );
                        ImGui::Checkbox( "Targeting Bar", &config::visuals::targeting_bar );
                        ImGui::Checkbox( "Stencil", &config::visuals::stencil );

                        ImGui::Columns( 1 );

                        ImGui::Spacing( );
                        ImGui::Separator( );
                        ImGui::Spacing( );
                    }
                    ImGui::EndChild( "Player", config::visuals::enabled );
                    break;
                }

                case 3:
                {
                    ImVec2 child_size = ImVec2( 480, ImGui::GetWindowHeight( ) - 130 - ImGui::GetStyle( ).WindowPadding.y );
                    ImGui::BeginChild( "World", child_size, &config::world::enabled, 0, "v" );
                    {
                        ImGui::Columns( 2, "WorldFeatures", false );
                        ImGui::SetColumnWidth( 0, 230 );

                        const char* loot_tier[ ]{ "Common", "Uncommon", "Rare", "Epic", "Legendary" };
                        ImGui::MultiCombo( "Loot Tier", config::world::loot_tier, loot_tier, IM_ARRAYSIZE( loot_tier ) );
                        ImGui::SliderFloat( "Max Distance", &config::world::max_distance, 50.0f, 300.0f );

                        ImGui::NextColumn( );

                        ImGui::Checkbox( "Mark Pickups", &config::world::mark_pickups );
                        ImGui::Checkbox( "Show Distance", &config::world::show_distance );
                        ImGui::SliderInt( "Max Pickups", &config::world::max_pickups, 10, 700 );

                        ImGui::Columns( 1 );

                        ImGui::Spacing( );
                        ImGui::Separator( );
                        ImGui::Spacing( );
                    }
                    ImGui::EndChild( "World", config::world::enabled );
                    break;
                }

                case 4:
                {
                    ImVec2 child_size = ImVec2( 480, ImGui::GetWindowHeight( ) - 130 - ImGui::GetStyle( ).WindowPadding.y );
                    ImGui::BeginChild( "Radar", child_size, &config::radar::enabled, 0, "v" );
                    {
                        ImGui::Columns( 2, "Radar", false );
                        ImGui::SetColumnWidth( 0, 230 );

                        ImGui::Checkbox( "Show Pickups", &config::radar::show_pickups );
                        ImGui::Checkbox( "Radar distance", &config::radar::distance );
                        ImGui::Checkbox( "Rotated orientation", &config::radar::rotation_mode );
                        ImGui::SliderFloat( "Radar Opacity", &config::radar::opacity, 0, 255 );
                        ImGui::SliderInt( "X", &config::radar::position_x, 0, m_width );
                        ImGui::SliderInt( "Y", &config::radar::position_y, 0, m_height );

                        ImGui::NextColumn( );

                        ImGui::Checkbox( "Radar Grid", &config::radar::grid );
                        ImGui::Checkbox( "Radar LOS", &config::radar::line_of_sight );
                        ImGui::Checkbox( "Filled LOS", &config::radar::los_fill );
                        ImGui::SliderInt( "Grid Divisions", &config::radar::grid_divisions, 0, 10 );
                        ImGui::SliderInt( "Radar Size", &config::radar::size, 0, 800 );
                        ImGui::SliderInt( "Radar Range", &config::radar::range, 0, 1000 );

                        ImGui::Columns( 1 );

                        ImGui::Spacing( );
                        ImGui::Separator( );
                        ImGui::Spacing( );
                    }
                    ImGui::EndChild( "Radar", config::radar::enabled );
                    break;
                }

                case 5:
                {
                    ImVec2 child_size = ImVec2( 480, ImGui::GetWindowHeight( ) - 130 - ImGui::GetStyle( ).WindowPadding.y );
                    ImGui::BeginChild( "Misc", child_size, &config::misc::enabled, 0, "v" );
                    {
                        ImGui::Columns( 2, "MiscColumns", false );
                        ImGui::SetColumnWidth( 0, 230 );

                        ImGui::Checkbox( "VSync", &config::misc::vsync );
                        ImGui::Checkbox( "Controller Support", &config::misc::controller_support );
                        ImGui::Checkbox( "Performance Counter", &config::misc::performance_counter );
                        ImGui::Checkbox( "Stream Proof", &config::misc::stream_proof );

                        ImGui::NextColumn( );

                        ImGui::Checkbox( "Inventory", &config::misc::inventory );
                        ImGui::Checkbox( "Spectators", &config::misc::spectators );
                        ImGui::Checkbox( "Bullet Tracers", &config::misc::bullet_tracers );
                        ImGui::SliderInt( "Max Tracers", &config::misc::max_tracers, 1, 10 );
                        ImGui::SliderFloat( "Fadeout Time", &config::misc::fadeout_time, 1, 5 );

                        ImGui::Columns( 1 );

                        ImGui::Spacing( );
                        ImGui::Separator( );
                        ImGui::Spacing( );
                    }
                    ImGui::EndChild( "Misc", config::misc::enabled );
                    break;
                }

                case 6:
                {
                    ImVec2 child_size = ImVec2( 480, ImGui::GetWindowHeight( ) - 130 - ImGui::GetStyle( ).WindowPadding.y );
                    ImGui::BeginChild( "Configs", child_size, &config::configs::enabled, 0, "v" );
                    {
                        static int selected_config = -1;
                        auto configs = g_config->get_configs( );

                        ImGui::Columns( 2, "Configs", false );
                        ImGui::SetColumnWidth( 0, 230 );

                        ImGuiWindow* window = ImGui::GetCurrentWindow( );
                        ImGuiContext& g = *GImGui;
                        const ImGuiID id = window->GetID( "config_list" );
                        const ImVec2 pos = window->DC.CursorPos;
                        const float width = ImGui::GetColumnWidth( ) - 15;
                        const ImRect list_box( pos, pos + ImVec2( width, 350 ) );

                        window->DrawList->AddRectFilled( list_box.Min, list_box.Max, ImColor( 22, 22, 22, 255 ), 0.f );
                        window->DrawList->AddRect( list_box.Min, list_box.Max, ImColor( 0, 0, 0, 255 ), 0.f, 0, 1.f );

                        float item_height = 35.f;
                        float scroll_offset = 10.f;

                        if ( configs.empty( ) ) {
                            ImRect item_rect(
                                list_box.Min + ImVec2( 10, scroll_offset ),
                                list_box.Min + ImVec2( width - 10, scroll_offset + item_height )
                            );

                            ImVec2 text_size = ImGui::CalcTextSize( "No Configs Available" );
                            ImVec2 text_pos = item_rect.Min + ImVec2(
                                ( item_rect.GetWidth( ) - text_size.x ) * 0.5f,
                                ( item_height - text_size.y ) * 0.5f
                            );

                            window->DrawList->AddText(
                                text_pos,
                                ImGui::GetColorU32( ImGuiCol_Text ),
                                "No Configs Available"
                            );
                        }
                        else {
                            for ( size_t i = 0; i < configs.size( ); i++ ) {
                                ImRect item_rect(
                                    list_box.Min + ImVec2( 10, scroll_offset ),
                                    list_box.Min + ImVec2( width - 10, scroll_offset + item_height )
                                );

                                window->DrawList->AddRectFilled(
                                    item_rect.Min,
                                    item_rect.Max,
                                    ImColor( 22, 22, 24, 255 ),
                                    4.f
                                );

                                window->DrawList->AddRect(
                                    item_rect.Min,
                                    item_rect.Max,
                                    ImColor( 1.f, 1.f, 1.f, 0.03f ),
                                    4.f,
                                    0,
                                    1.f
                                );

                                if ( selected_config == i ) {
                                    window->DrawList->AddRectFilled(
                                        item_rect.Min,
                                        item_rect.Max,
                                        ImColor( 25, 25, 27, 255 ),
                                        4.f
                                    );
                                    window->DrawList->AddRect(
                                        item_rect.Min,
                                        item_rect.Max,
                                        ImColor( 1.f, 1.f, 1.f, 0.03f ),
                                        4.f,
                                        0,
                                        1.f
                                    );
                                    window->DrawList->AddRectFilled(
                                        ImVec2(item_rect.Min.x, item_rect.Min.y),
                                        ImVec2(item_rect.Min.x + 2, item_rect.Max.y),
                                        ImColor(144, 154, 251, 255), 2.f
                                    );
                                }

                                ImVec2 text_size = ImGui::CalcTextSize( configs[ i ].c_str( ) );
                                ImVec2 text_pos = item_rect.Min + ImVec2( 10, ( item_height - text_size.y ) * 0.5f );

                                window->DrawList->AddText(
                                    text_pos,
                                    ImGui::GetColorU32( ImGuiCol_Text ),
                                    configs[ i ].c_str( )
                                );

                                if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) &&
                                    ImGui::IsMouseHoveringRect( item_rect.Min, item_rect.Max ) ) {
                                    if ( selected_config == i ) {
                                        selected_config = -1;
                                    }
                                    else {
                                        selected_config = i;
                                    }
                                }

                                scroll_offset += item_height + 5.f;
                            }
                        }

                        ImGui::NextColumn( );

                        const float button_height = 30.f;
                        const float button_spacing = 5.f;
                        float button_y = ImGui::GetCursorPosY( );

                        auto draw_button = [ & ]( const char* label, bool enabled = true ) -> bool {
                            ImRect button_rect(
                                ImGui::GetCursorScreenPos( ),
                                ImGui::GetCursorScreenPos( ) + ImVec2( width, button_height )
                            );

                            bool hovered = ImGui::IsMouseHoveringRect( button_rect.Min, button_rect.Max );
                            bool clicked = ImGui::IsMouseClicked( ImGuiMouseButton_Left ) && hovered;

                            window->DrawList->AddRectFilled(
                                button_rect.Min,
                                button_rect.Max,
                                enabled ? ( hovered ? ImColor( 25, 25, 27, 255 ) : ImColor( 22, 22, 24, 255 ) ) : ImColor( 19, 19, 21, 255 ),
                                4.f
                            );

                            window->DrawList->AddRect(
                                button_rect.Min,
                                button_rect.Max,
                                ImColor( 1.f, 1.f, 1.f, enabled ? 0.03f : 0.01f ),
                                4.f,
                                0,
                                1.f
                            );

                            ImVec2 text_size = ImGui::CalcTextSize( label );
                            ImVec2 text_pos = button_rect.Min + ImVec2( ( button_rect.GetWidth( ) - text_size.x ) * 0.5f, ( button_height - text_size.y ) * 0.5f );

                            window->DrawList->AddText(
                                text_pos,
                                ImGui::GetColorU32( enabled ? ImGuiCol_Text : ImGuiCol_TextDisabled ),
                                label
                            );

                            ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) + button_height + button_spacing );
                            return clicked && enabled;
                        };

                        if ( draw_button( "Create" ) ) {
                            std::string config_name = "Config " + std::to_string( configs.size( ) );
                            g_config->save_config( config_name );
                            g_config->get_available_configs( );
                            configs = g_config->get_configs( );
                        }

                        if ( selected_config >= 0 ) {
                            if ( draw_button( "Delete" ) ) {
                                g_config->delete_config( configs[ selected_config ] );
                                configs = g_config->get_configs( );
                                g_config->get_available_configs( );
                                selected_config = -1;
                            }

                            if ( draw_button( "Load" ) ) {
                                g_config->reset( );
                                g_config->load_config( configs[ selected_config ] );
                            }

                            if ( draw_button( "Save" ) ) {
                                g_config->save_config( configs[ selected_config ] );
                                g_config->get_available_configs( );
                                configs = g_config->get_configs( );
                            }
                        }

                        if ( draw_button( "Reset" ) ) {
                            g_config->reset( );
                        }

                        if ( draw_button( "Open Folder" ) ) {
                            g_config->open_config_folder( );
                        }

                        ImGui::Columns( 1 );
                    }
                    ImGui::EndChild( "Configs", config::configs::enabled );
                    break;
                }

                }

                if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) ) {
                    mouse_circle_col = main_color;
                    mouse_circle_radius = 0;
                    mouse_circle_pos = ImGui::GetMousePos( );
                }

                ImGui::GetForegroundDrawList( )->AddCircleFilled( mouse_circle_pos, mouse_circle_radius, ImGui::GetColorU32( mouse_circle_col ), 360 );
                mouse_circle_col = ImLerp( mouse_circle_col, ImColor( 0, 0, 0, 0 ), anim_speed );
                mouse_circle_radius = ImLerp( mouse_circle_radius, 20.f, anim_speed );
            }
            ImGui::End( );
        }

        void new_frame( ) {
            ImGui_ImplDX11_NewFrame( );
            POINT p;
            GetCursorPos( &p );
            ImGuiIO& io = ImGui::GetIO( );
            io.MousePos = ImVec2( p.x, p.y );
            io.MouseDown[ 0 ] = ( GetKeyState( VK_LBUTTON ) & 0x8000 ) != 0;
            io.MouseDown[ 1 ] = ( GetKeyState( VK_RBUTTON ) & 0x8000 ) != 0;
            ImGui_ImplWin32_NewFrame( );
            ImGui::NewFrame( );
        }

        void draw_frame( ) {
            ImGui::Render( );
            const float clear_color[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
            m_device_context->OMSetRenderTargets( 1, &m_render_target, nullptr );
            m_device_context->ClearRenderTargetView( m_render_target, clear_color );
            ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

            m_swap_chain->Present( config::misc::vsync, 0 );
        }

        std::string get_platform_name( std::string platform ) {
            if ( strstr( platform.c_str( ), "WIN" ) ) {
                return "Windows";
            }
            else if ( strstr( platform.c_str( ), ( "XBL" ) ) || strstr( platform.c_str( ), ( "XSX" ) ) ) {
                return "Xbox";
            }
            else if ( strstr( platform.c_str( ), ( "PSN" ) ) || strstr( platform.c_str( ), ( "PS5" ) ) ) {
                return "Playstation";
            }
            else if ( strstr( platform.c_str( ), ( "SWT" ) ) ) {
                return "Nintendo";
            }
            else if ( strstr( platform.c_str( ), ( "AND" ) ) || strstr( platform.c_str( ), ( "IOS" ) ) ) {
                return "Mobile";
            }

            return platform;
        }

        ImColor get_platform_color( std::string platform ) {
            if ( strstr( platform.c_str( ), "WIN" ) ) {
                return ImColor( 125, 165, 170, 255 );
            }
            else if ( strstr( platform.c_str( ), ( "XBL" ) ) || strstr( platform.c_str( ), ( "XSX" ) ) ) {
                return ImColor( 138, 154, 91, 255 );
            }
            else if ( strstr( platform.c_str( ), ( "PSN" ) ) || strstr( platform.c_str( ), ( "PS5" ) ) ) {
                return ImColor( 50, 74, 178, 255 );
            }
            else if ( strstr( platform.c_str( ), ( "SWT" ) ) ) {
                return ImColor( 127, 15, 5, 255 );
            }
            else if ( strstr( platform.c_str( ), ( "AND" ) ) ) {
                return ImColor( 115, 135, 83, 255 );
            }
            else if ( strstr( platform.c_str( ), ( "IOS" ) ) ) {
                return ImColor( 144, 154, 251, 255 );
            }

            return ImColor( 125, 165, 170, 255 );
        }

        ImColor get_tier_color( engine::e_fort_rarity tier ) {
            switch ( tier ) {
            case engine::e_fort_rarity::common:
                return ImColor( 102, 102, 102, 255 );
            case engine::e_fort_rarity::uncommon:
                return ImColor( 0, 106, 79, 255 );
            case engine::e_fort_rarity::rare:
                return ImColor( 93, 137, 186, 255 );
            case engine::e_fort_rarity::epic:
                return ImColor( 141, 78, 133, 255 );
            case engine::e_fort_rarity::legendary:
                return ImColor( 211, 175, 55, 255 );
            case engine::e_fort_rarity::mythic:
                return ImColor( 255, 207, 64, 255 );
            default:
                return ImColor( 102, 102, 102, 255 );
            }
        }

        std::string get_movement_name( engine::e_movement_mode movement ) {
            switch ( movement ) {
            case engine::e_movement_mode::walking:
                return "Walking";
            case engine::e_movement_mode::nav_walking:
                return "Inactive";
            case engine::e_movement_mode::falling:
                return "Falling";
            case engine::e_movement_mode::swimming:
                return "Swimming";
            case engine::e_movement_mode::flying:
                return "Flying";
            default:
                return "";
            }
        }

        ImColor get_movement_color( engine::e_movement_mode movement ) {
            switch ( movement ) {
            case engine::e_movement_mode::walking:
                return ImColor( 151, 53, 57, 255 );
            case engine::e_movement_mode::nav_walking:
                return ImColor( 65, 65, 65, 255 );
            case engine::e_movement_mode::falling:
                return ImColor( 0, 106, 79, 255 );
            case engine::e_movement_mode::swimming:
                return ImColor( 1, 15, 204, 255 );
            case engine::e_movement_mode::flying:
                return ImColor( 144, 154, 251, 255 );
            default:
                return ImColor( 0, 0, 0, 0 );
            }
        }

        ImColor get_rank_color( int32_t rank ) {
            if ( rank >= 0 && rank <= 2 )
                return ImColor( 0xCD, 0x7F, 0x32, 255 );
            else if ( rank >= 3 && rank <= 5 )
                return ImColor( 93, 136, 187, 255 );
            else if ( rank >= 6 && rank <= 8 )
                return ImColor( 255, 170, 51, 255 );
            else if ( rank >= 9 && rank <= 11 )
                return ImColor( 93, 137, 186, 255 );
            else if ( rank >= 12 && rank <= 14 )
                return ImColor( 48, 92, 222, 255 );
            else if ( rank == 15 )
                return ImColor( 222, 181, 181, 255 );
            else if ( rank == 16 )
                return ImColor( 217, 144, 88, 255 );
            else if ( rank == 17 )
                return ImColor( 116, 66, 227, 255 );
            return ImColor( 0x80, 0x80, 0x80, 255 );
        }

        std::string get_rank_name( int32_t rank ) {
            if ( rank == 0 )
                return ( "Bronze I" );
            else if ( rank == 1 )
                return ( "Bronze II" );
            else if ( rank == 2 )
                return ( "Bronze III" );
            else if ( rank == 3 )
                return ( "Silver I" );
            else if ( rank == 4 )
                return ( "Silver II" );
            else if ( rank == 5 )
                return ( "Silver III" );
            else if ( rank == 6 )
                return ( "Gold I" );
            else if ( rank == 7 )
                return ( "Gold II" );
            else if ( rank == 8 )
                return ( "Gold III" );
            else if ( rank == 9 )
                return ( "Platinum I" );
            else if ( rank == 10 )
                return ( "Platinum II" );
            else if ( rank == 11 )
                return ( "Platinum III" );
            else if ( rank == 12 )
                return ( "Diamond I" );
            else if ( rank == 13 )
                return ( "Diamond II" );
            else if ( rank == 14 )
                return ( "Diamond III" );
            else if ( rank == 15 )
                return ( "Elite" );
            else if ( rank == 16 )
                return ( "Champion" );
            else if ( rank == 17 )
                return ( "Unreal" );
            return ( "No Rank" );
        }

        bool is_window_focused( ) {

            m_window_handle = mem::get_window_handle(mem::process_id);

            if ( m_window_handle == GetForegroundWindow( ) ) {
                return true;
            }

            if ( m_window_handle == GetActiveWindow( ) ) {
                return true;
            }

            if ( GetActiveWindow( ) == GetForegroundWindow( ) ) {
                return true;
            }

            return false;
        }

        void destroy( ) {
            ImGui_ImplDX11_Shutdown( );
            ImGui_ImplWin32_Shutdown( );
            ImGui::DestroyContext( );

            if ( m_render_target ) { m_render_target->Release( ); m_render_target = nullptr; }
            if ( m_swap_chain ) { m_swap_chain->Release( ); m_swap_chain = nullptr; }
            if ( m_device_context ) { m_device_context->Release( ); m_device_context = nullptr; }
            if ( m_d3d_device ) { m_d3d_device->Release( ); m_d3d_device = nullptr; }
        }

        void update_affinity( ) const {
            DWORD window_affinity;
            if ( GetWindowDisplayAffinity( m_window_handle, &window_affinity ) ) {
                auto new_affinity = config::misc::stream_proof ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE;
                if ( window_affinity != new_affinity ) {
                    SetWindowDisplayAffinity( m_window_handle, new_affinity );
                }
            }
        }

        int m_width = 0;
        int m_height = 0;

        int m_width_center = 0;
        int m_height_center = 0;

        MSG m_msg{ nullptr };
        HWND m_window_handle{ nullptr };

        bool m_menu_active = true;

    private:
        ID3D11Device* m_d3d_device = nullptr;
        ID3D11DeviceContext* m_device_context = nullptr;
        IDXGISwapChain* m_swap_chain = nullptr;
        ID3D11RenderTargetView* m_render_target = nullptr;
    };
}