#include "mainwindow.h"
#include <QMessageBox>
#include <QDebug>

QIcon & getIcon();

void MainWindow::setLayouts() {
	centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	layout = new QVBoxLayout(centralWidget);
	header_layout = new QHBoxLayout();
	field_layout = new QHBoxLayout();
	footer_layout = new QHBoxLayout();
	
	header_opp = new QLabel("Opponent field:");
	header_opp->setAlignment(Qt::AlignCenter);
	header_opp->setStyleSheet("font-weight: bold;");
	header_user = new QLabel("Your field:");
	header_user->setAlignment(Qt::AlignCenter);
	header_user->setStyleSheet("font-weight: bold;");
	header_layout->addWidget(header_opp);
	header_layout->addWidget(header_user);
	header_layout->setSpacing(0);
	header_layout->setMargin(0);

	game_stat = new QLabel("Please place your ships.");
	footer_layout->addWidget(game_stat);

	left_layout = new QGridLayout();
	left_layout->setSpacing(0);
	left_layout->setMargin(0);
	right_layout = new QGridLayout();
	right_layout->setSpacing(0);
	right_layout->setMargin(0);
	field_layout->addLayout(left_layout);
	field_layout->addLayout(right_layout);

	layout->addLayout(header_layout);
	layout->addLayout(field_layout);
	layout->addLayout(footer_layout);
}

bool MainWindow::checkLocal(int direction, int x, int y, int edge, bool solo) const {
	if (solo) {
		if ((x > 0 && opp_table[y][x - 1] != 0) || 
			(x < size_v - 1 && opp_table[y][x + 1] != 0) ||
			(y > 0 && opp_table[y - 1][x] != 0) ||
			(y < size_v - 1 && opp_table[y + 1][x] != 0))
			return false;
		return true;
	}
	if (direction == 1) {
		if ((y > 0 && opp_table[y - 1][x] != 0) || 
			(y < size_v - 1 && opp_table[y + 1][x] != 0) ||
			(edge == 0 && x > 0 && opp_table[y][x - 1] != 0) ||
			(edge == 1 && x < size_v - 1 && opp_table[y][x + 1] != 0))
			return false;
	} else {
		if ((x > 0 && opp_table[y][x - 1] != 0) || 
			(x < size_v - 1 && opp_table[y][x + 1] != 0) ||
			(edge == 0 && y > 0 && opp_table[y - 1][x] != 0) ||
			(edge == 1 && y < size_v - 1 && opp_table[y + 1][x] != 0))
			return false;
	}
	return true;
}

bool MainWindow::checkPos(int direction, int x, int y, int len) const {
	if ((direction == 1 && x + len > size_v) || 
		(direction == 0 && y + len > size_v))
			return false;
	//qDebug() << "seg" << x << y << len << "|" << direction;
	// end: -1 - none, 0 - left (top), 1 - right (bottom)
	for (int i = 0; i < len; i++) {
		int edge = -1;
		if (i == 0)
			edge = 0;
		else if (i == len - 1)
			edge = 1;
		if (direction == 1) {
			bool free_local = checkLocal(direction, x + i, y, edge, len == 1);
			if (opp_table[y][x + i] != 0 || free_local == false) {
				return false;
			}
		} else {
			bool free_local = checkLocal(direction, x, y + i, edge, len == 1);
			if (opp_table[y + i][x] != 0 || free_local == false) {
				return false;
			}
		}
	}
	return true;
}

void MainWindow::setPos(int direction, int x, int y, int len) {
	qDebug() << x + 1 << y + 1 << len << direction;
	if (direction == 1)
		for (int i = 0; i < len; i++)
			opp_table[y][x + i] = 1;
	else
		for (int i = 0; i < len; i++)
			opp_table[y + i][x] = 1;
}

void MainWindow::printTable() const {
	for (int i = 0; i < size_v; i++) {
		for (int j = 0; j < size_v; j++) {
			fprintf(stderr, "%d ", opp_table[i][j]);
		}
		fprintf(stderr, "\n");
	}
}

