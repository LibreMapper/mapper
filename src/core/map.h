/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012-2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2013-2020, 2024 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_MAP_H
#define LIBREMAPPER_MAP_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <set>
#include <vector>

#include <QtGlobal>
#include <QExplicitlySharedDataPointer>
#include <QFlags>
#include <QHash>
#include <QMetaType>
#include <QObject>
#include <QPointer>
#include <QRectF>
#include <QScopedPointer>
#include <QSharedData>
#include <QString>
#include <QTransform>

#include "core/map_coord.h"
#include "core/map_grid.h"
#include "core/map_part.h"
// IWYU pragma: no_include "templates/template.h"

class QIODevice;
class QPainter;
class QTranslator;
class QWidget;
// IWYU pragma: no_forward_declare QRectF

namespace LibreMapper {

class CombinedSymbol;
class Georeferencing;
class LineSymbol;
class MapColor;
class MapColorMap;
class MapPrinterConfig;
class MapRenderables;
class MapView;
class MapWidget;
class Object;
class PointSymbol;
class RenderConfig;
class Symbol;
class Template;  // IWYU pragma: keep
class TextSymbol;
class UndoManager;
class UndoStep;


/**
 * The translator for color and symbol texts.
 * 
 * This translator is used by class Map but kept outside of the
 * class' namespace in order to allow for forward declaration
 * instead of including "map.h".
 */
extern QPointer<QTranslator> map_symbol_translator;



/** Central class for an OpenOrienteering map */
class Map : public QObject
{
Q_OBJECT
friend class MapTest;
friend class MapRenderables;
friend class XMLFileImporter;
friend class XMLFileExporter;
public:
	/** A set of selected objects represented by a std::set of object pointers. */
	typedef std::set<Object*> ObjectSelection;
	
	/**
	 * Different strategies for importing elements from another map.
	 */
	enum ImportModeFlag
	{
		ObjectImport   = 0x00,       ///< Import objects, symbols and colors.
		SymbolImport   = 0x01,       ///< Import symbols and colors.
		ColorImport    = 0x02,       ///< Import colors.
		GeorefImport   = 0x10,       ///< Use the georeferencing for object import.
		MinimalImport  = 0x20,       ///< Imports with minimal symbol and color dependencies.
		
		MinimalSymbolImport = SymbolImport | MinimalImport,
		MinimalObjectImport = ObjectImport | MinimalImport,
		CompleteImport      = ObjectImport | GeorefImport
	};
	
	Q_DECLARE_FLAGS(ImportMode, ImportModeFlag)
	
	
	/** Options for zooming to visibility of selection. */
	enum SelectionVisibility
	{
		FullVisibility,
		PartialVisibility,
		IgnoreVisibilty
	};
	
	/** Default parameters for loading of image templates. */
	struct ImageTemplateDefaults
	{
		bool use_meters_per_pixel = true;
		double meters_per_pixel   = 0.0;
		double dpi                = 0.0;
		double scale              = 0.0;
	};
	
	/** Creates a new, empty map. */
	Map();
	
	/** Destroys the map. */
	~Map() override;
	
	
	/**
	 * Deletes all map data.
	 * 
	 * The resulting map must not be modified before another init().
	 */
	void clear();
	
	/**
	 * Initializes an empty map.
	 * 
	 * A map is empty when it is newly constructed or after clear().
	 */
	void init();
	
	/**
	 * Deletes all map data, and reinitializes the empty map.
	 * 
	 * This method combines a call to clear() followed by init().
	 */
	void reset();
	
	
	/**
	 * Attempts to load the map from the specified path. Returns true on success.
	 * 
	 * This is a convenience function used by tests. Normally, an importer should be
	 * used explicitly.
	 * 
	 * @param path The file path to load the map from.
	 * @param view If not nullptr, restores this map view.
	 */
	bool loadFrom(const QString& path, MapView* view = nullptr);
	
	
	/**
	 * Imports another map into this map.
	 * 
	 * If the Map::GeorefImport mode flag is set, this overload will attempt to
	 * calculate a transformation based on the maps' georeferencing.
	 * All further processing is delegated to the other overload.
	 * 
	 */
	QHash<const Symbol*, Symbol*> importMap(
	        const Map& imported_map,
	        ImportMode mode,
	        std::vector<bool>* filter = nullptr,
	        int symbol_insert_pos = -1,
	        bool merge_duplicate_symbols = true
	);
	
	/**
	 * Imports another map into this map.
	 * 
	 * The amount of imported elements is controlled by the mode argument which
	 * is a combination of an enumeration of basic modes (ColorImport,
	 * SymbolImport, ObjectImport) and the flags (MinimalImport).
	 * - ObjectImport: Import objects, symbols and colors.
	 *   If the MinimalImport flag is set, symbols and colors not used
	 *   by the imported objects are ignored.
	 *   The filter argument is not used.
	 * - SymbolImport: Import symbols and colors.
	 *   If the MinimalImport flag is set, the filter argument may be used to
	 *   select a subset of the symbols, and colors not used by the imported
	 *   symbols are ignored.
	 * - ColorImport: Import colors.
	 *   If the MinimalImport flag is set, the filter argument may be used to
	 *   select a subset of the colors.
	 * 
	 * This overload ignores the Map::GeorefImport mode flag. It only uses the
	 * given transformation. It is applied to all imported objects.
	 * No other adjustment of object positions and no scaling of symbol sizes
	 * (with respect to possible different map scales) is performed.
	 */
	QHash<const Symbol*, Symbol*> importMap(
	        const Map& imported_map,
	        ImportMode mode,
	        const QTransform& transform,
	        std::vector<bool>* filter = nullptr,
	        int symbol_insert_pos = -1,
	        bool merge_duplicate_symbols = true
	);
	
	
	/**
	 * Serializes the map directly to the given IO device, in a fixed format.
	 * 
	 * This can be imported again using importFromIODevice().
	 * Returns true if successful.
	 */
	bool exportToIODevice(QIODevice& device) const;
	
	/**
	 * Loads the map directly from the given IO device.
	 * 
	 * The data must have been written by exportToIODevice() (or at least use
	 * the same format.)
	 * Returns true if successful.
	 */
	bool importFromIODevice(QIODevice& device);
	
	
	/**
	 * Draws the part of the map which is visible in the bounding box.
	 * 
	 * @param painter The QPainter used for drawing.
	 * @param config  The rendering configuration
	 */
	void draw(QPainter* painter, const RenderConfig& config);
	
	/**
	 * Draws a spot color overprinting simulation for the part of the map
	 * which is visible in the given bounding box.
	 * 
	 * @param painter Must be a QPainter on a QImage of Format_ARGB32_Premultiplied.
	 * @param config  The rendering configuration
	 */
	void drawOverprintingSimulation(QPainter* painter, const RenderConfig& config);
	
	/**
	 * Draws the separation for a particular spot color for the part of the
	 * map which is visible in the given bounding box.
	 * 
	 * Separations are normally drawn in levels of gray where black means
	 * full tone of the spot color. The parameter use_color can be used to
	 * draw in the actual spot color instead.
	 *
	 * @param painter The QPainter used for drawing.
	 * @param config  The rendering configuration
	 * @param spot_color The spot color to draw the separation for.
	 * @param use_color  If true, forces the separation to be drawn in its actual color.
	 */
	void drawColorSeparation(QPainter* painter, const RenderConfig& config,
		const MapColor* spot_color, bool use_color = false);
	
