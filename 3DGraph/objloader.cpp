#include "objloader.h"

OBJLoader::OBJLoader()
{

}

void OBJLoader::load(const QString &path)
{
    QFile file(path);
    QString textFromFile;
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(file.isOpen())
    {
        //todo: Уменьшить вложенность. Разбить на функции?
        while(!file.atEnd())
        {
            textFromFile += file.readLine().data();
            QStringList list = textFromFile.split(" ");
            textFromFile.clear();
            if (list.count() > 0)
            {
                if (list[0] == "v" && list.count() == 4)
                {
                    m_tempVertices.append(QVector3D(list[1].toFloat(),list[2].toFloat(),list[3].toFloat()));
                }
                if (list[0] == "vt" && list.count() == 3)
                {
                    m_tempUvs.append(QVector2D(list[1].toFloat(),list[2].toFloat()));
                }
                if (list[0] == "vn" && list.count() == 4)
                {
                    m_tempNormals.append(QVector3D(list[1].toFloat(),list[2].toFloat(),list[3].toFloat()));
                }
                if (list[0] == "f" && list.count() == 4)
                {
                    for(int i = 0;i<3;i++)
                    {
                        QStringList indexes = list[i+1].split("/");
                        m_vertexIndices.append(indexes[0].toInt());
                        m_uvIndices.append(indexes[1].toInt());
                        m_normalIndices.append(indexes[2].toInt());
                    }
                }
            }
        }
        //build
        foreach (int i, m_vertexIndices)
        {
            m_vertices.append(m_tempVertices[i-1][0]);
            m_vertices.append(m_tempVertices[i-1][1]);
            m_vertices.append(m_tempVertices[i-1][2]);
        }
        foreach (int i, m_uvIndices)
        {
            m_uvs.append(m_tempUvs[i-1][0]);
            m_uvs.append(m_tempUvs[i-1][1]);
        }
        foreach (int i, m_normalIndices)
        {
            m_normals.append(m_tempNormals[i-1][0]);
            m_normals.append(m_tempNormals[i-1][1]);
            m_normals.append(m_tempNormals[i-1][2]);
        }
        m_verticesCount = m_vertexIndices.count();
    }
}

QVector<float>* OBJLoader::getVertices()
{
    return &m_vertices;
}
QVector<float>* OBJLoader::getUvs()
{
    return &m_uvs;
}
QVector<float>* OBJLoader::getNormals()
{
    return &m_normals;
}
int OBJLoader::verticesCount()
{
    return m_verticesCount;
}
