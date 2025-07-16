#include "client.hpp"
#include "cef_server.hpp"

#include <iostream>

#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/classes/os.hpp>
using namespace godot;

#include <include/cef_app.h>
#include <include/cef_base.h>
#include <include/cef_client.h>
#include <include/cef_command_line.h>
#include <include/cef_frame.h>
#include <include/wrapper/cef_stream_resource_handler.h>

GDCefClient *g_instance = nullptr;

class GodotResourceProvider : public CefResourceManager::Provider
{
public:
	GodotResourceProvider() = default;

	bool OnRequest(scoped_refptr<CefResourceManager::Request> request) override
	{
		CEF_REQUIRE_IO_THREAD();

		std::cout << "RequestDumpResourceProvider: Request URL: "
				  << request->url() << std::endl;

		std::string str =
			R"(
			<html>
			<body bgcolor="white">
				<pre id="meow">MEOW</pre>
				<script>
					const meow = document.getElementById("meow");
					let i = 0;
					setInterval(() => {
						meow.innerText = "MEOW " + i;
						i++;
					}, 1000);
				</script>
			</body>
			</html>)";
		CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForData(
			static_cast<void *>(const_cast<char *>(str.c_str())), str.size());
		DCHECK(stream.get());
		request->Continue(new CefStreamResourceHandler("text/html", stream));
		return true;
	}

private:
	std::string m_url;

	DISALLOW_COPY_AND_ASSIGN(GodotResourceProvider);
};

// Add example Providers to the CefResourceManager.
void SetupResourceManager(CefRefPtr<CefResourceManager> resource_manager)
{
	if (!CefCurrentlyOn(TID_IO))
	{
		// Execute on the browser IO thread.
		CefPostTask(TID_IO, base::BindOnce(SetupResourceManager, resource_manager));
		return;
	}

	// Add the Provider for dumping request contents.
	resource_manager->AddProvider(
		new GodotResourceProvider, 0,
		std::string());
}

GDCefClient::GDCefClient()
{
	DCHECK(!g_instance) << "GDCefClient instance already exists.";
	g_instance = this;

	m_resource_manager = new CefResourceManager;
	SetupResourceManager(m_resource_manager);
}

GDCefClient::~GDCefClient()
{
	g_instance = nullptr;
}

GDCefClient *GDCefClient::GetInstance()
{
	return g_instance;
}

void GDCefClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	if (!m_browser.get())
	{
		m_browser = browser;
	}
}

void GDCefClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	m_browser = nullptr;
}

void GDCefClient::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{
	std::cout << "OnPaint called with type: " << type << ", width: " << width << ", height: " << height << " on TID: " << OS::get_singleton()->get_thread_caller_id() << std::endl;
	CefServer::get_singleton()->on_paint(buffer, width, height);
}

void GDCefClient::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
	rect.Set(0, 0, m_window_width, m_window_height);
}

cef_return_value_t GDCefClient::OnBeforeResourceLoad(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	CefRefPtr<CefCallback> callback)
{
	return m_resource_manager->OnBeforeResourceLoad(
		browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler> GDCefClient::GetResourceHandler(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request)
{
	return m_resource_manager->GetResourceHandler(browser, frame, request);
}