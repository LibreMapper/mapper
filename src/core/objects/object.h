/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2013-2020, 2025 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_OBJECT_H
#define LIBREMAPPER_OBJECT_H

#include <limits>
#include <utility>
#include <vector>

#include <QtGlobal>
#include <QRectF>
#include <QString>
// IWYU pragma: no_include <QTransform>

#include "core/map_coord.h"
#include "core/path_coord.h"
#include "core/virtual_path.h"
#include "core/renderables/renderable.h"
#include "core/symbols/symbol.h"
#include "util/key_value_container.h"

class QTransform;
class QXmlStreamReader;
class QXmlStreamWriter;
// IWYU pragma: no_forward_declare QRectF

namespace LibreMapper {

class Map;
class PointObject;
class PathObject;
class TextObject;
class VirtualCoordVector;


/**
 * Abstract base class which combines coordinates and a symbol to form an object
 * (in a map, or inside a point symbol as one of its elements).
 * 
 * Every object must have a symbol. If the symbol is not known, one of the
 * "undefined" symbols from the Map class can be used.
 * 
 * From the object's data, a call to update() will generate the object's "output",
 * that is a set of renderables and the calculation of the object's extent (bounding box).
 * The renderables can then be inserted into a map where they are used to display the object.
 */
class Object  // clazy:exclude=copyable-polymorphic
{
friend class ObjectRenderables;
friend class XMLImportExport;
public:
	/** Enumeration of possible object types. */
	enum Type
	{
		/**
		 * A single coordinate, no further coordinates can be added.
		 * For point symbols only.
		 */
		Point = 0,
		
		/**
		 * A dynamic list of coordinates.
		 * For line, area and combined symbols.
		 */
		Path = 1,
		
		/**
		 * Either one or two coordinates, for single-anchor or box text.
		 * For text symbols only.
		 */
		Text = 4
	};
	
	/** Creates an empty object with the given type and (optional) symbol. */
	explicit Object(Type type, const Symbol* symbol = nullptr);
	
	/** Creates an empty object with the given type, symbol, coords and (optional) map. */
	explicit Object(Type type, const Symbol* symbol, MapCoordVector coords, Map* map = nullptr);
	
protected:
	/**
	 * Constructs an Object, initialized from the given prototype.
	 * 
	 * Note that the object is NOT added to a map, and consequently,
	 * the map pointer is initialized to nullptr.
	 */
	explicit Object(const Object& proto);
	
public:
	/** Destructs the object. */
	virtual ~Object();
	
	Object& operator=(const Object& other) = delete;
	
	virtual void copyFrom(const Object& other);
	
	/** Creates an identical copy of the object.
	 *
	 * This needs to be implemented in non-abstract subclasses.
	 * Implementation should use the copy constructor to ensure proper initialization.
	 */
	virtual Object* duplicate() const = 0;
	
	/**
	 * Checks for equality with another object.
	 * 
	 * If compare_symbol is set, also the symbols are compared for having the same properties.
	 * Note that the map property is not compared.
	 */
	bool equals(const Object* other, bool compare_symbol) const;
	
	virtual bool validate() const;
	
	/** Returns the object type determined by the subclass */
	inline Type getType() const;
	
	/** Convenience cast to PointObject with type checking */
	PointObject* asPoint();
	/** Convenience cast to PointObject with type checking */
	const PointObject* asPoint() const;
	/** Convenience cast to PathObject with type checking */
	PathObject* asPath();
	/** Convenience cast to PathObject with type checking */
	const PathObject* asPath() const;
	/** Convenience cast to TextObject with type checking */
	TextObject* asText();
	/** Convenience cast to TextObject with type checking */
	const TextObject* asText() const;
	
	/** Saves the object in xml format to the given stream. */
	void save(QXmlStreamWriter& xml) const;
	/**
	 * Loads the object in xml format from the given stream.
	 * @param xml The stream to load the object from, must be at the correct tag.
	 * @param map The map in which the object will be inserted.
	 *            This value will be assigned to the object's map pointer
	 *            It may be nullptr.
	 * @param symbol_dict A dictionary mapping symbol IDs to symbol pointers.
	 * @param symbol If set, this symbol will be assigned to the object, rather
	 *               than reading the symbol from the stream.
	 */
	static Object* load(QXmlStreamReader& xml, Map* map, const SymbolDictionary& symbol_dict, const Symbol* symbol = nullptr);
	
	
	/**
	 * Returns the rotation for this object (in radians).
	 * 
	 * The interpretation of this value depends the object's symbol.
	 */
	qreal getRotation() const { return rotation; }
	
