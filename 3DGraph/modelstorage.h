#ifndef MODELSTORAGE_H
#define MODELSTORAGE_H

#include <QDebug>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QFile>

/* Самописный класс-контейнер вершин модели с поддержкой загрузки obj файлов.
 * Парсер тестирование не проходил, но на одном примере работает.
 * Загружает только вершины, нормали и координаты текстур треугольников.
 * Предоставляет прямые указатели на свои поля данных.
*/
class ModelStorage
{
public:
    ModelStorage();
    void loadObj(const QString &path);
    QVector<float>* getVertices();
    QVector<float>* getUvs();
    QVector<float>* getNormals();
    int coordCount();
private:
    QVector<unsigned int> m_vertexIndices;
    QVector<unsigned int> m_uvIndices;
    QVector<unsigned int> m_normalIndices;
    QVector<QVector3D> m_tempVertices;
    QVector<QVector2D> m_tempUvs;
    QVector<QVector3D> m_tempNormals;
    QVector<float> m_vertices;
    QVector<float> m_uvs;
    QVector<float> m_normals;
};

#endif // MODELSTORAGE_H
