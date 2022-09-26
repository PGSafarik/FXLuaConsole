print( "!!!Hello in test script!!!" ) 
print( "================================" )

for i = 1, 10 do
  print( i, " * 22 = ",  ( i * 22 ) ) 
end 

print( )
print( "================================" )

print( "Test vstupu : " ) 
print( "================================" )
local c = input( )
print( "Vlozena hodnota: ", c )
local name = input( "Zadejte sve jmeno :" )
print( "Jmenujete se: ", name )
local captcha = 48759
print( "CAPTCHA : Opiste prosim nasledujici kod a potvrdte" )
local cap_inp = input( captcha ) 

  
print( "Message box : ")
print( "================================" )
local test = message( "To fakt myslis vazne???", "DOTAZ", mbox_buttons.yes_no, mbox_type.question )
if test == "no" then print( "Uz jsem myslel..." ) else print( "Aha... Tak jo." ) end


print( )
print( "================================" )
print( "Programing by D.A.Tiger in C++ Fox Toolkit with LUA" )

print( )
print( "CHYBA :")
print( "================================" )
message( )