	/**
	 * Sets the object's rotation (in radians).
	 * 
	 * The interpretation of this value depends on the object's symbol.
	 * It may have no effect at all.
	 * The value must not be NaN.
	 */
	void setRotation(qreal new_rotation);
	
	
	/**
	 * If the output_dirty flag is set, regenerates output and extent, and updates the object's map (if set).
	 * 
	 * Returns true if output was dirty.
	 */
	bool update() const;
	
	/**
	 * Always regenerates output and extent, and updates the object's map (if set).
	 */
	void forceUpdate() const;
	
	
	/** Moves the whole object
	 * @param dx X offset in native map coordinates.
	 * @param dy Y offset in native map coordinates.
	 */
	void move(qint32 dx, qint32 dy);
	
	/** Moves the whole object by the given offset. */
	void move(const MapCoord& offset);
	
	/** Scales all coordinates, with the given scaling center */
	virtual void scale(const MapCoordF& center, double factor);
	
	/** Scales all coordinates, with the center (0, 0).
	 * @param factor_x horizontal scaling factor
	 * @param factor_y vertical scaling factor
	 */
	virtual void scale(double factor_x, double factor_y);
	
protected:
	/** Rotates the pattern origin around the center point.
	 *  The angle must be given in radians. */
	virtual void rotatePatternOrigin(const MapCoordF& center, qreal sin_angle, qreal cos_angle);
	
public:
	/** Rotates the whole object around the center point.
	 *  The angle must be given in radians. */
	void rotateAround(const MapCoordF& center, qreal angle);
	
	/** Rotates the whole object around the center (0, 0).
	 *  The angle must be given in radians. */
	void rotate(qreal angle);
	
	/**
	 * Apply a transformation to all coordinates.
	 * 
	 * \todo Handle rotation of patterns or text (?)
	 */
	virtual void transform(const QTransform& t) = 0;
	
	/**
	 * Checks if the given coord, with the given tolerance, is on this object.
	 * 
	 * With extended_selection, the coord is on point objects always
	 * if it is within their extent, otherwise it has to be close to
	 * their midpoint. Returns a Symbol::Type which specifies on which
	 * symbol type the coord is
	 * (important for combined symbols which can have areas and lines).
	 */
	int isPointOnObject(const MapCoordF& coord, qreal tolerance, bool treat_areas_as_paths, bool extended_selection) const;
	
	/**
	 * Checks if a path point (excluding curve control points) is included in the given box.
	 */
	virtual bool intersectsBox(const QRectF& box) const = 0;
	
	/** Takes ownership of the renderables */
	void takeRenderables();
	
	/** Deletes the renderables (and extent), undoing update() */
	void clearRenderables();
	
	/** Returns the renderables, read-only */
	const ObjectRenderables& renderables() const;
	
	// Getters / Setters
	
	/**
	 * Returns the raw MapCoordVector of the object.
	 * It's layout and interpretation depends on the object type.
	 */
	const MapCoordVector& getRawCoordinateVector() const;
	
	/** Sets the object output's dirty state. */
	void setOutputDirty(bool dirty = true);
	/** Returns true if the object's output must be regenerated. */
	bool isOutputDirty() const;
	
	/**
	 * Changes the object's symbol, returns true if successful.
	 * 
	 * Some conversions are impossible, for example point to line. Normally,
	 * this method checks if the types of the old and the new symbol are
	 * compatible. If the old symbol pointer is no longer valid, you can
	 * use no_checks to disable this.
	 */
	bool setSymbol(const Symbol* new_symbol, bool no_checks);
	/** Returns the object's symbol. */
	const Symbol* getSymbol() const;
	
	/** NOTE: The extent is only valid after update() has been called! */
	const QRectF& getExtent() const;
	
	/**
	 * Sets the object's map pointer.
	 * 
	 * May be nullptr if the object is not in a map.
	 */
	void setMap(Map* map);
	/** Returns the object's map pointer. */
	Map* getMap() const;
	
	/** Constructs an object of the given type with the given symbol. */
	static Object* getObjectForType(Type type, const Symbol* symbol = nullptr);
	
	
	/** Returns a const reference to the object's tags. */
	const KeyValueContainer& tags() const;
	
	/** Replaces the object's tags. */
	void setTags(const KeyValueContainer& tags);
	
	/** Returns the value of the given tag key. */
	QString getTag(const QString& key) const;
	
	/** Sets the given tag key to the value. */
	void setTag(const QString& key, const QString& value);
	
	/** Removes the given tag key and its value. */
	void removeTag(const QString& key);
	
	
	/**
	 * @brief Extends a rectangle to enclose all of the object's control points.
	 */
	void includeControlPointsRect(QRectF& rect) const;
	
protected:
	virtual void updateEvent() const;
	
	virtual void createRenderables(ObjectRenderables& output, Symbol::RenderableOptions options) const;
	
