#include "FluxPCH.h"
#include "OpenGLContext.h"

#include "Flux/Runtime/Core/Engine.h"

#include <glad/glad_wgl.h>

namespace Flux {

	OpenGLContext::OpenGLContext(WindowHandle windowHandle)
		: m_WindowHandle(windowHandle)
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();

		HWND hWnd = static_cast<HWND>(Platform::GetHelperWindowHandle());

		HDC hDC = GetDC(hWnd);
		if (!hDC)
		{
			FLUX_VERIFY(false, "Failed to retrieve DC for window");
			return;
		}

		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;

		if (!SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd))
		{
			FLUX_VERIFY(false, "Failed to set pixel format.");
			return;
		}

		HGLRC tempContext = wglCreateContext(hDC);
		if (!tempContext)
		{
			FLUX_VERIFY(false, "Failed to create OpenGL context.");
			return;
		}

		if (!wglMakeCurrent(hDC, tempContext))
		{
			FLUX_VERIFY(false, "Failed to make context current.");
			return;
		}

		if (!gladLoadWGL(hDC))
			FLUX_VERIFY(false, "Failed to load WGL extensions.");

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
	}

	OpenGLContext::~OpenGLContext()
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();

		if (m_Context)
			wglDeleteContext(m_Context);

		gladUnloadWGL();
	}

	bool OpenGLContext::Init()
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();

		HWND hWnd = static_cast<HWND>(m_WindowHandle);

		m_HDC = GetDC(hWnd);
		if (!m_HDC)
		{
			FLUX_VERIFY(false, "Failed to retrieve DC for window");
			return false;
		}

		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;

		if (!SetPixelFormat(m_HDC, ChoosePixelFormat(m_HDC, &pfd), &pfd))
		{
			FLUX_VERIFY(false, "Failed to set pixel format.");
			return false;
		}

		int32 flags = 0;
#ifndef FLUX_BUILD_SHIPPING
		flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif

		int32 attribList[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 2,
			WGL_CONTEXT_FLAGS_ARB, flags,
			0
		};

		m_Context = wglCreateContextAttribsARB(m_HDC, NULL, attribList);
		if (!m_Context)
		{
			FLUX_VERIFY(false, "wglCreateContextAttribsARB failed.");
			return false;
		}

		wglMakeCurrent(m_HDC, m_Context);

		if (!gladLoadGL())
		{
			FLUX_VERIFY(false, "Failed to initialize Glad.");
			return false;
		}

		wglSwapIntervalEXT(m_SwapInterval);

		FLUX_INFO_CATEGORY("Renderer", "GPU Info:");
		FLUX_INFO_CATEGORY("Renderer", "  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		FLUX_INFO_CATEGORY("Renderer", "  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		FLUX_INFO_CATEGORY("Renderer", "  Version: {0}", (const char*)glGetString(GL_VERSION));

#ifndef FLUX_BUILD_SHIPPING
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback([](uint32 source, uint32 type, uint32 id, uint32 severity, int32 length, const char* message, const void* userParam)
		{
			switch (severity) {
			case GL_DEBUG_SEVERITY_HIGH:
				FLUX_CRITICAL("[OpenGL] {0}", message);
				FLUX_VERIFY(false);
				break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				FLUX_ERROR("[OpenGL] {0}", message);
				break;
			case GL_DEBUG_SEVERITY_LOW:
				FLUX_WARNING("[OpenGL] {0}", message);
				break;
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				// FLUX_INFO("[OpenGL] {0}", message);
				break;
			}
		}, this);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

		return true;
	}

	void OpenGLContext::SwapBuffers(int32 swapInterval)
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();

		if (m_SwapInterval != swapInterval)
		{
			wglSwapIntervalEXT(swapInterval);
			m_SwapInterval = swapInterval;
		}

		if (!::SwapBuffers(m_HDC))
			FLUX_VERIFY(false, "SwapBuffers failed. ({0})", Platform::GetErrorMessage());
	}

}