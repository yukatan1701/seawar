#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <ctime>

#include "field.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

	static const int size_v = 10, size_h = 20;
	QWidget *centralWidget;
	QVBoxLayout *layout;
	QHBoxLayout *field_layout, *header_layout, *footer_layout;
	QGridLayout *left_layout, *right_layout;
	QVector< QVector<Field *> > fields;
	QLabel *header_user, *header_opp;
	QLabel *game_stat;
	int user_ship_count, opp_ship_count;
	int game_mode;

	// 0 - water; 1 - alive; 2 - dead.
	QVector< QVector<int> > user_table;
	QVector< QVector<int> > opp_table;

	QPoint *focus;

	void oppMove();
	void enableOppButtons(bool);
	void printTable() const;
	void generateOppField();
	bool checkLocal(int, int, int, int, bool) const;
	bool checkPos(int, int, int, int) const;
	void setPos(int, int, int, int);
	void setLayouts();
	void generateFields();
	void restart();
	void showResult(bool);
	void setCellFocus(QPoint *);
public:
	MainWindow(QWidget *parent = NULL);
	~MainWindow();
	void keyPressEvent(QKeyEvent *event);
public slots:
	void movehandler(int row, int col);
	void sethandler(int row, int col);
};

#endif
