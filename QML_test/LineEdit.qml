import QtQuick 2.0

Item {
    FocusScope {
        width: 96;
        height: input.height + 8
        // color: "lightsteelblue"
        // border.color: "gray"

        //增加一个 Rectangle 用作背景色和边框
        Rectangle {
            anchors.fill:parent
            color:"lightsteelblue"
            border.color: "gray"
        }

        // 让外界可以直接设置TextInput的text属性，我们给这个属性声明了一个别名。
        // 同时，为了让外界可以访问到内部的textInput，我们将这个子组件也暴露出来。
        property alias text: input.text
        property alias input: input

        TextInput {
            id: input
            height:20
            anchors.fill: parent
            anchors.margins: 4
            focus: true
        }
    }
}