	Type type;
	const Symbol* symbol = nullptr;
	MapCoordVector coords;
	Map* map = nullptr;
	KeyValueContainer object_tags;
	
private:
	qreal rotation = 0;               ///< The object's rotation (in radians).
	mutable bool output_dirty = true; // does the output have to be re-generated because of changes?
	mutable QRectF extent;            // only valid after calling update()
	mutable ObjectRenderables output; // only valid after calling update()
};



class PathPartVector;


/**
 * Helper class with information about parts of paths.
 * 
 * A part is a path segment which is separated from other parts by
 * a hole point at its end.
 */
class PathPart : public VirtualPath
{
public:
	/** Pointer to path object containing this part */
	PathObject* path = nullptr;
	
	PathPart() = delete;
	PathPart(const PathPart&) = default;
	PathPart(PathPart&&) = default;
	
	PathPart(
	        PathObject& path,
	        MapCoordVector::size_type start_index,
	        MapCoordVector::size_type end_index
	);
	
	PathPart(
	        const VirtualCoordVector& coords,
	        MapCoordVector::size_type start_index,
	        MapCoordVector::size_type end_index
	);
	
	PathPart(
	        PathObject& path,
	        const VirtualPath& virtual_path
	);
	
	~PathPart() = default;
	
	PathPart& operator=(const PathPart& rhs);
	
	/**
	 * Closes or opens the sub-path.
	 * 
	 * If closed == true and may_use_existing_close_point == false,
	 * a new point is added as closing point even if its coordinates
	 * are identical to the existing last point. Else, the last point
	 * may be reused.
	 */
	void setClosed(bool closed, bool may_use_existing_close_point = false);
	
	/**
	 * Closes the subpath, merging the start and end point at their center.
	 * 
	 * \see PathPart::setClosed()
	 */
	void connectEnds();
	
	/**
	 * Reverses the part's coordinates.
	 * 
	 * Reversing the coordinates results in switching the start/end/mid/dash
	 * symbol direction for line symbols.
	 * 
	 * \see PathObject::reverse()
	 */
	void reverse();
	
	static PathPartVector calculatePathParts(const VirtualCoordVector& coords);
};



class PathPartVector : public std::vector<PathPart> 
{
public:
	/**
	 * This is dangerous when copying objects (which own a PathPartVector).
	 * 
	 * Objects need to deal with the PathParts explicitly, at least as long as
	 * the PathPart contains distinct references to the object and to the
	 * coordinates.
	 * 
	 * Other use cases may consider using std::vector<PathPart>.
	 */
	PathPartVector& operator=(const PathPartVector&) = delete;
	
	/**
	 * Returns true if the part's end_index is lower than index.
	 * 
	 * This function can be used for doing a binary search on a sorted PathPartVector.
	 * 
	 * @see std::lower_bound()
	 */
	static bool compareEndIndex(const PathPart& part, VirtualPath::size_type index);
};



/**
 * Object type which can be used for line, area and combined symbols.
 * Has a dynamic number of coordinates.
 * 
 * The coordinates are divided into one or multiple PathParts. A PathPart
 * is ended by a coordinate with the "hole point" flag. For all types of
 * flags which can be set, see the MapCoord documentation.
 */
class PathObject : public Object  // clazy:exclude=copyable-polymorphic
{
	friend class PathPart;
	
public:
	
	/** Returned by calcAllIntersectionsWith(). */
	struct Intersection
	{
		/** Coordinate of the intersection */
		MapCoordF coord;
		/** Part index of intersection */
		PathPartVector::size_type part_index;
		/** Length of path until this intersection point */
		PathCoord::length_type length;
		/** Part index of intersection in other path */
		PathPartVector::size_type other_part_index;
		/** Length of other path until this intersection point */
		PathCoord::length_type other_length;
		
		/**
		 * Creates an Intersection at the position specified by factors a and b
		 * between the a0/a1 and b0/b1 PathCoords in the given parts.
		 */
		static Intersection makeIntersectionAt(
		        double a,
		        double b,
		        const PathCoord& a0,
		        const PathCoord& a1,
		        const PathCoord& b0,
		        const PathCoord& b1,
		        PathPartVector::size_type part_index,
		        PathPartVector::size_type other_part_index );
	};
	
	/** std::vector of Intersection with the ability to sort them and remove duplicates. */
	class Intersections : public std::vector<Intersection>
	{
	public:
		/** Sorts the intersections and removes duplicates. */
		void normalize();
	};
	
	
	/** Constructs a PathObject, optionally assigning a symbol. */
	explicit PathObject(const Symbol* symbol = nullptr);
	
	/** Constructs a PathObject, assigning initial coords and optionally the map pointer. */
	explicit PathObject(const Symbol* symbol, MapCoordVector coords, Map* map = nullptr);
	
