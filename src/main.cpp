#include <iostream>

#include "App.hpp"
#include "Client.hpp"

#include <include/cef_app.h>
#include <include/cef_client.h>

#include <include/cef_render_process_handler.h>
#include <include/cef_v8.h>
#include <include/cef_browser.h>

int main(int argc, char *argv[])
{
	bool b_res;
	HINSTANCE h_instance;
	CefMainArgs main_args;
	CefSettings settings;
	CefRefPtr<GDCefApp> app;

	h_instance = GetModuleHandle(NULL);

	main_args = CefMainArgs{h_instance};

	settings.windowless_rendering_enabled = true;
	settings.multi_threaded_message_loop = false;
	settings.no_sandbox = true;
	settings.persist_session_cookies = false;

	app = new GDCefApp;

	std::cout << "Executing subprocess..." << std::endl;
	return CefExecuteProcess(main_args, app.get(), nullptr);
}