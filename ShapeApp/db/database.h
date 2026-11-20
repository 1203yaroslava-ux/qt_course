#pragma once
#include <QString>
#include <QList>
#include <QPair>

struct ShapeData {
    int id;
    QString type;
    int w, h;
    double x, y;
};

class Database
{
public:
    bool open(const QString &fileName);
    int  addShape(const QString &type, int w, int h, double x, double y);
    bool removeShape(int id);
    bool addLink(int id1, int id2);
    void savePosition(int id, double x, double y);
    QList<ShapeData> loadShapes();
    QList<QPair<int, int> > loadLinks();

private:
    void createTables();
    void refreshInfo();   // пересчёт linked_ids и type_count
};
