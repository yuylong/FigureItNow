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
    ui->trwFuncList->addTopLevelItem(ctgitem);
    return ctgitem;
}

QTreeWidgetItem *finUiSysFuncList::findCategoryItem(const QString &ctgstr)
{
    QString searchstr = (ctgstr.isEmpty() ? _nullCatgoryString : ctgstr);

    QList<QTreeWidgetItem *> itemlist = ui->trwFuncList->findItems(searchstr, Qt::MatchFixedString);
    if ( itemlist.isEmpty() )
        return NULL;

    return itemlist.first();
}

QTreeWidgetItem *finUiSysFuncList::findAndCreateCategoryItem(const QString &ctgstr)
{
    QTreeWidgetItem *retitem = this->findCategoryItem(ctgstr);
    if ( retitem != NULL )
        return retitem;

    return this->createCategoryItem(ctgstr);
}

finErrorCode finUiSysFuncList::installFunction(finExecFunction *func)
{
    QTreeWidgetItem *ctgitem = this->findAndCreateCategoryItem(func->getCategory());
    if ( ctgitem == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QMap<QString, QVariant> propmap;
    propmap.insert(QString("Prototype"), func->getPrototypeExample());
    propmap.insert(QString("Description"), func->getDescription());

    QTreeWidgetItem *funcitem = new QTreeWidgetItem(ctgitem);
    funcitem->setText(0, func->getFunctionName());
    funcitem->setData(0, Qt::UserRole, propmap);
    ctgitem->addChild(funcitem);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finUiSysFuncList::installFunctionList(const QList<finExecFunction *> &funclist)
{
    finErrorCode errcode;
    foreach ( finExecFunction *func, funclist ) {
        errcode = this->installFunction(func);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finUiSysFuncList::installFunctionList(finExecEnvironment *env)
{
    QList<finExecFunction *> funclist = env->getAllFunctionList();
    return this->installFunctionList(funclist);
}
