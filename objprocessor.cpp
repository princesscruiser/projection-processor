#include "objprocessor.h"
#include <QFile>
#include <QTextStream>
#include <QFile>
#include <polygon.h>
#include <QVector>
#include <QVector3D>

OBJprocessor::OBJprocessor()
{

}

OBJobject OBJprocessor::parse(QFile &file2parse)
{

    OBJobject obj2return = OBJobject();

    float xcor,ycor,zcor;
    xcor=0;
    ycor=0;
    zcor=0;

    QString vertMark = QString("v");
    QString txCoorMark = QString("vt");
    QString normalsMark = QString("vn");
    QString vParamMark = QString("vp");
    QString surfaceMark = QString("f");
    QString groupMark = QString("g");

    QTextStream in(&file2parse);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList splitedStr = line.split( " ",  QString::SkipEmptyParts);

        if (splitedStr.value(0) == vertMark){
            xcor = splitedStr.value(1).toFloat();
            ycor = splitedStr.value(2).toFloat();
            zcor = splitedStr.value(3).toFloat();
            obj2return.vertices.append(QVector3D(xcor,ycor,zcor));
        }
        if (splitedStr.value(0) == surfaceMark){
            obj2return.faces.append(Polygon(splitedStr));
        }
    }
    return obj2return;
}

bool OBJprocessor::read(const QString &objfilename, OBJobject &obj2return)
{

    QFile objFile(objfilename);
    if (!objFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    obj2return = parse(objFile);

    return true;
}

QVector<Polygon> OBJprocessor::triangulate(const QVector<Polygon> &inpPolygonArr)
{
    Q_ASSERT(!inpPolygonArr.isEmpty());
    QVector<Polygon> returnArray;

    for(int polygonIndex=0; polygonIndex<inpPolygonArr.length(); polygonIndex++){
        const int nVertices = inpPolygonArr[polygonIndex].vertArr.length()-1;
        for (int polygVrtIndex = 1; polygVrtIndex < nVertices; polygVrtIndex++){
            Polygon polygon = inpPolygonArr[polygonIndex];
            QVector<int> locVVector;
            locVVector << polygon.vertex(0);
            locVVector << polygon.vertex(polygVrtIndex);
            locVVector << polygon.vertex(polygVrtIndex+1);
            Polygon polygForAdd;
            polygForAdd = Polygon(locVVector);
            returnArray.append(polygForAdd);
        }
    }
    return returnArray;
}

///Computes mean normals to every vertice, arranging it in the same order as a vertice array.
QVector<QVector3D> OBJprocessor::computeNormals(const QVector<Polygon> &inputPolygonArr, const QVector<QVector3D> &inputVertArr)
{
    int amountOfVerts = inputVertArr.count();
    int amountOfPolygons = inputPolygonArr.count();

    QVector<QVector3D> arr2return(amountOfVerts);
    QVector<QVector3D> emptyArr;
//    QVector<int> timesVertAppear(amountOfVerts);

//    for(int polygIndex=0; polygIndex<amountOfPolygons; polygIndex++){
//        for(int vertIndex=0; vertIndex<3; vertIndex++){
//            int verticeNumber = inputPolygonArr[polygIndex].vertArr[vertIndex];
//            timesVertAppear[verticeNumber-1]++;
//        }
//    }

    int polygonCount = inputPolygonArr.count();
    for(int polygonIndex=0; polygonIndex<polygonCount;polygonIndex++){
        int numOfPolygonVerts = inputPolygonArr[polygonIndex].vertArr.count();
        if (numOfPolygonVerts > 3){
            return emptyArr;
        }

        for (int polygVertIndex = 0; polygVertIndex < numOfPolygonVerts; polygVertIndex++){
            int vertNum1_isCurrent;
            int vertNum2;
            int vertNum3;
            switch (polygVertIndex) {
            case 0:
                vertNum1_isCurrent = inputPolygonArr[polygonIndex].vertArr[polygVertIndex];
                vertNum2 = inputPolygonArr[polygonIndex].vertArr[polygVertIndex+1];
                vertNum3 = inputPolygonArr[polygonIndex].vertArr[polygVertIndex+2];
                break;
            case 1:
                vertNum1_isCurrent = inputPolygonArr[polygonIndex].vertArr[polygVertIndex];
                vertNum2 = inputPolygonArr[polygonIndex].vertArr[polygVertIndex-1];
                vertNum3 = inputPolygonArr[polygonIndex].vertArr[polygVertIndex+1];
                break;
            case 2:
                vertNum1_isCurrent = inputPolygonArr[polygonIndex].vertArr[polygVertIndex];
                vertNum2 = inputPolygonArr[polygonIndex].vertArr[polygVertIndex-1];
                vertNum3 = inputPolygonArr[polygonIndex].vertArr[polygVertIndex-2];
                break;
            }

            float xvNum1 = inputVertArr[vertNum1_isCurrent-1].x();
            float yvNum1 = inputVertArr[vertNum1_isCurrent-1].y();
            float zvNum1 = inputVertArr[vertNum1_isCurrent-1].z();

            float xvNum2 = inputVertArr[vertNum2-1].x();
            float yvNum2 = inputVertArr[vertNum2-1].y();
            float zvNum2 = inputVertArr[vertNum2-1].z();


            float xvNum3 = inputVertArr[vertNum3-1].x();
            float yvNum3 = inputVertArr[vertNum3-1].y();
            float zvNum3 = inputVertArr[vertNum3-1].z();

            float xOne, yOne, zOne = 0;
            float xTwo, yTwo, zTwo = 0;

            xOne = xvNum2 - xvNum1;
            yOne = yvNum2 - yvNum1;
            zOne = zvNum2 - zvNum1;

            xTwo = xvNum3 - xvNum1;
            yTwo = yvNum3 - yvNum1;
            zTwo = zvNum3 - zvNum1;

            QVector3D productOfVectors;
            QVector3D leftVector(xOne, yOne, zOne);
            QVector3D rightVector(xTwo, yTwo, zTwo);

            productOfVectors = QVector3D::crossProduct(leftVector,rightVector);
            arr2return[vertNum1_isCurrent-1] += productOfVectors;
        }
    }

    for(int indexOfNormals=0; indexOfNormals < arr2return.count(); indexOfNormals++){
        arr2return[indexOfNormals].normalize();
    }
    return arr2return;
}



