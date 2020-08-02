import QtQuick 2.0
import QtQuick.Controls 2.12

Rectangle {
  id: root

  property string labelText: ""

  Label {
    anchors.centerIn: parent

    text: root.labelText
    font.pixelSize: 16
    font.bold: true
  }

  Rectangle {
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: 10
    anchors.rightMargin: 10

    height: 5
    radius: 10

    color: "black"
  }
}
