/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012, 2014, 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_TRANSFORMATION_H
#define OPENORIENTEERING_TRANSFORMATION_H

#include <vector>

#include "core/map_coord.h"

class QTransform;
class QXmlStreamReader;
class QXmlStreamWriter;

namespace OpenOrienteering {

struct TemplateTransform;


/** Pair of source and destination coordinates used to calculate transformations. */
struct PassPoint
{
	void save(QXmlStreamWriter& xml) const;
	static PassPoint load(QXmlStreamReader& xml);
	
	/** Start position specified by the user */
	MapCoordF src_coords;
	
	/** End position specified by the user */
	MapCoordF dest_coords;
	
	/** Position where the point really ended up */
	MapCoordF calculated_coords;
	
	/** Distance between dest_coords_map and calculated_coords;
	 *  negative if not calculated yet */
	double error;
};

/** List of pass points with methods for transformation calculation. */
class PassPointList : public std::vector< PassPoint >
{
public:
	/**
	 * Indicates arguments which must not be nullptr.
	 * \todo Use the Guideline Support Library
	 */
	template <typename T>
	using not_null = T;
	
	
	/** Estimates a similarity transformation based on the contained pass points
	 *  and applies it to the transformation passed in. */
	bool estimateSimilarityTransformation(not_null<TemplateTransform*> transform);
	
	bool estimateSimilarityTransformation(not_null<QTransform*> out);
	
	/** Estimates an affine transformation. */
	bool estimateNonIsometricSimilarityTransform(not_null<QTransform*> out);
};


}  // namespace OpenOrienteering

#endif
