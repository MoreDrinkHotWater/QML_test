import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2

import QtQuick.Scene3D 2.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

ApplicationWindow
{
    visible: true
    width: 640
    height: 480
    title: qsTr("3D Viewer")

    menuBar: MenuBar {

        Menu {
            title: qsTr("&File")
            MenuItem
            {
                text: qsTr("Open 3D Model")
                icon.source: "Images/new.png"
                onTriggered: fileOpenDialog.open()
            }
            MenuItem
            {
                text: qsTr("&Exit")
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: qsTr("&Editor")
            MenuItem
            {
                text: qsTr("ReMesh")
                icon.source: "Images/cut.png"

            }
        }
    }

    FileDialog
    {
        id: fileDialog

        title: "Select file"
        folder: shortcuts.home

        nameFilters: [
            "Stl files (*.stl )",
        ]

        onAccepted:
        {
            sceneLoader.source = fileDialog.fileUrl
        }
    }

    Scene3D
    {
        anchors.fill: parent

        // 自动宽高比
        aspects: ["input", "logic"]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

        Entity
        {
            id: sceneRoot

            Camera
            {
                id: camera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 30
                aspectRatio: 16/9
                nearPlane : 0.1
                farPlane : 1000.0
                position: Qt.vector3d( 0, 0.0, 100.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
            }

            // 用于控制相机
            OrbitCameraController
            {
                // 鼠标左键按下，沿x轴或y轴拖动时，移动相机位置
                // 鼠标右键按下，沿x轴或y轴拖动时，控制相机偏转
                camera: camera
            }

            components: [
                RenderSettings
                {
                    // 正向渲染。逐个光源计算的一种渲染方式。
                    activeFrameGraph: ForwardRenderer
                    {
                        clearColor: Qt.rgba(0, 0.5, 1, 1)
                        camera: camera
                    }
                },
                // InputSettings组件必须为场景根实体的组件。
                // 它存储一个指向对象的指针，该对象充当各种输入类要处理的输入事件的源。
                InputSettings
                {
                }
            ]


            Mesh {
                id: mesh
                //                source: "test_data/toyplane.obj"
                source: "test_data/snake.stl"
            }

            // 球体
//            SphereMesh {
//                id: mesh
//                //半径默认为1
//                radius: 15
//                //网格环数
//                rings: 30
//                //网格切片数
//                slices: 50
//            }

            Transform{
                id: torTransform
                scale3D:Qt.vector3d(1.5,1,0.5)
                rotation:fromAxisAndAngle(Qt.vector3d(0,1,0),1)
                property real hAngle:0.0

                matrix:{
                    var m=Qt.matrix4x4();
                    m.rotate(hAngle,Qt.vector3d(0,1,0));

                    // lode_entity 坐标
                    m.translate(Qt.vector3d(0,0,0));
                    return m;

                }
            }

            // 带透明通道的基础Phong光照模型材质
            PhongAlphaMaterial {
                id: material
                // 环境光
                ambient: "gray"

                // 透明度？
                alpha: 1.0
            }

            Entity {
                id: lode_entity
                components: [mesh, material, torTransform]
            }

            Entity
            {
                id: open_Entity
                components: [
                    SceneLoader
                    {
                        id: sceneLoader
                    }
                ]
            }
        }
    }
}
