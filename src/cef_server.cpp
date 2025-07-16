#include "cef_server.hpp"
#include "app.hpp"

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/os.hpp>

#include <include/cef_app.h>
#include <include/cef_client.h>

#include <include/cef_render_process_handler.h>
#include <include/cef_v8.h>
#include <include/cef_browser.h>

using namespace godot;

void CefServer::thread_func()
{
	print_line("CefServer thread started.");
	constexpr int64_t sleep_usec = 1000 * 1000;
	while (!exit_thread)
	{
		call_deferred("do_work");
		OS::get_singleton()->delay_usec(sleep_usec);
	}
}

void CefServer::do_work()
{
	print_line("CefServer::do_work on TID: " + itos(OS::get_singleton()->get_thread_caller_id()));
	CefDoMessageLoopWork();
}

void CefServer::on_paint(const void *buffer, int width, int height)
{
	print_line("CefServer::on_paint called with width: " + itos(width) + ", height: " + itos(height));
	PackedByteArray bytes;
	bytes.resize(width * height * 4);
	uint8_t *data = bytes.ptrw();
	memcpy(data, buffer, width * height * 4);

	lock();
	image->set_data(width, height, false, Image::FORMAT_RGBA8, bytes);
	unlock();
}

Error CefServer::init()
{
	thread_exited = false;
	mutex = Ref<Mutex>(memnew(Mutex));
	thread = Ref<Thread>(memnew(Thread));
	image = Image::create_empty(1, 1, false, Image::FORMAT_RGBA8);

	if (!initialize_cef())
	{
		print_error("Failed to initialize CEF.");
		return FAILED;
	}

	thread->start(Callable(this, "thread_func"), Thread::PRIORITY_NORMAL);

	return OK;
}

void CefServer::finish()
{
	if (!thread.is_valid())
	{
		return;
	}

	exit_thread = true;
	thread->wait_to_finish();

	mutex = Ref<Mutex>();
	thread = Ref<Thread>();

	uninitialize_cef();
}

void CefServer::_bind_methods()
{
	print_line("Binding CefServer methods...");

	ClassDB::bind_method(D_METHOD("thread_func"), &CefServer::thread_func);
	ClassDB::bind_method(D_METHOD("do_work"), &CefServer::do_work);
	ClassDB::bind_method(D_METHOD("get_image"), &CefServer::get_image);
}

CefServer *CefServer::singleton = nullptr;
CefServer *CefServer::get_singleton()
{
	return singleton;
}

// START LOCKING

void CefServer::lock()
{
	if (!thread.is_valid() || !mutex.is_valid())
	{
		return;
	}
	mutex->lock();
}

void CefServer::unlock()
{
	if (!thread.is_valid() || !mutex.is_valid())
	{
		return;
	}
	mutex->unlock();
}

// END LOCKING

CefServer::CefServer()
{
	singleton = this;
}

CefServer::~CefServer()
{
	singleton = nullptr;
}

Ref<Image> CefServer::get_image()
{
	lock();
	Ref<Image> copied_image = Ref<Image>(memnew(Image));
	copied_image->copy_from(image);
	unlock();
	return copied_image;
}

bool CefServer::initialize_cef()
{
	if (did_initialize)
	{
		return true;
	}

	print_line("Initializing CEF... TID: " + uitos(OS::get_singleton()->get_thread_caller_id()));

	bool b_res;
	CefMainArgs main_args;
	CefSettings settings;
	CefRefPtr<GDCefApp> app;
	int exit_code;

	// use empty args
	main_args = CefMainArgs{};

	settings.windowless_rendering_enabled = true;
	settings.multi_threaded_message_loop = false;
	settings.no_sandbox = true;
	settings.persist_session_cookies = false;
	settings.background_color = CefColorSetARGB(0, 0, 0, 0);

	{
		// get the absolute path of the browser subprocess
		Ref<FileAccess> file_access = FileAccess::open("res://gdcef/build/src/Debug/gdcef-subprocess.exe", FileAccess::READ);
		if (!file_access.is_valid())
		{
			print_error("Failed to open gdcef-subprocess.exe");
			return false;
		}
		CharString file_path = file_access->get_path_absolute().ascii();
		CefString(&settings.browser_subprocess_path).FromASCII(file_path.get_data());
		file_access->close();
	}

	app = new GDCefApp;

	b_res = CefInitialize(main_args, settings, app.get(), nullptr);

	if (!b_res)
	{
		int exit_code = CefGetExitCode();
		print_error("Failed to initialize CEF: " + itos(exit_code));
		return false;
	}
	did_initialize = true;
	return true;
}

void CefServer::uninitialize_cef()
{
	if (did_initialize)
	{
		print_line("Uninitializing CEF...");
		CefShutdown();
		did_initialize = false;
	}
}
