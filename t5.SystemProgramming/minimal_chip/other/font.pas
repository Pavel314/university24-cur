{$reference System.Drawing.dll}
uses System.Drawing;
begin
var bmp:=new Bitmap('font.png');
for var y:=0 to bmp.Height-1 do
  begin
  for var x:=0 to bmp.Width-1 do
  begin
  var px:=bmp.GetPixel(x,y);
  if (px=Color.FromArgb(255,0,0,0)) then
    write('|')
  else if (px=Color.FromArgb(255,153,102,0)) then
    write('.')
  else
    write('x');
  
  end;
  writeln();
  end;
end.