/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TEXT_SYMBOL_H
#define LIBREMAPPER_TEXT_SYMBOL_H

#include "symbol.h"

#include <vector>

#include <Qt>
#include <QFont>
#include <QFontMetricsF>
#include <QString>

class QXmlStreamReader;
class QXmlStreamWriter;
// IWYU pragma: no_forward_declare QFontMetricsF

namespace LibreMapper {

class Map;
class MapColor;
class MapColorMap;
class Object;
class ObjectRenderables;
class SymbolPropertiesWidget;
class SymbolSettingDialog;
class TextObject;
class VirtualCoordVector;



/** Symbol for text, can be applied to TextObjects.
 * 
 * NOTE: Uses a hack to disable hinting for fonts:
 * Uses a big internal font size, internal_point_size, where hinting is
 * neglegible and scales it down for output.
 * TODO: A maybe better solution would be to use FreeType directly.
 */
class TextSymbol : public Symbol
{
friend class TextSymbolSettings;
friend class PointSymbolEditorWidget;
public:
	/** Modes for text framing */
	enum FramingMode
	{
		/** Only the text itself is displayed. */
		NoFraming = 0,
		
		/**
		 * The text path is drawn with a QPainter with active QPen in addition
		 * to the normal fill, resulting in a line around the letters.
		 */
		LineFraming = 1,
		
		/**
		 * The text is drawn a second time with a slight offset to the main
		 * text, creating a shadow effect.
		 */
		ShadowFraming = 2
	};
	
	/** Creates an empty text symbol. */
	TextSymbol();
	~TextSymbol() override;
	
protected:
	explicit TextSymbol(const TextSymbol& proto);
	TextSymbol* duplicate() const override;
	
public:
	void createRenderables(
	        const Object *object,
	        const VirtualCoordVector &coords,
	        ObjectRenderables &output,
	        Symbol::RenderableOptions options) const override;
	
	using Symbol::createBaselineRenderables;
	void createBaselineRenderables(
	        const TextObject* text_object,
	        const VirtualCoordVector &coords,
	        ObjectRenderables &output) const;
	
	void createLineBelowRenderables(const Object* object, ObjectRenderables& output) const;
	void colorDeletedEvent(const MapColor* color) override;
	bool containsColor(const MapColor* color) const override;
	const MapColor* guessDominantColor() const override;
	void replaceColors(const MapColorMap& color_map) override;
	void scale(double factor) override;
	
	/** Updates the internal QFont from the font settings. */
	void updateQFont();
	
	/** Calculates the factor to convert from the real font size to the internal font size */
	inline double calculateInternalScaling() const {return internal_point_size / (0.001 * font_size);}
	
	// Getters and setters
	using Symbol::setRotatable;
	inline const MapColor* getColor() const {return color;}
	inline void setColor(const MapColor* color) {this->color = color;}
	inline const QString& getFontFamily() const {return font_family;}
	inline double getFontSize() const {return 0.001 * font_size;}
	inline bool isBold() const {return bold;}
	inline bool isItalic() const {return italic;}
	inline bool isUnderlined() const {return underline;}
	inline float getLineSpacing() const {return line_spacing;}
	inline double getParagraphSpacing() const {return 0.001 * paragraph_spacing;}
	inline double getCharacterSpacing() const {return 0.001 * character_spacing;}
	inline bool usesKerning() const {return kerning;}
	QString getIconText() const; // returns a default text if no custom text is set.
	inline bool usesFraming() const {return framing;}
	inline const MapColor* getFramingColor() const {return framing_color;}
	inline int getFramingMode() const {return framing_mode;}
	inline int getFramingLineHalfWidth() const {return framing_line_half_width;}
	inline int getFramingShadowXOffset() const {return framing_shadow_x_offset;}
	inline int getFramingShadowYOffset() const {return framing_shadow_y_offset;}
	inline bool hasLineBelow() const {return line_below;}
	inline const MapColor* getLineBelowColor() const {return line_below_color;}
	inline double getLineBelowWidth() const {return 0.001 * line_below_width;}
	inline double getLineBelowDistance() const {return 0.001 * line_below_distance;}
	inline int getNumCustomTabs() const {return (int)custom_tabs.size();}
	inline int getCustomTab(int index) const {return custom_tabs[index];}
	inline const QFont& getQFont() const {return qfont;}
	inline const QFontMetricsF& getFontMetrics() const { return metrics; }
	
	double getNextTab(double pos) const;
	
	constexpr static qreal internal_point_size = 256;
	
	SymbolPropertiesWidget* createPropertiesWidget(SymbolSettingDialog* dialog) override;
	
protected:
	void saveImpl(QXmlStreamWriter& xml, const Map& map) const override;
	bool loadImpl(QXmlStreamReader& xml, const Map& map, SymbolDictionary& symbol_dict, int version) override;
	bool equalsImpl(const Symbol* other, Qt::CaseSensitivity case_sensitivity) const override;
	
	
	// Members ordered for minimizing padding
	
	QFont qfont;
	QFontMetricsF metrics;
	QString font_family;
	QString icon_text;			// text to be drawn in the symbol's icon
	
	const MapColor* color;
	const MapColor* framing_color;
	const MapColor* line_below_color;
	
	std::vector<int> custom_tabs;
	
	double tab_interval;		/// default tab interval length in text coordinates
	float line_spacing;			// as factor of original line spacing
	float character_spacing;	// as a factor of the space character width
	int font_size;				// this defines the font size in 1000 mm. How big the letters really are depends on the design of the font though
	int paragraph_spacing;		// in mm
	int framing_mode;
	int framing_line_half_width;
	int framing_shadow_x_offset;
	int framing_shadow_y_offset;
	int line_below_width;
	int line_below_distance;
	bool bold;
	bool italic;
	bool underline;
	bool kerning;
	bool framing;
	bool line_below;
	
};


}  // namespace LibreMapper

#endif
