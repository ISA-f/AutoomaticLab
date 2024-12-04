object AboutProgramPanel: TAboutProgramPanel
  Left = 919
  Top = 206
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #1054' '#1087#1088#1086#1075#1088#1072#1084#1084#1077' ...'
  ClientHeight = 440
  ClientWidth = 447
  Color = clBtnFace
  DefaultMonitor = dmMainForm
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -17
  Font.Name = 'Arial'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  OnHide = FormHide
  OnShow = FormShow
  DesignSize = (
    447
    440)
  PixelsPerInch = 96
  TextHeight = 19
  object LcardUrlLink: TURLLink
    Left = 86
    Top = 240
    Width = 148
    Height = 19
    Cursor = crHandPoint
    AutoSize = False
    Caption = 'www.lcard.ru'
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -18
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
    FocusFont.Charset = RUSSIAN_CHARSET
    FocusFont.Color = clBlue
    FocusFont.Height = -18
    FocusFont.Name = 'Courier New'
    FocusFont.Style = [fsBold]
    Link = 'http://www.lcard.ru'
  end
  object URLLink: TURLLink
    Left = 86
    Top = 262
    Width = 167
    Height = 19
    Cursor = crHandPoint
    AutoSize = False
    Caption = 'lcard@lcard.ru'
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -18
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
    FocusFont.Charset = RUSSIAN_CHARSET
    FocusFont.Color = clBlue
    FocusFont.Height = -18
    FocusFont.Name = 'Courier New'
    FocusFont.Style = [fsBold]
    Link = 'mailto:lcard@lcard.ru?subject=L-CARD'
  end
  object URLLink1: TURLLink
    Left = 256
    Top = 262
    Width = 189
    Height = 19
    Cursor = crHandPoint
    AutoSize = False
    Caption = 'support@lcard.ru'
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -18
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
    FocusFont.Charset = RUSSIAN_CHARSET
    FocusFont.Color = clBlue
    FocusFont.Height = -18
    FocusFont.Name = 'Courier New'
    FocusFont.Style = [fsBold]
    Link = 'mailto:support@lcard.ru?subject=LCARD SUPPORT'
  end
  object VersionsLMDGroupBox: TLMDGroupBox
    Left = 171
    Top = 335
    Width = 271
    Height = 104
    CtlXPCaptionColor = False
    Bevel.Mode = bmEdge
    Caption = #1042#1077#1088#1089#1080#1080
    CaptionFont.Charset = RUSSIAN_CHARSET
    CaptionFont.Color = clMaroon
    CaptionFont.Height = -19
    CaptionFont.Name = 'Arial'
    CaptionFont.Style = []
    CaptionFont3D.LightColor = clYellow
    CaptionFont3D.Style = tdRaised
    CaptionFont3D.Tracing = 4
    CaptionParentFont = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -18
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object ModulesViewerVersionLabelLMDLabel: TLMDLabel
      Left = 7
      Top = 24
      Width = 119
      Height = 21
      Bevel.Mode = bmCustom
      Alignment = agCenterLeft
      AutoSize = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = []
      Options = []
      ParentFont = False
      Caption = 'ModulesViewer:'
    end
    object LusbapiVersionLabelLMDLabel: TLMDLabel
      Left = 7
      Top = 50
      Width = 119
      Height = 21
      Bevel.Mode = bmCustom
      Alignment = agCenterLeft
      AutoSize = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = []
      Options = []
      ParentFont = False
      Caption = 'Lusbapi:'
    end
    object ModulesViewerVersionLMDLabel: TLMDLabel
      Left = 128
      Top = 21
      Width = 136
      Height = 26
      Bevel.EdgeStyle = etBump
      Bevel.Mode = bmEdge
      Alignment = agCenter
      AutoSize = False
      Color = clWhite
      Options = []
      ParentColor = False
      Caption = '--------------------'
    end
    object LusbapiVersionLMDLabel: TLMDLabel
      Left = 128
      Top = 47
      Width = 136
      Height = 26
      Bevel.EdgeStyle = etBump
      Bevel.Mode = bmEdge
      Alignment = agCenter
      AutoSize = False
      Color = clWhite
      Options = []
      ParentColor = False
      Caption = '--------------------'
    end
    object LdevusbuVersionLabelLMDLabel: TLMDLabel
      Left = 8
      Top = 76
      Width = 119
      Height = 21
      Bevel.Mode = bmCustom
      Alignment = agCenterLeft
      AutoSize = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = []
      Options = []
      ParentFont = False
      Caption = 'Ldevusbu:'
    end
    object LdevusbuVersionLMDLabel: TLMDLabel
      Left = 128
      Top = 73
      Width = 136
      Height = 26
      Bevel.EdgeStyle = etBump
      Bevel.Mode = bmEdge
      Alignment = agCenter
      AutoSize = False
      Color = clWhite
      Options = []
      ParentColor = False
      Caption = '--------------------'
    end
  end
  object OkLMDButton: TLMDButton
    Left = 6
    Top = 346
    Width = 154
    Height = 91
    Cursor = crHandPoint
    Anchors = []
    Caption = 'OK'
    Default = True
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clNavy
    Font.Height = -60
    Font.Name = 'Monotype Corsiva'
    Font.Style = [fsBold, fsItalic]
    ParentFont = False
    TabOrder = 0
    OnClick = OkLMDButtonClick
    ButtonLayout.AlignText2Glyph = taTop
    ButtonLayout.Spacing = 0
    FontFX.Style = tdRaised
    MultiLine = True
  end
  object AboutESColorMemo: TESColorMemo
    Left = 3
    Top = 0
    Width = 442
    Height = 233
    TabStop = False
    Alignment = taCenter
    BorderStyle = bsNone
    Color = clBtnFace
    Enabled = False
    HideSelection = False
    Lines.Strings = (
      #1044#1072#1085#1085#1072#1103' '#1091#1090#1080#1083#1080#1090#1072' '#1087#1088#1077#1076#1085#1072#1079#1085#1072#1095#1077#1085#1072' '#1076#1083#1103' '#1084#1086#1085#1080#1090#1086#1088#1080#1085#1075#1072' '
      #1074#1085#1077#1096#1085#1080#1093' '#1084#1086#1076#1091#1083#1077#1081', '#1087#1086#1076#1082#1083#1102#1095#1105#1085#1085#1099#1093' '#1082' USB '#1096#1080#1085#1077' '
      #1082#1086#1084#1087#1100#1102#1090#1077#1088#1072'. '#1059#1090#1080#1083#1080#1090#1072' '#1087#1086#1076#1076#1077#1088#1078#1080#1074#1072#1077#1090' '#1082#1086#1085#1090#1088#1086#1083#1100' '#1090#1086#1083#1100#1082#1086' '
      #1085#1072#1076' '#1080#1079#1076#1077#1083#1080#1103#1084#1080', '#1088#1072#1079#1088#1072#1073#1086#1090#1072#1085#1085#1099#1084#1080' '#1054#1054#1054' "'#1051' '#1050#1072#1088#1076'". '#1044#1083#1103' '
      #1082#1086#1088#1088#1077#1082#1090#1085#1086#1081' '#1088#1072#1073#1086#1090#1099' '#1085#1077#1086#1073#1093#1086#1076#1080#1084#1072' '#1086#1087#1077#1088#1072#1094#1080#1086#1085#1085#1072#1103' '#1089#1088#1077#1076#1072' '
      'Windows'#39'XP/Vista/7/8.x/10/11.'
      '  '#1055#1086#1076#1076#1077#1088#1078#1080#1074#1072#1102#1090#1089#1103' '#1089#1083#1077#1076#1091#1102#1097#1080#1077' '#1074#1085#1077#1096#1085#1080#1077'  USB '#1084#1086#1076#1091#1083#1080':'
      '  1. E14-140;'
      '  2. E14-440;'
      '  3. E20-10;'
      '  4. E-154;'
      '  5. E-310.'
      ''
      #169' '#1047#1040#1054' "'#1051'-'#1050#1072#1088#1076'", '#1052#1072#1088#1090' 2008 '#1075'.')
    ReadOnly = True
    TabOrder = 2
    ColorItems = <
      item
        Name = 'ColorItem1'
        Color = clOlive
        Text = #1084#1086#1085#1080#1090#1086#1088#1080#1085#1075#1072' '
      end
      item
        Name = 'ColorItem2'
        Color = clMaroon
        Text = 'USB'
      end
      item
        Name = 'ColorItem3'
        Color = clTeal
        Text = #1054#1054#1054' "'#1051' '#1050#1072#1088#1076'"'
      end
      item
        Name = 'ColorItem4'
        Color = clGreen
        Text = 'XP/Vista/7/8.x/10/11'
      end
      item
        Name = 'ColorItem5'
        Color = clMaroon
        Text = 'Windows'
      end
      item
        Name = 'ColorItem6'
        Color = clBlue
        Text = 'E14-140'
      end
      item
        Name = 'ColorItem7'
        Color = clBlue
        Text = 'E14-440'
      end
      item
        Name = 'ColorItem8'
        Color = clBlue
        Text = 'E20-10'
      end
      item
        Name = 'ColorItem9'
        Color = clBlue
        Text = 'E-154'
      end
      item
        Name = 'ColorItem10'
        Color = clBlue
        Text = 'E-310'
      end>
  end
  object StaticText1: TStaticText
    Left = 3
    Top = 240
    Width = 48
    Height = 24
    Caption = 'WWW:'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -18
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
  end
  object StaticText2: TStaticText
    Left = 3
    Top = 262
    Width = 81
    Height = 24
    Caption = 'E-mail:'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -18
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
  end
  object ESColorMemo: TESColorMemo
    Left = 3
    Top = 289
    Width = 440
    Height = 46
    HelpType = htKeyword
    TabStop = False
    BorderStyle = bsNone
    Color = clBtnFace
    Enabled = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -18
    Font.Name = 'Courier New'
    Font.Style = []
    HideSelection = False
    Lines.Strings = (
      #1090#1077#1083'.  (495) 785-95-25'
      #1092#1072#1082#1089'. (495) 785-95-14'
      ' ')
    ParentFont = False
    ReadOnly = True
    TabOrder = 5
    ColorItems = <
      item
        Name = 'ColorItem1'
        Color = clOlive
        Text = #1084#1086#1085#1080#1090#1086#1088#1080#1085#1075#1072' '
      end
      item
        Name = 'ColorItem2'
        Color = clMaroon
        Text = 'USB'
      end
      item
        Name = 'ColorItem3'
        Color = clTeal
        Text = #1047#1040#1054' "'#1051'-'#1050#1072#1088#1076'"'
      end
      item
        Name = 'ColorItem4'
        Color = clOlive
        Text = 'Windows'#39'98/Me/2000/XP'
      end
      item
        Name = 'ColorItem5'
        Color = clBlue
        Text = 'E14-140'
      end
      item
        Name = 'ColorItem6'
        Color = clBlue
        Text = 'E14-440'
      end
      item
        Name = 'ColorItem7'
        Color = clBlue
        Text = 'E20-10'
      end
      item
        Name = 'ColorItem8'
        Color = clPurple
        Text = '(495) 785-95-25'
      end
      item
        Name = 'ColorItem9'
        Color = clPurple
        Text = '(495) 785-95-14'
      end>
  end
  object PJVersionInfo: TPJVersionInfo
    Left = 304
    Top = 294
  end
end
