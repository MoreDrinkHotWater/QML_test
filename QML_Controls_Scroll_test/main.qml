import QtQuick 2.1
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2

import Qt3D.Core 2.12
import Qt3D.Render 2.12
//import Qt3D.Input 2.12
import Qt3D.Extras 2.12

import QtQuick.Scene3D 2.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Simple Editor")

    // MenuBar具有层次结构，这是通过Menu的嵌套实现的。
    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem{ action: newAction }
            MenuItem{ action: exitAction }
        }

        Menu {
            title: qsTr("&Edit")
            MenuItem { action: cutAction }
            MenuItem { action: copyAction }
            MenuItem { action: pasteAction }
            MenuSeparator {}
            MenuItem {action: selectAllAction}
        }
    }

    // ToolBar默认没有提供布局，因此我们必须给它设置一个布局。
    //    ToolBar {

    //        Row {
    //            id: row
    //            anchors.fill: parent
    //            //            ToolButton { action: newAction }
    //            //            ToolButton { action: cutAction }
    //            //            ToolButton { action: copyAction }
    //            //            ToolButton { action: pasteAction }
    //        }
    //    }

    // 是整个窗口的主要控件，类似于setCentralWidget()函数调用。
//    TextArea {
//        id: textArea
//        anchors.fill:  parent

        Scene3D{
            anchors.fill: parent

            Entity {
                id: sceneRoot

                // 相机
                Camera {
                    id: camera
                    projectionType: CameraLens.PerspectiveProjection
                    fieldOfView: 450
                    aspectRatio: 16/9
                    nearPlane : 0.1
                    farPlane : 1000.0
                    position: Qt.vector3d( 10.0,0.0, 0.0 )
                    upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                    viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
                }

                FirstPersonCameraController { camera: camera }


                // list< Component3D >容器
                components: [
                    RenderSettings{
                        activeFrameGraph: ForwardRenderer{
                            clearColor: "blue"
                            camera: camera
                        }
                    }/*,
                    InputSettings{}*/
                ]

                // 加载的元素形状网格数据，也就是元素的形状。
                Mesh {
                    id: mesh
                    source: "qrc:/test_data/toyplane.obj"
//                    source: "test_data/snake.stl"
                }

                // 元素的移动旋转等。
                Transform{
                    id: torTransform
                    scale3D:Qt.vector3d(1.5,1,0.5)
                    rotation:fromAxisAndAngle(Qt.vector3d(0,1,0),1)
                    property real hAngle:0.0

                    NumberAnimation on hAngle{
                        from:0
                        to:360.0
                        duration: 10000
                        loops: Animation.Infinite
                    }

                    matrix:{
                        var m=Qt.matrix4x4();
                        m.rotate(hAngle,Qt.vector3d(0,1,0));
                        m.translate(Qt.vector3d(0,0,0));
                        return m;

                    }

                }

                // 元素的渲染方法，比如颜色等。
                PhongAlphaMaterial {
                    id: material
                    ambient: "gray"
                    alpha: 1.0
                }

                // 实体
                Entity {
                    id: entity
                    components: [mesh, material, torTransform]
                }

            }
        }
//    }

        Action {
            id: exitAction
            text: qsTr("E&xit")
            onTriggered: Qt.quit()
        }

        Action {
            id: newAction
            text: qsTr("New")
            icon.source: "Images/new.png"
            onTriggered: fileOpenDialog.open()
        }

        Action {
            id: cutAction
            text: qsTr("Cut")
            icon.source: "Images/cut.png"
            onTriggered: textArea.cut()
        }

        Action {
            id: copyAction
            text: qsTr("Copy")
            icon.source: "Images/copy.png"
            onTriggered: textArea.copy()
        }

        Action {
            id: pasteAction
            text: qsTr("Paste")
            icon.source: "Images/paste.png"
            onTriggered: textArea.paste()
        }

        Action {
            id: selectAllAction
            text: qsTr("Select All")
            onTriggered: textArea.selectAll()
        }

        FileDialog {
            id: fileOpenDialog
            title: "Select an image file"
            folder: shortcuts.home
            nameFilters: [
                "Stl files (*.stl )",
            ]
            onAccepted: {

                aboutDialog.open()

            }

        }

        Dialog {
            id: aboutDialog
            title: qsTr("About")
            Label {
                anchors.fill: parent
                text: qsTr(fileOpenDialog.fileUrl.toString())

                horizontalAlignment: Text.AlignHCenter
            }

            standardButtons: StandardButton.Ok
        }

}

