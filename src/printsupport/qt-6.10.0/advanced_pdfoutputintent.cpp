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

#include "advanced_pdfoutputintent.h"

#ifndef QT_NO_PDF

#include <QtCore/qfile.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qstring.h>
#include <QtCore/qurl.h>

#include <QtGui/qcolorspace.h>

QT_BEGIN_NAMESPACE

class AdvancedPdfOutputIntentPrivate : public QSharedData
{
public:
    AdvancedPdfOutputIntentPrivate()
    {
        QFile colorProfileFile(QStringLiteral(":/advanced_pdf/sRGB2014.icc"));
        bool ok = colorProfileFile.open(QIODevice::ReadOnly);
        Q_ASSERT(ok);
        outputProfile = QColorSpace::fromIccProfile(colorProfileFile.readAll());
    }

    QString outputConditionIdentifier = QStringLiteral("sRGB_IEC61966-2-1_black_scaled");
    QString outputCondition = QStringLiteral("sRGB IEC61966 v2.1 with black scaling");
    QUrl registryName = QStringLiteral("http://www.color.org");
    QColorSpace outputProfile;
};

/*!
    \class AdvancedPdfOutputIntent
    \inmodule QtGui
    \ingroup painting
    \since 6.8

    The AdvancedPdfOutputIntent class contains metadata that characterize
    the printing condition for which painting data has been prepared
    when generating a PDF file.

    \sa AdvancedPdfWriter
*/

/*!
    Constructs a new PDF output intent.
*/
AdvancedPdfOutputIntent::AdvancedPdfOutputIntent()
    : d(new AdvancedPdfOutputIntentPrivate)
{}

/*!
    Constructs a copy of the output intent \a other.
*/
AdvancedPdfOutputIntent::AdvancedPdfOutputIntent(const AdvancedPdfOutputIntent &other) = default;

/*!
    \fn AdvancedPdfOutputIntent::AdvancedPdfOutputIntent(AdvancedPdfOutputIntent &&other) noexcept

    Constructs a AdvancedPdfOutputIntent object by moving from \a other.
*/

/*!
    Assigns the output intent \a other over this intent.
*/
AdvancedPdfOutputIntent &AdvancedPdfOutputIntent::operator=(const AdvancedPdfOutputIntent &other) = default;

/*!
    \fn AdvancedPdfOutputIntent &AdvancedPdfOutputIntent::operator=(AdvancedPdfOutputIntent &&other) noexcept

    Move-assigns the output intent \a other over this intent.
*/

/*!
    Destroys this output intent.
*/
AdvancedPdfOutputIntent::~AdvancedPdfOutputIntent() = default;

/*!
    \fn void AdvancedPdfOutputIntent::swap(AdvancedPdfOutputIntent &other) noexcept
    \memberswap{output intent}
*/

/*!
    Returns the identifier of the output condition.

    If a registry name is provided, then this identifier should should
    match the reference name of an entry in that registry.

    The default identifier is \c{sRGB_IEC61966-2-1_black_scaled}.

    \sa setOutputConditionIdentifier()
*/
QString AdvancedPdfOutputIntent::outputConditionIdentifier() const
{
    return d->outputConditionIdentifier;
}

/*!
    Sets the identifier of the output condition to \a identifier.

    If a registry name is provided, then this identifier should should
    match the reference name of an entry in that registry.

    \sa setOutputCondition(), setRegistryName()
*/
void AdvancedPdfOutputIntent::setOutputConditionIdentifier(const QString &identifier)
{
    d.detach();
    d->outputConditionIdentifier = identifier;
}

/*!
    Returns the human-readable output condition.

    This is a string that concisely identifies the characterized
    printing condition in a form that will be meaningful to a
    human operator.

    The default output condition is
    \c{sRGB IEC61966 v2.1 with black scaling}.

    \sa setOutputCondition()
*/
QString AdvancedPdfOutputIntent::outputCondition() const
{
    return d->outputCondition;
}

/*!
    Sets the human-readable output condition to \a condition.

    \sa setOutputConditionIdentifier(), setRegistryName()
*/
void AdvancedPdfOutputIntent::setOutputCondition(const QString &condition)
{
    d.detach();
    d->outputCondition = condition;
}

/*!
    Returns the URL of a characterization registry for the intended
    printing condition.

    The default registry is \c{http://www.color.org}.

    \sa setOutputConditionIdentifier()
*/
QUrl AdvancedPdfOutputIntent::registryName() const
{
    return d->registryName;
}

/*!
    Sets the URL of the characterization registry to \a name.

    \sa setOutputConditionIdentifier()
*/
void AdvancedPdfOutputIntent::setRegistryName(const QUrl &name)
{
    d.detach();
    d->registryName = name;
}

/*!
    Returns the output device profile.

    The default profile is the sRGB v2 profile available
    from the
    \l{https://www.color.org/srgbprofiles.xalter#v2}{International
    Color Consortium}.
*/
QColorSpace AdvancedPdfOutputIntent::outputProfile() const
{
    return d->outputProfile;
}

/*!
    Sets the output device profile to \a profile.

    \note PDF/X-4 requires all the color specifications in the
    document to match the same colorspace of \a profile. It is
    the application's responsibility to ensure this is the case.

    \sa QColorSpace::fromIccProfile, AdvancedPdfWriter::setColorModel
*/
void AdvancedPdfOutputIntent::setOutputProfile(const QColorSpace &profile)
{
    d.detach();
    d->outputProfile = profile;
}

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(AdvancedPdfOutputIntentPrivate)

QT_END_NAMESPACE

#endif // QT_NO_PDF