	/**
	 * Draws the map grid.
	 * 
	 * @param painter The QPainter used for drawing.
	 * @param bounding_box Bounding box of area to draw, given in map coordinates.
	 */
	void drawGrid(QPainter* painter, const QRectF& bounding_box);
	
	/**
	 * Draws the templates with indices first_template until last_template which
	 * are visible in the given bounding box.
	 * 
	 * view determines template visibility and can be nullptr to show all templates.
	 * The initial transform of the given QPainter must be the map-to-paintdevice transformation.
     * If on_screen is set to true, some optimizations will be applied, leading to a possibly lower display quality.
	 * 
	 * @param painter The QPainter used for drawing.
	 * @param bounding_box Bounding box of area to draw, given in map coordinates.
	 * @param first_template Lowest index of the template range to draw.
	 * @param last_template Highest index of the template range to draw.
	 * @param view Optional pointer to MapView object which is used to query
	 *     template visibilities.
	 * @param on_screen Potentially enables some drawing optimizations which
	 *     decrease drawing quality. Should be enabled when drawing on-screen.
	 */
	void drawTemplates(QPainter* painter, const QRectF& bounding_box, int first_template,
					   int last_template, const MapView* view, bool on_screen) const;
	
	
	/**
	 * Updates the renderables and extent of all objects which have changed.
	 * This is automatically called by draw(), you normally do not need to call it directly.
	 */
	void updateObjects();
	
	/** 
	 * Calculates the extent of all map elements. 
	 * 
	 * If templates shall be included, view may either be nullptr to include all 
	 * templates, or specify a MapView to take the template visibilities from.
	 */
	QRectF calculateExtent(bool include_helper_symbols = false, bool include_templates = false, const MapView* view = nullptr) const;
	
	
	/**
	 * Must be called to notify the map of new widgets displaying it.
	 * Useful to notify the widgets about which parts of the map have changed
	 * and need to be redrawn.
	 */
	void addMapWidget(MapWidget* widget);
	
	/**
	 * Removes the map widget, see addMapWidget().
	 */
	void removeMapWidget(MapWidget* widget);
	
	/**
	 * Redraws all map widgets completely - this can be slow!
	 * Try to avoid this and do partial redraws instead, if possible.
	 */
	void updateAllMapWidgets();
	
	/**
	 * Makes sure that the selected object(s) are visible in all map widgets
	 * by moving the views in the widgets to the selected objects.
	 */
	void ensureVisibilityOfSelectedObjects(SelectionVisibility visibility);
	
	
	// Current drawing
	
	/**
	 * Sets the rect (given in map coordinates) as "dirty rect" for every
	 * map widget showing this map, enlarged by the given pixel border.
	 * This means that the area covered by the rect will be redrawn by
	 * the active tool. Use this if the current tool's display has changed.
	 * 
	 * @param map_coords_rect Area covered by the current tool's drawing in map coords.
	 * @param pixel_border Border around the map coords rect which is also covered,
	 *     given in pixels. Allows to enlarge the area given by the map coords
	 *     by some pixels which are independent from the zoom level.
	 *     For example if a tool displays markers with a radius of 3 pixels,
	 *     it would set the bounding box of all markers as map_coords_rect and
	 *     3 for pixel_border.
	 * @param do_update Whether a repaint of the covered area should be triggered.
	 */
	void setDrawingBoundingBox(const QRectF& map_coords_rect, int pixel_border, bool do_update = true);
	
	/**
	 * Removes the drawing bounding box and triggers a repaint. Use this if
	 * the current drawing is hidden or erased.
	 */
	void clearDrawingBoundingBox();
	
	
	/**
	 * This is the analogon to setDrawingBoundingBox() for activities.
	 * See setDrawingBoundingBox().
	 */
	void setActivityBoundingBox(const QRectF& map_coords_rect, int pixel_border, bool do_update = true);
	
	/**
	 * This is the analogon to clearDrawingBoundingBox() for activities.
	 * See clearDrawingBoundingBox().
	 */
	void clearActivityBoundingBox();
	
	
	/**
	 * Updates all dynamic drawings at the given positions,
	 * i.e. tool & activity drawings.
	 * 
	 * See setDrawingBoundingBox() and setActivityBoundingBox().
	 */
	void updateDrawing(const QRectF& map_coords_rect, int pixel_border);
	
	
	
	// Element translations
	
	/**
	 * Returns a translated symbol text (name or description), or the original text.
	 */
	QString translate(const QString& symbol_text) const;
	
	/**
	 * Returns a translated symbol text (name or description), or an empty string.
	 */
	QString raw_translation(const QString& symbol_text) const;
	
	
	
	// Colors
	
	/** Returns the number of map color priorities defined in this map. The
	 *  count does not include special colors returned by getColorByPrio().
	 *  The priorities are guaranteed to be a continuous series starting at
	 *  zero. */
	int getNumColorPrios() const;
	
	/** Returns a pointer to the MapColor identified by the non-negative priority i.
	 * 
	 *  Returns nullptr if the color is not defined, or if it is a special color (i.e i<0),
	 *  i.e. only actual map colors are returned.
	 */
	const MapColor* getMapColorByPrio(int i) const;
	
	/** Returns a pointer to the MapColor identified by the non-negative priority i.
	 * 
	 *  Returns nullptr if the color is not defined, or if it is a special color (i.e i<0),
	 *  i.e. only actual map colors are returned.
	 */
	MapColor* getMapColorByPrio(int i);
	
	/** Returns a pointer to the const MapColor identified by the priority i.
	 * 
	 *  Parameter i may also be negative for specifying special reserved colors.
	 *  This is different from getMapColor();
	 * 
	 *  Returns nullptr if the color is not defined.
	 */
	const MapColor* getColorByPrio(int i) const;

	/** Returns a pointer to the const MapColor with id id.
	 *  
	 *  Mind that the id's are not guaranteed to be contigous and cannot be
	 *  used for iteration over colors.
	 *  
	 *  \return nullptr in case the id is not present in the set.
	 */
	const MapColor* getMapColorById(int id) const;
	
	/**
	 * Replaces the color with priority prio with the given color, and updates
	 * dependent colors and symbol icons.
	 * 
	 * Emits colorChanged(). Does not delete the replaced color.
	 */
	void setColor(MapColor* color, int prio);
	
	/**
	 * Adds the given color as a new color at the given index.
	 * Emits colorAdded().
	 */
	void addColor(MapColor* color, int prio);
	
	/**
	 * Deletes the color with the given priority.
	 * Emits colorDeleted().
	 */
	void deleteColor(int prio);
	
	/**
	 * Loops through the color list, looking for the given color pointer.
	 * Returns the index of the color, or -1 if it is not found.
	 */
	int findColorPrio(const MapColor* color) const;
	
	/**
	 * Marks the colors as "dirty", i.e. as having unsaved changes.
	 * Emits hasUnsavedChanged(true) if the map did not have unsaved changes before.
	 */
	void setColorsDirty();
	
	/**
	 * Makes this map use the color set from the given map.
	 * Used to create the maps containing preview objects in the symbol editor.
	 */
	void useColorsFrom(Map* map);
	
	/**
	 * Checks and returns if the given color is used by at least one symbol.
	 */
	bool isColorUsedByASymbol(const MapColor* color) const;
	
