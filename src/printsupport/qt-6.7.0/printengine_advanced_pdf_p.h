/* SPDX-License-Identifier: GPL-3.0-only
 *
 * This file is part of LibreMapper.
 *
 * Changes:
 * 2024-04-17 Kai Pastor <dg0yt@darc.de> (OpenOrienteering)
 * - Adjustment of legal information
 * - Modifications required for separate compilation:
 *   - Renaming of selected files, classes, members and macros
 *   - Adjustment of include statements
 *   - Removal of Q_XXX_EXPORT
 */
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef PRINTENGINE_ADVANCED_PDF_P_H
#define PRINTENGINE_ADVANCED_PDF_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtPrintSupport/qprintengine.h"

#ifndef QT_NO_PRINTER
#include "QtCore/qdatastream.h"
#include "QtCore/qmap.h"
#include "QtCore/qstring.h"
#include "QtGui/qpaintengine.h"
#include "QtGui/qpainterpath.h"

#include <private/qfontengine_p.h>
#include "advanced_pdf_p.h"
#include <private/qpaintengine_p.h>
#include "qprintengine.h"
#include <private/qprint_p.h>

QT_BEGIN_NAMESPACE

class QImage;
class QDataStream;
class QPen;
class QPointF;
class QRegion;
class QFile;

class AdvancedPdfPrintEnginePrivate;

class AdvancedPdfPrintEngine : public AdvancedPdfEngine, public QPrintEngine
{
    Q_DECLARE_PRIVATE(AdvancedPdfPrintEngine)
public:
    AdvancedPdfPrintEngine(QPrinter::PrinterMode m, AdvancedPdfEngine::PdfVersion version = AdvancedPdfEngine::Version_1_4);
    virtual ~AdvancedPdfPrintEngine();

    // reimplementations QPaintEngine
    bool begin(QPaintDevice *pdev) override;
    bool end() override;
    // end reimplementations QPaintEngine

    // reimplementations QPrintEngine
    bool abort() override {return false;}
    QPrinter::PrinterState printerState() const override {return state;}

    bool newPage() override;
    int metric(QPaintDevice::PaintDeviceMetric) const override;
    virtual void setProperty(PrintEnginePropertyKey key, const QVariant &value) override;
    virtual QVariant property(PrintEnginePropertyKey key) const override;
    // end reimplementations QPrintEngine

    QPrinter::PrinterState state;

protected:
    AdvancedPdfPrintEngine(AdvancedPdfPrintEnginePrivate &p);

private:
    Q_DISABLE_COPY(AdvancedPdfPrintEngine)
};

class AdvancedPdfPrintEnginePrivate : public AdvancedPdfEnginePrivate
{
    Q_DECLARE_PUBLIC(AdvancedPdfPrintEngine)
public:
    AdvancedPdfPrintEnginePrivate(QPrinter::PrinterMode m);
    ~AdvancedPdfPrintEnginePrivate();

    virtual bool openPrintDevice();
    virtual void closePrintDevice();

private:
    Q_DISABLE_COPY(AdvancedPdfPrintEnginePrivate)

    friend class QCupsPrintEngine;
    friend class QCupsPrintEnginePrivate;

    QString printerName;
    QString printProgram;
    QString selectionOption;

    bool collate;
    int copies;
    QPrinter::PageOrder pageOrder;
    QPrinter::PaperSource paperSource;

    int fd;
};

QT_END_NAMESPACE

#endif // QT_NO_PRINTER

#endif // PRINTENGINE_ADVANCED_PDF_P_H
