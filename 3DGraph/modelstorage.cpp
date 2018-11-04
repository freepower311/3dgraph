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
                    QStringList indices = list[i+1].split("/");
                    m_vertexIndices.append(indices[0].toInt());
                    m_uvIndices.append(indices[1].toInt());
                    m_normalIndices.append(indices[2].toInt());
                }
            }
            else if (list.count() > 0 && list.count() != 4 && list[0] == "f")
            {
                qWarning() << "ModelStorage::loadObj: Loader supports triangles only";
                return;
            }
        }
        const QString parseErrorMessage = "ModelStorage::loadObj: Parse error";
        if (m_vertexIndices.count() != m_uvIndices.count()
                || m_vertexIndices.count() != m_normalIndices.count())
        {
            qWarning() << parseErrorMessage;
            return;
        }
        foreach (const int i, m_vertexIndices)
        {
            if (i > m_tempVertices.count())
            {
                qWarning() << parseErrorMessage;
                return;
            }
            m_vertices.append(m_tempVertices[i-1][0]);
            m_vertices.append(m_tempVertices[i-1][1]);
            m_vertices.append(m_tempVertices[i-1][2]);
        }
        foreach (const int i, m_uvIndices)
        {
            if (i > m_tempUvs.count())
            {
                qWarning() << parseErrorMessage;
                return;
            }
            m_uvs.append(m_tempUvs[i-1][0]);
            m_uvs.append(m_tempUvs[i-1][1]);
        }
        foreach (const int i, m_normalIndices)
        {
            if (i > m_tempNormals.count())
            {
                qWarning() << parseErrorMessage;
                return;
            }
            m_normals.append(m_tempNormals[i-1][0]);
            m_normals.append(m_tempNormals[i-1][1]);
            m_normals.append(m_tempNormals[i-1][2]);
        }
    }
    else
    {
        qWarning() << "ModelStorage::loadObj: Unable to open file" << path;
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
int ModelStorage::coordCount()
{
    return m_vertices.count();
}