	/**
	 * Checks which colors are in use by the symbols in this map.
	 * 
	 * WARNING (FIXME): returns an empty list if the map does not contain symbols!
	 * 
	 * @param by_which_symbols Must be of the same size as the symbol set.
	 *     If set to false for a symbol, it will be disregarded.
	 * @param out Output parameter: a vector of the same size as the color list,
	 *     where each element is set to true if the color is used by at least one symbol.
	 */
	void determineColorsInUse(const std::vector< bool >& by_which_symbols, std::vector< bool >& out) const;
	
	/** Returns true if the map contains spot colors. */
	bool hasSpotColors() const;
	
	/**
	 * Returns true if any visible object uses a non-opaque color.
	 */
	bool hasAlpha() const;
	
	/**
	 * Applies a const operation on all colors which match a particular condition.
	 */
	void applyOnMatchingColors(const std::function<void (const MapColor*)>& operation, const std::function<bool (const MapColor*)>& condition) const;
	
	/**
	 * Applies a const operation on all colors.
	 */
	void applyOnAllColors(const std::function<void (const MapColor*)>& operation) const;
	
	
	// Symbols
	
	/** Returns the symbol set ID. */
	QString symbolSetId() const;
	
	/** Sets the symbol set ID. */
	void setSymbolSetId(const QString& id);
	
	
	/** Returns the number of symbols in this map. */
	int getNumSymbols() const;
	
	/** Returns a pointer to the i-th symbol.
	 *  
	 * \sa findSymbolIndex()
	 */
	const Symbol* getSymbol(int i) const;
	
	/** Returns a pointer to the i-th symbol. */
	Symbol* getSymbol(int i);
	
	/**
	 * Replaces the symbol at the given index with another symbol.
	 * Emits symbolChanged() and possibly selectedObjectEdited().
	 */
	void setSymbol(Symbol* symbol, int pos);
	
	/**
	 * Adds the given symbol at the specified index.
	 * Emits symbolAdded().
	 */
	void addSymbol(Symbol* symbol, int pos);
	
	/**
	 * Moves a symbol from one index to another in the symbol list.
	 */
	void moveSymbol(int from, int to);
	
	/**
	 * Sorts the symbol list using the given comparator.
	 */
	template<typename T> void sortSymbols(T compare);
	
	/**
	 * Deletes the symbol at the given index.
	 * Emits symbolDeleted().
	 */
	void deleteSymbol(int pos);
	
	/**
	 * Loops over all symbols, looking for the given symbol pointer.
	 * Returns the index of the symbol, or -1 if the symbol is not found.
	 * For the "undefined" symbols, returns special indices smaller than -1.
	 * 
	 * \sa getSymbol()
	 */
	int findSymbolIndex(const Symbol* symbol) const;
	
	/**
	 * Marks the symbols as "dirty", i.e. as having unsaved changes.
	 * Emits hasUnsavedChanged(true) if the map did not have unsaved changes before.
	 */
	void setSymbolsDirty();
	
	/**
	 * Updates the icons of all symbols with the given color.
	 */
	void updateSymbolIcons(const MapColor* color);
	
	/**
	 * Scales all symbols by the given factor.
	 */
	void scaleAllSymbols(double factor);
	
	/**
	 * Checks which symbols are in use in this map.
	 * 
	 * Returns a vector of the same size as the symbol list, where each element
	 * is set to true if there is at least one object which uses this symbol or
	 * a derived (combined) symbol.
	 */
	void determineSymbolsInUse(std::vector<bool>& out) const;
	
	/**
	 * Adds to the given symbol bitfield all other symbols which are needed to
	 * display the symbols indicated by the bitfield because of symbol dependencies.
	 */
	void determineSymbolUseClosure(std::vector< bool >& symbol_bitfield) const;
	
	/**
	 * Applies a const operation on all symbols which match a particular condition.
	 */
	void applyOnMatchingSymbols(const std::function<void (const Symbol*)>& operation, const std::function<bool (const Symbol*)>& condition) const;
	
	/**
	 * Applies a const operation on all symbols.
	 */
	void applyOnAllSymbols(const std::function<void (const Symbol*)>& operation) const;
	
	/**
	 * Returns the scale factor to be used for default symbol icons.
	 * 
	 * The full icon size (width, height) is represented by 1.0.
	 */
	qreal symbolIconZoom() const;
	
public slots:
	/**
	 * Updates the symbol icon zoom from the current set of symbols.
	 * 
	 * The symbol icon zoom is chosen so that most symbols fit into the full
	 * icon space, and the number of symbols below 10% size is kept low.
	 * For a map without symbols, this returns 1.0.
	 */
	void updateSymbolIconZoom();
	
	
public:
	// Templates
	
	/**
	 * Returns the number of templates in this map.
	 */
	int getNumTemplates() const	{ return int(templates.size()); }
	
	/**
	 * Returns the i-th template.
	 */
	const Template* getTemplate(int pos) const { return templates[std::size_t(pos)].get(); }
	
	/**
	 * Returns the i-th template.
	 */
	Template* getTemplate(int pos) { return templates[std::size_t(pos)].get(); }
	
	/**
	 * Sets the template index which is the first (lowest) to be drawn in front of the map.
	 */
	void setFirstFrontTemplate(int pos);
	
	/**
	 * Returns the template index which is the first (lowest) to be drawn in front of the map.
	 */
	int getFirstFrontTemplate() const { return first_front_template; }
	
	/**
	 * Replaces the template at the given index with another.
	 * 
	 * Emits templateChanged().
	 */
	std::unique_ptr<Template> setTemplate(int pos, std::unique_ptr<Template> temp);
	
	/**
	 * Insert a new template at the given index.
	 * 
	 * If the index is lower then the first front template index, the function
	 * increments this index. To place a template immediately below the map,
	 * pass index -1.
	 */
	void addTemplate(int pos, std::unique_ptr<Template> temp);
	
	/**
	 * Moves a template to a new position.
	 */
	void moveTemplate(int old_pos, int new_pos);
	
	/**
	 * Removes the template with the given index from the template list,
	 * and returns it.
	 * 
	 * If the index is lower then the first front template index, the function
	 * decrements this index.
	 */
	std::unique_ptr<Template> removeTemplate(int pos);
	
	/**
	 * Removes the template with the given index from the template list,
	 * and deletes it.
	 * 
	 * If the index is lower then the first front template index, the function
	 * decrements this index.
	 */
	void deleteTemplate(int pos);
	
	/**
	 * Marks the area defined by the given QRectF (in map coordinates) and
	 * pixel border as "dirty", i.e. as needing a repaint, for the given template
	 * in all map widgets.
	 * 
	 * For an explanation of the area and pixel border, see setDrawingBoundingBox().
	 * 
	 * Warning: does nothing if the template is not visible in a widget!
	 * So make sure to call this and showing/hiding a template in the correct order!
	 */
	void setTemplateAreaDirty(Template* temp, const QRectF& area, int pixel_border);
	
	/**
	 * Marks the whole area of the i-th template as "to be repainted".
	 * 
	 * Does nothing for pos == -1.
	 * 
	 * @see setTemplateAreaDirty()
	 */
	void setTemplateAreaDirty(int pos);
	
	/**
	 * Finds the index of the given template, or returns -1.
	 */
	int findTemplateIndex(const Template* temp) const;
	