	/**
	 * Constructs a PathObject, assigning initial coords from a single piece of a line.
	 * 
	 * "Piece" refers to a single straight or curved arc from the point
	 * identified by parameter piece to the immediate next point on the path.
	 * 
	 * If the path is not closed, and piece refers to the last element in the
	 * path (part), then the arc ending in the point referred to by piece is
	 * returned instead.
	 */
	explicit PathObject(const Symbol* symbol, const PathObject& proto, MapCoordVector::size_type piece);
	
protected:
	/** Constructs a PathObject, initialized from the given prototype. */
	explicit PathObject(const PathObject& proto);
	
public:
	/**
	 * Constructs a PathObject, initialized from the given part of another object.
	 */
	explicit PathObject(const PathPart& proto_part);
	
	/**
	 * Creates a duplicate of the path object.
	 * 
	 * Use asPath() on the result to obtain an object of type PathObject.
	 */
	PathObject* duplicate() const override;
	
	PathObject& operator=(const PathObject& other) = delete;
	
	/** Replaces this object's contents by those of the other. */
	void copyFrom(const Object& other) override;
	
	
	bool validate() const override;
	
	
	/** Checks the path for valid flags, and makes corrections as necessary. */
	void normalize();
	
	
	bool intersectsBox(const QRectF& box) const override;
	
	
	// Coordinate access methods
	
	/** Returns the number of coordinates, including curve handles and close points. */
	MapCoordVector::size_type getCoordinateCount() const { return coords.size(); }
	
	/** Returns the i-th coordinate. */
	MapCoord getCoordinate(MapCoordVector::size_type pos) const
	{
		Q_ASSERT(pos < coords.size());
		return coords[pos];
	}
	
	/** Returns a non-const reference to the i-th coordinate.
	 * 
	 * Normally you should modify coordinates via PathObject::setCoordinate.
	 * Unlike that function, modifying a coordinate directly via the reference
	 * will not keep the first and last point of a closed path in sync.
	 */
	MapCoord& getCoordinateRef(MapCoordVector::size_type pos)
	{
		Q_ASSERT(pos < coords.size());
		setOutputDirty();
		return coords[pos];
	}
	
	/** Replaces the i-th coordinate with c. */
	void setCoordinate(MapCoordVector::size_type pos, const MapCoord& c);
	
	/** Adds the coordinate at the given index. */
	void addCoordinate(MapCoordVector::size_type pos, const MapCoord& c);
	
	/** Adds the coordinate at the end, optionally starting a new part.
	 *  If starting a new part, make sure that the last coord of the old part
	 *  has the hole point flag! */
	void addCoordinate(const MapCoord& c, bool start_new_part = false);
	
	/**
	 * Deletes a coordinate from the path.
	 * 
	 * When requesting a control point of a bezier arc to be deleted, the other
	 * control point is deleted, too.
	 * 
	 * If the number of regular points in the coordinate's part is not more
	 * than two, the whole part is delete from the object. 
	 * 
	 * @param pos Index of the coordinate to delete.
	 * @param adjust_other_coords If set and the deleted coordinate was joining
	 *     two bezier curves, adapts the adjacent curves with a strategy defined
	 *     by delete_bezier_point_action. adjust_other_coords does not work
	 *     when deleting bezier curve handles!
	 * @param delete_bezier_point_action Must be an enum value from
	 *     Settings::DeleteBezierPointAction if adjust_other_coords is set.
	 */
	void deleteCoordinate(MapCoordVector::size_type pos, bool adjust_other_coords, int delete_bezier_point_action = -1);
	
	/** Deletes all coordinates of the object. */
	void clearCoordinates();
	
	/**
	 * Assigns the given prototype's coordinates subset to this object's coordinates.
	 *
	 * The range must be within one part. Last may be smaller than first iff
	 * the path is closed.
	 */
	void assignCoordinates(const PathObject& proto, MapCoordVector::size_type first, MapCoordVector::size_type last);
	
	
	/** Finds the path part containing the given coord index. */
	PathPartVector::const_iterator findPartForIndex(MapCoordVector::size_type coords_index) const;
	
	/** Finds the path part containing the given coord index. */
	PathPartVector::iterator findPartForIndex(MapCoordVector::size_type coords_index);
	
	/**
	 * Finds the path part containing the given coord index.
	 * 
	 * \todo Review where this signature can be replace by the one returning an iterator.
	 */
	PathPartVector::size_type findPartIndexForIndex(MapCoordVector::size_type coords_index) const;
	
	
	/**
	 * Returns the path coordinate for the map coordinate with given index.
	 * 
	 * @param index Index of normal MapCoord for which to create the PathCoord.
	 */
	PathCoord findPathCoordForIndex(MapCoordVector::size_type index) const;
	
	
	/**
	 * Returns true if the given index is a curve handle.
	 */
	bool isCurveHandle(MapCoordVector::size_type index) const;
	
	
	/**
	 * Returns the vector of path parts.
	 */
	const PathPartVector& parts() const;
	
