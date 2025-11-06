/* SPDX-License-Identifier: GPL-3.0-only
 *
 * This file is part of LibreMapper.
 *
 * Changes:
 * 2015 Kai Pastor <dg0yt@darc.de> (OpenOrienteering)
 * 2025-11-06 Libor Pecháček <lpechacek@gmx.com>
 * - Adjustment of legal information
 * - Modifications required for separate compilation:
 *   - Renaming of selected files, classes, members and macros
 *   - Adjustment of include statements
 *   - Removal of Q_XXX_EXPORT
 */
// Copyright (C) 2024 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Giuseppe D'Angelo <giuseppe.dangelo@kdab.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef ADVANCED_PDFOUTPUTINTENT_H
#define ADVANCED_PDFOUTPUTINTENT_H

#include <QtGui/qtguiglobal.h>

#ifndef QT_NO_PDF

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QString;
class QUrl;
class QColorSpace;

class AdvancedPdfOutputIntentPrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR_WITH_EXPORT(AdvancedPdfOutputIntentPrivate, Q_GUI_EXPORT)

class AdvancedPdfOutputIntent
{
public:
    AdvancedPdfOutputIntent();
    AdvancedPdfOutputIntent(const AdvancedPdfOutputIntent &other);
    AdvancedPdfOutputIntent(AdvancedPdfOutputIntent &&other) noexcept = default;
    AdvancedPdfOutputIntent &operator=(const AdvancedPdfOutputIntent &other);
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(AdvancedPdfOutputIntent)
    ~AdvancedPdfOutputIntent();

    void swap(AdvancedPdfOutputIntent &other) noexcept { d.swap(other.d); }

    QString outputConditionIdentifier() const;
    void setOutputConditionIdentifier(const QString &identifier);

    QString outputCondition() const;
    void setOutputCondition(const QString &condition);

    QUrl registryName() const;
    void setRegistryName(const QUrl &name);

    QColorSpace outputProfile() const;
    void setOutputProfile(const QColorSpace &profile);

private:
    QExplicitlySharedDataPointer<AdvancedPdfOutputIntentPrivate> d;
};

Q_DECLARE_SHARED(AdvancedPdfOutputIntent)

QT_END_NAMESPACE

#endif // QT_NO_PDF

#endif // ADVANCED_PDFOUTPUTINTENT_H
