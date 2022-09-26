function print_table( t, n, index  )
  index = index or 0
  n = n or " "
  
  local tabs = ""
  if index > 0 then 
    for i = 1, index do tabs = tabs .. "\t" end
  end  
  print( "+", tabs .. n, "[table]" )
   
  for name, value in pairs( t ) do
    local vtype = type( value )
    if vtype ~= "number" or vtype ~= "string" or vtype ~= "boolean" then
      if vtype == "table" then
        print_table( value, name, index + 1 )
      else
        value = vtype
      end
    end
    
    print( " ", tabs .. name, value )
  end

end

test = { 
 a = 1,
 b = "hello world",
 c = {
   y = _VERSION,
   x = function( ) print( test.c.y, test.b ) end 
 }
}

print( "TEST : " )
print( "=====================================" )
print_table( test, "test" )
test.c.x( )

print( )
print( "=====================================" )
print_table( io, "io" )
print( )
f = io.open( "/home/gabriel/.bashrc", "r" )
--f:read = function( ) return "HAHA" end
print( f:read( "*a" ) )
--print_table( f, "file" )

print( )
print( "=====================================" )
print_table( msg_type, "mbox_type" )
print_table( msg_buttons, "mbox_buttons" )

