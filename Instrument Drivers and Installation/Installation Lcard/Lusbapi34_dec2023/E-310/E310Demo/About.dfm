object AboutProgramPanel: TAboutProgramPanel
  Left = 556
  Top = 301
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #1054' '#1087#1088#1086#1075#1088#1072#1084#1084#1077' ...'
  ClientHeight = 336
  ClientWidth = 519
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
    519
    336)
  PixelsPerInch = 120
  TextHeight = 19
  object LcardUrlLink: TURLLink
    Left = 86
    Top = 127
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
    Top = 149
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
    Top = 149
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
    Left = 234
    Top = 228
    Width = 238
    Height = 106
    Alignment = gcaTopCenter
    Bevel.Mode = bmWindows
    Caption = #1042#1077#1088#1089#1080#1080
    CaptionFont.Charset = RUSSIAN_CHARSET
    CaptionFont.Color = clBlack
    CaptionFont.Height = -18
    CaptionFont.Name = 'Arial'
    CaptionFont.Style = []
    CaptionFont3D.LightColor = clLime
    CaptionFont3D.Style = tdRaised
    CaptionFont3D.Tracing = 11
    CaptionParentFont = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -18
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object E310DemoVersionLabelLMDLabel: TLMDLabel
      Left = 7
      Top = 27
      Width = 93
      Height = 21
      Bevel.Mode = bmCustom
      Alignment = agCenterLeft
      AutoSize = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clMaroon
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = []
      Options = []
      ParentFont = False
      Caption = 'E310Demo:'
    end
    object LusbapiVersionLabelLMDLabel: TLMDLabel
      Left = 7
      Top = 53
      Width = 93
      Height = 21
      Bevel.Mode = bmCustom
      Alignment = agCenterLeft
      AutoSize = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clMaroon
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = []
      Options = []
      ParentFont = False
      Caption = 'Lusbapi:'
    end
    object E310DemoVersionLMDLabel: TLMDLabel
      Left = 96
      Top = 24
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
      Left = 96
      Top = 50
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
      Top = 79
      Width = 93
      Height = 21
      Bevel.Mode = bmCustom
      Alignment = agCenterLeft
      AutoSize = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clMaroon
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = []
      Options = []
      ParentFont = False
      Caption = 'Ldevusbu:'
    end
    object LdevusbuVersionLMDLabelLMDLabel: TLMDLabel
      Left = 96
      Top = 76
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
    Left = 47
    Top = 244
    Width = 177
    Height = 84
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
    ButtonLayout.Alignment = agTopCenter
    ButtonLayout.AlignText2Glyph = taTop
    ButtonLayout.Spacing = 0
    FontFX.LightDepth = 2
    FontFX.Style = tdRaised
    FontFX.Tracing = 11
  end
  object AboutESColorMemo: TESColorMemo
    Left = 3
    Top = 0
    Width = 515
    Height = 122
    TabStop = False
    Alignment = taCenter
    BorderStyle = bsNone
    Color = clBtnFace
    Enabled = False
    HideSelection = False
    Lines.Strings = (
      #1044#1072#1085#1085#1072#1103' '#1087#1088#1086#1075#1088#1072#1084#1084#1072' '#1087#1088#1077#1076#1085#1072#1079#1085#1072#1095#1077#1085#1072' '#1076#1083#1103' '#1076#1077#1084#1086#1085#1089#1090#1088#1072#1094#1080#1080
      #1074#1086#1079#1084#1086#1078#1085#1086#1089#1090#1077#1081' USB '#1084#1086#1076#1091#1083#1103' E-310, '#1088#1072#1079#1088#1072#1073#1086#1090#1072#1085#1085#1086#1075#1086' '#1054#1054#1054' "'#1051' '#1050#1072#1088#1076'". '
      #1044#1083#1103' '#1082#1086#1088#1088#1077#1082#1090#1085#1086#1081' '#1088#1072#1073#1086#1090#1099' '#1085#1077#1086#1073#1093#1086#1076#1080#1084#1072' '#1086#1087#1077#1088#1072#1094#1080#1086#1085#1085#1072#1103' '#1089#1088#1077#1076#1072' '
      'Windows'#39'98/Me/2000/XP/Vista/7/8.x/10.'
      ''
      #169' '#1054#1054#1054' "'#1051' '#1050#1072#1088#1076'", '#1071#1085#1074#1072#1088#1100' 2017 '#1075'.')
    ReadOnly = True
    TabOrder = 2
    ColorItems = <
      item
        Name = 'ColorItem1'
        Color = clBlue
        Text = 'E-310'
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
        Text = 'Windows'#39'98/Me/2000/XP/Vista/7/8.x/10'
      end>
  end
  object StaticText1: TStaticText
    Left = 3
    Top = 127
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
    Top = 149
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
    Top = 176
    Width = 480
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
    Top = 181
  end
end
