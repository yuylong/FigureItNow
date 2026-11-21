/*
 * ---------------------------------------------------------------------------
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 * Author: Yulong Yu
 * Copyright(c) 2026 Yulong Yu. All rights reserved.
 * ---------------------------------------------------------------------------
 *
 * Component script of the FigureItNow7 examples package. The payload is the
 * examples/ directory staged by build_installer.ps1; createOperations() only
 * adds a Start menu shortcut that opens the installed examples folder, so the
 * sample scripts are discoverable without digging into Program Files.
 *
 * The shortcut target is a directory, not an executable: Windows .lnk files
 * point at folders just fine and Explorer opens them. IFW records the
 * operation, so uninstall removes the shortcut together with the payload.
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

    component.addOperation("CreateShortcut",
                           "@TargetDir@/examples",
                           "@StartMenuDir@/Example Scripts.lnk",
                           "workingDirectory=@TargetDir@",
                           "description=FIN script examples");
};