	/**
	 * Returns the vector of path parts.
	 * 
	 * Marks the output as dirty.
	 */
	PathPartVector& parts();
	
	/**
	 * Deletes the i-th path part.
	 */
	void deletePart(PathPartVector::size_type part_index);
	
	/**
	 * Transforms the coordinates and the pattern origin.
	 */
	void transform(const QTransform& t) override;
	
	// Pattern methods
	
	/**
	 * Returns the rotation of the object pattern. Only has an effect in
	 * combination with a symbol interpreting this value.
	 */
	qreal getPatternRotation() const;
	
	/**
	 * Sets the rotation of the object pattern. Only has an effect in
	 * combination with a symbol interpreting this value.
	 */
	void setPatternRotation(qreal rotation);
	
	/**
	 * Returns the origin of the object pattern. Only has an effect in
	 * combination with a symbol interpreting this value.
	 */
	MapCoord getPatternOrigin() const;
	
	/**
	 * Sets the origin of the object pattern. Only has an effect in
	 * combination with a symbol interpreting this value.
	 */
	void setPatternOrigin(const MapCoord& origin);
	
	
	// Operations
	
	/**
	 * Calculates the closest point on the path to the given coordinate.
	 * 
	 * This does not need to be an existing path coordinate. This method is
	 * usually called to find the position on the path the user clicked on.
	 * The parameters start_index and end_index can be set to constrain searching
	 * to this specific path part.
	 */
	ClosestPathCoord findClosestPointTo(
	        const MapCoordF& coord,
	        MapCoordVector::size_type start_index = 0,
	        MapCoordVector::size_type end_index = std::numeric_limits<PathPartVector::size_type>::max()
	) const;
	
	/**
	 * Calculates a border path with the closest point to the given coordinate.
	 * 
	 * This function operates on path objects with a line symbol which has
	 * visible borders, or with a combined symbol which features such a line
	 * symbol. For all other symbols, the return value has an empty border.
	 * 
	 * The offsets of the border paths are determined by the main line width.
	 * If the border line is continuous (not dashed), border line width and
	 * offset are added so that the returned path is at the center of the
	 * border line.
	 * 
	 * In addition to the MapCoordF input parameter, this function takes a
	 * PathCoord input parameter which serves as a key to determining candidate
	 * border path locations. This PathCoord is meant to be looked up with
	 * findClosestPointTo() before calling this function.
	 * 
	 * If no point closer than distance_bound_squared, the return value has an
	 * empty border.
	 */
	ClosestBorderPathCoord findClosestPointOnBorder(
	        const MapCoordF& coord,
	        const PathCoord& path_coord,
	        double distance_bound_squared
	) const;
	
	/**
	 * Finds the index of the closest control point coordinate to the given coordinate.
	 */
	MapCoordVector::size_type findClosestCoordinate(const MapCoordF& coord) const;
	
	
	/**
	 * Splits the path at the position given by path_coord.
	 * 
	 * Must not be called while isOutputDirty() returns true.
	 * 
	 * Returns the index of the added point.
	 */
	MapCoordVector::size_type subdivide(const PathCoord& path_coord);
	
	/**
	 * Splits the path in the curve which starts at the given index.
	 * 
	 * The second parameter determines the split position between begin and end
	 * of the curve (0.0 ... 1.0).
	 * 
	 * Must not be called while isOutputDirty() returns true.
	 * 
	 * @return The index of the added point.
	 */
	MapCoordVector::size_type subdivide(MapCoordVector::size_type index, float param);
	
	/**
	 * Returns if connectIfClose() would change something with the given parameters
	 */
	bool canBeConnected(const PathObject* other, double connect_threshold_sq) const;
	
	/**
	 * Returns if the objects were connected (if so, you can delete the other object).
	 * If one of the paths has to be reversed, it is done for the "other" path.
	 * Otherwise, the "other" path is not changed.
	 * 
	 * \todo Review documentation, container usage,
	 */
	bool connectIfClose(PathObject* other, double connect_threshold_sq);
	
	/**
	 * Connects the given parts, optionally merging the end coordinates at the
	 * center position, and copying over the coordinates from other.
	 */
	void connectPathParts(
	        PathPartVector::size_type part_index,
	        const PathObject* other,
	        PathPartVector::size_type other_part_index,
	        bool prepend,
	        bool merge_ends = true
	);
	
	/**
	 * Returns the result of removing the section between begin and end from the path.
	 * 
	 * begin and end must belong to the path part with the given part_index.
	 * However, objects with holes, and part_index values greater than 0, are
	 * not supported at the moment.
	 * 
	 * Returns an empty vector when nothing remains after removal.
	 */
	std::vector<PathObject*> removeFromLine(
	        PathPartVector::size_type part_index,
	        PathCoord::length_type clen_begin,
	        PathCoord::length_type clen_end
	) const;
									   
