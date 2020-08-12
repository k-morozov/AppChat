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
    var page = gui.currentPageWidget();
    console.log("Step: " + page);

    page.loginWidget.EmailLineEdit.setText("mixas1k@list.ru");
    page.loginWidget.PasswordLineEdit.setText("zx3021ZX3021");

    gui.clickButton(buttons.NextButton, 5000);
}

Controller.prototype.ObligationsPageCallback = function() {
    var page = gui.currentPageWidget();
    console.log("Step: " + page);

    page.obligationsAgreement.setChecked(true);
    page.completeChanged();

    gui.clickButton(buttons.NextButton);
}

Controller.prototype.IntroductionPageCallback = function() {
    console.log("Step: " + gui.currentPageWidget());

    gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function() {
    var page = gui.currentPageWidget();
    console.log("Step: " + page);

    page.TargetDirectoryLineEdit.setText("C:/Qt");

    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ComponentSelectionPageCallback = function() {
    var page = gui.currentPageWidget();
    console.log("Step: " + page);

    page.deselectAll();
    page.selectComponent("qt.qt5.5142.win64_mingw73");

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
