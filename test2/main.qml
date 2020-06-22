import QtQuick.Controls 2.2
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

    header: ToolBar
    {
        ToolButton
        {
            text: "Open 3D Model"
            onPressed:
            {
                fileDialog.open()
            }
        }
    }

    FileDialog
    {
        id: fileDialog
        onAccepted:
        {
            sceneLoader.source = fileDialog.fileUrl
        }
    }

    Scene3D
    {
        anchors.fill: parent

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
                position: Qt.vector3d( 10.0, 0.0, 100.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
            }

            OrbitCameraController
            {
                camera: camera
            }

            components: [
                RenderSettings
                {
                    activeFrameGraph: ForwardRenderer
                    {
                        clearColor: Qt.rgba(0, 0.5, 1, 1)
                        camera: camera
                    }
                },
                InputSettings
                {
                }
            ]

            Entity
            {
                id: monkeyEntity
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
