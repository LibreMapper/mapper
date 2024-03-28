/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_DRAW_TEXT_TOOL_H
#define OPENORIENTEERING_DRAW_TEXT_TOOL_H

#include <memory>

#include <Qt>
#include <QObject>
#include <QVariant>
#include <QString>

#include "core/renderables/renderable.h"
#include "tools/tool_base.h"

class QAction;
class QEvent;
class QInputMethodEvent;
class QKeyEvent;
class QMouseEvent;
class QPainter;
class QRectF;

namespace OpenOrienteering {

class MapCoordF;
class MapEditorController;
class MapWidget;
class Symbol;
class TextObject;
class TextObjectEditorHelper;


/**
 * Tool to draw text objects.
 * 
 * This class is marked as final because its destructor calls virtual functions.
 */
class DrawTextTool final : public MapEditorToolBase
{
Q_OBJECT
public:
	DrawTextTool(MapEditorController* editor, QAction* tool_action);
	~DrawTextTool() override;
	
	void setDrawingSymbol(const Symbol* symbol);
	
protected:
	void initImpl() override;
	
	void objectSelectionChangedImpl() override;
	
	void startEditing();
	void abortEditing();
	void finishEditing() override;
	
	bool mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseMoveEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseReleaseEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	
	bool inputMethodEvent(QInputMethodEvent* event) override;
	QVariant inputMethodQuery(Qt::InputMethodQuery property, const QVariant& argument) const override;
	
	void mouseMove() override;
	void clickPress() override;
	void clickRelease() override;
	void dragMove() override;
	void dragFinish() override;
	
	bool keyPress(QKeyEvent* event) override;
	bool keyRelease(QKeyEvent* event) override;
	
	void leaveEvent(QEvent* event) override;
	
	void resetWaitingForMouseRelease();
	void updatePreview();
	void updatePreviewText();
	int updateDirtyRectImpl(QRectF& rect) override;
	void drawImpl(QPainter* painter, MapWidget* widget) override;
	void updateStatusText() override;
	
	const Symbol* drawing_symbol;
	MapRenderables renderables;
	std::unique_ptr<TextObject, MapRenderables::ObjectDeleter> preview_text;
	std::unique_ptr<TextObjectEditorHelper> text_editor;
	bool waiting_for_mouse_release = false;
};


}  // namespace OpenOrienteering

#endif
