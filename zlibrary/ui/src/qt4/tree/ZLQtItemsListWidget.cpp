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

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPixmap>
#include <QtGui/QPalette>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QScrollBar>

#include <QtCore/QDebug>

#include <ZLNetworkManager.h>
#include <ZLTreeTitledNode.h>

#include "../dialogs/ZLQtDialogManager.h"

#include "../image/ZLQtImageUtils.h"

#include "ZLQtPreviewWidget.h"

#include "ZLQtItemsListWidget.h"

static const int ITEM_HEIGHT = 98;
static const int ITEM_COUNT = 5;
static const int ITEM_SIZE = 77;

ZLQtItemsListWidget::ZLQtItemsListWidget(QWidget *parent) : QScrollArea(parent), myLayout(0) {

	myContainerWidget = new QWidget;
	myContainerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setWidget(myContainerWidget);
	setFrameShape(QFrame::NoFrame);
	setWidgetResizable(true);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

}

void ZLQtItemsListWidget::fillNodes(const ZLTreeNode *expandNode) {
	if (myLayout != 0) {
		delete myLayout;
		qDeleteAll(myContainerWidget->children());
	}
	myItems.clear();
	myLayout = new QVBoxLayout;
	myLayout->setContentsMargins(0,0,0,0);
	myLayout->setSpacing(0);
	myLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	myContainerWidget->setLayout(myLayout);

	foreach(ZLTreeNode* node, expandNode->children()) {
		if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
			//qDebug() << QString::fromStdString(titledNode->title());
			ZLQtTreeItem *item = new ZLQtTreeItem(titledNode);
			connect(item, SIGNAL(clicked(const ZLTreeNode*)), this, SLOT(onNodeClicked(const ZLTreeNode*))); //action ExpandAction used instead
			connect(item, SIGNAL(doubleClicked(const ZLTreeNode*)), this, SIGNAL(nodeDoubleClicked(const ZLTreeNode*)));
			myLayout->addWidget(item);
			myItems.push_back(item);
		}
	}

	myLayout->addStretch();
}

QSize ZLQtItemsListWidget::sizeHint() const {
	return QSize(0, ITEM_HEIGHT * ITEM_COUNT);
}

void ZLQtItemsListWidget::setMinimumWidth(int w) {
	myContainerWidget->setMinimumWidth(w - verticalScrollBar()->width());
	QScrollArea::setMinimumWidth(w);
}

void ZLQtItemsListWidget::onNodeClicked(const ZLTreeNode *node) {
	foreach(ZLQtTreeItem *item, myItems) {
		item->setActive(item->getNode() == node);
	}
	emit nodeClicked(node);
}


ZLQtTreeItem::ZLQtTreeItem(const ZLTreeTitledNode *node, QWidget *parent) : QFrame(parent), myNode(node) {
	setAutoFillBackground(true);
	setActive(false);

	QHBoxLayout *mainLayout = new QHBoxLayout;
	QVBoxLayout *titlesLayout = new QVBoxLayout;

	QLabel *icon = new QLabel;
	QLabel *title = new QLabel(QString("<b>%1</b>").arg(QString::fromStdString(node->title())));
	QLabel *subtitle = new QLabel(QString::fromStdString(node->subtitle()));
	title->setWordWrap(true);
	subtitle->setWordWrap(true);

	QSizePolicy policy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	policy.setHorizontalStretch(1);
	title->setSizePolicy(policy);
	subtitle->setSizePolicy(policy);

	titlesLayout->addWidget(title);
	titlesLayout->addWidget(subtitle);

	shared_ptr<const ZLImage> image = node->image();
	if (!image.isNull()) {
		ZLNetworkManager::Instance().perform(image->synchronizationData());
		QPixmap pixmap = ZLQtImageUtils::ZLImageToQPixmapWithSize(image, QSize(ITEM_SIZE,ITEM_SIZE), Qt::SmoothTransformation);
		//qDebug() << pixmap.isNull();
		icon->setPixmap(pixmap);
	}
	mainLayout->addWidget(icon);
	mainLayout->addLayout(titlesLayout);
	mainLayout->addStretch();
	setLayout(mainLayout);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setFixedHeight(ITEM_HEIGHT);
}

