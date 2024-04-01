/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_STORAGE_LOCATION_H
#define LIBREMAPPER_STORAGE_LOCATION_H


#include <memory>
#include <vector>
#include <type_traits>

#include <QCoreApplication>
#include <QString>

namespace OpenOrienteering {

namespace Android {

/**
 * Tells the media scanner to register the given file or folder.
 * 
 * This is required to make files quickly available for transfer via MTP.
 */
void mediaScannerScanFile(const QString& path);

}


// noexcept since Qt 5.5
constexpr bool qstring_is_nothrow_copy_constructible = std::is_nothrow_copy_constructible<QString>::value;
constexpr bool qstring_is_nothrow_move_constructible = std::is_nothrow_move_constructible<QString>::value;
constexpr bool qstring_is_nothrow_copy_assignable = std::is_nothrow_copy_assignable<QString>::value;
constexpr bool qstring_is_nothrow_move_assignable = std::is_nothrow_move_assignable<QString>::value;


/**
 * Provides information about document storage locations.
 */
class StorageLocation
{
	Q_DECLARE_TR_FUNCTIONS(OpenOrienteering::StorageLocation)
	
public:
	/** Various hints about locations. */
	enum Hint {
		HintNormal,       ///< Normal location
		HintApplication,  ///< Location which might get cleaned unexpectedly
		HintReadOnly,     ///< Read-only location
		HintNoAccess,     ///< Location which requires to establish AppPermissions::StorageAccess
		HintInvalid       ///< Not a valid location at all
	};
	
	/** Constructs a new location. */
	StorageLocation(const QString& path, Hint hint) noexcept;
	
	/** Default copy constructor. */
	StorageLocation(const StorageLocation&) noexcept(qstring_is_nothrow_copy_constructible) = default;
	
	/** Default move constructor. */
	StorageLocation(StorageLocation&&) noexcept(qstring_is_nothrow_move_constructible) = default;
	
	StorageLocation& operator=(const StorageLocation&) noexcept(qstring_is_nothrow_copy_assignable) = default;
	
	StorageLocation& operator=(StorageLocation&&) noexcept(qstring_is_nothrow_copy_assignable) = default;
	
	
	/** Returns the path of this location. */
	QString path() const;
	
	/**
	 * Returns the hint for this location.
	 * 
	 * This function returns an empty string for HintNormal.
	 */
	Hint hint() const;
	
	/** Returns the text representing the hint for this location. */
	QString hintText() const;
	
	/**
	 * Returns a text template for giving the hint for the given path.
	 * 
	 * This function returns an empty string for HintNormal.
	 */
	static QString fileHintTextTemplate(Hint hint);
	
	
	/**
	 * Returns the known locations for documents.
	 * 
	 * The returned shared-ptr will always have an object, even if it is an empty list.
	 */
	static std::shared_ptr<const std::vector<StorageLocation>> knownLocations();
	
	
	/** Forces a new scan of locations on the next call to knownLocations(). */
	static void refresh();
	
	
private:
	QString m_path;
	Hint    m_hint;
};



inline
StorageLocation::StorageLocation(const QString& path, StorageLocation::Hint hint) noexcept
: m_path { path }
, m_hint { hint }
{
	// nothing else
}

inline
QString StorageLocation::path() const
{
	return m_path;
}

inline
StorageLocation::Hint StorageLocation::hint() const
{
	return m_hint;
}


}  // namespace OpenOrienteering

#endif // LIBREMAPPER_STORAGE_LOCATION_H
