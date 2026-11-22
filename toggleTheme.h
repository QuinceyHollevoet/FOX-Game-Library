#pragma once

#include <QCheckBox>
#include <QPainter>

class SwitchButton : public QCheckBox {
    Q_OBJECT
public:
    explicit SwitchButton(QWidget *parent = nullptr) : QCheckBox(parent) {
        setFixedSize(40, 20);
        setCursor(Qt::PointingHandCursor);
        setAttribute(Qt::WA_StyledBackground, false);
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QRect trackRect(0, 2, width(), height() - 4);
        int radius = trackRect.height() / 2;

        int thumbSize = height() - 4;
        int thumbX = isChecked() ? (width() - thumbSize - 2) : 2;
        QRect thumbRect(thumbX, 2, thumbSize, thumbSize);

        if (isChecked()) {
            painter.setBrush(QColor("#a2a2a2"));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(trackRect, radius, radius);
        } else {
            painter.setBrush(QColor("#E5E5EA"));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(trackRect, radius, radius);
        }

        painter.setBrush(Qt::white);
        painter.setPen(QPen(QColor(0, 0, 0, 30), 1)); 
        painter.drawEllipse(thumbRect);
    }
};