	/**
	 * Marks the template settings as "dirty", i.e. as having unsaved changes.
	 * 
	 * Emits hasUnsavedChanged(true) if the map did not have unsaved changes before.
	 */
	void setTemplatesDirty();
	
	/**
	 * Emits templateChanged() for the given template.
	 */
	void emitTemplateChanged(Template* temp);
	
	
	/**
	 * Returns the number of closed templates for which the settings are still stored.
	 */
	int getNumClosedTemplates() const { return int(closed_templates.size()); }
	
	/**
	 * Returns the i-th closed template.
	 */
	const Template* getClosedTemplate(int i) const { return closed_templates[std::size_t(i)].get(); }
	
	/**
	 * Returns the i-th closed template.
	 */
	Template* getClosedTemplate(int i) { return closed_templates[std::size_t(i)].get(); }
	
	/**
	 * Empties the list of closed templates.
	 */
	void clearClosedTemplates();
	
	/**
	 * Removes the template with the given index from the normal template list,
	 * unloads the template file and adds the template to the closed template list.
	 * 
	 * NOTE: if required, adjust first_front_template manually with setFirstFrontTemplate()!
	 */
	void closeTemplate(int pos);
	
	/**
	 * Removes the template with the given index from the closed template list,
	 * adds it to the active list and loads the template file if possible.
	 * 
	 * @param i The index of the closed template to reload.
	 * @param target_pos The desired index in the normal template list after loading.
	 *                   To place a template immediately below the map, pass index -1.
	 * @param dialog_parent Widget as parent for possible dialogs.
	 * @param map_path Path where the map is saved currently. Used as possible
	 *     search location to locate missing templates.
	 * 
	 * @return true if loading was successful.
	 */
	bool reloadClosedTemplate(int i, int target_pos, QWidget* dialog_parent, const QString& map_path = {});
	
	/**
	 * Loads all "unloaded" templates which are marked visible in the given view.
	 */
	void loadTemplateFiles(const MapView& view);
	
	/**
	 * Requests all visible "unloaded" templates to be loaded asynchronously.
	 * 
	 * If the view is destroyed before template loading is finished, template
	 * loading will be stopped. (The map must not be destroyed before the view.)
	 */
	void loadTemplateFilesAsync(MapView& view, std::function<void(const QString&)> listener);
	
	
	// Undo & Redo
	
	/**
	 * Returns the UndoManager instance for this map.
	 */
	UndoManager& undoManager();
	
	/**
	 * Returns the UndoManager instance for this map.
	 */
	const UndoManager& undoManager() const;
	
	/**
	 * Pushes a new undo step to the map's undoManager.
	 */
	void push(UndoStep* step);
	
	
	// Map parts
	
	/**
	 * Returns the number of map parts in this map.
	 */
	int getNumParts() const;
	
	/**
	 * Returns the i-th map part.
	 */
	MapPart* getPart(std::size_t i) const;
	
	/** 
	 * Adds the new part at the given index.
	 */
	void addPart(MapPart* part, std::size_t index);

	/**
	 * Removes the map part at position.
	 */
	void removePart(std::size_t index);
	
	/**
	 * Loops over all map parts, looking for the given part pointer.
	 * Returns the part's index in the list. The part must be contained in the
	 * map, otherwise an assert will be triggered!
	 */
	int findPartIndex(const MapPart* part) const;
	
	/**
	 * Returns the current map part, i.e. the part where edit operations happen.
	 */
	MapPart* getCurrentPart() const;
	
	/**
	 * Changes the current map part.
	 * 
	 * This is a convenience method which looks up the part's index and then
	 * calls setCurrentPartIndex.
	 */
	void setCurrentPart(MapPart* part);
	
	/**
	 * Returns the index of the current map part.
	 * 
	 * @see getCurrentPart().
	 */
	std::size_t getCurrentPartIndex() const;
	
	/**
	 * Changes the current map part.
	 */
	void setCurrentPartIndex(std::size_t index);
	
	/**
	 * Moves all specified objects from the source to the target map part.
	 * 
	 * Objects are processed one by one. This means that processing one object
	 * changes the index of following objects. Thus the given indices must
	 * normally be in descending order.
	 * 
	 * The objects will be continuously located at the end to the objects in the target part.
	 * Source object which were selected will be removed from the object selection.
	 * 
	 * @return The index of the first object which has been reassigned.
	 */
	int reassignObjectsToMapPart(std::vector<int>::const_iterator first, std::vector<int>::const_iterator last, std::size_t source, std::size_t destination);
	
	/**
	 * Merges the source part with the destination part.
	 * 
	 * Removes the source part unless it is identical with the destination part.
	 * 
	 * The objects will be continuously located at the end to the objects in the target part.
	 * Does not change the object selection.
	 * 
	 * Makes the destination part the current part when the source part is the current part.
	 * 
	 * @return The index of the first object which has been reassigned.
	 */
	int mergeParts(std::size_t source, std::size_t destination);
	
	
	// Objects
	
	/** Returns the total number of objects in this map (sum of all parts) */
	int getNumObjects() const;
	
	/**
	 * Adds the object as new object in the part with the given index,
	 * or in the current part if the default -1 is passed.
	 * Returns the index of the added object in the part.
	 */
	int addObject(Object* object, int part_index = -1);
	
	/**
	 * Deletes the given object from the map.
	 * 
	 * Be sure to call removeObjectFromSelection() if necessary.
	 */
	void deleteObject(Object* object);

	/**
	 * Relinquish object ownership.
	 *
	 * Searches map parts and returns pointer if the object was found.
	 * Otherwise nullptr is returned.
	 */
	Object* releaseObject(Object* object);
	
	/**
	 * Marks the objects as "dirty", i.e. as having unsaved changes.
	 * Emits hasUnsavedChanged(true) if the map did not have unsaved changes before.
	 */
	void setObjectsDirty();
	
	/**
	 * Marks the area given by map_coords_rect as "dirty" in all map widgets,
	 * i.e. as needing to be redrawn because some object(s) changed there.
	 */
	void setObjectAreaDirty(const QRectF& map_coords_rect);
	
	/**
	 * Finds and returns all objects at the given position in the current part.
	 * 
	 * @param coord Coordinate where to query objects.
	 * @param tolerance Allowed distance from the query coordinate to the objects.
	 * @param treat_areas_as_paths If set to true, areas will be treated as paths,
	 *     i.e. they will be returned only if the query coordinate is close to
	 *     their border, not in all cases where the query coordinate is inside the area.
	 * @param extended_selection If set to true, more object than defined by the
	 *     default behavior will be selected. For example, by default point objects
	 *     will only be returned if the query coord is close to the point object
	 *     coord. With extended_selection, they are also returned if the query
	 *     coord is just inside the graphical extent of the point object.
	 *     This can be used for a two-pass algorithm to find the most relevant
	 *     objects first, and then query for all objects if no objects are found
	 *     in the first pass.
	 * @param include_hidden_objects Set to true if you want to find hidden objects.
	 * @param include_protected_objects Set to true if you want to find protected objects.
	 * @param out Output parameter. Will be filled with pairs of symbol types
	 *     and corresponding objects. The symbol type describes the way in which
	 *     an object has been found and is taken from Symbol::Type. This is e.g.
	 *     important for combined symbols, which can be found from a line or
	 *     an area.
	 */
	void findObjectsAt(const MapCoordF& coord, qreal tolerance, bool treat_areas_as_paths,
		bool extended_selection, bool include_hidden_objects,
		bool include_protected_objects, SelectionInfoVector& out) const;
	
