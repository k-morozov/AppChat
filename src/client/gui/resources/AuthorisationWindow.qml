import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
  id: root
  title: "AppChat"

  property color backgroundColor: "grey"
  property string login: ""
  property string password: ""

  signal registrationClicked;
  signal authorisationClicked;

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
    labelText: "Autorisation"
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
        clip: true

        TextInput {
          id: inputLogin
          anchors.fill: parent
          anchors.margins: 3

          font.pixelSize: 14
          verticalAlignment: Text.AlignVCenter

          focus: true

          KeyNavigation.tab: inputPassword
          Keys.onReturnPressed: { buttonAuthorisation.clicked(); }
          Keys.onEnterPressed: { buttonAuthorisation.clicked(); } // Numpad enter key
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
        clip: true

        TextInput {
          id: inputPassword
          anchors.fill: parent
          anchors.margins: 3

          font.pixelSize: 14
          verticalAlignment: Text.AlignVCenter

          echoMode: TextInput.Password

          KeyNavigation.tab: inputLogin
          Keys.onReturnPressed: { buttonAuthorisation.clicked(); }
          Keys.onEnterPressed: { buttonAuthorisation.clicked(); } // Numpad enter key
        }
      }
    }
  }

  footer: Rectangle {
    height: 50
    width: root.width

    color: root.backgroundColor

    GridLayout {
      anchors.fill: parent
      rows: 1
      columns: 2

      CustomButton {
        id: buttonRegistration

        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.column: 0

        contentText: "Sign up"

        onMouseEnter: { buttonRegistration.opacity = 0.5; }
        onMouseLeave: { buttonRegistration.opacity = 1; }
        onClicked: {
          root.login = inputLogin.text;
          root.password = inputPassword.text;
          root.registrationClicked();
        }
      }

      CustomButton {
        id: buttonAuthorisation

        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.column: 1

        contentText: "Sign in"
        pressedBackgroundColor: "white"

        onMouseEnter: { buttonAuthorisation.opacity = 0.5; }
        onMouseLeave: { buttonAuthorisation.opacity = 1; }
        onClicked: {
          root.login = inputLogin.text;
          root.password = inputPassword.text;
          root.authorisationClicked();
        }

      }
    }
  }
}
