/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_AUTOSAVE_DIALOG_H
#define LIBREMAPPER_AUTOSAVE_DIALOG_H

#include <Qt>
#include <QDialog>
#include <QObject>
#include <QString>
#include <QTextDocument>

#include "util/item_delegates.h"

class QBoxLayout;
class QCloseEvent;
class QListWidget;
class QModelIndex;

namespace OpenOrienteering {

class MainWindow;


/**
 * @brief A dialog for selection of an autosaved file vs. a user-saved file.
 * 
 * This dialog may be used a modal dialog (via exec()) or as a non-modal dialog
 * (via show()).
 */
class AutosaveDialog : public QDialog, public TextDocItemDelegate::Provider
{
Q_OBJECT
public:
	/**
	 * @brief Constructs the dialog.
	 * 
	 * @param original_path  The path of the file which was originally saved by the user.
	 * @param autosave_path The path of the file which was autosaved.
	 * @param actual_path    The path which is currently selected.
	 * @param parent         The parent window.
	 */
	AutosaveDialog(const QString& original_path, const QString& autosave_path, const QString& actual_path, MainWindow* parent = nullptr, Qt::WindowFlags f = {});
	
	/**
	 * Destructor.
	 */
	~AutosaveDialog() override;
	
	/**
	 * @brief Returns the currently selected path.
	 */
	QString selectedPath() const;
	
	/**
	 * @brief Provides the text documents for the list widget items.
	 * 
	 * @param index The model index for which the text documents is requested for.
	 * @return      A QTextDocument representing the list item, or nullptr.
	 */
	const QTextDocument* textDoc(const QModelIndex& index) const override;
	
public slots:
	/**
	 * @brief Shows this dialog as a modal dialog.
	 * 
	 * @override
	 * 
	 * @return The result (QDialog::DialogCode).
	 */
	int exec() override;
	
	/**
	 * @brief Sets the selected item to the one representing the given path.
	 * 
	 * If the path does not match either item, nothing is selected.
	 */
	void setSelectedPath(const QString& path);
	
	/**
	 * @brief Informs the dialog that the conflict is resolved.
	 * 
	 * This will close a non-modal dialog.
	 */
	void autosaveConflictResolved();
	
signals:
	/**
	 * @brief This signal is emitted when the user selects another item.
	 * 
	 * @param path The path which belongs to the newly selected item.
	 */
	void pathSelected(const QString& path);
	
protected:
	/**
	 * @brief Ignores the QCloseEvent.
	 * 
	 * @override
	 */
	void closeEvent(QCloseEvent* event) override;
	
private slots:
	void currentRowChanged(int row);
	
private:
	MainWindow* const main_window;
	
	const QString original_path;
	const QString autosave_path;
	
	bool resolved;
	
	QTextDocument autosaved_text;
	QTextDocument user_saved_text;
	
	QBoxLayout* layout;
	QListWidget* list_widget;
};


}  // namespace OpenOrienteering

#endif // LIBREMAPPER_AUTOSAVE_DIALOG_H