	/**
	 * Finds and returns all objects at the given position in all parts.
	 * 
	 * @see Map::findObjectsAt
	 */
	void findAllObjectsAt(const MapCoordF& coord, qreal tolerance, bool treat_areas_as_paths,
		bool extended_selection, bool include_hidden_objects,
		bool include_protected_objects, SelectionInfoVector& out) const;
	
	/**
	 * Finds and returns all objects intersecting the given box in the current part.
	 * 
	 * @param corner1 First corner of the query box.
	 * @param corner2 Second corner of the query box.
	 * @param include_hidden_objects Set to true if you want to find hidden objects.
	 * @param include_protected_objects Set to true if you want to find protected objects.
	 * @param out Output parameter. Will be filled with an object list.
	 */
	void findObjectsAtBox(const MapCoordF& corner1, const MapCoordF& corner2,
		bool include_hidden_objects, bool include_protected_objects,
		std::vector<Object*>& out) const;
	
	/**
	 * Counts the objects whose bounding boxes intersect the given rect.
	 * 
	 * This may be inaccurate because the true object shapes usually differ
	 * from the bounding boxes.
	 * 
	 * @param map_coord_rect The query rect.
	 * @param include_hidden_objects Set to true if you want to find hidden objects.
	 */
	int countObjectsInRect(const QRectF& map_coord_rect, bool include_hidden_objects);
	
	
	/**
	 * Applies a condition on all objects until the first match is found.
	 * 
	 * @return True if there is an object matching the condition, false otherwise.
	 */
	bool existsObject(const std::function<bool (const Object*)>& condition) const;
	
	/**
	 * Applies an operation on all objects which match a particular condition.
	 */
	void applyOnMatchingObjects(const std::function<void (Object*)>& operation, const std::function<bool (const Object*)>& condition);
	
	/**
	 * Applies a function on all objects which match a particular condition.
	 */
	void applyOnMatchingObjects(const std::function<void (const Object*)>& operation, const std::function<bool (const Object*)>& condition) const;
	
	/**
	 * Applies an operation on all objects which match a particular condition.
	 */
	void applyOnMatchingObjects(const std::function<void (Object*, MapPart*, int)>& operation, const std::function<bool (const Object*)>& condition);
	
	/**
	 * Applies an operation on all objects.
	 */
	void applyOnAllObjects(const std::function<void (Object*)>& operation);
	
	/**
	 * Applies an operation on all objects.
	 */
	void applyOnAllObjects(const std::function<void (const Object*)>& operation) const;
	
	/**
	 * Applies an operation on all objects.
	 */
	void applyOnAllObjects(const std::function<void (Object*, MapPart*, int)>& operation);
	
	
	/** Scales all objects by the given factor. */
	void scaleAllObjects(double factor, const MapCoord& scaling_center);
	
	/** Rotates all objects by the given rotation angle (in radians). */
	void rotateAllObjects(double rotation, const MapCoord& center);
	
	/** Forces an update of all objects, i.e. calls update(true) on each map object. */
	void updateAllObjects();
	
	/** Forces an update of all objects with the given symbol. */
	void updateAllObjectsWithSymbol(const Symbol* symbol);
	
	/** For all symbols with old_symbol, replaces the symbol by new_symbol. */
	void changeSymbolForAllObjects(const Symbol* old_symbol, const Symbol* new_symbol);
	
	/**
	 * Deletes all objects with the given symbol.
	 * 
	 * @return True if at least one object was deleted, false otherwise
	 */
	bool deleteAllObjectsWithSymbol(const Symbol* symbol);
	
	/**
	 * Returns if at least one object with the given symbol exists in the map.
	 * WARNING: Even if no objects exist directly, the symbol could still be
	 *          required by another (combined) symbol used by an object!
	 */
	bool existsObjectWithSymbol(const Symbol* symbol) const;
	
	
	/**
	 * Removes the renderables of the given object from display (does not
	 * delete them!).
	 */
	void removeRenderablesOfObject(const Object* object, bool mark_area_as_dirty);
	
	/**
	 * Inserts the renderables of the given object, so they will be displayed.
	 */
	void insertRenderablesOfObject(const Object* object);
	
	
	/**
	 * Marks an object as irregular.
	 */
	void markAsIrregular(Object* object);
	
	/**
	 * Returns the list of objects marked as irregular.
	 */
	const std::set<Object*> irregularObjects() const;
	
	/**
	 * Deletes the irregular objects.
	 * 
	 * This function deletes the objects which were previously marked as irregular.
	 * Only objects which are actually member of map parts are deleted. Objects in
	 * undo steps or similar are ignored.
	 * 
	 * \return The number of deleted objects.
	 */
	std::size_t deleteIrregularObjects();
	
	
	// Object selection
	
	const ObjectSelection& selectedObjects() const;
	
	/** Returns the number of selected objects. */
	int getNumSelectedObjects() const;
	
	/** Returns an iterator allowing to iterate over the selected objects. */
	ObjectSelection::const_iterator selectedObjectsBegin() const;
	
	/** Returns an end iterator allowing to iterate over the selected objects. */
	ObjectSelection::const_iterator selectedObjectsEnd() const;
	
	/**
	 * Returns the object in the selection which was selected first by the user.
	 * 
	 * If she later deselects it while other objects are still selected or if
	 * the selection is done as box selection, this "first" selected object is
	 * just a more or less random object from the selection.
	 */
	const Object* getFirstSelectedObject() const;
	
	/**
	 * Returns the object in the selection which was selected first by the user.
	 */
	Object* getFirstSelectedObject();
	
 	/**
	 * Checks the selected objects for compatibility with the given symbol.
	 * @param symbol the symbol to check compatibility for
	 * @param out_compatible returns if all selected objects are compatible
	 *     to the given symbol
	 * @param out_different returns if at least one of the selected objects'
	 *     symbols is different to the given symbol
	 */
	void getSelectionToSymbolCompatibility(const Symbol* symbol, bool& out_compatible, bool& out_different) const;
	
	/**
	 * Deletes the selected objects and creates an undo step for this action.
	 */
	void deleteSelectedObjects();
	
	/**
	 * Enlarges the given rect to cover all selected objects.
	 */
	void includeSelectionRect(QRectF& rect) const;
	
	/**
	 * Draws the selected objects.
	 * 
	 * @param painter The QPainter used for drawing.
	 * @param force_min_size See draw().
	 * @param widget The widget in which the drawing happens.
	 *     Used to get view and viewport information.
	 * @param replacement_renderables If given, draws these renderables instead
	 *     Of the selection renderables. TODO: HACK
	 * @param draw_normal If set to true, draws the objects like normal objects,
	 *     otherwise draws transparent highlights.
	 */
	void drawSelection(QPainter* painter, bool force_min_size, MapWidget* widget,
		MapRenderables* replacement_renderables = nullptr, bool draw_normal = false);
	
	/**
	 * Adds the given object to the selection.
	 * @param object The object to add.
	 * @param emit_selection_changed Set to true if objectSelectionChanged()
	 *     should be emitted. Do this only for the last in a
	 *     sequence of selection change operations to prevent bad performance!
	 */
	void addObjectToSelection(Object* object, bool emit_selection_changed);
	