	/**
	 * Returns the result of splitting the path at the given inner position.
	 * 
	 * Returns an empty vector when the object is not changed by the split.
	 * This happens when the path is not closed and split_pos is the begin or
	 * end of the path, or when the object has got more than a single PathPart.
	 */
	std::vector<PathObject*> splitLineAt(const PathCoord& split_pos) const;
	
	/**
	 * Replaces the path with a non-empty range of it starting and ending at the given lengths.
	 * 
	 * For open paths, the end length must be greater than the start length.
	 * For closed paths, an end length smaller than or equal to the start length
	 * will cause the resulting path to span the original start/end point.
	 */
	void changePathBounds(
	        PathPartVector::size_type part_index,
	        PathCoord::length_type start_len,
	        PathCoord::length_type end_len
	);
	
	/**
	 * Appends (copies) the coordinates of other to this path.
	 */
	void appendPath(const PathObject* other);
	
	/**
	 * Appends (copies) the coordinates of a specific part to this path.
	 * 
	 * The other object is determined from the part's path property.
	 */
	void appendPathPart(const PathPart& part);
	
	/**
	 * Reverses the object's coordinates, resulting in switching
	 * the start / end / mid / dash symbol direction for line symbols.
	 */
	void reverse();
	
	/** Ensures that all parts are closed. Useful for objects with area-only symbols. */
	void closeAllParts();
	
	/**
	 * Converts all polygonal sections in this path to splines.
	 * If at least one section is converted, returns true and
	 * returns an undo duplicate if the corresponding pointer is set.
	 */
	bool convertToCurves(PathObject** undo_duplicate = nullptr);
	
	/**
	 * Converts the given range of coordinates to a spline by inserting handles.
	 * The range must consist of only polygonal segments before.
	 * 
	 * @return The new index of the end of the range.
	 */
	PathPart::size_type convertRangeToCurves(const PathPart& part, PathPart::size_type start_index, PathPart::size_type end_index);
	
	/**
	 * Tries to remove points while retaining the path shape as much as possible.
	 * If at least one point is changed, returns true and
	 * returns an undo duplicate if the corresponding pointer is set.
	 */
	bool simplify(PathObject** undo_duplicate, double threshold);
	
	/** See Object::isPointOnObject() */
	int isPointOnPath(
	        const MapCoordF& coord,
	        qreal tolerance,
	        bool treat_areas_as_paths,
	        bool extended_selection
	) const;
	
	/**
	 * Returns true if the given coordinate is inside the area
	 * defined by this object, which must be closed.
	 */
	bool isPointInsideArea(const MapCoordF& coord) const;
	
	/**
	 * Calculates the maximum distance of the given coord ranges of two objects.
	 */
	double calcMaximumDistanceTo(
	        MapCoordVector::size_type start_index,
	        MapCoordVector::size_type end_index,
	        const PathObject* other,
	        MapCoordVector::size_type other_start_index,
	        MapCoordVector::size_type other_end_index
	) const;
	
	/**
	 * Calculates and adds all intersections with the other path to out.
	 * Note: intersections are not sorted and may contain duplicates!
	 * To clean them up, call clean() on the Intersections object after adding
	 * all intersections with objects you are interested in.
	 */
	void calcAllIntersectionsWith(const PathObject* other, Intersections& out) const;
	
	/** Called by Object::update() */
	void updatePathCoords() const;
	
	/** Called by Object::load() */
	void recalculateParts();
	
	/** Returns the 'paper' length of path objects in mm. */
	double getPaperLength() const;
	
	/** Returns the 'real' length of path objects in m. */
	double getRealLength() const;
	
	/**
	 * Calculates the 'paper' area of area objects in sq mm.
	 * 
	 * This is the area of the first part (outline) minus the area of other
	 * parts (holes). The calculation is agnostic to overlap of parts.
	 */
	double calculatePaperArea() const;
	
	/**
	 * Calculates 'real' area of area objects in sq m.
	 * 
	 * This is the area of the first part (outline) minus the area of other
	 * parts (holes). The calculation is agnostic to overlap of parts.
	 */
	double calculateRealArea() const;
	
	/** Returns true if the object is smaller than the minimum area required by its symbol. */
	bool isAreaTooSmall() const;
	
	/** Returns true if the object is shorter than the minimum length required by its symbol. */
	bool isLineTooShort() const;
	
	
protected:
	/**
	 * Adjusts the end index of the given part and the start/end indexes of the following parts.
	 * 
	 * output_dirty must be set before calling this function.
	 */
	void partSizeChanged(PathPartVector::iterator part, MapCoordVector::difference_type change);
	
	
	void rotatePatternOrigin(const MapCoordF& center, qreal sin_angle, qreal cos_angle) override;
	
	
	void prepareDeleteBezierPoint(MapCoordVector::size_type pos, int delete_bezier_point_action);
	
