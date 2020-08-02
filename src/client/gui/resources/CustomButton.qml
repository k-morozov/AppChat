import QtQuick 2.0
import QtQuick.Controls 2.12

Rectangle {
  id: root

  property color backgroundColor: "blue"
  property color pressedBackgroundColor: backgroundColor
  property string contentText: ""
  readonly property bool pressed: mouseArea.pressed

  signal mouseEnter;
  signal mouseLeave;
  signal clicked;

  color: root.pressed ? root.pressedBackgroundColor : root.backgroundColor;

  Text {
    anchors.centerIn: parent

    text: root.contentText
    font.pixelSize: 16
    font.bold: true
  }

  MouseArea {
    id: mouseArea
    anchors.fill: parent
    hoverEnabled: true

    onEntered: { root.mouseEnter(); }
    onExited: { root.mouseLeave(); }
    onClicked: { root.clicked(); }
  }
}
