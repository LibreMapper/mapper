/**
 * This file is part of LibreMapper.
 *
 * This is a modified version of a file from the Qt Toolkit.
 * You can redistribute it and/or modify it under the terms of
 * the GNU General Public License, version 3, as published by
 * the Free Software Foundation.
 *
 * OpenOrienteering is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenOrienteering.  If not, see <http://www.gnu.org/licenses/>
 *
 * Changes:
 * 2015-10-18 Kai Pastor <dg0yt@darc.de>
 * - Adjustment of legal information
 * - Modifications required for separate compilation:
 *   - Renaming of selected files, classes, members and macros
 *   - Adjustment of include statements
 *   - Removal of Q_XXX_EXPORT
 *   - Distinct paint engine type
 */
/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
****************************************************************************/

#ifndef ADVANCED_PDF_P_H
#define ADVANCED_PDF_P_H

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

#include <QtCore/qglobal.h>

#ifndef QT_NO_PDF

#include "QtGui/qmatrix.h"
#include "QtCore/qstring.h"
#include "QtCore/qvector.h"
#include <private/qstroker_p.h>
#include <private/qpaintengine_p.h>
#include <private/qfontengine_p.h>
#include "private/qfontsubset_p.h"
#include "qpagelayout.h"

QT_BEGIN_NAMESPACE

const char *qt_real_to_string(qreal val, char *buf);
const char *qt_int_to_string(int val, char *buf);

namespace AdvancedPdf {

    class ByteStream
    {
    public:
        // fileBacking means that ByteStream will buffer the contents on disk
        // if the size exceeds a certain threshold. In this case, if a byte
        // array was passed in, its contents may no longer correspond to the
        // ByteStream contents.
        explicit ByteStream(bool fileBacking = false);
        explicit ByteStream(QByteArray *ba, bool fileBacking = false);
        ~ByteStream();
        ByteStream &operator <<(char chr);
        ByteStream &operator <<(const char *str);
        ByteStream &operator <<(const QByteArray &str);
        ByteStream &operator <<(const ByteStream &src);
        ByteStream &operator <<(qreal val);
        ByteStream &operator <<(int val);
        ByteStream &operator <<(const QPointF &p);
        // Note that the stream may be invalidated by calls that insert data.
        QIODevice *stream();
        void clear();

        static inline int maxMemorySize() { return 100000000; }
        static inline int chunkSize()     { return 10000000; }

    protected:
        void constructor_helper(QIODevice *dev);
        void constructor_helper(QByteArray *ba);

    private:
        void prepareBuffer();

    private:
        QIODevice *dev;
        QByteArray ba;
        bool fileBackingEnabled;
        bool fileBackingActive;
        bool handleDirty;
    };

    enum PathFlags {
        ClipPath,
        FillPath,
        StrokePath,
        FillAndStrokePath
    };
    QByteArray generatePath(const QPainterPath &path, const QTransform &matrix, PathFlags flags);
    QByteArray generateMatrix(const QTransform &matrix);
    QByteArray generateDashes(const QPen &pen);
    QByteArray patternForBrush(const QBrush &b);

    struct Stroker {
        Stroker();
        void setPen(const QPen &pen, QPainter::RenderHints hints);
        void strokePath(const QPainterPath &path);
        ByteStream *stream;
        bool first;
        QTransform matrix;
        bool cosmeticPen;
    private:
        QStroker basicStroker;
        QDashStroker dashStroker;
        QStrokerOps *stroker;
    };

    QByteArray ascii85Encode(const QByteArray &input);

    const char *toHex(ushort u, char *buffer);
    const char *toHex(uchar u, char *buffer);

}


class AdvancedPdfPage : public AdvancedPdf::ByteStream
{
public:
    AdvancedPdfPage();

    QVector<uint> images;
    QVector<uint> graphicStates;
    QVector<uint> patterns;
    QVector<uint> fonts;
    QVector<uint> annotations;

    void streamImage(int w, int h, int object);

    QSize pageSize;
private:
};

class AdvancedPdfWriter;
class AdvancedPdfEnginePrivate;

class AdvancedPdfEngine : public QPaintEngine
{
    Q_DECLARE_PRIVATE(AdvancedPdfEngine)
    friend class AdvancedPdfWriter;
public:
    static struct PaintEngineTypeStruct
    {
        constexpr operator QPaintEngine::Type() const {
            return QPaintEngine::Type(QPaintEngine::User + 1);
        }
    } PaintEngineType;

    AdvancedPdfEngine();
    AdvancedPdfEngine(AdvancedPdfEnginePrivate &d);
    ~AdvancedPdfEngine() override {}

    void setOutputFilename(const QString &filename);

    void setResolution(int resolution);
    int resolution() const;

    // reimplementations QPaintEngine
    bool begin(QPaintDevice *pdev) Q_DECL_OVERRIDE;
    bool end() Q_DECL_OVERRIDE;

