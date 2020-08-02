import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
  id: root
  title: "AppChat"

  property color backgroundColor: "grey"
  property string login: inputLogin.text
  property string password: inputPassword.text
  property string passwordRepeat: inputPasswordRepeat.text

  signal confirmClicked;

  height: 500
  width: 300

  background: Rectangle {
    anchors.fill: parent
    color: root.backgroundColor
  }

  header: AuthHeader {
    height: 50
    width: root.width

    color: root.backgroundColor
    labelText: "Registration"
  }

  ColumnLayout {
    anchors.verticalCenter: parent.verticalCenter
    height: 100
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: 5
    anchors.rightMargin: 5

    Row {
      Layout.fillHeight: true
      Layout.fillWidth: true

      Image {
        id: imageLogin
        source: "images/avatar.svg"
        anchors.verticalCenter: rectangleLogin.verticalCenter
        height: 35
        width: 35
      }

      Rectangle {
        id: rectangleLogin
        height: 30
        width: parent.width - imagePassword.width;
        radius: 10

        TextInput {
          id: inputLogin
          anchors.fill: parent

          font.pixelSize: 14
          verticalAlignment: Text.AlignVCenter

          focus: true          

          KeyNavigation.tab: inputPassword
          Keys.onReturnPressed: { buttonRegistration.clicked(); }
          Keys.onEnterPressed: { buttonRegistration.clicked(); } // Numpad enter key

          readonly property string placeholderText: "Enter login . . ."

          Text {
            anchors.fill: parent
            text: inputLogin.placeholderText
            color: "#aaa"
            visible: !inputLogin.text
            font.pixelSize: inputLogin.font.pixelSize
            verticalAlignment: inputLogin.verticalAlignment            
          }
        }
      }
    }

    Row {
      Layout.fillHeight: true
      Layout.fillWidth: true

      Image {
        id: imagePassword
        source: "images/key.svg"
        anchors.verticalCenter: rectanglePassword.verticalCenter
        height: 35
        width: 35
      }

      Rectangle {
        id: rectanglePassword
        height: 30
        width: parent.width - imagePassword.width;
        radius: 10

        TextInput {
          id: inputPassword
          anchors.fill: parent

          font.pixelSize: 14
          verticalAlignment: Text.AlignVCenter

          echoMode: TextInput.Password

          KeyNavigation.tab: inputPasswordRepeat
          Keys.onReturnPressed: { buttonRegistration.clicked(); }
          Keys.onEnterPressed: { buttonRegistration.clicked(); } // Numpad enter key

          readonly property string placeholderText: "Enter password . . ."

          Text {
            anchors.fill: parent
            text: inputPassword.placeholderText
            color: "#aaa"
            visible: !inputPassword.text
            font.pixelSize: inputPassword.font.pixelSize
            verticalAlignment: inputPassword.verticalAlignment            
          }
        }
      }
    }

    Row {
      Layout.fillHeight: true
      Layout.fillWidth: true

      Image {
        id: imagePasswordRepeat
        source: "images/keyrepeat.svg"
        anchors.verticalCenter: rectanglePasswordRepeat.verticalCenter
        height: 35
        width: 35
      }

      Rectangle {
        id: rectanglePasswordRepeat
        height: 30
        width: parent.width - imagePasswordRepeat.width;
        radius: 10

        TextInput {
          id: inputPasswordRepeat
          anchors.fill: parent

          font.pixelSize: 14
          verticalAlignment: Text.AlignVCenter

          echoMode: TextInput.Password

          KeyNavigation.tab: inputLogin
          Keys.onReturnPressed: { buttonRegistration.clicked(); }
          Keys.onEnterPressed: { buttonRegistration.clicked(); } // Numpad enter key

          readonly property string placeholderText: "Repeat password . . ."

          Text {
            anchors.fill: parent
            text: inputPasswordRepeat.placeholderText
            color: "#aaa"
            visible: !inputPasswordRepeat.text
            font.pixelSize: inputPasswordRepeat.font.pixelSize
            verticalAlignment: inputPasswordRepeat.verticalAlignment            
          }
        }
      }
    }
  }

  footer: Rectangle {
    height: 50
    width: root.width

    color: root.backgroundColor

    CustomButton {
      id: buttonRegistration
      anchors.fill: parent

      contentText: "Confirm"

      onMouseEnter: { buttonRegistration.opacity = 0.5; }
      onMouseLeave: { buttonRegistration.opacity = 1; }
      onClicked: { root.confirmClicked(); }
    }
  }
}
