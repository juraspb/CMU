unit MNTR;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, SerialLink, ExtCtrls, Menus, Buttons, ComCtrls, Grids, Spin, GraphUtil;

type
    TPlayList =  record
      prog:integer;
      subprog:integer;
      direction:integer;
      tempo:integer;
end;


type
  TDSForm = class(TForm)
    SB: TStatusBar;
    cnfgTimer: TTimer;
    Timer1: TTimer;
    SvDlg: TSaveDialog;
    GroupBox1: TGroupBox;
    Label4: TLabel;
    ComboBox1: TComboBox;
    Label1: TLabel;
    opDlg: TOpenDialog;
    Button3: TButton;
    edt1: TEdit;
    Image1: TImage;
    DG1: TDrawGrid;
    procedure FormCreate(Sender: TObject);
    procedure cnfgTimerTimer(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure DG1DrawCell(Sender: TObject; ACol, ARow: Integer; Rect: TRect;
      State: TGridDrawState);
  private
    { Private declarations }
    FLink: TSerialLink;
    number_bytes: integer;
    noiselvl,uolvl: single;
    str_recv: ansistring;
    procedure sendCMD(s:ansistring;answ:boolean);
  public
    { Public declarations }
    CommunicPortName: string;
    CommunicPortSpeed: integer;
    time: Cardinal;
    buffcount,bytecount,num: integer;
    buff : array[0..511] of integer;
    zbuff : array[0..511] of integer;
    ledColor : array[0..240] of TColor;
    ledAmp : array[0..240] of TColor;
    ledTabColor : array[0..240] of TColor;
    SendBuff : array[0..255] of byte;
    RcvBuff : array[0..32767] of byte;
  end;

var
  DSForm : TDSForm;
  Timeout: Cardinal;

implementation

{$R *.DFM}

procedure TDSForm.FormCreate(Sender: TObject);
var i:integer;
begin
  CommunicPortName:='COM7';
  CommunicPortSpeed:=115200;
  cnfgTimer.Enabled:=true;
  noiselvl:=0;
  uolvl:=0;
  num:=0;
  for i:=0 to 239 do
   begin
    ledTabColor[i]:=ColorHLSToRGB(i,128,240);
   end;
  for i:=0 to 239 do ledColor[i]:=ledTabColor[i];
  buffcount:=0;
  bytecount:=0;
  str_recv:='';
  number_bytes:=0;
end;

procedure TDSForm.cnfgTimerTimer(Sender: TObject);
begin
  cnfgTimer.Enabled:=false;
  try
    if FLink = nil then
     begin
      FLink:= TSerialLink.Create(nil);
     end;
    FLink.Port:=CommunicPortName;
    FLink.Speed:=CommunicPortSpeed;
    FLink.Open;
   finally
    if not FLink.Active then
     begin
      Label1.Font.Color:=clRed;
      Label1.Caption:='Порт не доступен';
      MessageDlg('Нет связи', mtError, [mbOK], 0);
     end else
     begin
      Label1.Font.Color:=clGreen;
      Label1.Caption:='Порт открыт';
      timer1.Enabled:=true;
     end;
  end;
end;

procedure TDSForm.DG1DrawCell(Sender: TObject; ACol, ARow: Integer; Rect: TRect;
  State: TGridDrawState);
begin
  with DG1.Canvas do
    begin
      Brush.Color := ledColor[ACol];
      FillRect(Rect);
    end;
end;

procedure TDSForm.Timer1Timer(Sender: TObject);
var i,l,x,n,max,maxx,amp:integer;
    s:string;
    freq,noise,uo:single;
    bmp:TBitmap;
begin
   if FLink.GetBytesReceived then
    begin
     n:=FLink.BytesReceived;          // байт в буфере
     if n>1 then
      begin
       FLink.ReceiveBuffer(RcvBuff,n);
       for i:=0 to n-1 do
        begin
         if (RcvBuff[i-1]=$FF)and (RcvBuff[i]=$FF) then
          begin
           if buffcount=512 then
            begin
             //buffcount:=buffcount-1;
             bmp:=TBitmap.Create;
             bmp.Height:=266;
             bmp.Width:=buffcount;
             bmp.Canvas.Brush.Color:=clSilver;
             bmp.Canvas.FillRect(bmp.Canvas.ClipRect);
             bmp.Canvas.Pen.Color:=clRed;


//             bmp.Canvas.MoveTo(0,255-buff[0]);
             bmp.Canvas.MoveTo(0,256);
             max:=0;
             uo:=0;
             noise:=0;
             for x:=1 to buffcount-1 do
              begin
               //midlvl[x]:=0.1*buff[x]+(1-0.1)*midlvl[x];
               //bmp.Canvas.LineTo(x,128+(round(midlvl[x])-buff[x]));
               bmp.Canvas.LineTo(x,256-buff[x]);
               uo:=uo+buff[x];
               amp:=zbuff[x]-buff[x];
               noise:=noise+amp*amp;
               if max<buff[x] then
                begin
                  max:=buff[x];
                  maxx:=x;
                end;
              end;
             uo:=uo/(buffcount-1);
             noise:=noise/(buffcount-1);
             uolvl:=0.1*uo+0.9*uolvl;
             noiselvl:=0.1*noise+0.9*noiselvl;

             for x:=1 to 240 do
              begin
               //ledAmp[x-1]:=ledAmp[x-1]*7 div 10;
               //amp:=buff[x*2]+buff[x*2-1];
               amp:=buff[x];
               //if ledAmp[x-1]<amp then ledAmp[x-1]:=amp;
               if amp>128 then amp:=128;
               ledAmp[x-1]:=amp div 2;
              end;
             for x:=0 to 239 do ledColor[x]:=ColorHLSToRGB(x,ledAmp[x],240);
             DG1.Refresh;


             if maxx>2 then
              begin
               freq:=((maxx-2)*buff[maxx-2]+(maxx-1)*buff[maxx-1]+maxx*buff[maxx]+(maxx+1)*buff[maxx+1]+(maxx+2)*buff[maxx+2])/(buff[maxx-2]+buff[maxx-1]+buff[maxx]+buff[maxx+1]+buff[maxx+2]);
               //f:=maxx;
              end else freq:=maxx;
             s:='fmax='+floattostrf(freq*(96000000/2178)/1024,ffFixed, 6, 2)+'::'+inttostr(max);
             bmp.Canvas.Font.Size:=18;
             bmp.Canvas.TextOut(300,10,s);

             s:='uo='+floattostrf(uo,ffFixed, 6, 2);
             bmp.Canvas.Font.Size:=18;
             bmp.Canvas.TextOut(300,50,s);

             s:='noise='+floattostrf(noise,ffFixed, 6, 2);
             bmp.Canvas.Font.Size:=18;
             bmp.Canvas.TextOut(300,90,s);

             bmp.Canvas.Pen.Color:=clYellow;
             bmp.Canvas.MoveTo(0,round(256-uolvl));
             bmp.Canvas.LineTo(buffcount-1,round(256-uolvl));

             bmp.Canvas.Pen.Color:=clLime;
             bmp.Canvas.MoveTo(0,round(256-uo-3*noiselvl));
             bmp.Canvas.LineTo(buffcount-1,round(256-uo-3*noiselvl));

             image1.Picture.Bitmap.Assign(bmp);
             bmp.Free;

             for x:=0 to buffcount-1 do zbuff[x]:=buff[x];


            end;
           buffcount:=0;
           bytecount:=0;
          end else
          begin
           if bytecount>0 then
             begin
              Buff[buffcount]:=RcvBuff[i]shl 8 + RcvBuff[i-1];
              buffcount:=buffcount+1;
             end;
           bytecount:=(bytecount+1)and 1;
          end;
        end;
      end;
    end;
end;

procedure TDSForm.Button3Click(Sender: TObject);
begin
    sendCMD(edt1.Text,False);
    Timer1.Enabled:=True;
end;

procedure TDSForm.sendCMD(s:ansistring;answ:Boolean);
var n,i:Byte;
    ch:array[0..255] of AnsiChar;
    si:AnsiString;
    timeRead,timeCrt:Cardinal;
    str_ready:Boolean;
begin
    n:=Length(s);
    for i:=0 to n-1 do ch[i]:=s[i+1];
    ch[n]:=#10;
    ch[n+1]:=#13;
    FLink.SendChar(ch,n+2);
    SB.Panels[0].Text:='Send:'+s;
    if answ then
     begin
      si:='time_out';
      str_ready:=False;
      timeRead:=GetTickCount;
      repeat
       if FLink.GetBytesReceived then
        begin
         n:=FLink.BytesReceived;          // байт в буфере
         if n>0 then
          begin
           timeRead:=GetTickCount;
           FLink.ReceiveBuffer(RcvBuff,n);
           for i:=1 to n do
            begin
               if (RcvBuff[i-1]=13)or(RcvBuff[i-1]=10) then
                begin
                 if str_recv>'' then
                  begin
                   si:=str_recv;
                   str_recv:='';
                   str_ready:=True;
                  end;
                end else
                begin
                 str_recv:=str_recv+ansichar(RcvBuff[i-1]);
                end;
            end;
          end;
        end;
        timeCrt:=GetTickCount;
      until (((timeCrt-timeRead)>500) or str_ready);
      SB.Panels[0].Text:='Answer:'+si;
     end;
end;

end.

