; �ű��� Inno Setup �ű��� ���ɣ�
; �йش��� Inno Setup �ű��ļ�����ϸ��������İ����ĵ���

#define MyAppName "controlrobots"
#define MyAppCheseName "controlrobots"
#define MyAppVersion "3.0.0"
#define MyAppPublisher "Wingmark Robotics"
#define MyAppURL "http://robot.wingmark.cn/"
#define MyAppExeName "controlrobots.exe"
#define BasePath "..\bin\win32\release"
#define OutputName "CFB001-controlrobots-x32-Setup-"
#define MyDateTimeString GetDateTimeString('yyyymmddhhnnss', '', '')
#define MyCompanyName "Wingmark Robotics"

[Setup]
; ע: AppId��ֵΪ������ʶ��Ӧ�ó���
; ��ҪΪ������װ����ʹ����ͬ��AppIdֵ��
; (�����µ�GUID����� ����|��IDE������GUID��)
AppId={{5E47597D-A139-4EB0-80B7-5B838FF102A2}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyCompanyName}\{#MyAppCheseName}
DefaultGroupName={#MyCompanyName}\{#MyAppCheseName}
DisableProgramGroupPage=yes
OutputDir=.\win32
OutputBaseFilename={#OutputName}{#MyDateTimeString}
SetupIconFile=.\wukong.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinesesimplified"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone

[Files]
Source: "{#BasePath}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BasePath}\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BasePath}\Config\*.*"; DestDir: "{app}\Config"; Flags: ignoreversion
Source: "{#BasePath}\Config\actionlist\*.*"; DestDir: "{app}\Config\actionlist"; Flags: ignoreversion
Source: "{#BasePath}\platforms\*.*"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "{#BasePath}\Config\music\*.*"; DestDir: "{app}\Config\music"; Flags: ignoreversion

; ע��: ��Ҫ���κι���ϵͳ�ļ���ʹ�á�Flags: ignoreversion��

[Icons]
Name: "{commondesktop}\{#MyAppCheseName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; Tasks: desktopicon
Name: "{group}\{#MyAppCheseName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppCheseName}}"; Filename: "{app}/uninst.exe"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[CODE]                                        
procedure CurStepChanged(CurStep: TSetupStep);
var uninspath, uninsname, NewUninsName, ApplicationName : string;
begin
  if CurStep = ssDone then
  begin
      // ָ���µ�ж���ļ�������������չ����������Ӧ�޸ģ�
      NewUninsName := 'uninst';
      ApplicationName := '{#MyAppName}';
      // ����������ж���ļ�
      uninspath:= ExtractFilePath(ExpandConstant('{uninstallexe}'));
      uninsname:= Copy(ExtractFileName(ExpandConstant('{uninstallexe}')),1,8);
      deleteFile(uninspath + NewUninsName + '.exe');
      deleteFile(uninspath + NewUninsName + '.dat');
      RenameFile(uninspath + uninsname + '.exe', uninspath + NewUninsName + '.exe');
      RenameFile(uninspath + uninsname + '.dat', uninspath + NewUninsName + '.dat');
      // �����޸���Ӧ��ע�������
      if RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE/Microsoft/Windows/CurrentVersion/Uninstall/' + ApplicationName + '_is1') then
      begin
         RegWriteStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE/Microsoft/Windows/CurrentVersion/Uninstall/' + ApplicationName + '_is1', 'UninstallString', '"' + uninspath + NewUninsName + '.exe"');
         RegWriteStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE/Microsoft/Windows/CurrentVersion/Uninstall/' + ApplicationName + '_is1', 'QuietUninstallString', '"' + uninspath + NewUninsName + '.exe" /SILENT');
      end;
  end;
end;