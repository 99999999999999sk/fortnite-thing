namespace world {
    class c_world {
    public:
        void init( ) {
            std::jthread( [ this ]( std::stop_token token ) { update_engine( token ); } ).detach( );
            std::jthread( [ this ]( std::stop_token token ) { update_actors( token ); } ).detach( );
            std::jthread( [ this ]( std::stop_token token ) { update_players( token ); } ).detach( );
            std::jthread( [ this ]( std::stop_token token ) { update_pickups( token ); } ).detach( );
        }

        void loop_actors( const std::function<void( lists::c_actor const )>& render ) {
            auto player_list = m_player_list[ m_player_index.load( std::memory_order_acquire ) ];

            for ( const auto& actor : player_list ) {
                render( actor );
            }
        }

        void loop_pickups( const std::function<void( lists::c_loot const )>& render ) {
            auto pickup_list = m_pickup_list[ m_pickup_index.load( std::memory_order_acquire ) ];

            for ( const auto& loot : pickup_list ) {
                render( loot );
            }
        }

        engine::f_vector m_camera_location{};
        engine::f_rotator m_camera_rotation{};
        float m_camera_fov = 0.0f;

        double m_projectile_impact_distance = 0.f;
        std::vector< lists::c_bullet_tracer > m_tracer_list;
        engine::tarray< engine::a_player_state* > m_spectators{ };

        float m_projectile_speed = 0.f;
        float m_projectile_gravity = 0.f;
        engine::f_vector m_projectile_location{};
        engine::f_vector m_projectile_direction{};
        engine::e_weapon_core_animation m_weapon_animation{};

        engine::f_rotator m_player_rotation{ };
        engine::f_vector m_head_location{ };
        engine::f_vector m_local_velocity{ };

        engine::a_fort_player_state_athena* m_player_state = nullptr;
        engine::a_fort_player_pawn_athena* m_acknowledged_pawn = nullptr;
        engine::a_player_controller* m_player_controller = nullptr;
        engine::u_scene_view_state* m_view_state = nullptr;
        engine::u_localplayer* m_local_player = nullptr;
        engine::a_game_state_base* m_game_state = nullptr;
        engine::u_game_instance* m_game_instance = nullptr;
        engine::u_world* m_world = nullptr;

        bool m_is_lobby = false;
        double m_world_time_seconds = 0.0;

    private:
        constexpr static auto player_update_interval = std::chrono::milliseconds( 15 );
        constexpr static auto pickup_update_interval = std::chrono::milliseconds( 10 );
        constexpr static auto actor_update_interval = std::chrono::milliseconds( 5 );
        constexpr static auto engine_update_interval = std::chrono::milliseconds( 1 );

        struct engine_state_t {
            std::atomic<bool> m_is_valid{ false };
        } m_engine_state;

        struct player_buffer_t {
            std::vector<engine::a_fort_player_pawn_athena*> m_data;
        };

        struct pickup_buffer_t {
            std::vector<engine::a_fort_pickup*> m_data;
        };

        player_buffer_t m_player_cache[ 2 ];
        pickup_buffer_t m_pickup_cache[ 2 ];
        std::atomic<int> m_player_cache_index{ 0 };
        std::atomic<int> m_pickup_cache_index{ 0 };

        std::vector<lists::c_actor> m_player_list[ 2 ];
        std::vector<lists::c_loot> m_pickup_list[ 2 ];
        std::atomic<int> m_player_index{ 0 };
        std::atomic<int> m_pickup_index{ 0 };

        mutable srw_lock m_engine_lock;
        srw_lock m_player_cache_lock;
        srw_lock m_pickup_cache_lock; 

        void update_matrix( ) {
            auto projection = m_view_state->projection( );

            m_camera_rotation.pitch = engine::math::to_deg( std::asin( projection.z_plane.w ) );
            m_camera_rotation.yaw = engine::math::to_deg( std::atan2( projection.y_plane.w, projection.x_plane.w ) );
            m_camera_rotation.roll = 0.0;

            m_camera_location.x = projection.m[ 3 ][ 0 ];
            m_camera_location.y = projection.m[ 3 ][ 1 ];
            m_camera_location.z = projection.m[ 3 ][ 2 ];

            auto fov = atanf( 1 / m_view_state->field_of_view( ) ) * 2;
            m_camera_fov = engine::math::to_deg( fov );
        }

        bool update_classes( ) {
            auto world = engine::u_world::get( );
            if ( !world ) return false;

            auto game_state = world->game_state( );
            if ( !game_state ) return false;

            auto game_instance = world->game_instance( );
            if ( !game_instance ) return false;

            auto local_player = game_instance->get_localplayer( );
            if ( !local_player ) return false;

            auto view_state = local_player->get_view_state( );
            if ( !view_state ) return false;

            auto player_controller = local_player->player_controller( );
            if ( !player_controller ) return false;

            this->m_world = world;
            this->m_world_time_seconds = world->world_time_seconds( );
            this->m_game_state = game_state;
            this->m_is_lobby = game_state->server_world_time( ) ? false : true;
            this->m_game_instance = game_instance;
            this->m_local_player = local_player;
            this->m_view_state = view_state;
            this->m_player_controller = player_controller;
            this->update_matrix( );

            auto acknowledged_pawn = player_controller->acknowledged_pawn( );
            if ( acknowledged_pawn ) {
                auto mesh = acknowledged_pawn->mesh( );
                if ( mesh ) {
                    auto bone_array = mesh->bone_array( );
                    m_head_location = mesh->get_bone_location( bone_array, engine::e_bone::vb_head_fx );
                }

                auto root_component = acknowledged_pawn->root_component( );
                if ( root_component ) {
                    m_player_rotation = root_component->relative_rotation( );
                }

                auto current_weapon = acknowledged_pawn->current_weapon( );
                if ( current_weapon ) {
                    m_projectile_speed = current_weapon->projectile_speed( );
                    m_projectile_gravity = current_weapon->projectile_gravity( );
                    m_weapon_animation = current_weapon->weapon_core_animation( );
                    m_projectile_impact_distance = current_weapon->current_projected_impact_distance( );
                }

                auto player_state = acknowledged_pawn->player_state( );
                if ( player_state ) {
                    m_player_state = player_state;
                    m_spectators = player_state->spectators( );
                }

                auto character_movement = acknowledged_pawn->character_movement( );
                if ( character_movement ) {
                    m_local_velocity = character_movement->last_update_velocity( );
                }

                m_acknowledged_pawn = acknowledged_pawn;
                m_projectile_location = acknowledged_pawn->last_fired_location( );
                m_projectile_direction = acknowledged_pawn->last_fired_direction( );

                if ( config::misc::bullet_tracers ) {
                    if ( current_weapon && current_weapon->is_firing( ) ) {
                        if ( m_tracer_list.size( ) > config::misc::max_tracers )
                            m_tracer_list.clear( );

                        auto direction = m_camera_rotation.get_forward_vector( );
                        auto impact_direction = m_projectile_direction;

                        auto impact_angle = acos( direction.vector_scalar( impact_direction ) );
                        auto angle_degrees = impact_angle * ( 180.0 / std::numbers::pi );

                        auto start_location = m_projectile_location;
                        auto end_location = m_projectile_location + ( impact_direction * m_projectile_impact_distance );

                        if ( !is_duplicate_tracer( start_location, end_location ) ) {
                            add_bullet_tracer( start_location, end_location, impact_angle );
                        }
                    }
                }
            }

            return true;
        }

        void update_engine( std::stop_token stop_token ) {
            while ( !stop_token.stop_requested( ) ) {
                auto tick_start = std::chrono::steady_clock::now( );

                {
                    srw_unique_lock lock( m_engine_lock );
                    auto result = update_classes( );
                    if ( result ) {
                        m_engine_state.m_is_valid.store( true, std::memory_order_release );
                    }
                    else {
                        m_engine_state.m_is_valid.store( false, std::memory_order_release );
                    }
                }

                auto elapsed = std::chrono::steady_clock::now( ) - tick_start;
                auto sleep_time = engine_update_interval - elapsed;
                if ( sleep_time > std::chrono::milliseconds( 0 ) ) {
                    std::this_thread::sleep_for( sleep_time );
                }
            }
        }

        void update_actors( std::stop_token stop_token ) {
            while ( !stop_token.stop_requested( ) ) {
                auto tick_start = std::chrono::steady_clock::now( );

                m_engine_lock.lock_shared();
                bool engine_valid = m_engine_state.m_is_valid.load(std::memory_order_acquire);
                if ( !engine_valid ) {
                    m_engine_lock.unlock_shared();
                    std::this_thread::sleep_for( actor_update_interval );
                    continue;
                }

                auto world = m_world;
                auto game_state = m_game_state;
                auto local_state = m_player_state;
                auto acknowledged_pawn = m_acknowledged_pawn;

                if ( config::world::enabled && world ) {
                    m_pickup_cache_lock.lock_exclusive();
                    auto pickup_write_idx = 1 - m_pickup_cache_index.load( std::memory_order_acquire );
                    auto& pickup_buffer = m_pickup_cache[ pickup_write_idx ];
                    pickup_buffer.m_data.clear( );

                    auto levels_array = world->levels( );
                    if ( levels_array.is_valid( ) ) {
                        for ( auto idx = 0ul; idx < levels_array.size( ); idx++ ) {
                            auto level = levels_array.get( idx );
                            if ( !level ) continue;

                            auto actors_array = level->actors( );
                            if ( !actors_array.is_valid( ) ) continue;

                            for ( auto i = 0ul; i < actors_array.size( ); i++ ) {
                                auto actor = actors_array.get( i );
                                if ( !actor ) continue;

                                auto pickup = reinterpret_cast< engine::a_fort_pickup* >( actor );
                                if ( pickup && pickup->is_pickup( ) ) {
                                    pickup_buffer.m_data.emplace_back( pickup );
                                }
                            }
                        }
                    }

                    m_pickup_cache_index.store( pickup_write_idx, std::memory_order_release );
                    m_pickup_cache_lock.unlock_exclusive();
                }

                if ( game_state ) {
                    m_player_cache_lock.lock_exclusive();
                    auto player_write_idx = 1 - m_player_cache_index.load( std::memory_order_acquire );
                    auto& player_buffer = m_player_cache[ player_write_idx ];
                    player_buffer.m_data.clear( );

                    auto player_array = game_state->player_array( );
                    if ( player_array.is_valid( ) ) {
                        for ( auto idx = 0ul; idx < player_array.size( ); idx++ ) {
                            auto player_state = player_array.get( idx );
                            if ( !player_state ||
                                (local_state && player_state->team_id( ) == local_state->team_id( )) )  continue;

                            auto pawn_private = player_state->pawn_private( );
                            if ( !pawn_private ||
                                pawn_private == acknowledged_pawn ||
                                pawn_private->is_dying( ) ) continue;

                            player_buffer.m_data.emplace_back( pawn_private );
                        }
                    }

                    m_player_cache_index.store( player_write_idx, std::memory_order_release );
                    m_player_cache_lock.unlock_exclusive();
                }

                m_engine_lock.unlock_shared();

                auto elapsed = std::chrono::steady_clock::now( ) - tick_start;
                auto sleep_time = actor_update_interval - elapsed;
                if ( sleep_time > std::chrono::milliseconds( 0 ) ) {
                    std::this_thread::sleep_for( sleep_time );
                }
            }
        }

        void update_players( std::stop_token stop_token ) {
            while ( true ) {
                auto tick_start = std::chrono::steady_clock::now( );

                const auto& cache = m_player_cache[ m_player_cache_index.load( std::memory_order_acquire ) ];
                auto player_list = cache.m_data;


                auto write_idx = 1 - m_player_index.load( std::memory_order_acquire );
                auto& list_buffer = m_player_list[ write_idx ];
                list_buffer.clear( );

                for ( auto player : player_list ) {
                    if ( !player ) continue;

                    auto mesh = player->mesh( );
                    if ( !mesh ) continue;

                    auto bone_array = mesh->bone_array( );
                    if ( !bone_array.is_valid( ) ) continue;

                    auto base_location = mesh->get_bone_location( bone_array, engine::e_bone::root );
                    auto head_location = mesh->get_bone_location( bone_array, engine::e_bone::vb_head_fx );
                    auto camera_location = mesh->get_bone_location( bone_array, engine::e_bone::camera );
                    auto camera_rotation = mesh->get_bone_rotation( bone_array, engine::e_bone::camera );
                    if ( !base_location.is_valid( ) || !head_location.is_valid( ) || !camera_location.is_valid( ) ) continue;

                    auto head_screen = engine::project( head_location );
                    auto base_screen = engine::project( base_location );
                    if ( !base_screen.is_valid( ) || !head_screen.is_valid( ) ) continue;

                    auto distance = m_camera_location.distance_to( base_location );
                    if ( distance >= config::visuals::max_distance ) continue;

                    lists::c_actor actor;
                    actor.m_actor = player;
                    actor.m_mesh = mesh;
                    actor.m_bone_array = bone_array;
                    actor.m_base_location = base_location;
                    actor.m_base_screen = base_screen;
                    actor.m_head_location = head_location;
                    actor.m_head_screen = head_screen;
                    actor.m_camera_location = camera_location;
                    actor.m_camera_rotation = camera_rotation;
                    actor.m_distance = distance;
                    actor.m_root_component = player->root_component( );
                    actor.m_movement = player->character_movement( );
                    actor.m_mesh_components = player->skeletal_meshes( );
                    actor.m_pawn_materials = player->pawn_materials_all( );
                    actor.m_state = player->player_state( );
                    actor.m_weapon = player->current_weapon( );
                    actor.m_vehicle = player->current_vehicle( );
                    actor.m_is_downed = player->is_dbno( );
                    actor.m_is_visible = mesh->is_visible( m_world_time_seconds );
                    actor.m_bounds = mesh->get_bounds( );

                    if ( actor.m_state ) {
                        actor.m_player_name = actor.m_state->get_player_name( m_is_lobby );
                        actor.m_player_id = actor.m_state->player_id( );
                        actor.m_player_kills = actor.m_state->kill_count( );
                        actor.m_player_level = actor.m_state->season_level_ui_display( );
                        actor.m_habanero_component = actor.m_state->habanero_component( );
                        actor.m_controller = actor.m_state->get_player_controller( );
                        actor.m_platform_name = actor.m_state->get_platform_name( );
                        actor.m_is_bot = actor.m_state->b_is_a_bot( );
                    }

                    if ( actor.m_weapon ) {
                        actor.m_weapon_ammo = actor.m_weapon->ammo_count( );
                        actor.m_is_reloading = actor.m_weapon->b_is_reloading_weapon( );
                        actor.m_weapon_data = actor.m_weapon->weapon_data( );
                        actor.m_weapon_name = actor.m_weapon_data->item_name( )->get( );
                        actor.m_weapon_rarity = actor.m_weapon_data->rarity( );
                    }

                    if ( actor.m_vehicle ) {
                        actor.m_root_component = actor.m_vehicle->root_component( );
                    }

                    if ( actor.m_root_component ) {
                        actor.m_player_rotation = actor.m_root_component->relative_rotation( );
                        actor.m_player_location = actor.m_root_component->relative_location( );
                        actor.m_velocity = actor.m_root_component->component_velocity( );
                    }

                    if ( actor.m_movement ) {
                        actor.m_movement_mode = actor.m_movement->movement_mode( );
                    }

                    if ( actor.m_habanero_component ) {
                        actor.m_player_rank = actor.m_habanero_component->rank_progress( );
                    }

                    if ( config::visuals::enabled ) {
                        if ( config::visuals::stencil ) {
                            for ( auto idx = 0ul; idx < actor.m_mesh_components.size( ); idx++ ) {
                                auto component = actor.m_mesh_components.get( idx );
                                if ( !component || g_chams->is_modified( component ) ) {
                                    continue;
                                }

                                auto selected_context = 11;
                                component->render_state_dirty( 1 );
                                component->custom_depth_stencil_value( selected_context );
                                component->b_render_in_depth_pass( 1 );
                                component->replicates( 1 );

                                g_chams->add( component );
                            }
                        }

                        if ( config::visuals::skeleton ) {
                            for ( const auto& pair : engine::skeleton_pairs ) {
                                auto world_location_first = mesh->get_bone_location( bone_array, pair.first );
                                auto world_location_second = mesh->get_bone_location( bone_array, pair.second );

                                actor.m_skeleton_pairs.push_back( { world_location_first, world_location_second } );
                            }
                        }

                        if ( config::visuals::movement_tracers ) {
                            g_player_traces[ actor.m_actor ].add_trace( actor.m_base_location );
                        }
                    }

                    list_buffer.emplace_back( std::move( actor ) );
                }

                m_player_index.store( write_idx, std::memory_order_release );
                std::this_thread::sleep_until( tick_start + player_update_interval );
            }
        }

        void update_pickups( std::stop_token stop_token ) {
            while ( true ) {
                auto tick_start = std::chrono::steady_clock::now( );

                const auto& cache = m_pickup_cache[ m_pickup_cache_index.load( std::memory_order_acquire ) ];
                auto pickup_list = cache.m_data;

                auto write_idx = 1 - m_pickup_index.load( std::memory_order_acquire );
                auto& list_buffer = m_pickup_list[ write_idx ];
                list_buffer.clear( );
                list_buffer.reserve( pickup_list.size( ) );

                for ( auto pickup : pickup_list ) {
                    if ( !pickup ) continue;

                    auto item_entry = pickup->primary_pickup_item_entry( );
                    auto item_definition = item_entry.item_definition;
                    if ( !item_definition ) continue;

                    auto root_component = pickup->root_component( );
                    if ( !root_component ) continue;

                    auto world_location = root_component->relative_location( );
                    if ( !world_location.is_valid( ) ) continue;

                    auto world_screen = engine::project( world_location );
                    if ( !world_screen.is_valid( ) ) continue;

                    auto distance = m_camera_location.distance_to( world_location );
                    if ( distance >= config::world::max_distance ) continue;

                    lists::c_loot loot;
                    loot.m_actor = pickup;
                    loot.m_item_definition = item_definition;
                    loot.m_pickup_name = item_definition->item_name( )->get( );
                    loot.m_pickup_rarity = item_definition->rarity( );
                    loot.m_root_component = root_component;
                    loot.m_world_location = world_location;
                    loot.m_world_screen = world_screen;
                    loot.m_distance = distance;

                    list_buffer.emplace_back( std::move( loot ) );
                }

                m_pickup_index.store( write_idx, std::memory_order_release );
                std::this_thread::sleep_until( tick_start + pickup_update_interval );
            }
        }

        bool is_duplicate_tracer( engine::f_vector& start, engine::f_vector& end, double threshold = 0.50 ) {
            for ( const auto& tracer : m_tracer_list ) {
                auto tracer_start = tracer.m_start_location;
                auto tracer_end = tracer.m_end_location;
                if ( tracer_start.distance_to( start ) < threshold &&
                    tracer_end.distance_to( end ) < threshold )
                    return true;
            }
            return false;
        }

        void add_bullet_tracer( const engine::f_vector& start, const engine::f_vector& end, float impact_angle ) {
            if ( m_tracer_list.size( ) > config::misc::max_tracers ) {
                m_tracer_list.clear( );
            }

            lists::c_bullet_tracer bullet_tracer;
            bullet_tracer.m_start_location = start;
            bullet_tracer.m_end_location = end;
            bullet_tracer.m_impact_angle = impact_angle;
            bullet_tracer.m_lifetime = 0.0f;
            bullet_tracer.m_alpha = 1.0f;
            bullet_tracer.m_trail_points.push_back( end );

            m_tracer_list.emplace_back( std::move( bullet_tracer ) );
        }
    };
}