/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2020 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVE_MAINFORM_H
#define COVE_MAINFORM_H

#include <memory>
#include <vector>

#include <QDialog>
#include <QImage>
#include <QObject>
#include <QRgb>
#include <QString>
#include <Qt>
#include <QUndoCommand>

#include "libvectorizer/Vectorizer.h"

#include "Settings.h"
#include "ui_mainform.h"

class QPushButton;
class QWidget;
class QUndoStack;

namespace LibreMapper {
class Map;
class Template;
class TemplateImage;
} // namespace LibreMapper

namespace cove {

class mainForm : public QDialog
{
	Q_OBJECT

	struct BwBitmapUndoStep : public QUndoCommand
	{
		BwBitmapUndoStep(mainForm& form, QImage image, bool vectorizable);
		void redo() override;
		void undo() override;

		mainForm& form;
		QImage image;
		bool suitableForVectorization;
	};

public:
	Ui::mainForm ui;

protected:
	std::unique_ptr<Vectorizer> vectorizerApp;
	LibreMapper::Map* ooMap;
	LibreMapper::Template* ooTempl;
	QString imageFileName;
	QImage imageBitmap;
	QImage classifiedBitmap;
	QImage bwBitmap;
	bool bwBitmapVectorizable {};
	QUndoStack* bwBitmapUndo {};
	std::vector<QPushButton*> colorButtons;
	Settings settings;

	bool performMorphologicalOperation(Vectorizer::MorphologicalOperation mo);
	void clearColorsTab();
	void clearBWImageTab();
	void setTabEnabled(QWidget* tab, bool state);
	void afterLoadImage();
	static QRgb getColorFromImage(const QImage& image);

public:
	mainForm(QWidget* parent, LibreMapper::Map* map,
	         LibreMapper::TemplateImage* templ, Qt::WindowFlags flags = {});
	~mainForm() override;
	void loadImage(const QImage& imageToLoad, const QString& imageName);
	void clearColorButtonsGroup();
	void setColorButtonsGroup(std::vector<QRgb> colors);
	std::vector<bool> getSelectedColors();

public slots:
	void setInitialColors(bool on);
	void colorButtonToggled(bool on);

	void on_bwImageSaveButton_clicked();        // clazy:exclude=connect-by-name
	void on_classificationOptionsButton_clicked();  // clazy:exclude=connect-by-name
	void on_initialColorsButton_clicked();      // clazy:exclude=connect-by-name
	void on_howManyColorsSpinBox_valueChanged(int n);  // clazy:exclude=connect-by-name
	void on_mainTabWidget_currentChanged(int w);  // clazy:exclude=connect-by-name
	void on_setVectorizationOptionsButton_clicked();  // clazy:exclude=connect-by-name
	void on_createVectorsButton_clicked();      // clazy:exclude=connect-by-name
	void on_saveVectorsButton_clicked();        // clazy:exclude=connect-by-name
	void on_runClassificationButton_clicked();  // clazy:exclude=connect-by-name
	void on_runErosionButton_clicked();         // clazy:exclude=connect-by-name
	void on_runDilationButton_clicked();        // clazy:exclude=connect-by-name
	void on_runThinningButton_clicked();        // clazy:exclude=connect-by-name
	void on_runPruningButton_clicked();         // clazy:exclude=connect-by-name
	void on_bwImageHistoryBack_clicked();       // clazy:exclude=connect-by-name
	void on_bwImageHistoryForward_clicked();    // clazy:exclude=connect-by-name
	void on_applyFIRFilterPushButton_clicked(); // clazy:exclude=connect-by-name
};
} // cove

#endif
