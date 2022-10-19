object DSForm: TDSForm
  Left = 224
  Top = 326
  Caption = 'Musical LEDs service programm'
  ClientHeight = 440
  ClientWidth = 1221
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Image1: TImage
    Left = 0
    Top = 70
    Width = 1221
    Height = 256
    Align = alTop
    Stretch = True
    ExplicitTop = 68
  end
  object SB: TStatusBar
    Left = 0
    Top = 416
    Width = 1221
    Height = 24
    Panels = <
      item
        Width = 200
      end
      item
        Width = 210
      end
      item
        Width = 50
      end>
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 1221
    Height = 70
    Align = alTop
    Caption = #1055#1086#1088#1090
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object Label4: TLabel
      Left = 18
      Top = 28
      Width = 43
      Height = 20
      Caption = #1055#1086#1088#1090':'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label1: TLabel
      Left = 167
      Top = 29
      Width = 132
      Height = 20
      Caption = #1055#1086#1088#1090' '#1085#1077#1076#1086#1089#1090#1091#1087#1077#1085
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ComboBox1: TComboBox
      Left = 65
      Top = 26
      Width = 96
      Height = 28
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ItemIndex = 6
      ParentFont = False
      TabOrder = 0
      Text = 'COM7'
      Items.Strings = (
        'COM1'
        'COM2'
        'COM3'
        'COM4'
        'COM5'
        'COM6'
        'COM7'
        'COM8')
    end
    object Button3: TButton
      Left = 583
      Top = 22
      Width = 41
      Height = 25
      Hint = #1055#1077#1088#1077#1076#1072#1090#1100' AT '#1082#1086#1084#1072#1085#1076#1091
      Caption = 'SEND'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      OnClick = Button3Click
    end
    object edt1: TEdit
      Left = 460
      Top = 22
      Width = 109
      Height = 24
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
      Text = 'AT+H'
    end
  end
  object DG1: TDrawGrid
    Left = 8
    Top = 330
    Width = 1204
    Height = 80
    ColCount = 240
    DefaultColWidth = 5
    DefaultRowHeight = 80
    FixedCols = 0
    RowCount = 1
    FixedRows = 0
    GridLineWidth = 0
    TabOrder = 2
    OnDrawCell = DG1DrawCell
    ColWidths = (
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5
      5)
    RowHeights = (
      80)
  end
  object cnfgTimer: TTimer
    Enabled = False
    OnTimer = cnfgTimerTimer
    Left = 808
    Top = 32
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 10
    OnTimer = Timer1Timer
    Left = 848
    Top = 32
  end
  object SvDlg: TSaveDialog
    DefaultExt = 'txt'
    Left = 840
    Top = 96
  end
  object opDlg: TOpenDialog
    Left = 808
    Top = 96
  end
end
