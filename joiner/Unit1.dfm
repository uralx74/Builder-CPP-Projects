object Form1: TForm1
  Left = 219
  Top = 174
  Width = 589
  Height = 516
  Caption = 'Joiner'
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
  object Button1: TButton
    Left = 400
    Top = 440
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Выполнить'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 488
    Top = 440
    Width = 75
    Height = 25
    Hint = 'Открыть файл конфигурации...'
    Anchors = [akRight, akBottom]
    Caption = 'Открыть...'
    TabOrder = 1
    OnClick = Button2Click
  end
  object RichEdit1: TRichEdit
    Left = 0
    Top = 0
    Width = 573
    Height = 425
    Align = alTop
    Anchors = [akLeft, akTop, akRight, akBottom]
    Color = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Lines.Strings = (
      'RichEdit1')
    ParentFont = False
    ReadOnly = True
    TabOrder = 2
  end
end
