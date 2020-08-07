#ifndef IDENTIFICATION_RELATION_H
#define IDENTIFICATION_RELATION_H
#include <QVector2D>

class Common;

class Identification_relation
{
public:
    Identification_relation();

    ~Identification_relation() = default;

    static Identification_relation *getInstance();

public:

    void find_cylinderNode(QVector<float> vec);

    void find_straightLineNode(QVector<float> vec);

    void find_curveLineNode(QVector<float> vec);

    void find_wavyLineNode(QVector<float> vec);

    float height;

    // 相互关系: 平行，相连, 相离，垂直,等长
    bool parallel(QVector<float> _vec1, QVector<float> _vec2);
    bool equilong(QVector<float> _vec1, QVector<float> _vec2);

    bool join(QString str_1, QString str_2, QVector<float> _vec1, QVector<float> _vec2);
    bool separation(QString str_1, QString str_2, QVector<float> _vec1, QVector<float> _vec2);
    bool verticality(QString str_1, QString str_2, QVector<float> _vec1, QVector<float> _vec2);

private:
    Common *common;
    // 记录结点的坐标值
    QVector2D cylinder_left,cylinder_right;
    QVector2D straightLine_first, straightLine_end;
    QVector2D curveLine_first, curveLine_end;
    QVector2D wavyLine_first, wavyLine_end;

};

#endif // IDENTIFICATION_RELATION_H
