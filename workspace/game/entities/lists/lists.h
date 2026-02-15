namespace lists {
	class c_bullet_tracer {
	private:
	public:
		c_bullet_tracer( ) = default;

		engine::f_vector m_start_location;
		engine::f_vector m_end_location;
		double m_impact_angle;
		float m_lifetime;
		float m_alpha;
		std::vector<engine::f_vector> m_trail_points;
	};

	class c_loot {
	private:
	public:
		c_loot( ) = default;

		engine::a_fort_pickup* m_actor;
		engine::u_scene_component* m_root_component;
		engine::u_fort_item_definition* m_item_definition;
		engine::f_vector m_world_location;
		engine::f_vector2d m_world_screen;
		engine::e_fort_rarity m_pickup_rarity;
		std::string m_pickup_name;
		float m_distance;
	};

	class c_actor {
	private:
	public:
		c_actor( ) = default;

		std::uint32_t m_player_id;
		engine::a_fort_player_pawn_athena* m_actor;
		engine::a_fort_player_state_athena* m_state;
		engine::a_fort_weapon* m_weapon;
		engine::a_actor* m_vehicle;
		engine::u_skeletal_mesh_component* m_mesh;
		engine::u_fort_weapon_item_defintion* m_weapon_data;
		engine::u_fort_player_state_component_habanero* m_habanero_component;
		engine::tarray< engine::u_skeletal_mesh_component* > m_mesh_components;
		engine::tarray< engine::u_material_instance_dynamic* > m_pawn_materials;
		engine::u_character_movement_component* m_movement;
		engine::u_scene_component* m_root_component;
		engine::a_player_controller* m_controller;
		engine::e_movement_mode m_movement_mode;
		engine::f_rotator m_player_rotation;
		engine::f_vector m_player_location;
		engine::f_vector m_velocity;
		std::string m_platform_name;
		std::string m_player_name;
		std::uint32_t m_weapon_ammo;
		std::uint32_t m_reserve_ammo;
		bool m_is_reloading;
		std::uint32_t m_player_level;
		std::uint32_t m_player_kills;
		std::uint32_t m_player_rank;
		engine::e_fort_rarity m_weapon_rarity;
		std::string m_weapon_name;
		engine::tarray<engine::f_transform> m_bone_array;
		engine::f_vector m_base_location;
		engine::f_vector m_head_location;
		engine::f_vector m_camera_location;
		engine::f_rotator m_camera_rotation;
		engine::f_vector2d m_base_screen;
		engine::f_vector2d m_head_screen;
		bool m_is_downed;
		bool m_is_bot;
		bool m_is_teammate;
		bool m_is_visible;
		float m_distance;
		double m_degrees;
		engine::fbox_sphere_bounds m_bounds;
		std::vector<std::pair<engine::f_vector, engine::f_vector>> m_skeleton_pairs;
	};
}