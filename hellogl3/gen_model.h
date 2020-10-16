#ifndef GENMODEL_H
#define GENMODEL_H

#include <QVector2D>
#include <QVector3D>

class Common;

class Recognize_cup;

class Recognize_deskLamp;

class Recognize_stool;

class gen_Model
{
public:
    gen_Model();

    ~gen_Model() = default;

    static gen_Model *getInstance();

    bool offset_cup, offset_deskLamp, offset_stool;

public:
    // 圆柱
    void genCylinder(QVector<float> &vec,QVector2D cylinder_center,float r,float z,QVector3D offset);
    // 倾斜圆柱
    void genIncline_Cylinder(QVector<float> &vec,QVector<QVector2D> head_path,float z,QVector3D offset);
    // 对称
    void genSymmetric(QVector<float> &vec,QVector<QVector2D> head_path, QVector<QVector2D> line_path, float height,QVector3D offset);
    // 任意
    void genArbitrary(QVector<float> &vec,QVector<QVector2D> head_path, QVector<QVector2D> line_path_1, QVector<QVector2D> line_path_2, float height_1, float height_2, QVector3D offset);
    // 琦角
    void genMarch_Angle(QVector<float> &vec,QVector<QVector2D> head_path, QVector<QVector2D> line_path, QVector3D offset);
    // 画花生
    void genPeanut(QVector<float> &vec, QVector<QVector2D> line_path, QVector3D offset);

    // line
    void genLine(QVector<float> &vec, QVector<QVector2D> line_path, float width_var, QVector3D offset);

    // circle
    void genCircle(QVector<float> &vec, QVector<QVector2D> line_path, QVector3D offset);

    // extrude
    void genExtrude(QVector<float> &vec, QVector<QVector2D> line_path, float width_var, float up_var, float down_var, QVector3D offset);

private:
    Common *common;

    Recognize_cup *recognizeCup;

    Recognize_deskLamp *recognizeDeskLamp;

    Recognize_stool *recognizeStool;

private:
    // 记录圆柱的底部中心
    QVector3D centerBottom;
};

#endif // GENMODEL_H