void MainWindow::generateOppField() {
	srand(time(NULL));
	// 1 - right, 0 - bottom
	int direction = 0;
	for (int len = 4; len > 0; len--) {
		for (int count = 5 - len; count > 0; ) {
			int x = rand() % size_v, y = rand() % size_v;
			direction = rand() % 2;
			bool is_ok = false;
			if (direction == 1) {
				if (checkPos(direction, x, y, len)) {
					setPos(direction, x, y, len);
					is_ok = true;
				}
			} else {
				if (checkPos(direction, x, y, len)) {
					setPos(direction, x, y, len);	
					is_ok = true;
				}
			}
			if (is_ok) {
				//printTable();
				count--;
			}
		}
	}
}

void MainWindow::generateFields() {
	user_table.resize(size_v);
	opp_table.resize(size_v);
	for (int i = 0; i < size_v; i++) {
		user_table[i].resize(size_h);
		opp_table[i].resize(size_h);
		for (int j = 0; j < size_h; j++) {
			user_table[i][j] = 0;
			opp_table[i][j] = 0;
		}
	}
	generateOppField();
}

void MainWindow::enableOppButtons(bool val) {
	for (int i = 0; i < size_v; i++) {
		for (int j = 0; j < size_v; j++) {
			fields[i][j]->setEnabled(val);
		}
	}
}

MainWindow::MainWindow(QWidget *parent) :
						QMainWindow(parent) {
	resize(800, 400);
	setLayouts();
	generateFields();
	const QSize BUTTON_SIZE = QSize(35, 35);
	fields.resize(size_v);
	for (int i = 0; i < size_v; ++i) {
		fields[i].resize(size_h);
		for (int j = 0; j < size_h; ++j) {
			Field *newfield = new Field(i, j, this);
			newfield->setDefault(true);
			newfield->setAutoDefault(true);
			newfield->setText("");
			newfield->setFixedSize(BUTTON_SIZE);
			newfield->setStyleSheet("background-color: #00bfff;");
			fields[i][j] = newfield;
			if (j < size_v) {
				left_layout->addWidget(newfield, i, j);
				connect(newfield, SIGNAL(moved(int, int)), this, 
				        SLOT(movehandler(int, int)));
			}
			else {
				right_layout->addWidget(newfield, i, j);
				connect(newfield, SIGNAL(moved(int, int)), this, 
				        SLOT(sethandler(int, int)));
			}
		}
	}
	focus = new QPoint(size_v, 0);
	setFocus();
	setCellFocus(focus);
	enableOppButtons(false);
	game_mode = 0;
	user_ship_count = 0;
	opp_ship_count = 20;
}

void MainWindow::setCellFocus(QPoint *point) {
	for (int i = 0; i < size_v; i++) {
		for (int j = 0; j < size_h; j++) {
			QString rem_str("border: 2px solid black;");
			QString str = fields[i][j]->styleSheet();
			int index = str.indexOf("border: 2px solid black;", 0);
			str.replace(index, rem_str.length(), "");
			fields[i][j]->setStyleSheet(str);
		}
	}
	fields[point->y()][point->x()]->focus();
}

MainWindow::~MainWindow() {
	for (int i = 0; i < fields.size(); ++i) {
		for (int j = 0; j < fields[i].size(); ++j) {
			delete fields[i][j];
		}
		fields[i].clear();
	}
	fields.clear();
	delete layout;
	delete centralWidget;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
	int key = event->key();
	//qDebug() << "KEY PRESSED" << *focus;
	switch (key) {
		case Qt::Key_Up:
			if (focus->y() > 0)
				focus->setY(focus->y() - 1);
			break;
		case Qt::Key_Down:
			if (focus->y() < size_v - 1)
				focus->setY(focus->y() + 1);
			break;
		case Qt::Key_Left:
			if (focus->x() > 0)
				focus->setX(focus->x() - 1);
			break;
		case Qt::Key_Right:
			if (focus->x() < size_h - 1)
				focus->setX(focus->x() + 1);
			break;
		case 16777220:
			int x = focus->x(), y = focus->y();
			fields[y][x]->animateClick();
			break;
	}
	setCellFocus(focus);
}

