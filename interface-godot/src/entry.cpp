#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <game_board.h>

using namespace godot;

void register_gameplay_types(ModuleInitializationLevel p_level) 
{
	if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) 
	{
		return;
	}
	ClassDB::register_class<GameBoard>();
}

void unregister_gameplay_types(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
	{
        return;
	}
}

extern "C" {
	GDExtensionBool GDE_EXPORT gameplay_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
		
		init_obj.register_initializer(register_gameplay_types);
		init_obj.register_terminator(unregister_gameplay_types);
		init_obj.set_minimum_library_initialization_level(ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}