	/**
	 * Removes the given object from the selection.
	 * @param object The object to remove.
	 * @param emit_selection_changed See addObjectToSelection().
	 */
	void removeObjectFromSelection(Object* object, bool emit_selection_changed);
	
	/**
	 * Removes from the selection all objects with the given symbol.
	 * Returns true if at least one object has been removed.
	 * @param symbol The symbol of the objects to remove.
	 * @param emit_selection_changed See addObjectToSelection().
	 */
	bool removeSymbolFromSelection(const Symbol* symbol, bool emit_selection_changed);
	
	/** Returns true if the given object is selected. */
	bool isObjectSelected(const Object* object) const;
	
	/**
	 * Toggles the selection of the given object.
	 * Returns true if the object was selected, false if deselected.
	 * @param object The object to select or deselect.
	 * @param emit_selection_changed See addObjectToSelection().
	 */
	bool toggleObjectSelection(Object* object, bool emit_selection_changed);
	
	/**
	 * Empties the object selection.
	 * @param emit_selection_changed See addObjectToSelection().
	 */
	void clearObjectSelection(bool emit_selection_changed);
	
	/**
	 * Emits objectSelectionChanged(). Use this if setting emit_selection_changed
	 * in a selection change method is unfeasible.
	 */
	void emitSelectionChanged();
	
	/**
	 * Emits selectedObjectEdited(). Use this after making changes
	 * to a selected object.
	 */
	void emitSelectionEdited();
	
	
	// Other settings
	
	/** Sets the map's scale denominator. */
	void setScaleDenominator(unsigned int value);
	
	/** Returns the map's scale denominator. */
	unsigned int getScaleDenominator() const;
	
	/**
	 * Changes the map's scale.
	 * 
	 * @param new_scale_denominator The new scale denominator.
	 * @param additional_stretch Additional stretch/shrink applied to the map.
	 * @param scaling_center The coordinate to use as scaling center.
	 * @param scale_symbols Whether to scale the map symbols.
	 * @param scale_objects Whether to scale the map object coordinates.
	 * @param scale_georeferencing Whether to adjust the map's georeferencing reference point.
	 * @param scale_templates Whether to scale non-georeferenced templates.
	 */
	void changeScale(unsigned int new_scale_denominator, double additional_stretch,
		const MapCoord& scaling_center, bool scale_symbols, bool scale_objects,
		bool scale_georeferencing, bool scale_templates);
	
	
	/**
	 * Rotate the map around a point.
	 * 
	 * @param rotation The rotation angle (in radians).
	 * @param center The rotation center point.
	 * @param adjust_georeferencing Whether to adjust the georeferencing reference point.
	 * @param adjust_declination Whether to adjust the georeferencing declination.
	 * @param adjust_templates Whether to adjust non-georeferenced templates.
	 */
	void rotateMap(double rotation, const MapCoord& center,
		bool adjust_georeferencing, bool adjust_declination,
		bool adjust_templates);
	
	
	/** Returns the map notes string. */
	const QString& getMapNotes() const;
	
	/**
	 * Sets the map notes string.
	 * NOTE: Set the map to dirty manually!
	 */
	void setMapNotes(const QString& text);
	
	
	/** Returns the map's georeferencing object. */
	const Georeferencing& getGeoreferencing() const;
	
	/**
	 * Assigns georeferencing settings for the map from the given object and
	 * sets the map to have unsaved changes.
	 */
	void setGeoreferencing(const Georeferencing& georeferencing);
	
	
	/**
	 * Returns the map's grid settings.
	 */
	const MapGrid& getGrid() const;
	
	/**
	 * Sets the map's grid settings from the given object and
	 * may set the map to have unsaved changes.
	 */
	void setGrid(const MapGrid& grid);
	
	
	/**
	 * TODO: These two options should really be view options, but are not due
	 * to a limitation:
	 * the current architecture makes it impossible to have different
	 * renderables of the same objects in different views!
	 */
	
	/** Returns if area hatching is enabled. */
	bool isAreaHatchingEnabled() const;
	
	/** Sets if area hatching is enabled. */
	void setAreaHatchingEnabled(bool enabled);
	
	
	/** Returns if the baseline view is enabled. */
	bool isBaselineViewEnabled() const;
	
	/** Sets if the baseline view is enabled. */
	void setBaselineViewEnabled(bool enabled);
	
	
	/** Returns the rendering options as an int representing Symbol::RenderableOptions. */
	int renderableOptions() const;
	
	
	/** Returns true if the map has a print configuration. */
	bool hasPrinterConfig() const noexcept;
	
	/** Returns a const reference to the current print configuration.
	 * 
	 * If the map does not have a print configuration, a default configuration
	 * is created first.
	 */
	const MapPrinterConfig& printerConfig();
	
	/** Returns a copy of the current print configuration.
	 *
	 * If the map does not have a print configuration, the function will return
	 * a default configuration for this map.
	 */
	MapPrinterConfig printerConfig() const;
	
	/** Sets the print configuration. */
	void setPrinterConfig(const MapPrinterConfig& config);
	
	/** Clears the print configuration.
	 *
	 * After calling this method, hasPrinterConfig() returns false.
	 */
	void resetPrinterConfig();
	
	
	/** Returns the default parameters for loading of image templates. */
	const ImageTemplateDefaults& getImageTemplateDefaults() const { return image_template_defaults; };
	
	/** Sets default parameters for loading of image templates. */
	void setImageTemplateDefaults(const ImageTemplateDefaults& defaults) { image_template_defaults = defaults; };
	
	
	/**
	 * Returns whether there are unsaved changes in the map.
	 * 
	 * To toggle this state, never use setHasUnsavedChanges() directly unless
	 * you know what you are doing, instead use setOtherDirty() or set one of
	 * the more specific 'dirty' flags. This is because a call to
	 * setHasUnsavedChanges() alone followed by a map change and an undo would
	 * result in no changed flag.
	 */
	bool hasUnsavedChanges() const;
	
	/** Do not use this in usual cases, see hasUnsavedChanges(). */
	void setHasUnsavedChanges(bool has_unsaved_changes);
	
	/** Returns if there are unsaved changes to the colors. */
	bool areColorsDirty() const;
	
	/** Returns if there are unsaved changes to the symbols. */
	bool areSymbolsDirty() const;
	
	/** Returns if there are unsaved changes to the templates. */
	bool areTemplatesDirty() const;
	
	/** Returns if there are unsaved changes to the objects. */
	bool areObjectsDirty() const;
	
	/** Returns if there are unsaved changes to anything else than the above. */
	bool isOtherDirty() const;
	
	/**
	 * Marks something unspecific in the map as "dirty", i.e. as having unsaved changes.
	 * Emits hasUnsavedChanged(true) if the map did not have unsaved changes before.
	 * 
	 * Use setColorsDirty(), setSymbolsDirty(), setTemplatesDirty() or
	 * setObjectsDirty() if you know more specifically what has changed.
	 */
	void setOtherDirty();
	
	
	// Static
	
	/** Returns the special covering red color. */
	static const MapColor* getCoveringRed();
	
	/** Returns the special covering white color. */
	static const MapColor* getCoveringWhite();
	
	/** Returns the special covering gray color for "undefined" objects. */
	static const MapColor* getUndefinedColor();
	