void MainWindow::restart() {
	*focus = QPoint(size_v, 0);
	game_mode = 0;
	user_ship_count = 0;
	opp_ship_count = 20;
	for (int i = 0; i < fields.size(); i++) {
		for (int j = 0; j < fields[i].size(); j++) {
			if (j < size_v)
				fields[i][j]->setEnabled(false);
			else
				fields[i][j]->setEnabled(true);
			fields[i][j]->setIcon(QIcon());
			fields[i][j]->setStyleSheet("background-color: #00bfff;");
		}
	}
	for (int i = 0; i < size_v; i++) {
		for (int j = 0; j < size_v; j++) {
			user_table[i][j] = 0;
			opp_table[i][j] = 0;
		}
	}
	generateOppField();
	game_stat->setText("Please place your ships.");
}

void MainWindow::showResult(bool user_won) {	
	for (int i = 0; i < size_v; i++)
		for (int j = 0; j < size_h; j++)
			fields[i][j]->setEnabled(false);
	QMessageBox msgBox;
	msgBox.setText("Game over!");
	if (user_won)
		msgBox.setInformativeText("You won!\nDo you want to restart the game?");
	else
		msgBox.setInformativeText("Opponent won!\nDo you want to restart the game?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::Yes);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes)
		restart();
	else
		exit(0);
}

void MainWindow::oppMove() {
	int x = size_v + rand() % size_v, y = rand() % size_v;
	if (user_table[y][x] == 1) {
		fields[y][x]->setIcon(QIcon());
		fields[y][x]->setStyleSheet(styleSheet().append(QString("background-color: #b30000; qproperty-iconSize: 35px;")));
		fields[y][x]->setIcon(getIcon());
		user_table[y][x] = 2;
		user_ship_count--;
		if (user_ship_count == 0) {
			game_stat->setText("Opponent wins!");
			showResult(false);
		}
	} else if (user_table[y][x] == 0) {
	///	fields[y][x]->setText(".");
		fields[y][x]->setStyleSheet("background-color: #005e80;");
	}
	QString text = "Your move | Your alive ship details: " + 
			       QString::number(user_ship_count) + 
			       " | Opponent alive ship details: " + 
			       QString::number(opp_ship_count);
	game_stat->setText(text);
}

void MainWindow::movehandler(int row, int col) {
	setFocus();
	setCellFocus(focus);
	//qDebug() << row << col;
	if (opp_table[row][col] == 1) {
		qDebug() << row << col;
		fields[row][col]->setStyleSheet(styleSheet().append(QString("background-color: #b30000; qproperty-iconSize: 35px;")));
		fields[row][col]->setIcon(getIcon());
		opp_table[row][col] = 2;
		opp_ship_count--;
		if (opp_ship_count == 0) {
			game_stat->setText("You win!");
			showResult(true);
			game_mode = 2;
		}
	} else if (opp_table[row][col] == 0) {
		fields[row][col]->setStyleSheet(styleSheet().append(QString("background-color: #005e80; ")));
	}
	// game over
	if (game_mode == 2)
		return;
	fields[row][col]->setEnabled(false);
	oppMove();
}

QIcon & getIcon() {
	QIcon *button_icon = new QIcon();
	button_icon->addPixmap(QPixmap("ship.png"), QIcon::Normal);
	button_icon->addPixmap(QPixmap("ship.png"), QIcon::Disabled);
	return *button_icon;
}

void MainWindow::sethandler(int row, int col) {
	setFocus();
	if (game_mode == 1)
		return;
	if (user_table[row][col] == 0) {
		user_table[row][col] = 1;
		//fields[row][col]->setText("S");
		fields[row][col]->setIcon(getIcon());
		fields[row][col]->setStyleSheet(styleSheet().append(QString("background-color: #00bfff; qproperty-iconSize: 35px;")));
		user_ship_count++;
	}
	if (user_ship_count == 20) {
		for (int i = 0; i < size_v; i++) {
			for (int j = 0; j < size_h; j++) {
				if (j < size_v)
					fields[i][j]->setEnabled(false);		
			}
		}
		enableOppButtons(true);
		QString text = "Your move | Your alive ship details: " + 
		               QString::number(user_ship_count) + 
					   " | Opponent alive ship details: " + 
					   QString::number(opp_ship_count);
		game_stat->setText(text);
		game_mode = 1;
	}
	setCellFocus(focus);
}