    void drawPoints(const QPointF *points, int pointCount) Q_DECL_OVERRIDE;
    void drawLines(const QLineF *lines, int lineCount) Q_DECL_OVERRIDE;
    void drawRects(const QRectF *rects, int rectCount) Q_DECL_OVERRIDE;
    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode) Q_DECL_OVERRIDE;
    void drawPath (const QPainterPath & path) Q_DECL_OVERRIDE;

    void drawTextItem(const QPointF &p, const QTextItem &textItem) Q_DECL_OVERRIDE;

    void drawPixmap (const QRectF & rectangle, const QPixmap & pixmap, const QRectF & sr) Q_DECL_OVERRIDE;
    void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,
                   Qt::ImageConversionFlags flags = Qt::AutoColor) Q_DECL_OVERRIDE;
    void drawTiledPixmap (const QRectF & rectangle, const QPixmap & pixmap, const QPointF & point) Q_DECL_OVERRIDE;

    void updateState(const QPaintEngineState &state) Q_DECL_OVERRIDE;

    int metric(QPaintDevice::PaintDeviceMetric metricType) const;
    Type type() const Q_DECL_OVERRIDE;
    // end reimplementations QPaintEngine

    // Printer stuff...
    bool newPage();

    // Page layout stuff
    void setPageLayout(const QPageLayout &pageLayout);
    void setPageSize(const QPageSize &pageSize);
    void setPageOrientation(QPageLayout::Orientation orientation);
    void setPageMargins(const QMarginsF &margins, QPageLayout::Unit units = QPageLayout::Point);

    QPageLayout pageLayout() const;

    void setPen();
    void setBrush();
    void setupGraphicsState(QPaintEngine::DirtyFlags flags);

private:
    void updateClipPath(const QPainterPath & path, Qt::ClipOperation op);
};

class AdvancedPdfEnginePrivate : public QPaintEnginePrivate
{
    Q_DECLARE_PUBLIC(AdvancedPdfEngine)
public:
    AdvancedPdfEnginePrivate();
    ~AdvancedPdfEnginePrivate() override;

    inline uint requestObject() { return currentObject++; }

    void writeHeader();
    void writeTail();

    int addImage(const QImage &image, bool *bitmap, qint64 serial_no);
    int addConstantAlphaObject(int brushAlpha, int penAlpha = 255);
    int addBrushPattern(const QTransform &matrix, bool *specifyColor, int *gStateObject);

    void drawTextItem(const QPointF &p, const QTextItemInt &ti);

    QTransform pageMatrix() const;

    void newPage();

    int currentObject;

    AdvancedPdfPage* currentPage;
    AdvancedPdf::Stroker stroker;

    QPointF brushOrigin;
    QBrush brush;
    QPen pen;
    QList<QPainterPath> clips;
    bool clipEnabled;
    bool allClipped;
    bool hasPen;
    bool hasBrush;
    bool simplePen;
    qreal opacity;

    QHash<QFontEngine::FaceId, QFontSubset *> fonts;

    QPaintDevice *pdev;

    // the device the output is in the end streamed to.
    QIODevice *outDevice;
    bool ownsDevice;

    // printer options
    QString outputFileName;
    QString title;
    QString creator;
    bool embedFonts;
    int resolution;
    bool grayscale;

    // Page layout: size, orientation and margins
    QPageLayout m_pageLayout;

private:
    int gradientBrush(const QBrush &b, const QTransform &matrix, int *gStateObject);
    int generateGradientShader(const QGradient *gradient, const QTransform &matrix, bool alpha = false);
    int generateLinearGradientShader(const QLinearGradient *lg, const QTransform &matrix, bool alpha);
    int generateRadialGradientShader(const QRadialGradient *gradient, const QTransform &matrix, bool alpha);
    int createShadingFunction(const QGradient *gradient, int from, int to, bool reflect, bool alpha);

    void writeInfo();
    void writePageRoot();
    void writeFonts();
    void embedFont(QFontSubset *font);

    QVector<int> xrefPositions;
    QDataStream* stream;
    int streampos;

    int writeImage(const QByteArray &data, int width, int height, int depth,
                   int maskObject, int softMaskObject, bool dct = false);
    void writePage();

    int addXrefEntry(int object, bool printostr = true);
    void printString(const QString &string);
    void xprintf(const char* fmt, ...);
    inline void write(const QByteArray &data) {
        stream->writeRawData(data.constData(), data.size());
        streampos += data.size();
    }

    int writeCompressed(const char *src, int len);
    inline int writeCompressed(const QByteArray &data) { return writeCompressed(data.constData(), data.length()); }
    int writeCompressed(QIODevice *dev);

    // various PDF objects
    int pageRoot, catalog, info, graphicsState, patternColorSpace;
    QVector<uint> pages;
    QHash<qint64, uint> imageCache;
    QHash<QPair<uint, uint>, uint > alphaCache;
};

QT_END_NAMESPACE

#endif // QT_NO_PDF

#endif // ADVANCED_PDF_P_H

