/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2020, 2025 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_GEOREFERENCING_DIALOG_H
#define LIBREMAPPER_GEOREFERENCING_DIALOG_H

#include <vector>

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

#include "core/map_coord.h"
#include "tools/tool.h"

class QAction;
class QCursor;
class QCheckBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QLabel;
class QMouseEvent;
class QPushButton;
class QRadioButton;
class QNetworkReply;
class QWidget;

namespace LibreMapper {

class CRSSelector;
class Georeferencing;
class Map;
class MapEditorController;
class MapWidget;


/**
 * The GeoreferencingDialog allows the user to adjust the georeferencing properties
 * of a map.
 */
class GeoreferencingDialog : public QDialog
{
Q_OBJECT
public:
	/**
	 * Constructs a new georeferencing dialog for the map handled by the given 
	 * controller. The optional parameter initially allows to override the current 
	 * properties of the map's georeferencing. The parameter
	 * allow_no_georeferencing determines if the okay button can
	 * be clicked while "- none -" is selected.
	 */
	GeoreferencingDialog(MapEditorController* controller, const Georeferencing* initial = nullptr, bool allow_no_georeferencing = true);
	
	/**
	 * Constructs a new georeferencing dialog for the given map. The optional 
	 * parameter initial allows to override the current properties of the map's
	 * georeferencing. Since the dialog will not know a MapEditorController,
	 * it will not allow to select a new reference point from the map.
	 * The parameter allow_no_georeferencing determines if the okay button can
	 * be clicked while "- none -" is selected.
	 */
	GeoreferencingDialog(QWidget* parent, Map* map, const Georeferencing* initial = nullptr, bool allow_no_georeferencing = true);
	
protected:
	/**
	 * Constructs a new georeferencing dialog.
	 * 
	 * The map parameter must not be nullptr, and it must not be a different
	 * map than the one handled by controller.
	 * 
	 * @param parent                  A parent widget.
	 * @param controller              A controller which operates on the map.
	 * @param map                     The map.
	 * @param initial                 An override of the map's georeferencing
	 * @param allow_no_georeferencing Determines if the okay button can be
	 *                                be clicked while "- none -" is selected.
	 */
	GeoreferencingDialog(
	        QWidget* parent,
	        MapEditorController* controller,
	        Map* map,
	        const Georeferencing* initial,
	        bool allow_no_georeferencing
	);
	
public:
	/**
	 * Releases resources.
	 */
	~GeoreferencingDialog() override;
	
	
	/**
	 * Updates the dialog from georeferencing state changes.
	 */
	void georefStateChanged();
	
	/**
	  * Moves transformation properties from the georeferencing to the widgets.
	  */
	void transformationChanged();
	
	/**
	  * Moves projection properties from the georeferencing to the widgets.
	  */
	void projectionChanged();
	
	/**
	  * Updates the declination widget from the georeferencing.
	  */
	void declinationChanged();
	
	/**
	  * Updates the scale factor widget from the georeferencing.
	  */
	void auxiliaryFactorChanged();
	
	/**
	  * Sets visibility of scale compensation widgets.
	  */
	void showScaleChanged(bool checked);
	
	/**
	 * Triggers an online request for the magnetic declination.
	 * 
	 * @param no_confirm If true, the user will not be asked for confirmation.
	 */
	void requestDeclination(bool no_confirm = false);
	
	/**
	 * Sets the map coordinates of the reference point
	 */
	void setMapRefPoint(const MapCoord& coords);
	
	/**
	 * Activates the "keep projected reference point coordinates on CRS changes" radio button.
	 */
	void setKeepProjectedRefCoords();
	
	/**
	 * Activates the "keep geographic reference point coordinates on CRS changes" radio button.
	 */
	void setKeepGeographicRefCoords();
	
	/**
	 * Notifies the dialog that the active GeoreferencingTool was deleted.
	 */
	void toolDeleted();
	
	/**
	 * Opens this dialog's help page.
	 */
	void showHelp();
	
	/** 
	 * Resets all input fields to the values in the map's Georeferencing.
	 * 
	 * This will also reset initial values passed to the constructor.
	 */
	void reset();
	
	/** 
	 * Pushes the changes from the dialog to the map's Georeferencing
	 * and closes the dialog. The dialog's result is set to QDialog::Accepted,
	 * and the active exec() function will return.
	 */
	void accept() override;
	
protected:
	/**
	 * Updates enabled / disabled states of all widgets.
	 */
	void updateWidgets();
	
