[Setup]
AppName=${PROJECT_NAME}
AppVersion=${VERSION}
DefaultDirName={commonpf64}\Dimethoxy\${PROJECT_NAME}
DefaultGroupName=${PROJECT_NAME}
OutputBaseFilename=${PROJECT_NAME}_Setup
DirExistsWarning=no
UninstallFilesDir={app}\Uninstall
UninstallDisplayIcon={app}\uninstall.ico

[Files]
; Install VST3 to the standard VST3 location
Source: "..\artifacts\${PROJECT_NAME}.vst3"; DestDir: "{commoncf64}\VST3\Dimethoxy\${PROJECT_NAME}"; Flags: recursesubdirs replacesameversion

; Install CLAP to the standard CLAP location
Source: "..\artifacts\${PROJECT_NAME}.clap"; DestDir: "{commoncf64}\CLAP\Dimethoxy\${PROJECT_NAME}"; Flags: recursesubdirs replacesameversion

; Install resources and uninstaller to Dimethoxy folder in Program Files
Source: "..\resources\uninstall.ico"; DestDir: "{app}"; Flags: replacesameversion
