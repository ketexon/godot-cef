#pragma once

#include <include/cef_app.h>
#include <include/cef_client.h>

class GDCefApp : public CefApp, public CefRenderProcessHandler, public CefBrowserProcessHandler
{
public:
	GDCefApp();
	~GDCefApp();

	// CefBrowserProcessHandler methods
	CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
	{
		return this;
	}

	virtual void OnContextInitialized() override;
	virtual CefRefPtr<CefClient> GetDefaultClient() override;

	// CefRenderProcessHandler methods
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override
	{
		return this;
	}

private:
	IMPLEMENT_REFCOUNTING(GDCefApp);
};