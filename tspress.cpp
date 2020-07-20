/*
 * Copyright (c) 2015, Jumpnow Technologies, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "tspress.h"
#include <qboxlayout.h>
#include <qformlayout.h>
#include <qevent.h>
#include <qpainter.h>
#include <qdebug.h>
#include <qstyle.h>
#include <QApplication>

#define MIN_WIDTH_FOR_BUTTONS 500

TsPress::TsPress(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);

    QSize availableSize =  qApp->desktop()->availableGeometry().size();
    int width = availableSize.width();
    int height = availableSize.height();
    qDebug() << "Available dimensions " << width << "x" << height;
    width *= 0.9; // 90% of the screen size
    height *= 0.9; // 90% of the screen size
    qDebug() << "Computed dimensions " << width << "x" << height;
    QSize newSize( width, height );

    setGeometry(
        QStyle::alignedRect( 
            Qt::LeftToRight,
            Qt::AlignCenter,
            newSize,
            qApp->desktop()->availableGeometry()
        )
    );
    
    m_down.setX(-1);
    m_up.setX(-1);

    layoutWindow();

    connect(m_exitButton, SIGNAL(pressed()), SLOT(close()));
}

void TsPress::onPressed(int btn)
{
    m_which->setText(QString::number(btn));
    m_down.setX(-1);
    m_up.setX(-1);
    update();
}

void TsPress::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Down:" << event->x() << event->y();
    m_down = event->pos();
    m_up.setX(-1);
    update();
}

void TsPress::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "Up  :" << event->x() << event->y();
    m_up = event->pos();
    update();
}

void TsPress::paintEvent(QPaintEvent *)
{
    QPen pen;

    QPainter painter(this);

    pen.setColor(Qt::black);
    painter.setPen(pen);

    drawCalTargets(&painter);

    if (m_down.x() >= 0 && m_down != m_up) {
        pen.setColor(Qt::blue);
        painter.setPen(pen);
        painter.drawLine(m_down.x() - 8, m_down.y(), m_down.x() + 8, m_down.y());
        painter.drawLine(m_down.x(), m_down.y() - 8, m_down.x(), m_down.y() + 8);
    }

    if (m_up.x() >= 0) {
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.drawLine(m_up.x() - 8, m_up.y(), m_up.x() + 8, m_up.y());
        painter.drawLine(m_up.x(), m_up.y() - 8, m_up.x(), m_up.y() + 8);
    }
}

void TsPress::drawCalTargets(QPainter *painter)
{
    int h = height() - 100;
    int w = width() - 100;
    int w_div4 = width() / 4;

    for (int x = 100, y = 100; x <= w; x += 100) {
        painter->drawLine(x - 5, y, x + 5, y);
        painter->drawLine(x, y - 5, x, y + 5);
    }

    for (int x = 100, y = h; x <= w; x += 100) {
        painter->drawLine(x - 5, y, x + 5, y);
        painter->drawLine(x, y - 5, x, y + 5);
    }

    for (int x = 100, y = 200; y <= h; y += 100) {
        painter->drawLine(x - 5, y, x + 5, y);
        painter->drawLine(x, y - 5, x, y + 5);
    }

    for (int x = w, y = 200; y <= h; y += 100) {
        painter->drawLine(x - 5, y, x + 5, y);
        painter->drawLine(x, y - 5, x, y + 5);
    }

    for (int x = w_div4, y = 200; y <= h; y += 100) {
        painter->drawLine(x - 5, y, x + 5, y);
        painter->drawLine(x, y - 5, x, y + 5);
    }

    for (int x = 3 * w_div4, y = 200; y <= h; y += 100) {
        painter->drawLine(x - 5, y, x + 5, y);
        painter->drawLine(x, y - 5, x, y + 5);
    }
}

void TsPress::layoutWindow()
{
    QHBoxLayout *btnLayout;
    QPushButton *btn;

    if (width() > MIN_WIDTH_FOR_BUTTONS) {
        for (int i = 0; i < 12; i++) {
            btn = new QPushButton(QString::number(i+1));
            btn->setFixedSize(60, 50);
            connect(btn, &QPushButton::pressed, [this, i]{ onPressed(i+1); });
            m_btns.append(btn);
        }
    }

    m_exitButton = new QPushButton("Exit");
    m_exitButton->setMinimumSize(80, 50);

    m_which = new QLabel("#");
    m_which->setStyleSheet("font-size: 28pt; color: red;");

    QVBoxLayout *mainLayout = new QVBoxLayout;

    if (width() > MIN_WIDTH_FOR_BUTTONS) {
        btnLayout = new QHBoxLayout;

        for (int i = 0; i < 5; i++) {
            btnLayout->addWidget(m_btns.at(i));

            if (i < 4)
                btnLayout->addStretch();
        }

        mainLayout->addLayout(btnLayout);
    }

    mainLayout->addStretch();

    btnLayout = new QHBoxLayout;

    if (width() > MIN_WIDTH_FOR_BUTTONS)
        btnLayout->addWidget(m_btns.at(5));

    btnLayout->addStretch();
    btnLayout->addWidget(m_exitButton);
    btnLayout->addStretch();

    if (width() > MIN_WIDTH_FOR_BUTTONS)
        btnLayout->addWidget(m_btns.at(6));

    mainLayout->addLayout(btnLayout);

    if (width() > MIN_WIDTH_FOR_BUTTONS) {
        QHBoxLayout *outputLayout = new QHBoxLayout;
        outputLayout->addStretch();
        outputLayout->addWidget(m_which);
        outputLayout->addStretch();

        mainLayout->addSpacing(8);
        mainLayout->addLayout(outputLayout);
        mainLayout->addStretch();

        btnLayout = new QHBoxLayout;

        for (int i = 7; i < 12; i++) {
            btnLayout->addWidget(m_btns.at(i));

            if (i < 11)
                btnLayout->addStretch();
        }

        mainLayout->addLayout(btnLayout);
    }
    else {
        mainLayout->addStretch();
    }

    QWidget *widget = new QWidget;
    widget->setLayout(mainLayout);

    setCentralWidget(widget);
}

