import QtQuick 2.12
import QtQuick.Window 2.12
// 根元素： Window
Window {
    id: root
    visible: true
    width: 640
    height: 480
    // qsTr()函数就是 QObject::tr()函数的 QML 版本，用于返回可翻译的字符串。
    title: qsTr("社会人")

    Rectangle {
        id:rect
        width: 100; height: 100
        // 使用 parent 引用父元素
        anchors.centerIn: parent
        color: "green"

        Image {
            id: pig
            // anchors.centerIn: rect
            // 子元素的 x 和 y 的坐标值始终相对于父元素。
            x:10; y:10
            source: "IMages/1.png"
        }

        MouseArea {
            // anchors: 锚点，充满整个矩形
            anchors.fill: rect
            // 回调函数 onClicked

            onClicked:
            {
                parent.color='red'
            }

        }

    }

    Text {
        id: test
        //  test 位置定义为 parent 中心
        anchors.left: root
        text: qsTr("小猪佩奇！")

        ColorAnimation on color{
            from: "red"
            to: "blue"
            duration: 5000
        }
    }
}
