/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2017, 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TOAST_H
#define LIBREMAPPER_TOAST_H

#include <QObject>
#include <QWidget>

class QHideEvent;
class QLabel;
class QPaintEvent;
class QRect;
class QTimerEvent;
// IWYU pragma: no_include <QString>

namespace OpenOrienteering {


/**
 * @brief A Toast displays a short message for a few seconds.
 * 
 * Toasts are initially hidden. Upon showText(), they show just the text.
 * Normally they do not offer any interaction. But they can display links
 * which will trigger the linkActivated() signal.
 * 
 * Toast does not use standard colors, but white text on a translucent dark
 * background when the regular background is bright, or black text on a
 * translucent bright background otherwise.
 * 
 * Toast shall be constructed with a parent widget. Then they will place
 * themselves at the bottom of the parents window upon show. Without a parent,
 * they will be treated as independent windows which may cause visual glitches
 * on some platforms, and the caller is responsible for positioning.
 * 
 * Due do the lack of interaction, the text shall be kept small, and the
 * timeout is limited to an implementation-defined maximum value (5000 ms
 * at the moment). A minimum value is enforced, too (500 ms at the moment.)
 * 
 * Synopsis:
 * 
 *    auto* toast = new Toast(parent);
 *    toast->showText("Hello world.", 1500);
 * 
 * \see QTooltip, Android Toast, QStatusBar::showMessage
 */
class Toast : public QWidget
{
	Q_OBJECT
	
public:
	/**
	 * Constructs a new Toast.
	 */
	Toast(QWidget* parent = nullptr);
	
	/**
	 * Shows a Toast.
	 * 
	 * @param text     The text to be displayed. 
	 * @param timeout  The time how long the Toast will be visible.
	 */
	void showText(const QString& text, int timeout);
	
	/**
	 * Returns the current text.
	 */
	QString text() const;
	
	/**
	 * Moves the Toast to the center of the bottom of the given region.
	 */
	void adjustPosition(const QRect& region);
	
	
signals:
	/**
	 * This signal is emitted when the user clicks a link in the Toast.
	 */
	void linkActivated(const QString& link);
	
	
protected:
	/**
	 * Start the timer.
	 * 
	 * This will stop any timer which is already running.
	 */
	void startTimer(int timeout);
	
	/**
	 * Stops the timer if it is running.
	 */
	void stopTimer();
	
	/**
	 * Hides the Toast on timeout.
	 */
	void timerEvent(QTimerEvent* event) override;
	
	/**
	 * Stops the timer if it is still running.
	 */
	void hideEvent(QHideEvent* event) override;
	
	/**
	 * Draws the Toast's background.
	 */
	void paintEvent(QPaintEvent* event) override;
	
	
private:
	QLabel* label;
	int timer_id = 0;
	
};


}  // namespace OpenOrienteering

#endif
