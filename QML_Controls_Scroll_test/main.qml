import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2

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
    ToolBar {

        Row {
            id: row
            anchors.fill: parent
            ToolButton { action: newAction }
//            ToolButton { action: cutAction }
//            ToolButton { action: copyAction }
//            ToolButton { action: pasteAction }
        }
    }

    // 是整个窗口的主要控件，类似于setCentralWidget()函数调用。
    TextArea {
        id: textArea
        anchors.bottom:  row

        // Repeater
        Column {
            y:35
            spacing: 2
            Repeater {
                model: ListModel {
                    ListElement { name: "Mercury"; surfaceColor: "gray" }
                    ListElement { name: "Venus"; surfaceColor: "yellow" }
                    ListElement { name: "Earth"; surfaceColor: "blue" }
                    ListElement { name: "Mars"; surfaceColor: "orange" }
                    ListElement { name: "Jupiter"; surfaceColor: "orange" }
                    ListElement { name: "Saturn"; surfaceColor: "yellow" }
                    ListElement { name: "Uranus"; surfaceColor: "lightBlue" }
                    ListElement { name: "Neptune"; surfaceColor: "lightBlue" }
                }

                Rectangle {
                    width: 100
                    height: 20
                    radius: 3
                    color: "lightBlue"
                    Text {
                        anchors.centerIn: parent
                        text: name
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.leftMargin: 2

                        width: 16
                        height: 16
                        radius: 8
                        border.color: "black"
                        border.width: 1

                        color: surfaceColor
                    }
                }
            }
        }
    }

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
            folder: shortcuts.documents
            nameFilters: [
                "Image files (*.png *.jpeg *.jpg)",
            ]
            onAccepted: {
                image.source = fileOpenDialog.fileUrl
            }
        }
}
