import QtQuick 2.13
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
  id: root

  height: 400
  width: 500

  property color backgroundColor: "grey"
  property string message: ""
  property int roomId: 0

  ListModel { id: chatMessages }
  function addMessage(msg) {
    chatMessages.append(msg);
  }

  signal buttonChangeClicked;
  signal buttonSendClicked;

  background: Rectangle {
    anchors.fill: parent
    color: root.backgroundColor
  }

  GridLayout {
    anchors.fill: parent

    rows: 3
    columns: 4

    Row {
      Layout.preferredHeight: 20
      Layout.fillWidth: true
      Layout.row: 0
      Layout.column: 0
      Layout.columnSpan: 4
      Layout.margins: 5

      spacing: 10

      Label {
        text: "room id: " + root.roomId
        font.pixelSize: 14
        anchors.verticalCenter: parent.verticalCenter
      }

      Rectangle {
        height: parent.height
        width: 40
        clip: true

        TextInput {
          id: inputRoomId
          anchors.fill: parent
          verticalAlignment: Text.AlignVCenter
        }
      }

      CustomButton {
        id: buttonChange
        height: parent.height
        width: 70
        contentText: "change"
        pressedBackgroundColor: "white"

        onClicked: {
          root.roomId = parseInt(inputRoomId.text);
          chatMessages.clear();
          root.buttonChangeClicked();
        }
      }
    }

    Rectangle {
      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.margins: 5

      Layout.row: 1
      Layout.column: 0
      Layout.columnSpan: 4

      color: "white"
      radius: 5

      ListView {
        id: chatView
        anchors.fill: parent
        anchors.margins: 2

        model: chatMessages
        clip: true

        delegate: TextEdit {
          selectByMouse: true
          text: "[" + datetime + "] " + from + ": " + msg
        }

        onCountChanged: {
          let newIndex = chatView.count - 1;
          chatView.positionViewAtEnd();
          chatView.currentIndex = newIndex;
        }
      }
    }

    Rectangle {
      id: inputRectangle

      Layout.minimumHeight: 55
      Layout.fillWidth: true
      Layout.row: 2
      Layout.column: 0
      Layout.columnSpan: 3
      Layout.margins: 5

      clip: true
      color: "white"
      radius: 10

      Flickable {
        anchors.fill: parent

        TextArea.flickable: TextArea {
          id: inputText
          wrapMode: TextArea.Wrap

          focus: true
          Keys.onReturnPressed: { buttonSend.clicked(); }
          Keys.onEnterPressed: { buttonSend.clicked(); } // Numpad enter key
        }
      }
    }

    CustomButton {
      id: buttonSend

      Layout.minimumHeight: 55
      Layout.preferredWidth: 100
      Layout.row: 2
      Layout.column: 3
      Layout.margins: 5

      radius: 10

      contentText: "S E N D"
      pressedBackgroundColor: "white"

      onMouseEnter: { buttonSend.opacity = 0.5; }
      onMouseLeave: { buttonSend.opacity = 1; }
      onClicked: {
        root.message = inputText.text;
        inputText.text = "";
        root.buttonSendClicked();
      }
    }
  }
}