	/** Returns the special registration color. */
	static const MapColor* getRegistrationColor();
	
	/** Returns the special covering white line symbol. */
	static LineSymbol* getCoveringWhiteLine();
	
	/** Returns the special covering red line symbol. */
	static LineSymbol* getCoveringRedLine();
	
	/** Returns the special covering combined symbol (white + red). */
	static CombinedSymbol* getCoveringCombinedLine();
	
	/** Returns the special gray "undefined" line symbol. */
	static LineSymbol* getUndefinedLine();
	
	/** Returns the special gray "undefined" point symbol. */
	static PointSymbol* getUndefinedPoint();
	
	/** Returns the special gray "undefined" text symbol. */
	static TextSymbol* getUndefinedText();
	
	
signals:
	/**
	 * Emitted when the map enters or leaves the state which is saved on map.
	 */
	void hasUnsavedChanged(bool is_clean);
	
	
	/** Emitted when a color is added to the map, gives the color's index and pointer. */
	void colorAdded(int pos, const LibreMapper::MapColor* color);
	
	/** Emitted when a map color is changed, gives the color's index and pointer. */
	void colorChanged(int pos, const LibreMapper::MapColor* color);
	
	/** Emitted when a map color is deleted, gives the color's index and pointer. */
	void colorDeleted(int pos, const LibreMapper::MapColor* old_color);
	
	/** Emitted when the presence of spot colors in the map changes. */
	void spotColorPresenceChanged(bool has_spot_colors) const;  // clazy:exclude=const-signal-or-slot
	
	
	/** Emitted when a symbol is added to the map, gives the symbol's index and pointer. */
	void symbolAdded(int pos, const LibreMapper::Symbol* symbol);
	
	/** Emitted when a symbol in the map is changed. */
	void symbolChanged(int pos, const LibreMapper::Symbol* new_symbol, const LibreMapper::Symbol* old_symbol);
	
	/** Emitted when the icon of the symbol with the given index changes. */
	void symbolIconChanged(int pos);
	
	/** Emitted when a symbol in the map is deleted. */
	void symbolDeleted(int pos, const LibreMapper::Symbol* old_symbol);
	
	/** Emitted when the symbol icon zoom changes. */
	void symbolIconZoomChanged();
	
	
	/** Emitted before an explicit change of the first front template.
	 * 
	 * This signal is not emitted when the change is done implicitly by
	 * addTemplate or removeTemplate.
	 */
	void firstFrontTemplateAboutToBeChanged(int old_pos, int new_pos);
	
	/** Emitted after an explicit change of the first front template.
	 * 
	 * This signal is not emitted when the change is done implicitly by
	 * addTemplate or removeTemplate.
	 */
	void firstFrontTemplateChanged(int old_pos, int new_pos);
	
	/** Emitted before a template is added to the map.
	 * 
	 * Adding a template immediately below the map uses the same value for pos
	 * as adding one immediately above the map. That's why there is a third
	 * parameter which always signals whether the template is being added in
	 * the background.
	 */
	void templateAboutToBeAdded(int pos, LibreMapper::Template* temp, bool in_background);
	
	/** Emitted when a template is added to the map, gives the template's index and pointer. */
	void templateAdded(int pos, LibreMapper::Template* temp);
	
	/** Emitted when a template in the map is changed, gives the template's index and pointer. */
	void templateChanged(int pos, LibreMapper::Template* temp);
	
	/** Emitted before a template in the map is moved to a different index. */
	void templateAboutToBeMoved(int old_pos, int new_pos, LibreMapper::Template* temp);
	
	/** Emitted when a template in the map was moved to a different index. */
	void templateMoved(int old_pos, int new_pos, LibreMapper::Template* temp);
	
	/** Emitted before a template in the map is deleted. */
	void templateAboutToBeDeleted(int pos, LibreMapper::Template* temp);
	
	/** Emitted when a template in the map is deleted, gives the template's index and pointer. */
	void templateDeleted(int pos, const LibreMapper::Template* old_temp);
	
	/** Emitted when the number of closed templates changes between zero and one. */
	void closedTemplateAvailabilityChanged();
	
	
	/** Emitted when the set of selected objects changes. Also emitted when the
	 *  symbol of a selected object changes (which is similar to selecting another
	 *  object). */
	void objectSelectionChanged();
	
	/**
	 * Emitted when at least one of the selected objects is edited in any way.
	 * For example, this includes the case where a symbol of one of the
	 * selected objects is edited, too.
	 */
	void selectedObjectEdited();

	
	/**
	 * Emitted when the map part currently used for drawing changes.
	 * 
	 * @see currentMapPartIndexChanged()
	 */
	void currentMapPartChanged(const LibreMapper::MapPart* part);
	
	/**
	 * Emitted when the index of map part currently used for drawing changes.
	 * 
	 * This signal may be emitted even when the current MapPart object does not
	 * change. This happens when the index changes due to addition or removal
	 * of map parts.
	 */
	void currentMapPartIndexChanged(std::size_t index);
	
	/**
	 * Emitted when a part is added to the map.
	 */
	void mapPartAdded(std::size_t index, const LibreMapper::MapPart* part);
	
	/**
	 * Emitted when a part's properties are changed.
	 */
	void mapPartChanged(std::size_t index, const LibreMapper::MapPart* part);
	
	/**
	 * Emitted when a part is removed from the map.
	 */
	void mapPartDeleted(std::size_t index, const LibreMapper::MapPart* part);
	
protected slots:
	void checkSpotColorPresence();
	
	void undoCleanChanged(bool is_clean);
	
private:
	typedef std::vector<MapColor*> ColorVector;
	typedef std::vector<Symbol*> SymbolVector;
	typedef std::vector<std::unique_ptr<Template>> TemplateVector;
	typedef std::vector<MapPart*> PartVector;
	typedef std::vector<MapWidget*> WidgetVector;
	
	class MapColorSet : public QSharedData
	{
	public:
		ColorVector colors; ///< Colors ordered by priority. Compact vector.
		ColorVector ids;    ///< Colors indexed by their id. Sparse with nullptrs.
		
		MapColorSet();
		
		MapColorSet(const MapColorSet& ) = delete;
		
		~MapColorSet();
		
		void insert(int pos, MapColor* color);
		
		void erase(int pos);
		
		/** Merges another MapColorSet into this set, trying to maintain
		 *  the relative order of colors.
		 *  If a filter is given, imports only the colors for  which
		 *  filter[color_index] is true, or which are spot colors referenced
		 *  by the selected colors.
		 *  If a map is given, this color set is modified through the map's
		 *  color accessor methods so that other object become aware of the
		 *  changes.
		 *  @return a mapping from the imported color pointer in the other set
		 *          to color pointers in this set.
		 */
		MapColorMap importSet(const MapColorSet& other, 
		                      std::vector<bool>* filter = nullptr,
		                      Map* map = nullptr);
		
