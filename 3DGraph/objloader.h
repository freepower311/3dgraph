#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QFile>

/* Самописный класс-загрузчик файла .obj
 * Тестирование не проходил, но на одном примере работает.
 * Загружает только вершины, нормали и координаты текстур.
 * Предоставляет прямые указатели на свои поля данных.
*/
class OBJLoader
{
public:
    OBJLoader();
    void load(const QString &path);
    QVector<float>* getVertices();
    QVector<float>* getUvs();
    QVector<float>* getNormals();
    int verticesCount();
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
    int m_verticesCount;
};

#endif // OBJLOADER_H
