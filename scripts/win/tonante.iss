; Installer Windows di Tonante (Inno Setup 6)
#ifndef AppVersion
  #define AppVersion "1.0.0"
#endif
[Setup]
AppId={{6E2B6C8A-4F57-4C1E-9C2F-5A1D7B3E9A10}
AppName=Tonante
AppVersion={#AppVersion}
AppPublisher=Tune Talks
DefaultDirName={commonpf64}\Tune Talks\Tonante
DisableDirPage=yes
DisableProgramGroupPage=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputBaseFilename=Tonante-{#AppVersion}-Windows-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayName=Tonante (Tune Talks)

[Languages]
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "en"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "..\..\build\Tonante_artefacts\Release\VST3\Tonante.vst3\*"; DestDir: "{commoncf64}\VST3\Tonante.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Messages]
it.FinishedLabel=Tonante è installato in C:\Program Files\Common Files\VST3. In Ableton Live: Impostazioni → Plug-in → attiva "Use VST3 Plug-in System Folders" → Rescan.
