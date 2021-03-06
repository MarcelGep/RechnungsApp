; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "RechungsApp"
#define MyAppVersion "1.0.0.2"
#define MyAppExeName "RechnungsApp.exe"
#define MyAppID "{BEEC3EE5-086F-4CAC-8000-11A20162DB3F}"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{#MyAppID}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
DefaultDirName={sd}\{#MyAppName}
DefaultGroupName=RechnungsApp
AllowNoIcons=yes
OutputDir=C:\Users\Marcel\Desktop
OutputBaseFilename=RechnungsApp-Setup_v{#MyAppVersion}
SetupIconFile=C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\rechnungsapp_icon.ico
Compression=lzma
SolidCompression=yes

;[Setup] 
;PrivilegesRequired=admin 

[Languages]
Name: "german"; MessagesFile: "compiler:Languages\German.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Files]
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\RechnungsApp.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\database\*"; DestDir: "{app}\database"; Flags: onlyifdoesntexist recursesubdirs createallsubdirs
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\sqldrivers\*"; DestDir: "{app}\sqldrivers"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\Qt5PrintSupport.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\Qt5Sql.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Marcel\Documents\Develop\build-RechnungsApp-Desktop-Release\release\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Dirs]
Name: {app}\invoice

[Icons]
Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Messages]
BeveledLabel=RechnungsApp Installer version 1.0 � 2018 Marcel Gepr�gs

[CustomMessages]
german.NewerVersionExists=Es ist bereits schon eine neuere Version von {#MyAppName} installiert.%n%nInstaller Version: {#MyAppVersion}%nVorhandene Version: 

[Code]
// find current version before installation
function InitializeSetup: Boolean;
var Version: String;
begin
  if RegValueExists(HKEY_LOCAL_MACHINE,'Software\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppID}_is1', 'DisplayVersion') then
    begin
      RegQueryStringValue(HKEY_LOCAL_MACHINE,'Software\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppID}_is1', 'DisplayVersion', Version);
      if Version > '{#MyAppVersion}' then
        begin
          MsgBox(ExpandConstant('{cm:NewerVersionExists} '+Version), mbInformation, MB_OK);
          Result := False;
        end
      else
        begin
          Result := True;
        end
    end
  else
    begin
      Result := True;
    end
end;