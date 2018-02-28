/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2018 Yulong Yu. All rights reserved.
 */

#ifndef FINUISYSFUNCLIST_H
#define FINUISYSFUNCLIST_H

#include <QWidget>
#include <QTreeWidgetItem>

#include "finErrorCode.h"
#include "finExecFunction.h"

namespace Ui {
class finUiSysFuncList;
}

class finUiSysFuncList : public QWidget
{
    Q_OBJECT

public:
    explicit finUiSysFuncList(QWidget *parent = 0);
    ~finUiSysFuncList();

    finErrorCode clearList();
    finErrorCode installFunction(const finExecSysFuncRegItem *func);
    finErrorCode installFunctionList(const QList<finExecSysFuncRegItem> &funclist);
    finErrorCode installFunctionList();

private:
    Ui::finUiSysFuncList *ui;

    QTreeWidgetItem *createCategoryItem(const QString &ctgstr);
    QTreeWidgetItem *findCategoryItem(const QString &ctgstr);
    QTreeWidgetItem *findAndCreateCategoryItem(const QString &ctgstr);
};

#endif // FINUISYSFUNCLIST_H
