/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 31st, 2016
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#ifndef FINFIGURECONFIG_H
#define FINFIGURECONFIG_H

#include <QString>
#include <QSizeF>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QMarginsF>

#include "finErrorCode.h"
#include "finFigureArrow.h"


/*! \class finFigureConfig
 *  \brief Stores the drawing and text appearance of a figure object.
 *
 *  The configuration owns border and fill styles, arrowheads at both line ends, and the pen, brush,
 *  font, and margins used for text. A process-wide default instance supplies initial settings.
 */
class finFigureConfig
{
protected:
    QPen _borderPen;            //!< Pen used to draw figure borders and lines.
    QBrush _fillBrush;          //!< Brush used to fill closed figure paths.
    finFigureArrow _startArrow; //!< Arrowhead drawn at the start of a line.
    finFigureArrow _endArrow;   //!< Arrowhead drawn at the end of a line.

    QPen _textPen;              //!< Pen used to draw text outlines.
    QBrush _textBrush;          //!< Brush used to fill text glyphs.
    QFont _font;                //!< Font used to render figure text.
    QMarginsF _textMargins;     //!< Margins around figure text.

    static finFigureConfig *_defFigCfg;  //!< Lazily allocated process-wide default configuration.

public:
    /*! \brief Constructs a configuration with the standard border, fill, arrow, and text styles. */
    finFigureConfig();

    /*! \brief Copies all figure style properties to \a outcfg. */
    void cloneFigureConfig(finFigureConfig *outcfg) const;

    /*! \name Direct Style Accessors
     *  \brief Read the Qt style objects and arrow descriptors held by this configuration.
     */
    ///@{
    /*! \brief Returns the border pen. */
    const QPen &getBorderPen() const;
    /*! \brief Returns the fill brush. */
    const QBrush &getFillBrush() const;
    /*! \brief Returns the start arrow descriptor. */
    const finFigureArrow &getStartArrow() const;
    /*! \brief Returns the end arrow descriptor. */
    const finFigureArrow &getEndArrow() const;
    /*! \brief Returns the text outline pen. */
    const QPen &getTextPen() const;
    /*! \brief Returns the text fill brush. */
    const QBrush &getTextBrush() const;
    /*! \brief Returns the text font. */
    const QFont &getFont() const;
    /*! \brief Returns the margins around figure text. */
    const QMarginsF &getTextMargins() const;
    ///@}

    /*! \name Convenience Style Accessors
     *  \brief Read individual border, arrow, and text properties.
     */
    ///@{
    /*! \brief Returns the border pen width. */
    double getDotSize() const;
    /*! \brief Returns the border color. */
    QColor getBorderColor() const;
    /*! \brief Returns the fill color. */
    QColor getFillColor() const;
    /*! \brief Returns the start arrowhead type. */
    finFigureArrowType getStartArrowType() const;
    /*! \brief Returns the start arrowhead length. */
    double getStartArrowSize() const;
    /*! \brief Returns the start arrowhead opening angle in radians. */
    double getStartArrowRadian() const;
    /*! \brief Returns the end arrowhead type. */
    finFigureArrowType getEndArrowType() const;
    /*! \brief Returns the end arrowhead length. */
    double getEndArrowSize() const;
    /*! \brief Returns the end arrowhead opening angle in radians. */
    double getEndArrowRadian() const;

    /*! \brief Returns the text font family name. */
    QString getFontName() const;
    /*! \brief Returns the text font point size. */
    double getFontPointSize() const;
    /*! \brief Returns whether the text font is bold. */
    bool getFontBold() const;
    /*! \brief Returns whether the text font is italic. */
    bool getFontItalic() const;
    /*! \brief Returns the text fill color. */
    QColor getFontColor() const;
    ///@}

    /*! \name Direct Style Mutators
     *  \brief Replace complete Qt style objects and arrow descriptors.
     */
    ///@{
    /*! \brief Replaces the border pen. */
    void setBorderPen(const QPen &pen);
    /*! \brief Replaces the fill brush. */
    void setFillBrush(const QBrush &brush);
    /*! \brief Replaces the start arrow descriptor. */
    void setStartArrow(const finFigureArrow &arrow);
    /*! \brief Replaces the end arrow descriptor. */
    void setEndArrow(const finFigureArrow &arrow);
    /*! \brief Replaces the text outline pen. */
    void setTextPen(const QPen &pen);
    /*! \brief Replaces the text fill brush. */
    void setTextBrush(const QBrush &brush);
    /*! \brief Replaces the text font. */
    void setFont(const QFont &font);
    /*! \brief Replaces the margins around figure text. */
    void setTextMargins(const QMarginsF &margins);
    ///@}

    /*! \name Convenience Style Mutators
     *  \brief Update individual border, arrow, and text properties.
     */
    ///@{
    /*! \brief Sets the positive border pen width. */
    void setDotSize(double size);
    /*! \brief Sets the border color. */
    void setBorderColor(const QColor &color);
    /*! \brief Sets the fill color. */
    void setFillColor(const QColor &color);
    /*! \brief Sets the start arrowhead type. */
    void setStartArrowType(finFigureArrowType type);
    /*! \brief Sets the start arrowhead length. */
    void setStartArrowSize(double size);
    /*! \brief Sets the start arrowhead opening angle in radians. */
    void setStartArrowRadian(double rad);
    /*! \brief Sets the end arrowhead type. */
    void setEndArrowType(finFigureArrowType type);
    /*! \brief Sets the end arrowhead length. */
    void setEndArrowSize(double size);
    /*! \brief Sets the end arrowhead opening angle in radians. */
    void setEndArrowRadian(double rad);

    /*! \brief Sets the text font family. */
    void setFontName(const QString &ftname);
    /*! \brief Sets the text font point size. */
    void setFontPointSize(double size);
    /*! \brief Enables or disables bold text. */
    void setFontBold(bool bold);
    /*! \brief Enables or disables italic text. */
    void setFontItalic(bool bold);
    /*! \brief Sets the text fill color. */
    void setFontColor(const QColor &color);
    ///@}

    /*! \name Default Configuration
     *  \brief Access and copy the process-wide default figure style.
     */
    ///@{
    /*! \brief Returns the lazily created default figure configuration. */
    static finFigureConfig *getDefaultFigureConfig();
    /*! \brief Copies the default figure configuration to \a outfig. */
    static void cloneFromDefaultFigureConfig(finFigureConfig *outfig);
    ///@}
};

#endif // FINFIGURECONFIG_H