	private:
		/**
		 * Adjust the priorities of the colors in the range [first,last).
		 */
		void adjustColorPriorities(int first, int last);
	};
	
	
	/** 
	 * Imports the other symbol set into this map's symbols.
	 * 
	 * If a filter is given, imports only the symbols for which filter[color_index] == true.
	 * Imported symbols are placed at insert_pos (if positive), or after the existing symbols.
	 * Returns a mapping from original symbols (in other) to imported symbols.
	 */
	QHash<const Symbol*, Symbol*> importSymbols(
	        const Map& other,
	        const MapColorMap& color_map,
	        int insert_pos = -1,
	        bool merge_duplicates = true,
	        const std::vector<bool>& filter = {}
	);
	
	
	void addSelectionRenderables(const Object* object);
	void updateSelectionRenderables(const Object* object);
	void removeSelectionRenderables(const Object* object);
	
	static void initStatic();
	
	QExplicitlySharedDataPointer<MapColorSet> color_set;
	bool has_spot_colors;
	QString symbol_set_id;
	SymbolVector symbols;
	mutable qreal symbol_icon_scale = 0;
	TemplateVector templates;
	TemplateVector closed_templates;
	int first_front_template = 0;		// index of the first template in templates which should be drawn in front of the map
	PartVector parts;
	ObjectSelection object_selection;
	Object* first_selected_object = nullptr;
	QScopedPointer<UndoManager> undo_manager;
	std::size_t current_part_index = 0;
	WidgetVector widgets;
	QScopedPointer<MapRenderables> renderables;
	QScopedPointer<MapRenderables> selection_renderables;
	
	QString map_notes;
	
	QScopedPointer<Georeferencing> georeferencing;
	
	MapGrid grid;
	
	int renderable_options;
	
	QScopedPointer<MapPrinterConfig> printer_config;
	
	ImageTemplateDefaults image_template_defaults;
	
	bool colors_dirty;				// are there unsaved changes for the colors?
	bool symbols_dirty;				//    ... for the symbols?
	bool templates_dirty;			//    ... for the templates?
	bool objects_dirty;				//    ... for the objects?
	bool other_dirty;				//    ... for any other settings?
	bool unsaved_changes;			// are there unsaved changes for any component?
	bool unsaved_changes_signaled = false; // state of unsaved_changes before signals were blocked
	
	std::set<Object*> irregular_objects;
	
	// Static
	
	static bool static_initialized;
	static MapColor covering_white;
	static MapColor covering_red;
	static MapColor undefined_symbol_color;
	static MapColor registration_color;
	static LineSymbol* covering_white_line;
	static LineSymbol* covering_red_line;
	static LineSymbol* undefined_line;
	static PointSymbol* undefined_point;
	static TextSymbol* undefined_text;
	static CombinedSymbol* covering_combined_line;
};



// ### Map inline code ###

inline
int Map::getNumColorPrios() const
{
	return (int)color_set->colors.size();
}

inline
MapColor* Map::getMapColorByPrio(int i)
{
	return const_cast<MapColor*>(static_cast<const Map*>(this)->getMapColorByPrio(i));
}

inline
const MapColor* Map::getMapColorByPrio(int i) const
{
	if (0 <= i && i < (int)color_set->colors.size())
	{
		return color_set->colors[i];
	}
	return nullptr;
}

inline
const MapColor* Map::getColorByPrio(int i) const
{
	if (0 <= i && i < (int)color_set->colors.size())
	{
		return color_set->colors[i];
	}
	else switch (i)
	{
		case -1005:
			return getCoveringRed();
		case -1000:
			return getCoveringWhite();
		case -900:
			return getRegistrationColor();
		case -500:
			return getUndefinedColor();
		default:
			return nullptr;
	}
}

inline
const MapColor* Map::getMapColorById(int id) const
{
	return (id >= 0 && id < static_cast<int>(color_set->ids.size())) ? color_set->ids[id] : nullptr;
}


inline
QString Map::symbolSetId() const
{
	return symbol_set_id;
}


inline
int Map::getNumSymbols() const
{
	return (int)symbols.size();
}

template<typename T> 
void Map::sortSymbols(T compare)
{
	std::stable_sort(symbols.begin(), symbols.end(), compare);
	// TODO: emit symbolChanged(pos, symbol); ? s/b same choice as for moveSymbol()
	setSymbolsDirty();
}



inline
UndoManager& Map::undoManager()
{
	return const_cast<UndoManager&>(static_cast<const Map*>(this)->undoManager());
}

inline
const UndoManager& Map::undoManager() const
{
	return *(undo_manager.data());
}

inline
int Map::getNumParts() const
{
	return parts.size();
}

inline
MapPart* Map::getPart(std::size_t i) const
{
	return parts[i];
}

inline
MapPart* Map::getCurrentPart() const
{
	return parts[current_part_index];
}

inline
void Map::setCurrentPart(MapPart* part)
{
	setCurrentPartIndex(findPartIndex(part));
}

inline
std::size_t Map::getCurrentPartIndex() const
{
	return current_part_index;
}

inline
const Map::ObjectSelection& Map::selectedObjects() const
{
	return object_selection;
}

inline
int Map::getNumSelectedObjects() const
{
	return (int)object_selection.size();
}

inline
Map::ObjectSelection::const_iterator Map::selectedObjectsBegin() const
{
	return object_selection.cbegin();
}

inline
Map::ObjectSelection::const_iterator Map::selectedObjectsEnd() const
{
	return object_selection.cend();
}

inline
const Object* Map::getFirstSelectedObject() const
{
	return first_selected_object;
}

inline
Object* Map::getFirstSelectedObject()
{
	return first_selected_object;
}

inline
const QString& Map::getMapNotes() const
{
	return map_notes;
}

inline
const Georeferencing& Map::getGeoreferencing() const
{
	return *georeferencing;
}

inline
const MapGrid& Map::getGrid() const
{
	return grid;
}

inline
int Map::renderableOptions() const
{
	return renderable_options;
}

inline
bool Map::hasPrinterConfig() const noexcept
{
	return !printer_config.isNull();
}

inline
bool Map::hasUnsavedChanges() const
{
	return unsaved_changes;
}

inline
bool Map::areColorsDirty() const
{
	return colors_dirty;
}

inline
bool Map::areSymbolsDirty() const
{
	return symbols_dirty;
}

inline
bool Map::areTemplatesDirty() const
{
	return templates_dirty;
}

inline
bool Map::areObjectsDirty() const
{
	return objects_dirty;
}

inline
bool Map::isOtherDirty() const
{
	return other_dirty;
}

inline
const MapColor* Map::getCoveringRed()
{
	return &covering_red;
}

inline
const MapColor* Map::getCoveringWhite()
{
	return &covering_white;
}

inline
const MapColor* Map::getUndefinedColor()
{
	return &undefined_symbol_color;
}

inline
const MapColor* Map::getRegistrationColor()
{
	return &registration_color;
}

inline
LineSymbol* Map::getCoveringWhiteLine()
{
	return covering_white_line;
}

inline
LineSymbol* Map::getCoveringRedLine()
{
	return covering_red_line;
}

inline
CombinedSymbol* Map::getCoveringCombinedLine()
{
	return covering_combined_line;
}

inline
LineSymbol* Map::getUndefinedLine()
{
	return undefined_line;
}

inline
PointSymbol* Map::getUndefinedPoint()
{
	return undefined_point;
}

inline
TextSymbol* Map::getUndefinedText()
{
	return undefined_text;
}


}  // namespace LibreMapper


Q_DECLARE_METATYPE(const LibreMapper::Map*)

Q_DECLARE_OPERATORS_FOR_FLAGS(LibreMapper::Map::ImportMode)


#endif // OPENORIENTEERING_MAP_H