	/**
	 * Calculates the factors which should be applied to the length of the
	 * remaining bezier curve handle vectors when deleting a point joining
	 * two bezier curves to try to retain the original curves' shape.
	 * 
	 * This is a simple version, the result should be optimized with
	 * calcBezierPointDeletionRetainingShapeOptimization().
	 * 
	 * p0, p1, p2, q0 make up the first original curve,
	 * q0, q1, q2, q3 make up the second original curve.
	 * out_pfactor is set to the factor to apply to the vector (p1 - p0),
	 * out_qfactor is set to the factor to apply to the vector (q2 - q3),
	 */
	static void calcBezierPointDeletionRetainingShapeFactors(
		MapCoord p0,
		MapCoord p1,
		MapCoord p2,
		MapCoord q0,
		MapCoord q1,
		MapCoord q2,
		MapCoord q3,
		double& out_pfactor,
		double& out_qfactor
	);
	
	/**
	 * Uses nonlinear optimization to improve the first result obtained by
	 * calcBezierPointDeletionRetainingShapeFactors().
	 */
	static void calcBezierPointDeletionRetainingShapeOptimization(
		MapCoord p0,
		MapCoord p1,
		MapCoord p2,
		MapCoord q0,
		MapCoord q1,
		MapCoord q2,
		MapCoord q3,
		double& out_pfactor,
		double& out_qfactor
	);
	
	/**
	 * Is used internally by calcBezierPointDeletionRetainingShapeOptimization()
	 * to calculate the current cost. Evaluates the distance between p0 ... p3
	 * and the reference path.
	 */
	static double calcBezierPointDeletionRetainingShapeCost(
		MapCoord p0,
		MapCoordF p1,
		MapCoordF p2,
		MapCoord p3,
		PathObject* reference
	);
	
	/**
	 * Sets coord as the point which closes a part: sets the correct flags
	 * on it and replaces the coord at the given index by it.
	 * TODO: make separate methods? Setting coords exists already.
	 */
	void setClosingPoint(MapCoordVector::size_type index, const MapCoord& coord);
	
	void updateEvent() const override;
	
	void createRenderables(ObjectRenderables& output, Symbol::RenderableOptions options) const override;
	
private:
	/**
	 * Origin shift of the object pattern. Only used if the object
	 * has a symbol which interprets this value.
	 */
	MapCoord pattern_origin = {};
	
	/** Path parts list */
	mutable PathPartVector path_parts;
};



/** Compares the length of the intersections. */
inline
bool operator< (const PathObject::Intersection& lhs, const PathObject::Intersection& rhs)
{
	return lhs.length < rhs.length;
}

/** Fuzzy equality check. */
inline
bool operator== (const PathObject::Intersection& lhs, const PathObject::Intersection& rhs)
{
	// NOTE: coord is not compared, as the intersection is defined by the other params already.
	const double epsilon = 1e-10;
	return lhs.part_index == rhs.part_index &&
	       lhs.other_part_index == rhs.other_part_index &&
	       qAbs(lhs.length - rhs.length) <= epsilon &&
	       qAbs(lhs.other_length - rhs.other_length) <= epsilon;
}



/**
 * Object type which can only be used for point symbols,
 * and is also the only object which can be used with them.
 * 
 * Has exactly one coordinate, and additionally a rotation parameter.
 */
class PointObject : public Object  // clazy:exclude=copyable-polymorphic
{
public:
	/** Constructs a PointObject, optionally assigning the symbol. */
	explicit PointObject(const Symbol* symbol = nullptr);
	
protected:
	/** Constructs a PointObject, initialized from the given prototype. */
	explicit PointObject(const PointObject& proto);
	
public:
	/**
	 * Creates a duplicate of the point.
	 * 
	 * Use asPoint() on the result to obtain an object of type PointObject.
	 */
	PointObject* duplicate() const override;
	
	PointObject& operator=(const PointObject& other) = delete;
	
	/** Replaces the content of this object by that of another. */
	void copyFrom(const Object& other) override;
	
	
	/** Sets the point's position to a new position given in native map coordinates. */
	void setPosition(qint32 x, qint32 y);
	
	/** Changes the point's position. */
	void setPosition(const MapCoord& coord);
	
	/** Changes the point's position. */
	void setPosition(const MapCoordF& coord);
	
	/** Returns the point's position as MapCoordF. */
	MapCoordF getCoordF() const;
	
	/** Returns the point's coordinate. */
	MapCoord getCoord() const;
	
	
	/** 
	 * Transforms the position.
	 */
	void transform(const QTransform& t) override;
	
	
	bool intersectsBox(const QRectF& box) const override;
};



/**
 * A single PathCoord together with the object it belongs to.
 * 
 * This is a convenient structure for passing around as parameter and return value.
 */
struct ObjectPathCoord : public PathCoord
{
	PathObject* object;
	
