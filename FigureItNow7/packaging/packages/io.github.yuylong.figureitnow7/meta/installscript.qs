/*
 * ---------------------------------------------------------------------------
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 * Author: Yulong Yu
 * Copyright(c) 2026 Yulong Yu. All rights reserved.
 * ---------------------------------------------------------------------------
 *
 * Component script of the FigureItNow7 package. binarycreator picks it up from
 * the component's meta directory; createOperations() runs after the payload is
 * extracted, and that is where the shortcuts are created. <StartMenuDir> in
 * config.xml only names the Start menu group, it does not create anything.
 *
 * The .fsc file association is registered by the RegisterFileType operation: it
 * writes the ProgId, the shell open command, the OpenWithProgIds entry and the
 * icon reference under Software\Classes, refreshes the shell icon cache, and
 * removes all of it again on uninstall.
 *
 * AllUsers is set to true so the association lands in HKLM\Software\Classes
 * instead of the per-user hive: the application installs into Program Files with
 * administrator rights, so the file type belongs to the machine, not to the
 * installing user. The variable is persisted into the maintenance tool
 * configuration, hence the uninstaller removes the keys from the same hive, and
 * the Start menu shortcuts move to the all-users location to match.
 *
 * The application component is mandatory but does NOT use ForcedInstallation:
 * IFW renders forced components without any check box at all. Disabling the
 * component instead keeps the check box visible - grayed out, checked, and
 * immune to clicks as well as to the "reset to default" / "uncheck all" actions
 * of the component selection page, because the model skips disabled components
 * when it recalculates check states.
 */

function Component()
{
    // Must be set before createOperations() runs: RegisterFileType and the shortcut
    // operations read it when they perform, and the maintenance tool persists it so
    // uninstall undoes the registry writes in the same hive.
    installer.setValue("AllUsers", "true");

    // Lock the mandatory application component: grayed, checked, untouchable.
    component.enabled = false;
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.kernelType !== "winnt") {
        return;
    }

    var exe = "@TargetDir@/FigureItNow7.exe";
    var description = "Draw mathematical figures with a small script language.";

    component.addOperation("CreateShortcut", exe, "@StartMenuDir@/FigureItNow7.lnk",
                           "workingDirectory=@TargetDir@", "description=" + description);
    component.addOperation("CreateShortcut", exe, "@DesktopDir@/FigureItNow7.lnk",
                           "workingDirectory=@TargetDir@", "description=" + description);

    // Arguments: <extension> <command> [description [contentType [icon]]]. The
    // command is stored verbatim in the shell key, so the exe path and the %1
    // placeholder must be double-quoted to survive paths with spaces. The content
    // type is left empty (FIN script has no registered MIME type), and the icon
    // references the exe's first embedded icon; it shows the generic executable icon
    // until a version resource with an icon is added.
    //
    // @TargetDir@ is only substituted when the operation performs, so building the
    // command from the placeholder would leave the exe path with forward slashes,
    // which the shell refuses to launch with "Access is denied". Resolve the value
    // through installer.value() instead and normalize it to native separators. The
    // shortcut operations above are not affected, shortcut paths tolerate slashes.
    //
    // No ProgId=... argument is passed on purpose: the operation strips it from its
    // recorded arguments at perform time, so at uninstall the undo pass no longer
    // sees it, falls back to the default ProgId and would clean "fsc_auto_file"
    // instead of the registered keys, leaving them behind in the registry. With the
    // default ProgId both passes agree and uninstall removes everything it wrote.
    var nativeExe = installer.value("TargetDir").split("/").join("\\") + "\\FigureItNow7.exe";
    component.addOperation("RegisterFileType",
                           "fsc",
                           "\"" + nativeExe + "\" \"%1\"",
                           "FIN Script",
                           "",
                           nativeExe + ",0");
};
