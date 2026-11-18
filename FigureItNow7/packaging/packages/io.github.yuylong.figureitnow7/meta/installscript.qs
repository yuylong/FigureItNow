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
 */

function Component()
{
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
};
