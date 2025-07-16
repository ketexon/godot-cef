#include "register_types.hpp"

#include "cef_server.hpp"

#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

bool init = false;

void initialize_gdcef_module(ModuleInitializationLevel p_level)
{
	if(init) return;
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		GDREGISTER_CLASS(CefServer);

		CefServer* cef_server = CefServer::get_singleton();
		if(cef_server == nullptr)
		{
			print_line("Creating CefServer singleton instance.");
			cef_server = memnew(CefServer);
			cef_server->init();
		}

		print_line("Registering CefServer singleton instance.");

		Engine::get_singleton()->register_singleton("CefServer", cef_server);

		init = true;
	}
}

void uninitialize_gdcef_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		CefServer* cef_server = CefServer::get_singleton();
		if (cef_server != nullptr){
			print_line("Deleting CefServer singleton instance.");
			cef_server->finish();
			memdelete(cef_server);
		}
		if (Engine::get_singleton()->has_singleton("CefServer")){
			print_line("Unregistering CefServer singleton from Engine.");
			Engine::get_singleton()->unregister_singleton("CefServer");
		}
		init = false;
	}
}

extern "C"
{
	GDExtensionBool GDE_EXPORT
	gdcef_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		GDExtensionBinding::InitObject init_object{
			p_get_proc_address,
			p_library,
			r_initialization};

		init_object.register_initializer(initialize_gdcef_module);
		init_object.register_terminator(uninitialize_gdcef_module);
		init_object.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SERVERS);

		return init_object.init();
	}
}