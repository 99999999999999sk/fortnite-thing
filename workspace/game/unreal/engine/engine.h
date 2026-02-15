#define current_class reinterpret_cast<uint64_t>( this )
#define declare_member(type, name, offset) type name() { return read<type>( current_class + offset ); }
#define declare_member_bit(Bit, Name, Offset) bool Name( ) { return bool( read<char>( current_class + Offset) & (1 << Bit)); }
#define apply_member(type, name, offset) void name( type val ) { write<type>( current_class + offset, val); }
#define apply_member_bit(Bit, Name, Offset) void Name( bool Value ) { write<char>( read<char>( current_class + Offset) | (1 << Bit), Value); }
namespace engine {
	class a_fort_player_pawn_athena;
	class a_player_controller;
	f_vector2d project(f_vector& world_location);
	f_rotator find_look_at_rotation(f_vector& start, f_vector& target);
	class f_text {
	public:
		declare_member(std::uint64_t, data, 0x20);
		declare_member(std::int32_t, length, 0x28);
		std::string get() {
			const auto& __ftext = current_class;
			if (__ftext) {
				const auto& src = this->data();
				if (src) {
					const auto& size = this->length();
					if (size > 0 && size < 100) {
						std::unique_ptr<wchar_t[]> dst(new (std::nothrow) wchar_t[size]);
						mem::read_physical((PVOID)src, dst.get(), size * sizeof(wchar_t));
						std::wstring conversion(dst.get());
						return std::string(conversion.begin(), conversion.end());
					}
				}
			}
			return "";
		}
	};
	class f_string {
	public:
		std::string get() {
			const auto& __ftext = current_class;
			if (__ftext) {
				const auto& src = read<uintptr_t>(__ftext);
				if (src) {
					const auto& size = read<uintptr_t>(__ftext + 0x8);
					std::unique_ptr<wchar_t[]> dst(new (std::nothrow) wchar_t[size + 1]);
					mem::read_physical((PVOID)src, dst.get(), size * sizeof(wchar_t));
					std::wstring conversion(dst.get());
					return std::string(conversion.begin(), conversion.end());
				}
			}
			return "";
		}
	};
	class u_object {
	public:
		declare_member(fname, name_private, 0xc);
	};
	class u_actor_component : public u_object {
	public:
	};
	class u_scene_component : public u_actor_component {
	public:
		declare_member(f_vector, component_velocity, 0x180);
		declare_member(f_vector, relative_location, 0x138);
		declare_member(f_rotator, relative_rotation, 0x150);
	};
	class u_primitive_component : public u_scene_component {
	public:
		declare_member(float, last_render_time_on_screen, 0x32C);
		declare_member(int, custom_depth_stencil_value, 0x2a4);
		apply_member(int, custom_depth_stencil_value, 0x2a4);
		void render_state_dirty(bool enable) {
			auto current_byte = read<BYTE>(current_class + 0x95);
			auto new_byte = enable ? (current_byte | (1 << 0))
				: (current_byte & ~(1 << 0));
			if (current_byte == new_byte) return;
			write<BYTE>(current_class + 0x95, new_byte);
		}
		void b_render_in_depth_pass(bool enable) {
			auto field_depth = read<BYTE>(current_class + 0x271);
			auto new_depth = enable ? (field_depth | (1 << 6)) : (field_depth & ~(1 << 6));
			if (field_depth == new_depth) return;
			write<BYTE>(current_class + 0x271, new_depth);
		}
		void replicates(bool enable) {
			auto replication = read<BYTE>(current_class + 0x8c);
			BYTE new_replication = enable ? (replication | (1 << 3)) : (replication & ~(1 << 3));
			if (replication == new_replication) return;
			write<BYTE>(current_class + 0x8c, new_replication);
		}
	};
	class u_mesh_component : public u_primitive_component {
	public:
	};
	class u_skinned_mesh_component : public u_mesh_component {
	public:
		declare_member(std::int32_t, is_cached, 0x5E8 + 0x48);
		declare_member(tarray<f_transform>, bone_array, 0x5E8 + (this->is_cached() * 0x10));
		declare_member(f_transform, component_to_world, 0x1E0);
		declare_member(fbox_sphere_bounds, get_bounds, 0x100);
		f_vector get_bone_location(tarray < f_transform > bone_array, std::uint32_t bone_index) {
			auto component_to_world = this->component_to_world();
			component_to_world.fix_scaler();
			auto bone = bone_array.get(bone_index);
			auto matrix = bone.to_matrix().to_multiplication(
				component_to_world.to_matrix()
			);
			return matrix.w_plane;
		}
		f_rotator get_bone_rotation(tarray < f_transform > bone_array, std::uint32_t bone_index) {
			auto component_to_world = this->component_to_world();
			component_to_world.fix_scaler();
			auto bone = bone_array.get(bone_index);
			auto matrix = bone.to_matrix().to_multiplication(
				component_to_world.to_matrix()
			);
			float pitch, yaw, roll;
			pitch = atan2(-matrix.m[2][1], sqrt(matrix.m[0][1] * matrix.m[0][1] + matrix.m[1][1] * matrix.m[1][1]));
			yaw = atan2(matrix.m[0][1], matrix.m[1][1]);
			roll = atan2(matrix.m[2][0], matrix.m[2][2]);
			pitch = pitch * (180.0f / std::numbers::pi);
			yaw = yaw * (180.0f / std::numbers::pi);
			roll = roll * (180.0f / std::numbers::pi);
			return f_rotator(pitch, yaw, roll);
		}
	};
	class u_skeletal_mesh_component : public u_skinned_mesh_component {
	public:
		auto is_visible(double world_time_seconds) -> bool {
			return world_time_seconds - this->last_render_time_on_screen() <= 0.06f;
		}
	};
	class u_material_instance : public u_object {
	public:
		declare_member(u_material_instance*, parent, 0x128);
		declare_member(tarray<f_vector_parameter_value>, vector_parameter_values, 0x1a0);
		void wireframe(bool enable) {
			auto current_byte = read<BYTE>(current_class + 0x1d8);
			auto new_byte = enable ? (current_byte | (1 << 6))
				: (current_byte & ~(1 << 6));
			if (current_byte == new_byte) return;
			write<BYTE>(current_class + 0x1d8, new_byte);
		}
		void b_disable_depth_test(bool enable) {
			auto current_byte = read<BYTE>(current_class + 0x1c8);
			auto new_byte = enable ? (current_byte | (1 << 0))
				: (current_byte & ~(1 << 0));
			if (current_byte == new_byte) return;
			write<BYTE>(current_class + 0x1c8, new_byte);
		}
	};
	class u_material_instance_dynamic : public u_material_instance {
	public:
	};
	class a_actor : public u_object {
	public:
		declare_member(u_scene_component*, root_component, 0x1b0);
		declare_member(a_actor*, owner, 0x158);
	};
	class a_controller : public a_actor {
	public:
	};
	class u_fort_player_state_component_habanero : public a_actor {
	public:
		declare_member(int32_t, rank_progress, 0xd0 + 0x10);
	};
	class u_fort_item_definition : public a_actor {
	public:
		declare_member(e_fort_rarity, rarity, 0xa2);
		declare_member(f_text*, item_name, 0x40);
	};
	class u_fort_world_item_definition : public u_fort_item_definition {
	public:
	};
	class u_fort_weapon_item_defintion : public u_fort_world_item_definition {
	public:
	};
	class u_fort_weapon_anim_set {
	public:
	};
	class f_fort_ranged_weapon_recoil {
	public:
	};
	class u_fort_weapon_mod_runtime_data : public u_object {
	public:
	};
	class a_fort_weapon : public a_actor {
	public:
		declare_member(float, projectile_speed, 0x2264);
		declare_member(float, projectile_gravity, 0x2268);
		declare_member(float, current_projected_impact_distance, 0x11a0);
		declare_member(f_vector, current_adjusted_aim_direction, 0xfb8);
		declare_member(u_fort_weapon_item_defintion*, weapon_data, 0x558);
		declare_member(tarray<u_skeletal_mesh_component*>, all_weapon_meshes, 0xf08);
		declare_member(bool, b_is_reloading_weapon, 0x399);
		declare_member(int, ammo_count, 0x126c);
		declare_member(int, current_gun_fire_index, 0x10c8);
		declare_member(e_weapon_core_animation, weapon_core_animation, 0x1740);
		bool is_firing() {
			static auto old_fire_index = 0;
			auto gun_fire_index = this->current_gun_fire_index();
			if (gun_fire_index != old_fire_index) {
				old_fire_index = gun_fire_index;
				return true;
			}
			return false;
		}
	};
	struct f_item_component_data_list {
		uint8_t pad[0x108];
	};
	struct f_fast_array_serializer_item {
		int32_t replica_id;
		int32_t replica_id_next;
		uint8_t pad_08[0x4];
	};
	struct f_item_entry : f_fast_array_serializer_item {
		class u_fort_item_definition* item_definition;
		fguid item_entry_id;
		int32_t slot_number;
		f_item_component_data_list item_data;
		tarray<class u_script_struct*> removed_data_types;
		u_object* parent_object;
	};
	struct f_fort_item_entry : f_item_entry {
		int32_t previous_count;
	};
	class a_fort_pickup : public a_actor {
	public:
		declare_member(float, default_fly_time, 0x5d4);
		declare_member(f_fort_item_entry, primary_pickup_item_entry, 0x370);
		bool is_pickup() {
			auto default_fly_time = this->default_fly_time();
			if (default_fly_time != 1.5f)
				return false;
			return true;
		}
	};
	class a_player_state : public a_actor {
	public:
		declare_member(a_fort_player_pawn_athena*, pawn_private, 0x328);
		declare_member(tarray<a_player_state*>, spectators, 0xb98 + 0x108);
		declare_member(f_string*, platform, 0x430);
		declare_member(std::uint32_t, player_id, 0x2ac);
		declare_member_bit(3, b_is_a_bot, 0x2b2);
		declare_member_bit(4, b_is_inactive, 0x2b2);
		a_player_controller* get_player_controller() {
			return reinterpret_cast<a_player_controller*>(this->owner());
		}
		std::string get_platform_name() {
			std::uint64_t src;
			int size;
			const auto& state = current_class;
			if (state) {
				size = read<int>(current_class + 0x430 + 0x8);
				if (!size || size > 100)
					return "";
				src = read<std::uint64_t>(current_class + 0x430);
				if (!src)
					return "";
				if (size > 0 && size < 100) {
					std::unique_ptr<wchar_t[]> dst(new (std::nothrow) wchar_t[size]);
					mem::read_physical((PVOID)src, dst.get(), size * sizeof(wchar_t));
					std::wstring conversion(dst.get());
					return std::string(conversion.begin(), conversion.end());
				}
			}
			return "";
		}
		std::string get_player_name(bool is_lobby) {
			int v25;
			char v21;
			int v22;
			__int64 v6;
			std::uint64_t src;
			int size;
			const auto& state = current_class;
			if (state) {
				if (is_lobby) {
					size = read<int>(current_class + 0x340 + 0x8);
					if (!size || size > 100)
						return "Bot";
					src = read<std::uint64_t>(current_class + 0x340);
					if (!src)
						return "Bot";
				}
				else {
					const auto& __routine = read<std::uint64_t>(current_class + 0xb20);
					if (!__routine)
						return "Bot";
					size = read<int>(__routine + 0x10);
					if (!size || size > 100)
						return "Bot";
					src = read<std::uint64_t>(__routine + 0x8);
					if (!src)
						return "Bot";
				}
				if (size > 0 && size < 100) {
					std::unique_ptr<wchar_t[]> dst(new (std::nothrow) wchar_t[size]);
					mem::read_physical((PVOID)src, dst.get(), size * sizeof(wchar_t));
					v6 = size;
					v21 = v6 - 1;
					v22 = 0;
					if (v6 == 0)
						v21 = 0;
					_WORD* v23 = (_WORD*)(dst.get());
					for (int i = v21 & 3;; *v23++ += i & 7) {
						v25 = v6 - 1;
						if (v6 == 0)
							v25 = 0;
						if (v22 >= v25)
							break;
						i += 3;
						++v22;
					}
					std::wstring conversion(dst.get());
					return std::string(conversion.begin(), conversion.end());
				}
			}
			return "Bot";
		}
	};
	class a_fort_player_state : public a_player_state {
	public:
		declare_member(u_fort_player_state_component_habanero*, habanero_component, 0xa60);
	};
	class a_fort_player_state_zone : public a_fort_player_state {
	public:
	};
	class a_fort_player_state_athena : public a_fort_player_state_zone {
	public:
		declare_member(std::int32_t, kill_count, 0x1288);
		declare_member(std::int32_t, season_level_ui_display, 0x128c);
		declare_member(std::int32_t, team_id, 0x1271);
	};
	class a_pawn : public a_actor {
	public:
		declare_member(a_fort_player_state_athena*, player_state, 0x2d0);
	};
	class u_character_movement_component : public a_actor {
	public:
		declare_member(e_movement_mode, movement_mode, 0x231);
		declare_member(f_vector, last_update_velocity, 0x378);
	};
	class a_character : public a_pawn {
	public:
		declare_member(tarray<u_skeletal_mesh_component*>, skeletal_meshes, 0x6020);
		declare_member(tarray<u_material_instance_dynamic*>, pawn_materials_all, 0x6030);
		declare_member(u_character_movement_component*, character_movement, 0x330);
		declare_member(u_skeletal_mesh_component*, mesh, 0x330);
	};
	class afgf_character : public a_character {
	public:
	};
	class a_fort_pawn : public afgf_character {
	public:
		declare_member(a_fort_weapon*, current_weapon, 0xaa8);
		declare_member_bit(5, is_dying, 0x728);
		declare_member_bit(7, is_dbno, 0x98a);
	};
	class f_replicated_athena_vehicle_attributes {
	public:
	};
	class a_fort_athena_vehicle : public a_pawn {
	public:
	};
	class a_fort_player_pawn : public a_fort_pawn {
	public:
	};
	class a_fort_player_pawn_athena : public a_fort_player_pawn {
	public:
		declare_member(a_actor*, current_vehicle, 0x2c30);
		declare_member(f_vector, last_fired_location, 0x5960);
		declare_member(f_vector, last_fired_direction, 0x5978);
		declare_member(float, revive_from_dbno_time, 0x4d90);
	};
	class a_fort_damage_numbers_actor : public a_actor {
	public:
		declare_member(float, component_lifespan, 0x2b0);
		declare_member(fname, player_material_parameter_name, 0x300);
		declare_member(float, player_material_parameter_value, 0x304);
		declare_member(fname, color_parameter_name, 0x308);
		declare_member(fname, animation_lifespan_parameter_name, 0x3a0);
		declare_member(fname, is_critical_hit_parameter_name, 0x3a4);
		declare_member(float, spacing_percentage_for_ones, 0x3a8);
		declare_member(tarray<fname>, position_parameter_names, 0x3b0);
		declare_member(float, distance_from_camera_before_doubling_size, 0x3c0);
		declare_member(float, critical_hit_size_multiplier, 0x3c4);
		declare_member(float, font_x_size, 0x3c8);
		declare_member(float, font_y_size, 0x3cc);
		declare_member(f_vector, world_location_offset, 0x3d0);
		declare_member(float, number_of_number_rotations, 0x3e8);
		declare_member(tarray<fname>, scale_rotation_angle_parameter_names, 0x3f0);
		declare_member(tarray<fname>, duration_parameter_names, 0x400);
		declare_member(float, max_score_number_distance, 0x410);
		declare_member(fname, move_to_camera_parameter_name, 0x414);
		declare_member(tarray<f_linear_color*>, percent_damage_colors, 0x418);
		declare_member(tarray<int32_t>, percent_damage_thresholds, 0x428);
		declare_member(int32_t, custom_stencil_depth_value, 0x438);
		declare_member(float, custom_bounds_scale, 0x43c);
	};
	class a_player_controller : public a_controller {
	public:
		declare_member(a_fort_player_pawn_athena*, acknowledged_pawn, 0x358);
		apply_member(f_rotator, weapon_aim_offset, 0x2f48);
		apply_member(f_rotator, weapon_recoil_offset, 0x2f60);
		declare_member(a_fort_damage_numbers_actor*, damage_numbers_actor, 0x1ac8);
	};
	class u_scene_view_state : public u_object {
	public:
		declare_member(f_matrix, projection, 0x930);
		declare_member(double, field_of_view, 0x730);
	};
	class u_player : public u_object {
	public:
		declare_member(a_player_controller*, player_controller, 0x30);
	};
	class u_localplayer : public u_player {
	public:
		declare_member(tarray<u_scene_view_state*>, view_state, 0xd0);
		u_scene_view_state* get_view_state() {
			return view_state().get(1);
		}
	};
	class u_level : public u_object {
	public:
		declare_member(tarray<a_actor*>, actors, 0x48);
	};
	class u_game_instance : public u_object {
	public:
		declare_member(tarray<u_localplayer*>, localplayers, 0x38);
		auto get_localplayer() -> u_localplayer* {
			return this->localplayers().get_itter()[0];
		}
	};
	class a_game_state_base : public u_object {
	public:
		declare_member(float, server_world_time, 0x2e8);
		declare_member(tarray<a_fort_player_state_athena*>, player_array, 0x2c8);
	};
	class u_world : public u_object {
	public:
		declare_member(double, world_time_seconds, 0x190);
		declare_member(a_game_state_base*, game_state, 0x1d0);
		declare_member(u_game_instance*, game_instance, 0x248);
		declare_member(tarray<u_level*>, levels, 0x1e8);
		static auto get() -> u_world* {
			return read<u_world*>(virtualaddy + offsets::g_world);
		}
	};
}