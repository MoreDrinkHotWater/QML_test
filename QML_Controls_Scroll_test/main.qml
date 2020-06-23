import QtQuick 2.1
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2

import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Input 2.12
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
            MenuItem
            {
                text: qsTr("New")
                icon.source: "Images/new.png"
                onTriggered: fileOpenDialog.open()
            }
            MenuItem
            {
                text: qsTr("E&xit")
                onTriggered: Qt.quit()
            }
        }
	
	FileDialog {
        id: fileOpenDialog
        title: "Select an image file"
        folder: shortcuts.home
        nameFilters: [
            "Stl files (*.stl )",
        ]
        onAccepted: {
            sceneLoader.source = fileOpenDialog.fileUrl
        }

    }

    }
    
    Scene3D{
        anchors.fill: parent

        aspects: ["input", "logic"]
        // 自动长宽比
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

        Entity {
            id: sceneRoot

            // 相机
            Camera {
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

            // 固定相机
            OrbitCameraController { camera: camera }

            // list< Component3D >容器
            components: [

                RenderSettings{
                    activeFrameGraph: ForwardRenderer{
                        clearColor: "blue"
                        camera: camera
                    }
                },

                InputSettings
		{
		}
            ]

            Entity
            {
                id: entity
                components: [
                    SceneLoader
                    {
                        id: sceneLoader

                    }
                ]
            }

        }
    }

    Action {
        id: exitAction
    }

    Action {
        id: newAction
    }

}

