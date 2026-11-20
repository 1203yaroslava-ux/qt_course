#include "database.h"
#include <QtSql>

bool Database::open(const QString &fileName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(fileName);
    if (!db.open()) {
        qDebug() << "Error open database:" << db.lastError().text();
        return false;
    }
    createTables();
    return true;
}

void Database::createTables()
{
    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS shapes ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
           "type TEXT NOT NULL, w INTEGER, h INTEGER, x REAL, y REAL, "
           "linked_ids TEXT DEFAULT '', type_count INTEGER DEFAULT 0)");
    q.exec("CREATE TABLE IF NOT EXISTS links ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
           "shape1 INTEGER, shape2 INTEGER)");
}

int Database::addShape(const QString &type, int w, int h, double x, double y)
{
    QSqlQuery q;
    q.prepare("INSERT INTO shapes (type, w, h, x, y) VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(type);
    q.addBindValue(w);
    q.addBindValue(h);
    q.addBindValue(x);
    q.addBindValue(y);
    if (!q.exec())
        return -1;
    int id = q.lastInsertId().toInt();
    refreshInfo();
    return id;
}

bool Database::removeShape(int id)
{
    QSqlDatabase::database().transaction();
    QSqlQuery q;
    q.prepare("DELETE FROM links WHERE shape1 = ? OR shape2 = ?");
    q.addBindValue(id);
    q.addBindValue(id);
    bool ok = q.exec();
    q.prepare("DELETE FROM shapes WHERE id = ?");
    q.addBindValue(id);
    ok = ok && q.exec();
    if (ok) QSqlDatabase::database().commit();
    else    QSqlDatabase::database().rollback();
    refreshInfo();
    return ok;
}

bool Database::addLink(int id1, int id2)
{
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM links WHERE "
              "(shape1 = ? AND shape2 = ?) OR (shape1 = ? AND shape2 = ?)");
    q.addBindValue(id1); q.addBindValue(id2);
    q.addBindValue(id2); q.addBindValue(id1);
    q.exec();
    if (q.next() && q.value(0).toInt() > 0)
        return false;                       // такая связь уже есть

    q.prepare("INSERT INTO links (shape1, shape2) VALUES (?, ?)");
    q.addBindValue(id1);
    q.addBindValue(id2);
    bool ok = q.exec();
    refreshInfo();
    return ok;
}

void Database::savePosition(int id, double x, double y)
{
    QSqlQuery q;
    q.prepare("UPDATE shapes SET x = ?, y = ? WHERE id = ?");
    q.addBindValue(x);
    q.addBindValue(y);
    q.addBindValue(id);
    q.exec();
}

QList<ShapeData> Database::loadShapes()
{
    QList<ShapeData> result;
    QSqlQuery q("SELECT id, type, w, h, x, y FROM shapes");
    while (q.next()) {
        ShapeData s;
        s.id = q.value(0).toInt();
        s.type = q.value(1).toString();
        s.w = q.value(2).toInt();
        s.h = q.value(3).toInt();
        s.x = q.value(4).toDouble();
        s.y = q.value(5).toDouble();
        result.append(s);
    }
    return result;
}

QList<QPair<int, int> > Database::loadLinks()
{
    QList<QPair<int, int> > result;
    QSqlQuery q("SELECT shape1, shape2 FROM links");
    while (q.next())
        result.append(qMakePair(q.value(0).toInt(), q.value(1).toInt()));
    return result;
}

void Database::refreshInfo()
{
    QSqlQuery q;
    // сколько всего фигур такого же типа
    q.exec("UPDATE shapes SET type_count = ("
           "SELECT COUNT(*) FROM shapes s2 WHERE s2.type = shapes.type)");
    // с кем связана фигура, например "2, 5"
    q.exec("UPDATE shapes SET linked_ids = COALESCE(("
           "SELECT group_concat(CASE WHEN shape1 = shapes.id "
           "THEN shape2 ELSE shape1 END, ', ') "
           "FROM links WHERE shape1 = shapes.id OR shape2 = shapes.id), '')");
}
