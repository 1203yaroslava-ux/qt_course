#include "mainwindow.h"
#include "scene/shapeitem.h"
#include "scene/linkitem.h"
#include "delegates/shapedelegate.h"
#include <QtWidgets>
#include <QtSql>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    if (!m_db.open("shapes.db"))
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных");
    setupUi();
    loadFromDatabase();
}

void MainWindow::setupUi()
{
    //сцена
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 800, 600);
    m_view = new QGraphicsView(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);

    //создание фигур
    m_typeBox = new QComboBox;
    m_typeBox->addItem("Эллипс", "ellipse");
    m_typeBox->addItem("Прямоугольник", "rectangle");
    m_typeBox->addItem("Многоугольник", "polygon");
    m_wSpin = new QSpinBox;
    m_wSpin->setRange(20, 300);
    m_wSpin->setValue(80);
    m_hSpin = new QSpinBox;
    m_hSpin->setRange(20, 300);
    m_hSpin->setValue(60);

    QPushButton *addBtn  = new QPushButton("Добавить фигуру");
    QPushButton *linkBtn = new QPushButton("Связать две выбранные (Ctrl+клик)");
    QPushButton *delBtn  = new QPushButton("Удалить выбранную фигуру");

    QGridLayout *createLay = new QGridLayout;
    createLay->addWidget(new QLabel("Тип:"), 0, 0);
    createLay->addWidget(m_typeBox, 0, 1);
    createLay->addWidget(new QLabel("Ширина:"), 1, 0);
    createLay->addWidget(m_wSpin, 1, 1);
    createLay->addWidget(new QLabel("Высота:"), 2, 0);
    createLay->addWidget(m_hSpin, 2, 1);
    createLay->addWidget(addBtn, 3, 0, 1, 2);

    //фильтры
    m_idEdit = new QLineEdit;
    m_idEdit->setPlaceholderText("например: 1, 3, 5");
    m_filterType = new QComboBox;
    m_filterType->addItem("Все типы", "");
    m_filterType->addItem("Эллипс", "ellipse");
    m_filterType->addItem("Прямоугольник", "rectangle");
    m_filterType->addItem("Многоугольник", "polygon");
    m_filterCount = new QSpinBox;
    m_filterCount->setRange(0, 1000);          // 0 = не фильтровать

    QPushButton *applyBtn = new QPushButton("Применить фильтр");
    QPushButton *resetBtn = new QPushButton("Сбросить");

    QGridLayout *filterLay = new QGridLayout;
    filterLay->addWidget(new QLabel("Номера фигур:"), 0, 0);
    filterLay->addWidget(m_idEdit, 0, 1);
    filterLay->addWidget(new QLabel("Тип:"), 1, 0);
    filterLay->addWidget(m_filterType, 1, 1);
    filterLay->addWidget(new QLabel("Фигур типа не меньше:"), 2, 0);
    filterLay->addWidget(m_filterCount, 2, 1);
    filterLay->addWidget(applyBtn, 3, 0);
    filterLay->addWidget(resetBtn, 3, 1);

    //модель и таблица
    m_model = new QSqlTableModel(this);
    m_model->setTable("shapes");
    m_model->setHeaderData(0, Qt::Horizontal, "ID");
    m_model->setHeaderData(1, Qt::Horizontal, "Тип");
    m_model->setHeaderData(6, Qt::Horizontal, "Связана с");
    m_model->setHeaderData(7, Qt::Horizontal, "Фигур такого типа");
    m_model->select();

    m_table = new QTableView;
    m_table->setModel(m_model);
    for (int col = 2; col <= 5; ++col)         // w, h, x, y пользователю не нужны
        m_table->setColumnHidden(col, true);
    m_table->setItemDelegateForColumn(7, new ShapeDelegate(this));
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setDefaultSectionSize(34);
    m_table->resizeColumnsToContents();
    m_table->horizontalHeader()->setStretchLastSection(true);

    //компоновка
    QVBoxLayout *rightLay = new QVBoxLayout;
    rightLay->addLayout(createLay);
    rightLay->addWidget(linkBtn);
    rightLay->addWidget(delBtn);
    rightLay->addWidget(new QLabel("Фильтр таблицы:"));
    rightLay->addLayout(filterLay);
    rightLay->addWidget(m_table, 1);

    QHBoxLayout *mainLay = new QHBoxLayout;
    mainLay->addWidget(m_view, 3);
    mainLay->addLayout(rightLay, 2);

    QWidget *central = new QWidget;
    central->setLayout(mainLay);
    setCentralWidget(central);
    setWindowTitle("Фигуры и связи");

    //сигналы и слоты
    connect(addBtn,   &QPushButton::clicked, this, &MainWindow::onAddShape);
    connect(linkBtn,  &QPushButton::clicked, this, &MainWindow::onAddLink);
    connect(delBtn,   &QPushButton::clicked, this, &MainWindow::onDeleteShape);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::onApplyFilter);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::onResetFilter);
}