void ZLQtTreeItem::setActive(bool active) {
	isActive = active;

	//TODO due to different themes on OS, take a color from existed palette
	QColor mainColor = isActive ? QColor::fromHsv(0, 0, 0.75 * 255) : QColor::fromHsv(0, 0, 0.95 * 255);

	setFrameStyle(QFrame::Panel | QFrame::Raised);
	setLineWidth(2);

	QPalette p = palette();
	p.setColor(QPalette::Window, mainColor);
	setPalette(p);

	update();
}

const ZLTreeTitledNode *ZLQtTreeItem::getNode() const {
	return myNode;
}

void ZLQtTreeItem::mousePressEvent(QMouseEvent *) {
	emit clicked(myNode);
}

void ZLQtTreeItem::mouseDoubleClickEvent(QMouseEvent *) {
	emit doubleClicked(myNode);
}

void ZLQtTreeItem::paintEvent(QPaintEvent *event) {
	//qDebug() << Q_FUNC_INFO << event->rect();
	QFrame::paintEvent(event);
	return;
//	QColor mainColor = isActive ? QColor::fromHsv(0, 0, 0.75 * 255) : QColor::fromHsv(0, 0, 0.95 * 255);
//	int h = mainColor.hue();
//	int s = mainColor.saturation();
//	int v = mainColor.value();
//	QColor shadowColor1 = QColor::fromHsv(h,s,v - 23); //these numbers are getted from experiments with Photoshop
//	QColor shadowColor2 = QColor::fromHsv(h,s,v - 43);
//	QColor shadowColor3 = QColor::fromHsv(h,s,v - 71);
//	QColor shadowColor4 = QColor::fromHsv(h,s,v - 117);
//	QColor shadowColor5 = QColor::fromHsv(h,s,v - 155);

//	QColor shadowUpColor = QColor::fromHsv(h,s,v - 114);



//	//QColor shadow(196,193,189); //TODO not hardcode the color automatically

//	QPainter painter(this);
//	QRect rect = this->rect();

//	//painter.setBrush(mainColor);
//	painter.fillRect(rect, mainColor);

//	painter.setPen(shadowColor5);
//	painter.drawLine(rect.left() + 2, rect.bottom(), rect.right(), rect.bottom());

//	painter.setPen(shadowColor4);
//	//painter.drawLine(rect.left() + 2, rect.top() + 1, rect.right() - 2, rect.top() + 1);

//	painter.setPen(shadowColor3);
//	painter.drawLine(rect.left() + 4, rect.bottom() - 1, rect.right(), rect.bottom() - 1);

//	//painter.drawLine(rect.left() + 2, rect.top(), rect.right() - 2, rect.top());

//	painter.drawLine(rect.right(), rect.top() + 2, rect.right(), rect.bottom() - 1);
//	painter.drawLine(rect.left() + 1, rect.top(), rect.left() + 1, rect.bottom() - 1);

//	painter.setPen(shadowColor2);
//	painter.drawLine(rect.left(), rect.top() + 1, rect.left(), rect.bottom() - 2);
//	painter.drawLine(rect.right() - 1, rect.top() + 2, rect.right() - 1, rect.bottom() - 2);

//	painter.setPen(shadowColor1);
//	painter.drawLine(rect.left() + 5, rect.bottom() - 2, rect.right() - 2, rect.bottom() - 2);
//	painter.drawLine(rect.right() - 2, rect.top() + 2, rect.right() - 2, rect.bottom() - 2);

//	painter.setPen(shadowUpColor);
	//	painter.drawLine(rect.left() + 2, rect.top(), rect.right() - 2, rect.top());

}