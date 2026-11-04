/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finGraphTrans.h
 *  \brief Declaration of the optional script-coordinate transforms used by finGraphConfig.
 *
 *  A transform maps script coordinates to script coordinates before they are converted into pixels,
 *  which allows axis zooming or a sequence of affine operations to deform the whole figure. The
 *  hierarchy contains an identity base class, a per-axis scaling transform, and an affine transform
 *  built from an ordered list of rotate, scale and translate actions.
 */

#ifndef FINGRAPHTRANS_H
#define FINGRAPHTRANS_H

#include <QPointF>
#include <QTransform>
#include <QComboBox>

#include "finErrorCode.h"


/*! \class finGraphTrans
 *  \brief Identity transform and the polymorphic interface of all graph transforms.
 *
 *  finGraphConfig owns one instance created by setTransformType() and calls transPoint() while
 *  mapping script coordinates to pixels, and arcTransPoint() while mapping them back. The base
 *  class itself performs no mapping at all; it only carries the type tag used to tell the concrete
 *  subclasses apart. Overrides must therefore be mutually consistent: arcTransPoint() has to undo
 *  transPoint().
 */
class finGraphTrans
{
public:
    /*! \brief Concrete kind of a transform, as reported by getTransformType(). */
    enum Type {
        TP_NONE,   //!< No transform; points pass through unchanged.
        TP_RECT,   //!< Independent per-axis scaling, see finGraphTransRect.
        TP_AFFINE, //!< Ordered rotate/scale/translate sequence, see finGraphTransAffine.
    };

    /*! \name Type Names for the UI
     *  \brief Convert between a transform type and its textual name used by combo box items.
     */
    ///@{
    /*! \brief Returns the textual name of \a type ("none", "rect" or "affine"). */
    static QString getTransformTypeName(Type type);
    /*! \brief Parses \a name case-insensitively; unknown names yield TP_NONE. */
    static Type parseTransformType(const QString &name);
    /*! \brief Appends one item per transform type to \a cmbox.
     *
     *  \exception finException EC_NULL_POINTER when \a cmbox is nullptr.
     */
    static void fillTypesInComboBox(QComboBox *cmbox);
    ///@}

    /*! \name Combo Box Selection
     *  \brief Locate or select the combo box item that represents a transform type.
     */
    ///@{
    /*! \brief Returns the item index whose data matches \a type, or -1 when there is none.
     *
     *  A nullptr \a cmbox also yields -1 instead of throwing.
     */
    static int getComboBoxIndexForType(QComboBox *cmbox, Type type);
    /*! \brief Selects the item representing \a type in \a cmbox.
     *
     *  \exception finException EC_NOT_FOUND when \a cmbox has no matching item.
     */
    static void setComboBoxCurrentItemToType(QComboBox *cmbox, Type type);
    ///@}

protected:
    Type _type;     //!< Concrete transform kind, set by the constructor of each subclass.

public:
    /*! \brief Constructs an identity transform of type TP_NONE. */
    finGraphTrans();
    /*! \brief Destroys the transform; the base class owns no resources. */
    virtual ~finGraphTrans();

    /*! \brief Copies the parameters of \a trans into this transform.
     *
     *  Subclasses call this implementation first, which only validates the types, and then copy
     *  their own fields. The implementation itself copies nothing.
     *
     *  \exception finException EC_INVALID_PARAM when \a trans is nullptr or of a different type.
     */
    virtual void cloneTransform(const finGraphTrans *trans);
    /*! \brief Returns the concrete transform kind. */
    Type getTransformType() const;
    /*! \brief Returns whether the mapping is linear.
     *
     *  The base class always reports true, and no subclass overrides it, so every existing
     *  transform currently counts as linear.
     */
    virtual bool isLinear() const;

    /*! \brief Maps a script point forward; the base class returns \a ptr unchanged. */
    virtual QPointF transPoint(const QPointF &ptr);
    /*! \brief Maps a script point back; the base class returns \a ptr unchanged. */
    virtual QPointF arcTransPoint(const QPointF &ptr);
};

/*! \brief Short alias of finGraphTrans::Type, used throughout the graph and exec layers. */
typedef finGraphTrans::Type finGraphTransType;

/*! \class finGraphTransRect
 *  \brief Scales the two script axes independently by the configured zoom factors.
 *
 *  The mapping is a plain per-axis multiplication, so x and y are stretched or shrunk without any
 *  rotation or translation. Both zoom factors default to 1.0 and must stay positive.
 */
class finGraphTransRect : public finGraphTrans
{
protected:
    double _axisZoomX, _axisZoomY;  //!< Zoom factors applied to the x and y coordinates.

public:
    /*! \brief Constructs a transform with both zoom factors set to 1.0. */
    finGraphTransRect();
    /*! \brief Destroys the transform; it owns no resources. */
    virtual ~finGraphTransRect();

    /*! \brief Copies both zoom factors from \a trans.
     *
     *  \exception finException EC_INVALID_PARAM when \a trans is nullptr or not of type TP_RECT.
     */
    virtual void cloneTransform(const finGraphTrans *trans);
    /*! \brief Returns true, since per-axis scaling is a linear mapping. */
    virtual bool isLinear() const;