	/**
	 * Updates enabled / disabled state and text of the declination query button.
	 */
	void updateDeclinationButton();
	
	
	/** 
	 * Notifies the dialog of a change in the CRS configuration.
	 */
	void crsEdited();
	
	/**
	 * Notifies the dialog of a change in the auxiliary scale factor.
	 */
	void auxiliaryFactorEdited(double value);
	
	/**
	 * Updates the combined scale factor field from the underlying Georeferencing.
	 */
	void updateCombinedFactor();
	
	/**
	 * Hides the dialog and activates a GeoreferencingTool for selecting
	 * the reference point on the map.
	 */
	void selectMapRefPoint();
	
	/**
	 * Notifies the dialog of a change in the map reference point fields.
	 */
	void mapRefChanged();
	
	/**
	 * Notifies the dialog of a change in the easting / northing fields.
	 */
	void eastingNorthingEdited();
	
	/**
	 * Notifies the dialog of change of the keep-coords buttons.
	 */
	void keepCoordsChanged();
	
	/**
	 * Notifies the dialog of a change in the latitude / longitude fields.
	 */
	void latLonEdited();
	
	/** 
	 * Notifies the dialog of a change in the declination field.
	 */
	void declinationEdited(double value);
	
	/**
	 * Handles replies from the online declination service.
	 */
	void declinationReplyFinished(QNetworkReply* reply);
	
	/**
	 * Updates the grivation field from the underlying Georeferencing.
	 */
	void updateGrivation();
	
private:
	/* Internal state */
	MapEditorController* const controller;
	Map* const map;
	const Georeferencing* initial_georef;
	QScopedPointer<Georeferencing> georef; // A working copy of the current or given initial Georeferencing
	bool allow_no_georeferencing;
	bool tool_active;
	QString magcalc_service_key {};
	bool declination_query_in_progress;
	bool grivation_locked;
	bool scale_factor_locked;
	
	/* GUI elements */
	CRSSelector* crs_selector;
	QLabel* status_label;
	QLabel* status_field;
	
	QDoubleSpinBox* map_x_edit;
	QDoubleSpinBox* map_y_edit;
	QPushButton* ref_point_button;
	
	QLabel* projected_ref_label;
	QDoubleSpinBox* easting_edit;
	QDoubleSpinBox* northing_edit;
	
	QDoubleSpinBox* lat_edit;
	QDoubleSpinBox* lon_edit;
	QLabel* show_refpoint_label;
	QLabel* link_label;
	
	QRadioButton* keep_projected_radio;
	QRadioButton* keep_geographic_radio;
	
	QDoubleSpinBox* declination_edit;
	QPushButton* declination_button;
	QLabel* grivation_label;

	QCheckBox* show_scale_check;
	std::vector<QWidget*> scale_widget_list;
	QDoubleSpinBox* scale_factor_edit;
	QLabel* combined_factor_display;
	
	QDialogButtonBox* buttons_box;
	QPushButton* reset_button;
};



/** 
 * GeoreferencingTool is a helper to the GeoreferencingDialog which allows 
 * the user to select the position of the reference point on the map 
 * The GeoreferencingDialog hides when it activates this tool. The tool
 * takes care of reactivating the dialog.
 */
class GeoreferencingTool : public MapEditorTool
{
Q_OBJECT
public:
	/** 
	 * Constructs a new tool for the given dialog and controller.
	 */
	GeoreferencingTool(
	        GeoreferencingDialog* dialog,
	        MapEditorController* controller,
	        QAction* action = nullptr
	);
	
	/**
	 * Notifies the dialog that the tool is deleted.
	 */
	~GeoreferencingTool() override;
	
	/**
	 * Activates the tool.
	 */
	void init() override;
	
	/** 
	 * Consumes left and right clicks. They are handled in mouseReleaseEvent.
	 */
	bool mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	
	/** 
	 * Reacts to the user activity by sending the reference point coordinates
	 * to the dialog (on left click) and reactivating the dialog.
	 */
	bool mouseReleaseEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	
	/**
	 * Returns the mouse cursor that will be shown when the tool is active.
	 */
	const QCursor& getCursor() const override;
	
private:
	GeoreferencingDialog* const dialog;
};


}  // namespace LibreMapper

#endif // OPENORIENTEERING_GEOREFERENCING_DIALOG_H
