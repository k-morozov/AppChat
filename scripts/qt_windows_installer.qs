function Controller() {
    installer.autoRejectMessageBoxes();

    installer.installationFinished.connect(function() {
        console.log("Finished")
        gui.clickButton(buttons.NextButton);
    })
}

Controller.prototype.WelcomePageCallback = function() {
    console.log("Step: " + gui.currentPageWidget());

    gui.clickButton(buttons.NextButton, 5000);
}

Controller.prototype.CredentialsPageCallback = function() {
    console.log("Step: " + gui.currentPageWidget());

    gui.currentPageWidget().loginWidget.EmailLineEdit.setText("mixas1k@list.ru");
    gui.currentPageWidget().loginWidget.PasswordLineEdit.setText("zx3021ZX3021");

    gui.clickButton(buttons.NextButton, 5000);
}

Controller.prototype.IntroductionPageCallback = function() {
    console.log("Step: " + gui.currentPageWidget());

    gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function() {
    console.log("Step: " + gui.currentPageWidget());

    gui.currentPageWidget().TargetDirectoryLineEdit.setText("C:/Qt");

    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ComponentSelectionPageCallback = function() {
    console.log("Step: " + gui.currentPageWidget());

    var page = gui.currentPageWidget();
    page.deselectAll();

    for (var i = 0; i < INSTALL_COMPONENTS.length; i++) {
        console.log(INSTALL_COMPONENTS[i]);
    }

    gui.clickButton(buttons.NextButton);
}

Controller.prototype.LicenseAgreementPageCallback = function() {
    console.log("Step: " + gui.currentPageWidget());

    gui.currentPageWidget().AcceptLicenseRadioButton.setChecked(true);
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.StartMenuDirectoryPageCallback = function() {
    console.log("Step: " + gui.currentPageWidget());

    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function()
{
    console.log("Step: " + gui.currentPageWidget());

    gui.clickButton(buttons.NextButton);
}

Controller.prototype.FinishedPageCallback = function() {
    console.log("Step: " + gui.currentPageWidget());

    var checkBoxForm = gui.currentPageWidget().LaunchQtCreatorCheckBoxForm
    if (checkBoxForm && checkBoxForm.launchQtCreatorCheckBox) {
        checkBoxForm.launchQtCreatorCheckBox.checked = false;
    }

    gui.clickButton(buttons.FinishButton);
} 