    /*! \name Axis Zoom
     *  \brief Read or set the zoom factor of each script axis.
     */
    ///@{
    /*! \brief Returns the zoom factor of the x axis. */
    double getAxisZoomX() const;
    /*! \brief Returns the zoom factor of the y axis. */
    double getAxisZoomY() const;
    /*! \brief Sets the zoom factor of the x axis.
     *
     *  \exception finException EC_INVALID_PARAM when \a zoomx is not positive.
     */
    void setAxisZoomX(double zoomx);
    /*! \brief Sets the zoom factor of the y axis.
     *
     *  \exception finException EC_INVALID_PARAM when \a zoomy is not positive.
     */
    void setAxisZoomY(double zoomy);
    ///@}

    /*! \brief Multiplies each coordinate by the zoom factor of its axis. */
    virtual QPointF transPoint(const QPointF &ptr);
    /*! \brief Divides each coordinate by the zoom factor of its axis. */
    virtual QPointF arcTransPoint(const QPointF &ptr);
};

/*! \class finGraphTransAffine
 *  \brief Applies an ordered sequence of rotate, scale and translate actions.
 *
 *  Every appended action is immediately merged into the accumulated QTransform matrix, and the
 *  inverted matrix is recomputed at the same time so that arcTransPoint() stays consistent with
 *  transPoint(). The actions are also recorded in a list, which the graph configuration dialog
 *  displays and edits; the list is kept in application order.
 */
class finGraphTransAffine : public finGraphTrans
{
public:
    /*! \brief Kind of one entry in the affine action list. */
    enum ActionType {
        AT_DUMMY,     //!< Placeholder for an unknown or uninitialized action.
        AT_ROTATE,    //!< Rotation by one radian argument.
        AT_SCALE,     //!< Scaling by an x and a y factor.
        AT_TRANSLATE  //!< Translation by an x and a y offset.
    };

    /*! \brief Interpretation of the arguments of one action. */
    enum ActArgType {
        AAT_NONE,   //!< The action takes no arguments.
        AAT_RADIAN, //!< The single argument is an angle in radians.
        AAT_XY,     //!< The two arguments are an x and a y component.
    };

    /*! \name Action Names for the UI
     *  \brief Convert between an action type and its textual name used by combo box items.
     */
    ///@{
    /*! \brief Returns the textual name of \a type ("rotate", "scale", "translate" or "none"). */
    static QString getAffineTransActionName(ActionType type);
    /*! \brief Parses \a name case-insensitively; unknown names yield AT_DUMMY. */
    static ActionType parseAffineTransAction(const QString &name);
    /*! \brief Appends one item per supported action to \a cmbox; AT_DUMMY is not offered.
     *
     *  \exception finException EC_NULL_POINTER when \a cmbox is nullptr.
     */
    static void fillAffineTransActionsInComboBox(QComboBox *cmbox);
    ///@}

    /*! \name Action Metadata
     *  \brief Describe how many arguments an action takes and how they must be read.
     */
    ///@{
    /*! \brief Returns the argument count of \a type: 1 for rotate, 2 for scale and translate. */
    static int getAffineTransActionArgCnt(ActionType type);
    /*! \brief Returns the argument interpretation of \a type. */
    static ActArgType getAffineTransActionArgType(ActionType type);
    ///@}

    /*! \brief One recorded action together with its arguments.
     *
     *  For AT_ROTATE both arguments hold the angle in radians; only _arg1 is meaningful.
     */
    struct Action {
        ActionType _type;   //!< Kind of the action.
        double _arg1, _arg2; //!< Action arguments; for rotate _arg2 merely repeats _arg1.
    };

protected:
    QTransform _matrix;     //!< Accumulated forward matrix of all appended actions.
    QTransform _invMatrix;  //!< Inverse of _matrix, or identity when _matrix is not invertible.
    QList<Action> _actList; //!< Actions in application order, kept for display and editing.

public:
    /*! \brief Constructs a transform with identity matrices and an empty action list. */
    finGraphTransAffine();
    /*! \brief Destroys the transform; it owns no resources. */
    virtual ~finGraphTransAffine();

    /*! \brief Copies the action list and both matrices from \a trans.
     *
     *  \exception finException EC_INVALID_PARAM when \a trans is nullptr or not of type TP_AFFINE.
     */
    virtual void cloneTransform(const finGraphTrans *trans);
    /*! \brief Returns true, since every recorded action is an affine mapping. */
    virtual bool isLinear() const;

    /*! \brief Returns the accumulated forward matrix. */
    QTransform getTransformMatrix() const;
    /*! \brief Returns the accumulated inverse matrix. */
    QTransform getInvertedTransformMatrix() const;

    /*! \brief Returns the number of recorded actions. */
    int getActionCount() const;
    /*! \brief Returns the action at \a idx.
     *
     *  An out-of-range index yields a zeroed AT_ROTATE action rather than AT_DUMMY, so callers
     *  should compare the index against getActionCount() instead of testing the returned type.
     */
    Action getActionAt(int idx) const;

    /*! \brief Clears the action list and resets both matrices to identity. */
    void reset();
    /*! \brief Appends a rotation by \a rad radians and updates the matrices. */
    void appendRotate(double rad);
    /*! \brief Appends a scaling by \a sx and \a sy and updates the matrices. */
    void appendScale(double sx, double sy);
    /*! \brief Appends a translation by \a tx and \a ty and updates the matrices. */
    void appendTranslate(double tx, double ty);

    /*! \brief Maps a script point through the forward matrix. */
    virtual QPointF transPoint(const QPointF &ptr);
    /*! \brief Maps a script point through the inverse matrix. */
    virtual QPointF arcTransPoint(const QPointF &ptr);

 private:
    /*! \brief Recomputes _invMatrix from _matrix, falling back to identity when not invertible. */
    void calcInvertedMatrix();
};

#endif // FINGRAPHTRANS_H
