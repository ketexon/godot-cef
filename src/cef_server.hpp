#pragma once

#include <godot_cpp/templates/rid_owner.hpp>
#include <godot_cpp/templates/rb_set.hpp>
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/image.hpp>

using namespace godot;

class GDCefBrowser {
	RID self;
};

class CefServer : public Object
{
	GDCLASS(CefServer, Object);

	static CefServer *singleton;
	void thread_func();
	void do_work();
	void on_paint(const void *buffer, int width, int height);

	friend class GDCefClient;

private:
	bool thread_exited = false;
	mutable bool exit_thread = false;
	Ref<Mutex> mutex;
	Ref<Thread> thread;
	Ref<Image> image;

public:
	static CefServer *get_singleton();
	Error init();
	void lock();
	void unlock();
	void finish();

private:
	bool did_initialize = false;
	bool initialize_cef();
	void uninitialize_cef();

	RID_Owner<GDCefBrowser> browser_owner;

public:
	RID browser_create()
	{

	}

protected:
	static void _bind_methods();

public:
	Ref<Image> get_image();

	CefServer();
	~CefServer();
};