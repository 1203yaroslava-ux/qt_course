#pragma once
#include <QMainWindow>
#include "db/database.h"

class QGraphicsScene;
class QGraphicsView;
class QSqlTableModel;
class QTableView;
class QComboBox;
class QSpinBox;
class QLineEdit;
class ShapeItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onAddShape();
    void onAddLink();
    void onDeleteShape();
    void onApplyFilter();
    void onResetFilter();

private:
    void setupUi();
    void loadFromDatabase();
    ShapeItem *createShapeItem(int id, const QString &type, int w, int h,
                               double x, double y);
    ShapeItem *findShape(int id);
    void removeShapeItem(ShapeItem *item);

    Database m_db;
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QSqlTableModel *m_model;
    QTableView *m_table;
    QComboBox *m_typeBox;        // создание фигуры
    QSpinBox *m_wSpin;
    QSpinBox *m_hSpin;
    QLineEdit *m_idEdit;         // фильтр
    QComboBox *m_filterType;
    QSpinBox *m_filterCount;
};
