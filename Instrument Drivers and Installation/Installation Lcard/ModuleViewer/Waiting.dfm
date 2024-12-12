object WaitingPanel: TWaitingPanel
  Left = 742
  Top = 429
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = #1046#1076#1105#1084'-c...'
  ClientHeight = 112
  ClientWidth = 352
  Color = clBtnFace
  DefaultMonitor = dmMainForm
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Arial'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  object WaitingLMDProgress: TLMDProgress
    Left = 3
    Top = 79
    Width = 345
    Height = 29
    Bevel.StyleOuter = bvLowered
    Bevel.Mode = bmEdge
    Delta = 2
    SmallBar.BarColor = 16711808
    SmallBar.BarCount = 25
    SmallBar.HSpace = 5
    Position = 21
    UserValue = 21
  end
  object ESColorMemo1: TESColorMemo
    Left = 4
    Top = 0
    Width = 345
    Height = 69
    Alignment = taCenter
    BorderStyle = bsNone
    Color = clBtnFace
    Enabled = False
    HideSelection = False
    Lines.Strings = (
      #1054#1073#1085#1072#1088#1091#1078#1077#1085' '#1086#1076#1080#1085' '#1080#1083#1080' '#1085#1077#1089#1082#1086#1083#1100#1082#1086' '#1084#1086#1076#1091#1083#1077#1081' E14-440. '
      #1042#1088#1077#1084#1103' '#1085#1077#1086#1073#1093#1086#1076#1080#1084#1086#1077' '#1076#1083#1103' '#1079#1072#1075#1088#1091#1079#1082#1080' '#1086#1076#1085#1086#1075#1086' '#1084#1086#1076#1091#1083#1103' '
      'E14-440 '#1089#1086#1089#1090#1072#1074#1083#1103#1077#1090' '#1087#1086#1088#1103#1076#1082#1072' 2 '#1089#1077#1082'.'
      #1058#1072#1082' '#1095#1090#1086' '#1078#1076#1105#1084'-'#1089'...')
    ReadOnly = True
    TabOrder = 0
    ColorItems = <
      item
        Name = 'ColorItem1'
        Color = clBlue
        Text = 'E14-440'
      end
      item
        Name = 'ColorItem2'
        Color = clPurple
        Text = '2 '#1089#1077#1082'.'
      end
      item
        Name = 'ColorItem3'
        Color = clMaroon
        Text = #1078#1076#1105#1084'-'#1089
      end>
  end
  object Timer: TTimer
    Enabled = False
    Interval = 200
    OnTimer = TimerTimer
    Left = 364
    Top = 53
  end
end
