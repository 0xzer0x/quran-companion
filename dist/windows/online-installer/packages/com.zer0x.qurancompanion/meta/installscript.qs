function Component()
{
    // constructor
    if (installer.isInstaller() && !installer.addWizardPage(component, "Page", QInstaller.ReadyForInstallation) )
        console.log("Could not add the shortcuts page.");
}

Component.prototype.isDefault = function()
{
    // select the component by default
    return true;
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();
		var page = component.userInterface("Page");
            if (page.chkDesktop.checked) {
                component.addOperation("CreateShortcut", "@TargetDir@/quran-companion.exe", "@DesktopDir@/Quran Companion.lnk", "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/quran-companion.exe");
            }
            if (page.chkStart.checked){
                component.addOperation("CreateShortcut", "@TargetDir@/quran-companion.exe", "@StartMenuDir@/Quran Companion.lnk",
                            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/quran-companion.exe", "description=Read the Quran");
            }
    } catch (e) {
        console.log(e);
    }
}
