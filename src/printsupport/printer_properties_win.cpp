/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "printer_properties.h"

#include <QApplication>
#include <QDialog>
#include <private/qprintengine_win_p.h>
#include <qpa/qplatformnativeinterface.h>

namespace
{
	/** 
	 * A helper class which makes the QWin32PrintEnginePrivate member accessible.
	 */
	class AccessToPrintEnginePrivate : public QWin32PrintEngine
	{
	public:
		Q_DECLARE_PRIVATE(QWin32PrintEngine)
	};
	
	/**
	 * Returns the printer's QWin32PrintEngine if possible, or nullptr.
	 */
	template <typename PointerToPrinter>
	AccessToPrintEnginePrivate* win32PrintEngine(PointerToPrinter printer)
	{
		// Attention, QPrintPreviewWidget uses QPreviewPaintEngine,
		// type() == QPaintEngine::Picture
		if (printer
		    && printer->outputFormat() == QPrinter::NativeFormat
		    && printer->paintEngine()->type() == QPaintEngine::Windows)
			return static_cast<AccessToPrintEnginePrivate*>(printer->printEngine());
		else
			return nullptr;
	}
	
	/**
	 * Deletes the properties' data structure.
	 */
	struct
	{
		void operator()(void* handle) const
		{
			if (handle
			    && GlobalUnlock(handle) == 0
			    && GetLastError() == NO_ERROR)
			{
				GlobalFree(handle);
			}
			else
			{
				qDebug("Could not unlock and free handle");
			}
		}
	} tryUnlockAndFree;
	
}


namespace PlatformPrinterProperties
{
	void save(const QPrinter* printer, std::shared_ptr<void>& buffer)
	{
		auto engine = win32PrintEngine(printer);
		if (!engine)
			return;
		
		auto ep = engine->d_func();
		if (!ep->hPrinter)
			return;
		
		auto ep_devmode = ep->devMode;
		auto ep_handle = ep->globalDevMode;
		if (ep_handle)
		{
			if (ep_handle == buffer.get())
				return; // Printer properties already in buffer
			ep_devmode = static_cast<DEVMODE*>(GlobalLock(ep_handle));
		}
		if (!ep_devmode)
			return;
		
		auto devmode_size = sizeof(DEVMODE) + ep_devmode->dmDriverExtra;
		if (auto devmode_handle = GlobalAlloc(GHND, devmode_size))
		{
			if (auto devmode = static_cast<DEVMODE*>(GlobalLock(devmode_handle)))
			{
				memcpy(devmode, ep_devmode, devmode_size);
				buffer.reset(devmode_handle, tryUnlockAndFree);
				// Handle in buffer will have a lock count of one.
			}
			else
			{
				qDebug("Could not lock handle");
				GlobalFree(devmode_handle);
			}
		}
		
		if (ep_handle)
			GlobalUnlock(ep_handle);
	}
	
	void restore(QPrinter* printer, const std::shared_ptr<void>& buffer)
	{
		if (!buffer)
			return;
		
		auto engine = win32PrintEngine(printer);
		if (!engine)
			return;
		
		auto ep = engine->d_func();
		if (!ep->hPrinter)
			return;
		
		auto ep_devmode = ep->devMode;
		auto ep_handle = ep->globalDevMode;
		if (ep_handle)
		{
			if (ep_handle == buffer.get())
				return; // Buffer already effective
			ep_devmode = static_cast<DEVMODE*>(GlobalLock(ep_handle));
		}
		if (!ep_devmode)
			return;
		
		// Handle in buffer already has a lock count of one.
		// We need a (temporary) lock to get the right pointer.
		auto devmode = static_cast<const DEVMODE*>(GlobalLock(buffer.get()));
		GlobalUnlock(buffer.get());
		if (wcsncmp(ep_devmode->dmDeviceName, devmode->dmDeviceName, CCHDEVICENAME) == 0
		    && ep_devmode->dmSpecVersion == devmode->dmSpecVersion
		    && ep_devmode->dmDriverVersion == devmode->dmDriverVersion
		    && ep_devmode->dmDriverExtra == devmode->dmDriverExtra)
		{
			// Same device
			engine->setGlobalDevMode(nullptr, buffer.get());
			tryUnlockAndFree(ep_handle);
			return;
		}
		
		if (ep_handle)
			GlobalUnlock(ep_handle);
	}
	
	bool dialogSupported()
	{
		return true;
	}
	
	int execDialog(QPrinter* printer, std::shared_ptr<void>& buffer, QWidget* parent)
	{
		auto engine = win32PrintEngine(printer);
		if (!engine)
			return QDialog::Rejected;
		
		auto ep = engine->d_func();
		if (!ep->hPrinter)
			return QDialog::Rejected;
		
		auto ep_devmode = ep->devMode;
		auto ep_handle = ep->globalDevMode;
		if (ep_handle)
			ep_devmode = static_cast<DEVMODE*>(GlobalLock(ep_handle));
		if (!ep_devmode)
			return QDialog::Rejected;
		
		HWND hwnd = nullptr;
		auto window = parent ? parent->window() : QApplication::activeWindow();
		if (auto window_handle = window->windowHandle())
			hwnd = HWND(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("handle", window_handle));
		
		auto devmode_size = sizeof(DEVMODE) + ep_devmode->dmDriverExtra;
		if (auto devmode_handle = GlobalAlloc(GHND, devmode_size))
		{
			if (auto devmode = static_cast<DEVMODE*>(GlobalLock(devmode_handle)))
			{
				auto result = DocumentProperties(hwnd, ep->hPrinter, LPWSTR(ep->m_printDevice.id().utf16()),
				                                 devmode, ep_devmode,
				                                 DM_IN_BUFFER | DM_IN_PROMPT | DM_OUT_BUFFER);
				if (result == IDOK)
				{
					// Properties accepted
					engine->setGlobalDevMode(nullptr, devmode_handle);
					buffer.reset(devmode_handle, tryUnlockAndFree);
					tryUnlockAndFree(ep_handle);
					return QDialog::Accepted;
				}
			}
			else
			{
				qDebug("Could not lock handle");
			}
			GlobalFree(devmode_handle);
		}
		
		if (ep_handle)
			GlobalUnlock(ep_handle);
		
		return QDialog::Rejected;
	}
}
