#include "modelstorage.h"

ModelStorage::ModelStorage()
{

}

void ModelStorage::loadObj(const QString &path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(file.isOpen())
    {
        while(!file.atEnd())
        {
            QString textFromFile = file.readLine().data();
            QStringList list = textFromFile.split(" ");
            if (list.count() == 4 && list[0] == "v")
            {
                m_tempVertices.append(QVector3D(list[1].toFloat(),list[2].toFloat(),list[3].toFloat()));
            }
            else if (list.count() == 3 && list[0] == "vt")
            {
                m_tempUvs.append(QVector2D(list[1].toFloat(),list[2].toFloat()));
            }
            else if (list.count() == 4 && list[0] == "vn")
            {
                m_tempNormals.append(QVector3D(list[1].toFloat(),list[2].toFloat(),list[3].toFloat()));
            }
            else if (list.count() == 4 && list[0] == "f" )
            {
                for(int i = 0;i<3;i++)
                {
                    QStringList indexes = list[i+1].split("/");
                    m_vertexIndices.append(indexes[0].toInt());
                    m_uvIndices.append(indexes[1].toInt());
                    m_normalIndices.append(indexes[2].toInt());
                }
            }
            else if (list.count() > 0 && list.count() != 4 && list[0] == "f")
            {
                qWarning() << "ModelStorage::loadObj:" << "Loader supports triangles only.";
                return;
            }
        }
        //build
        foreach (const int i, m_vertexIndices)
        {
            m_vertices.append(m_tempVertices[i-1][0]);
            m_vertices.append(m_tempVertices[i-1][1]);
            m_vertices.append(m_tempVertices[i-1][2]);
        }
        foreach (const int i, m_uvIndices)
        {
            m_uvs.append(m_tempUvs[i-1][0]);
            m_uvs.append(m_tempUvs[i-1][1]);
        }
        foreach (const int i, m_normalIndices)
        {
            m_normals.append(m_tempNormals[i-1][0]);
            m_normals.append(m_tempNormals[i-1][1]);
            m_normals.append(m_tempNormals[i-1][2]);
        }
        m_verticesCount = m_vertexIndices.count();
    }
    else
    {
        qWarning() << "ModelStorage::loadObj:" << "Unable to open file" << path;
    }
}

QVector<float>* ModelStorage::getVertices()
{
    return &m_vertices;
}
QVector<float>* ModelStorage::getUvs()
{
    return &m_uvs;
}
QVector<float>* ModelStorage::getNormals()
{
    return &m_normals;
}
int ModelStorage::verticesCount()
{
    return m_verticesCount;
}
