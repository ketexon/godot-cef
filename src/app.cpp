#include "app.hpp"
#include "client.hpp"

#include <iostream>

#include <include/wrapper/cef_helpers.h>

GDCefApp::GDCefApp()
{
}

GDCefApp::~GDCefApp()
{
}

void GDCefApp::OnContextInitialized()
{
	CEF_REQUIRE_UI_THREAD();

	CefRefPtr<GDCefClient> client = new GDCefClient;

	CefWindowInfo window_info;
	window_info.SetAsWindowless(nullptr);

	CefBrowserSettings browser_settings;
	browser_settings.windowless_frame_rate = 30;

	CefBrowserHost::CreateBrowserSync(
		window_info,
		client.get(),
		"res://gdcef/html/index.html",
		browser_settings,
		nullptr,
		nullptr);
	std::cout << "Browser created successfully." << std::endl;
}

CefRefPtr<CefClient> GDCefApp::GetDefaultClient()
{
	return GDCefClient::GetInstance();
}