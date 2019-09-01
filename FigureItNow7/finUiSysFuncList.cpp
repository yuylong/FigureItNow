/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2018 Yulong Yu. All rights reserved.
 */

#include "finUiSysFuncList.h"
#include "ui_finUiSysFuncList.h"

#include <QVariant>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTextStream>

finUiSysFuncList::finUiSysFuncList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::finUiSysFuncList)
{
    ui->setupUi(this);
}

finUiSysFuncList::~finUiSysFuncList()
{
    delete ui;
}

finErrorCode finUiSysFuncList::clearList()
{
    ui->trwFuncList->clear();
    return finErrorKits::EC_SUCCESS;
}

static QString _nullCatgoryString("Common Functions");
static QString _nullFuncNameString("Unnamed_Function");

QTreeWidgetItem *finUiSysFuncList::createCategoryItem(const QString &ctgstr)
{
    QString searchstr = (ctgstr.isEmpty() ? _nullCatgoryString : ctgstr);

    QTreeWidgetItem *ctgitem = new QTreeWidgetItem(ui->trwFuncList);
    ctgitem->setText(0, searchstr);

    QMap<QString, QVariant> propmap;
    propmap.insert(QString("Type"), QString("Category"));
    ctgitem->setData(0, Qt::UserRole, propmap);

    ui->trwFuncList->addTopLevelItem(ctgitem);
    return ctgitem;
}

QTreeWidgetItem *finUiSysFuncList::findCategoryItem(const QString &ctgstr)
{
    QString searchstr = (ctgstr.isEmpty() ? _nullCatgoryString : ctgstr);

    QList<QTreeWidgetItem *> itemlist = ui->trwFuncList->findItems(searchstr, Qt::MatchFixedString);
    if ( itemlist.isEmpty() )
        return nullptr;

    return itemlist.first();
}

QTreeWidgetItem *finUiSysFuncList::findAndCreateCategoryItem(const QString &ctgstr)
{
    QTreeWidgetItem *retitem = this->findCategoryItem(ctgstr);
    if ( retitem != nullptr )
        return retitem;

    return this->createCategoryItem(ctgstr);
}

finErrorCode finUiSysFuncList::installFunction(const finExecSysFuncRegItem *func)
{
    QTreeWidgetItem *ctgitem = this->findAndCreateCategoryItem(func->_category);
    if ( ctgitem == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QMap<QString, QVariant> propmap;
    propmap.insert(QString("Type"), QString("Function"));
    propmap.insert(QString("Category"), func->_category);
    propmap.insert(QString("Prototype"), func->_prototype);
    propmap.insert(QString("Description"), func->_description);

    QTreeWidgetItem *funcitem = new QTreeWidgetItem(ctgitem);
    funcitem->setText(0, func->_funcName);
    funcitem->setData(0, Qt::UserRole, propmap);
    ctgitem->addChild(funcitem);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finUiSysFuncList::installFunctionList(const QList<finExecSysFuncRegItem> &funclist)
{
    finErrorCode errcode;
    foreach ( const finExecSysFuncRegItem &func, funclist ) {
        errcode = this->installFunction(&func);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finUiSysFuncList::installFunctionList()
{
    const QList<finExecSysFuncRegItem> &funclist = finExecFunction::getSysFuncRegList();
    return this->installFunctionList(funclist);
}

void _propMapToTextStream(const QMap<QString, QVariant> &propmap, QTextStream &strm, const QStringList &keylist)
{
    foreach ( const QString &key, keylist ) {
        const QVariant &value = propmap.value(key, QVariant());
        if ( value.isNull() )
            continue;

        strm << key << ": " << value.toString() << endl;
    }
}

void _propMapToTextStream(const QMap<QString, QVariant> &propmap, QTextStream &strm)
{
    return _propMapToTextStream(propmap, strm, propmap.keys());
}


QString finUiSysFuncList::getItemDescString(QTreeWidgetItem *item)
{
    QString itemname = item->text(0);
    QMap<QString, QVariant> propmap = item->data(0, Qt::UserRole).toMap();
    QString type = propmap.value(QString("Type")).toString();

    QString retstr;
    QTextStream strm(&retstr);
    strm << QString("Name: ") << itemname << endl;
    strm << QString("Type: ") << type << endl;

    if ( type.compare(QString("Function")) == 0 ) {
        QStringList keylist;
        keylist << "Category" << "Prototype" << "Description";
        _propMapToTextStream(propmap, strm, keylist);
    }
    return retstr;
}

void finUiSysFuncList::on_trwFuncList_itemSelectionChanged()
{
    QList<QTreeWidgetItem *> selitems = ui->trwFuncList->selectedItems();
    if ( selitems.isEmpty() )
        return;

    QTreeWidgetItem *selitem = selitems.first();
    QString descstr = this->getItemDescString(selitem);
    ui->pteFunctionDesc->setPlainText(descstr);
}

void finUiSysFuncList::on_trwFuncList_itemActivated(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    if ( item == nullptr )
        return;

    QString itemname = item->text(0);
    QMap<QString, QVariant> propmap = item->data(0, Qt::UserRole).toMap();
    emit this->itemTriggerred(itemname, propmap);
}
