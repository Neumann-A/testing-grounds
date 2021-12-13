//import related modules
import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Controls.Material
import QtQuick.Controls.Universal
import QtQuick.Window
import QtQuick.Controls

//window containing the application
ApplicationWindow {
    Universal.theme: darkMode.position < 1.0 ? Universal.Dark : Universal.Light
    Material.theme: darkMode.position  < 1.0 ? Material.Dark : Material.Light
    //title of the application
    title: qsTr("Hello World")
    id: root
    width: 640
    height: 480
    visible: true
    Component.onCompleted: {
        root.x = Screen.width / 2 - root.width / 2;
        root.y = Screen.height / 2 - root.height / 2;
    }

    header: ToolBar {
        // ...
        

    }



    //menu containing two menu items
    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
        Menu {
            title: qsTr("Second Tag")
            MenuItem {
                text: qsTr("Settings")
                onTriggered: console.log("Open setttings triggered");
            }
            MenuItem {
                text: qsTr("About")
                onTriggered: console.log("Open setttings triggered");
            }
        }
        Menu {
            title: qsTr("Options")
            Switch {
                id: darkMode
                anchors.right: parent.anchors.right
                anchors.top: parent.anchors.top
                anchors.margins: 10
                text: "Dark theme"
                checked: false
            }
        }
        
    }


    //Content Area

    //a button in the middle of the content area
    Button {
        text: qsTr("Hello World!")
        anchors.horizontalCenter: parent.contentItem.horizontalCenter
        anchors.verticalCenter: parent.contentItem.verticalCenter
    }
}