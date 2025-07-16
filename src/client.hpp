#pragma once

#include <include/cef_render_process_handler.h>
#include <include/cef_client.h>
#include <include/cef_v8.h>
#include <include/cef_browser.h>
#include <include/wrapper/cef_resource_manager.h>

class GDCefClient : public CefClient,
					public CefLifeSpanHandler,
					public CefRenderHandler,
					public CefRequestHandler,
					public CefResourceRequestHandler
{
public:
	GDCefClient();
	~GDCefClient();

	static GDCefClient *GetInstance();

	// CefClient methods
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }

	// CefLifeSpanHandler methods

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

	// CefRenderHandler methods
	virtual void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) override;
	virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;

	// CefRequestHandler methods
	virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool is_navigation,
		bool is_download,
		const CefString &request_initiator,
		bool &disable_default_handling) override {
			return this;
		}

	// CefResourceRequestHandler methods
	virtual cef_return_value_t OnBeforeResourceLoad(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		CefRefPtr<CefCallback> callback) override;

	virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request) override;

protected:
	CefRefPtr<CefBrowser> m_browser;
	CefRefPtr<CefResourceManager> m_resource_manager;

	int m_window_width = 800;
	int m_window_height = 600;

	IMPLEMENT_REFCOUNTING(GDCefClient);
};
