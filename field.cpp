#include "field.h"
#include <QDebug>
#include <QSizePolicy>

Field::Field(int row, int col, QWidget *parent)
      :QPushButton(parent), row(row), col(col) {
  //qDebug() << "CONSTRUCTOR";
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(this, SIGNAL(clicked()), this, SLOT(clickhandler()));
}

void Field::focus() {
	setStyleSheet(styleSheet().append(QString("border: 2px solid black;")));
}

void Field::clickhandler() {
  //qDebug() << "emit signal";
	emit moved(row, col);
}

