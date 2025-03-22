[Setup]
AppName=${PRODUCT_NAME}
AppVersion=${VERSION}
DefaultDirName={commonpf64}\Dimethoxy\${PRODUCT_NAME}
DefaultGroupName=${PRODUCT_NAME}
OutputBaseFilename=setup
DirExistsWarning=no
UninstallFilesDir={app}\Uninstall
UninstallDisplayIcon={app}\uninstall.ico

[Files]
; Install VST3 to the standard VST3 location
Source: "..\..\artifacts\${PRODUCT_NAME}.vst3"; DestDir: "{commoncf64}\VST3\${PRODUCT_NAME}"; Flags: recursesubdirs

; Install CLAP to the standard CLAP location
Source: "..\..\artifacts\${PRODUCT_NAME}.clap"; DestDir: "{commoncf64}\CLAP\${PRODUCT_NAME}"; Flags: recursesubdirs

; Install resources and uninstaller to Dimethoxy folder in Program Files
Source: "..\..\resources\uninstall.ico"; DestDir: "{app}"; 