	constexpr ObjectPathCoord() noexcept;
	constexpr ObjectPathCoord(PathObject* object) noexcept;
	constexpr ObjectPathCoord(PathObject* object, const PathCoord& coord) noexcept;
	constexpr ObjectPathCoord(PathObject* object, const PathCoord&& coord) noexcept;
	constexpr ObjectPathCoord(const ObjectPathCoord&) noexcept = default;
	constexpr ObjectPathCoord(ObjectPathCoord&&) noexcept = default;
	ObjectPathCoord& operator=(const ObjectPathCoord&) noexcept = default;
	ObjectPathCoord& operator=(ObjectPathCoord&&) noexcept = default;
	
	/**
	 * This constructor sets the PathCoord members according to the given coordinate index.
	 */
	ObjectPathCoord(PathObject* object, MapCoordVector::size_type index);
	
	/**
	 * Returns true iff the object is not null.
	 */
	constexpr operator bool() const;
	
	/**
	 * Sets this PathCoord to the point on this path which is the closest to the
	 * given coordinate.
	 * 
	 * \return The squared distance of these points.
	 * 
	 * \see PathObject::findClosestPointTo
	 */
	double findClosestPointTo(const MapCoordF& map_coord);
};



//### Object inline code ###

inline
Object::Type Object::getType() const
{
	return type;
}

inline
const ObjectRenderables& Object::renderables() const
{
	return output;
}

inline
const MapCoordVector& Object::getRawCoordinateVector() const
{
	return coords;
}

inline
void Object::setOutputDirty(bool dirty)
{
	output_dirty = dirty;
}

inline
bool Object::isOutputDirty() const
{
	return output_dirty;
}

inline
const Symbol* Object::getSymbol() const
{
	return symbol;
}

inline
const QRectF& Object::getExtent() const
{
	return extent;
}

inline
void Object::setMap(Map* map)
{
	this->map = map;
	setOutputDirty();
}

inline
Map* Object::getMap() const
{
	return map;
}

inline
const KeyValueContainer& Object::tags() const
{
	return object_tags;
}



//### PathPart inline code ###

inline
PathPart::PathPart(
        const VirtualCoordVector& coords,
        MapCoordVector::size_type start_index,
        MapCoordVector::size_type end_index)
 : VirtualPath(coords, start_index, end_index)
{
	// nothing else
}

inline
PathPart::PathPart(
        PathObject& path,
        MapCoordVector::size_type start_index,
        MapCoordVector::size_type end_index )
 : VirtualPath(path.getRawCoordinateVector(), start_index, end_index)
 , path(&path)
{
	// nothing else
}

inline
PathPart& PathPart::operator=(const PathPart& rhs)
{
	Q_ASSERT(path == rhs.path);
	VirtualPath::operator=(rhs);
	return *this;
}



//### PathObject inline code ###

inline
const PathPartVector& PathObject::parts() const
{
	return path_parts;
}

inline
PathPartVector& PathObject::parts()
{
	setOutputDirty();
	return path_parts;
}

inline
qreal PathObject::getPatternRotation() const
{
	return getRotation();
}

inline
MapCoord PathObject::getPatternOrigin() const
{
	return pattern_origin;
}



//### ObjectPathCoord inline code ###

inline
constexpr ObjectPathCoord::ObjectPathCoord() noexcept
: ObjectPathCoord { nullptr }
{
	// nothing else
}


inline
constexpr ObjectPathCoord::ObjectPathCoord(PathObject* object) noexcept
: object { object }
{
	// nothing else
}


inline
constexpr ObjectPathCoord::ObjectPathCoord(PathObject* object, const PathCoord& coord) noexcept
: PathCoord { coord }
, object    { object }
{
	// nothing else
}


inline
constexpr ObjectPathCoord::ObjectPathCoord(PathObject* object, const PathCoord&& coord) noexcept
: PathCoord { std::move(coord) }
, object    { object }
{
	// nothing else
}


inline
ObjectPathCoord::ObjectPathCoord(PathObject* object, MapCoordVector::size_type index)
: PathCoord { object->findPathCoordForIndex(index) }
, object    { object }
{
	// nothing else
}


inline
double ObjectPathCoord::findClosestPointTo(const MapCoordF& map_coord)
{
	auto const closest = object->findClosestPointTo(map_coord);
	static_cast<PathCoord&>(*this) = closest.path_coord;
	return closest.distance_squared;
	
}


inline
constexpr ObjectPathCoord::operator bool() const
{
	return static_cast<bool>(object);
}

}  // namespace LibreMapper


#endif // LIBREMAPPER_OBJECT_H
