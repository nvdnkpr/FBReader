/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __ZLQTITEMSLISTWIDGET_H__
#define __ZLQTITEMSLISTWIDGET_H__

#include <QtGui/QWidget>
#include <QtGui/QFrame>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QScrollArea>

#include <ZLTreeTitledNode.h>

class ZLQtTreeItem;

class ZLQtItemsListWidget : public QScrollArea {
	Q_OBJECT
public:
	ZLQtItemsListWidget(QWidget *parent = 0);
	void fillNodes(const ZLTreeNode *rootNode);
	QSize sizeHint() const;
	void setMinimumWidth(int w);

Q_SIGNALS:
	void nodeClicked(const ZLTreeNode *node);
	void nodeDoubleClicked(const ZLTreeNode *node);

public Q_SLOTS:
	void onNodeClicked(const ZLTreeNode *node);

private:
	QWidget *myContainerWidget;
	QVBoxLayout *myLayout;
	QList<ZLQtTreeItem*> myItems;
};

class ZLQtTreeItem : public QFrame {
	Q_OBJECT

public:
	ZLQtTreeItem(const ZLTreeTitledNode *node, QWidget *parent = 0);
	void setActive(bool active);
	const ZLTreeTitledNode *getNode() const;

Q_SIGNALS:
	void clicked(const ZLTreeNode *node);
	void doubleClicked(const ZLTreeNode *node);

protected:
	 void mousePressEvent(QMouseEvent *event);
	 void mouseDoubleClickEvent(QMouseEvent *event);
	 void paintEvent(QPaintEvent *event);
private:
	 const ZLTreeTitledNode *myNode;
	 bool isActive;
};


#endif /* __ZLQTITEMSLISTWIDGET_H__ */