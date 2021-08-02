//import related modules
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Controls.Universal

//window containing the application
ApplicationWindow {
    Universal.theme: darkMode ? Universal.Dark : Universal.Light
    Material.theme: darkMode ? Material.Dark : Material.Light
    //title of the application
    title: qsTr("Hello World")
    width: 640
    height: 480
    visible: true

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
    }

    //Content Area

    //a button in the middle of the content area
    Button {
        text: qsTr("Hello World")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
}