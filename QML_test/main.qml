import QtQuick 2.12
import QtQuick.Window 2.12
// 根元素： Window
Window {
    id: root
    visible: true
    width: 720
    height: 720
    // qsTr()函数就是 QObject::tr()函数的 QML 版本，用于返回可翻译的字符串。
    title: qsTr("社会人")

    Grid {
        id: grid
        // 使用 parent 引用父元素
        anchors.centerIn: parent
        spacing: 8

        Rectangle {
            id:rect
            width: 100; height: 100
            color: "green"

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

        Rectangle {
            id: gradient_rect
            width: 100; height: 100
            border {
                color: "Blue"
                width: 4
            }

            // 渐变
            gradient: Gradient {
                GradientStop { position: 0.0; color: "red" }
                GradientStop { position: 0.33; color: "yellow" }
                GradientStop { position: 1.0; color: "green" }
            }

            // This property holds the corner radius used to draw a rounded rectangle.
            radius: 8
        }

        Rectangle{
            id: rect_Image
            width: 140; height: 140
            Image {
                id: pig
                // 子元素的 x 和 y 的坐标值始终相对于父元素。
                source: "IMages/1.png"

                focus: true

                Keys.onLeftPressed: pig.x -= 8
                Keys.onRightPressed: pig.x += 8
                Keys.onUpPressed: pig.y -= 8
                Keys.onDownPressed: pig.y += 8

                Keys.onPressed: {
                    switch(event.key) {
                        case Qt.Key_Plus:
                            pig.scale += 0.2
                            break;
                        case Qt.Key_Minus:
                            pig.scale -= 0.2
                            break;
                    }
                }
            }

            MouseArea{
                anchors.fill: rect_Image
                drag.target: rect_Image
            }
        }

        Rectangle {
            id: rect_Repeater
            width: 252
            height: 252
            color: "black"
            property variant colorArray: ["red", "green", "yellow"]

            // Grid 默认 4 行
            Grid {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4
                Repeater {
                    model: 16
                    Rectangle {
                        width: 56; height: 56
                        property int colorIndex: Math.floor(Math.random()*3)
                        color: rect_Repeater.colorArray[colorIndex]
                        border.color: Qt.lighter(color)
                        Text {
                            anchors.centerIn: parent
                            color: "black"
                            text: "Cell " + index
                        }
                    }
                }
            }
        }

        // 输入元素
        Rectangle{
            width: 120; height: 100
            color: "linen"

            LineEdit {
                id: input1
                x: 8; y: 8
                width: 96; height: 20
                focus: true
                //                text: "text1"
                // KeyNavigation是一个附加属性。当用户点击了指定的按键时，属性指定的组件就会获得焦点。
                KeyNavigation.tab: input2
            }

            LineEdit {
                id: input2
                x: 8; y: 36
                width: 96; height: 20
                //                text: "text2"
                KeyNavigation.tab: input1
            }
        }
    }




    Text {
        id: test
        //  test 位置定义为 parent 中心

        text: qsTr("小猪佩奇！")

        ColorAnimation on color{
            from: "red"
            to: "blue"
            duration: 5000
        }
    }
}
