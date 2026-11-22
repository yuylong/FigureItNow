/*
 * ---------------------------------------------------------------------------
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 * Author: Yulong Yu
 * Copyright(c) 2026 Yulong Yu. All rights reserved.
 * ---------------------------------------------------------------------------
 *
 * Installer control script: unattended upgrade / repair logic.
 *
 * IFW deliberately refuses to install into a directory that already contains an installation:
 * the pre-installation check TargetDirectoryInUse is a hard error that --default-answer cannot
 * dismiss, so running a newer setup.exe over an existing installation just fails. This script
 * implements the upgrade users expect instead: when the target directory holds the maintenance
 * tool of a previous installation, that old installation is purged silently with its own
 * maintenance tool first ("purge" uninstalls every component and removes the program directory,
 * including the maintenance tool itself, the shortcuts, the .fsc file association and the
 * Add/Remove Programs entry), and the wizard then continues as a fresh install into the same
 * directory. Exactly one installation - the new one - is registered afterwards, so Windows
 * "Settings" never shows two entries, and files dropped from the new payload do not linger
 * from the old version.
 *
 * The purge must run elevated: the maintenance tool's manifest is asInvoker, so it inherits
 * the rights of whoever starts it, and it deletes files under Program Files and undoes HKLM
 * registry writes. Script-side execute() always runs in the installer's own process, which is
 * a normal user process in the double-click wizard until the install operations start. The
 * purge is therefore always launched through PowerShell: elevated with ShellExecute runas
 * (UAC prompt) when the installer itself is not elevated, directly when it already runs in an
 * administrator session. The same PowerShell command also waits until the old installation
 * directory is really gone, because the maintenance tool deletes it with a detached helper
 * that outlives the tool process.
 *
 * The GUI wizard runs the purge in the target directory page callback, the last hook before
 * the TargetDirectoryInUse check fires. Command line mode (--confirm-command install)
 * performs that check before any page callback fires, so there the same logic must run from
 * the Controller() constructor instead. Revisiting the target page is a no-op because the
 * maintenance tool is gone after a successful purge.
 */

function UpgradeExistingInstallation()
{
    var targetDir = installer.value("TargetDir");
    var maintenanceTool = targetDir + "/FigureItNow7Maintenance.exe";
    if (!installer.fileExists(maintenanceTool))
        return;

    // The files of a running application are locked, the purge could not delete them and the
    // install would abort with a cryptic error. Ask the user to close it and retry instead.
    if (installer.isProcessRunning("FigureItNow7.exe")) {
        QMessageBox["warning"]("upgrade.appRunning", "FigureItNow7 Setup",
                               "FigureItNow7 is currently running. Close it and run the installer again.");
        return;
    }

    var result;
    // The maintenance tool removes its own directory with a detached helper that keeps
    // deleting after the tool process has exited. Waiting for the process is therefore
    // not enough: the command below also polls until the maintenance tool binary is really
    // gone, otherwise the TargetDirectoryInUse check still sees the old installation and
    // aborts the upgrade run that performed the purge.
    //
    // The double-click wizard is a normal user process at this point, so the maintenance
    // tool is elevated with ShellExecute runas (UAC prompt). Exit code 1223 is
    // ERROR_CANCELLED: the user dismissed that prompt.
    var purgeCommand = "try { $p = Start-Process -FilePath '" + maintenanceTool + "'" +
                      " -ArgumentList 'purge','-c' -Wait -PassThru";
    if (!installer.hasAdminRights())
        purgeCommand += " -Verb RunAs";
    purgeCommand += "; $i = 0" +
                   "; while ((Test-Path -LiteralPath '" + maintenanceTool + "') -and ($i -lt 60))" +
                   " { Start-Sleep -Milliseconds 500; $i++ }" +
                   "; if ($p.ExitCode -ne 0) { exit $p.ExitCode }" +
                   " elseif (Test-Path -LiteralPath '" + maintenanceTool + "') { exit 1 }" +
                   " else { exit 0 } } catch { exit 1223 }";
    result = installer.execute("powershell.exe", ["-NoProfile", "-Command", purgeCommand]);
    if (result.length < 2 || Number(result[1]) !== 0) {
        QMessageBox["warning"]("upgrade.purgeFailed", "FigureItNow7 Setup",
                               "Removing the previous installation failed. Close FigureItNow7 and other programs " +
                               "that use the installation directory, then run the installer again.");
    }
}

function Controller()
{
    // Command line mode runs the TargetDirectoryInUse check before any page callback fires,
    // so the upgrade logic must run here; the GUI wizard runs it from the target directory
    // page callback below instead.
    if (installer.isCommandLineInstance() && installer.isInstaller())
        UpgradeExistingInstallation();
}

Controller.prototype.TargetDirectoryPageCallback = function()
{
    if (installer.isInstaller())
        UpgradeExistingInstallation();
};
