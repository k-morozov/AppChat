import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2

ChatWindow {
  id: chatWindow

  title: "AppChat: " + login

  property string login: "";

  Component.onCompleted: { authWindow.show(); }

  onButtonChangeClicked: { control.change_room(chatWindow.roomId); }

  onButtonSendClicked: {
    control.get_text_from_gui(chatWindow.login, chatWindow.message, chatWindow.roomId);
  }

  Connections {
    target: control

    onRegistrationOk: { chatWindow.login = regWindow.login; messageDialogRO.open(); }
    onRegistrationBusy: { messageDialogRB.open(); }

    onAuthorisationOk: { chatWindow.login = authWindow.login; messageDialogAO.open(); }
    onAuthorisationIncorrect: { messageDialogAI.open(); }

    onSend_text_to_gui: {
      let msg = {
        "datetime": Qt.formatDateTime(dt, "hh:mm:ss"),
        "from": login,
        "msg": text
      };

      chatWindow.addMessage(msg);
    }
  }

  AuthorisationWindow {
    id: authWindow

    onRegistrationClicked: {
      authWindow.close();
      regWindow.show();
    }

    onAuthorisationClicked: { control.authorisation(authWindow.login, authWindow.password); }
  }

  RegistrationWindow {
    id: regWindow

    onConfirmClicked: { control.registration(regWindow.login, regWindow.password); }
  }

  MessageDialog {
    id: messageDialogRO
    title: "registration"
    text: "ok"
    onAccepted: { regWindow.close(); chatWindow.show(); }
  }

  MessageDialog {
    id: messageDialogRB
    title: "registration"
    text: "login busy"
  }

  MessageDialog {
    id: messageDialogAO
    title: "authorisation"
    text: "ok"
    onAccepted: { authWindow.close(); chatWindow.show(); }
  }

  MessageDialog {
    id: messageDialogAI
    title: "authorisation"
    text: "incorrect login/password"
  }
}
