/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_GDAL_MANAGER_H
#define OPENORIENTEERING_GDAL_MANAGER_H


#include <vector>

class QByteArray;
class QString;
class QStringList;


namespace OpenOrienteering {

/**
 * A utility class which takes care of GDAL settings and options.
 * 
 * This class provides lists of extensions supported via GDAL in Mapper.
 * It sets and updates GDAL configuration parameters from Mapper's settings.
 * 
 * There is no need to keep objects of this class for an extended life time:
 * instantiation is cheap; the actual state is shared and retained.
 */
class GdalManager
{
private:
	class GdalManagerPrivate;
	
	GdalManagerPrivate* p;
	
public:
	enum FileFormat
	{
		GPX,
	};

	enum ImportOption
	{
		ClipLayers
	};
	
	enum ExportOption
	{
		OneLayerPerSymbol
	};
	
	/**
	 * Constructs a new manager object.
	 */
	GdalManager();
	
	/**
	 * Sets the GDAL configuration from Mapper's defaults and settings.
	 */
	void configure();
	
	
	/**
	 * Returns the area hatching display setting.
	 */
	bool isAreaHatchingEnabled() const;
	
	/**
	 * Sets the area hatching template display setting.
	 */
	void setAreaHatchingEnabled(bool enabled);
	
	
	/**
	 * Returns the baseline view template display setting.
	 */
	bool isBaselineViewEnabled() const;
	
	/**
	 * Sets the baseline view template display setting.
	 */
	void setBaselineViewEnabled(bool enabled);
	
	
	/**
	 * Enables or disables handling of a particular file format by GDAL.
	 */
	void setFormatEnabled(FileFormat format, bool enabled);
	
	/**
	 * Returns if GDAL will handle a particular file format.
	 */
	bool isFormatEnabled(FileFormat format) const;
	

	/**
	 * Enables or disables a GDAL export option
	 */
	void setExportOptionEnabled(ExportOption option, bool enabled);
	
	/**
	 * Returns if a GDAL export option is enabled
	 */
	bool isExportOptionEnabled(ExportOption option) const;


	/**
	 * Enables or disables a GDAL import option
	 */
	void setImportOptionEnabled(ImportOption option, bool enabled);
	
	/**
	 * Returns if a GDAL import option is enabled
	 */
	bool isImportOptionEnabled(ImportOption option) const;


	/**
	 * Returns the file name extensions for supported raster formats.
	 */
	const std::vector<QByteArray>& supportedRasterExtensions() const;
	
	/**
	 * Returns the file name extensions for supported vector formats.
	 */
	const std::vector<QByteArray>& supportedVectorImportExtensions() const;

	/**
	 * Returns the file name extensions for supported vector export formats.
	 */
	const std::vector<QByteArray>& supportedVectorExportExtensions() const;
	
	
	/**
	 * Tests if a particular driver is available.
	 */
	static bool isDriverEnabled(const char* driver_name);
	
	
	/**
	 * Returns the list of GDAL configuration parameters.
	 */
	QStringList parameterKeys() const;

	/**
	 * Returns a GDAL configuration parameter value.
	 */
	QString parameterValue(const QString& key) const;
	
	/**
	 * Sets a GDAL configuration parameter value.
	 */
	void setParameterValue(const QString& key, const QString& value);
	
	/**
	 * Unsets a GDAL configuration parameter value.
	 */
	void unsetParameter(const QString& key);
	
	
	/**
	 * A proxy for OSRSetPROJSearchPaths.
	 * 
	 * Does nothing for GDAL < 3.0.0.
	 */
	static void setProjSearchPaths(const char* const* search_paths);
};


}  // namespace OpenOrienteering

#endif // OPENORIENTEERING_GDAL_MANAGER_H