//работа со сценой

ShapeItem *MainWindow::createShapeItem(int id, const QString &type, int w, int h,
                                       double x, double y)
{
    ShapeItem *item = new ShapeItem(id, type, w, h);
    item->setPos(x, y);
    m_scene->addItem(item);
    return item;
}

ShapeItem *MainWindow::findShape(int id)
{
    for (QGraphicsItem *it : m_scene->items()) {
        ShapeItem *s = dynamic_cast<ShapeItem *>(it);
        if (s && s->id() == id)
            return s;
    }
    return nullptr;
}

void MainWindow::removeShapeItem(ShapeItem *item)
{
    // сначала убираем все линии этой фигуры
    const QList<LinkItem *> links = item->links();
    for (LinkItem *l : links) {
        ShapeItem *other = (l->first() == item) ? l->second() : l->first();
        other->removeLink(l);
        delete l;               // delete убирает объект и со сцены
    }
    delete item;
}

void MainWindow::loadFromDatabase()
{
    const QList<ShapeData> shapes = m_db.loadShapes();
    for (const ShapeData &s : shapes)
        createShapeItem(s.id, s.type, s.w, s.h, s.x, s.y);

    const QList<QPair<int, int> > links = m_db.loadLinks();
    for (const QPair<int, int> &p : links) {
        ShapeItem *a = findShape(p.first);
        ShapeItem *b = findShape(p.second);
        if (a && b)
            m_scene->addItem(new LinkItem(a, b));
    }
}

//слоты

void MainWindow::onAddShape()
{
    QString type = m_typeBox->currentData().toString();
    int w = m_wSpin->value();
    int h = m_hSpin->value();
    double x = QRandomGenerator::global()->bounded(20, 600);
    double y = QRandomGenerator::global()->bounded(20, 450);

    int id = m_db.addShape(type, w, h, x, y);    // 1) в базу
    if (id < 0) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить фигуру");
        return;
    }
    createShapeItem(id, type, w, h, x, y);       // 2) на сцену
    m_model->select();                           // 3) обновить таблицу
}

void MainWindow::onAddLink()
{
    QList<QGraphicsItem *> sel = m_scene->selectedItems();
    if (sel.size() != 2) {
        QMessageBox::information(this, "Связь",
            "Выделите ровно две фигуры (Ctrl + клик).");
        return;
    }
    ShapeItem *a = static_cast<ShapeItem *>(sel[0]);   // выделяются только фигуры
    ShapeItem *b = static_cast<ShapeItem *>(sel[1]);
    if (!m_db.addLink(a->id(), b->id())) {
        QMessageBox::information(this, "Связь", "Эти фигуры уже связаны.");
        return;
    }
    m_scene->addItem(new LinkItem(a, b));
    m_model->select();
}

void MainWindow::onDeleteShape()
{
    int id = -1;
    QList<QGraphicsItem *> sel = m_scene->selectedItems();
    if (sel.size() == 1)
        id = static_cast<ShapeItem *>(sel[0])->id();           // выбрана на сцене
    else if (m_table->currentIndex().isValid())                // или в таблице
        id = m_model->record(m_table->currentIndex().row()).value("id").toInt();

    if (id < 0) {
        QMessageBox::information(this, "Удаление",
            "Выберите фигуру на сцене или строку в таблице.");
        return;
    }
    m_db.removeShape(id);                  // из БД: сама фигура и все её связи
    if (ShapeItem *item = findShape(id))
        removeShapeItem(item);             // со сцены: фигура и её линии
    m_model->select();                     // таблица
}

void MainWindow::onApplyFilter()
{
    QStringList conditions;

    // номера: вытаскиваем все числа из строки
    QStringList ids;
    QRegularExpression re("\\d+");
    QRegularExpressionMatchIterator it = re.globalMatch(m_idEdit->text());
    while (it.hasNext())
        ids << it.next().captured();
    if (!ids.isEmpty())
        conditions << "id IN (" + ids.join(",") + ")";

    QString type = m_filterType->currentData().toString();
    if (!type.isEmpty())
        conditions << "type = '" + type + "'";

    if (m_filterCount->value() > 0)
        conditions << QString("type_count >= %1").arg(m_filterCount->value());

    m_model->setFilter(conditions.join(" AND "));   // только отображение!
    m_model->select();
}

void MainWindow::onResetFilter()
{
    m_idEdit->clear();
    m_filterType->setCurrentIndex(0);
    m_filterCount->setValue(0);
    m_model->setFilter("");
    m_model->select();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // запоминаем, где стоят фигуры
    for (QGraphicsItem *it : m_scene->items()) {
        ShapeItem *s = dynamic_cast<ShapeItem *>(it);
        if (s)
            m_db.savePosition(s->id(), s->pos().x(), s->pos().y());
    }
    QMainWindow::closeEvent(event);
}
