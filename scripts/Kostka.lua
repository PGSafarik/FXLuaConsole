verbrose = true

function kostka( pocet_hodu, pocet_sten, random )
  if random == nil then random = math.random end 
  local s = { }
  local num = 0
  local rep = true
  
  for i = 1, pocet_hodu do 
    repeat    
      local act = random( pocet_sten )
      table.insert( s, #s + 1, act )
      if act == 6 and pocet_sten == 6 then rep = false else rep = true end
    until rep
  end
  
  return s   
end  

--function k( s, n ) print( kostka( s, n ) ) end 

function nkm( str )
  local n, m, o = 0, 0, 0       --    
  local k_pos, o_pos            --
  local oprava, vysledek = 0, 0 -- oprava pripocitana k vygenerovanemu cislu, vysledne cislo
  local h = nil                 -- pole obsahujici hodnoty jednotlivych hodu 
  local hlist_str = ""  
  
  if str ~= nil and str ~= "" then
    k_pos = string.find( str, "[Kk]" )
    o_pos = string.find( str, "+" )
    
    if o_pos ~= nil then 
      oprava = tonumber( string.sub( str, o_pos + 1 ) )
    end
   
    -- parsovani prikazu kostky 
    if k_pos == nil then  
      n = 1      
      k_pos = 0  
    elseif k_pos < 2 then 
      n = 1 
    else    
      n = tonumber( string.sub( str, 0, k_pos - 1 ) )
    end
    if o_pos ~= nil then o_pos = o_pos - 1 end
    m = tonumber( string.sub( str, k_pos + 1, o_pos ) ) 
    
    -- hod kostkou  
    h = kostka( n, m ) 
    
    -- vypocet vysledne hodnoty
    local num       = #h
    local sep       = ""
    if num > 0 then
      for i = 1, num do 
        vysledek = vysledek + h[ i ] 
        if verbrose == true then
          if i > 1 then sep = ", " end
          hlist_str = hlist_str .. sep .. h[ i ] 
        end
      end
      if verbrose == true then hlist_str = "(" .. hlist_str .. " + " .. oprava .. ")" end     
    end  
  else
    vysledek = nil
  end  
  
  return vysledek + oprava, hlist_str  
end


print( "=== RPG Kostka v 1.1 ===" )
local cont = "yes" 
local inp = nil

while cont == "yes" do
  inp = input( "Hod kostkou: " )
  if inp ~= nil and inp ~= "" then  
    local hod, list = nkm( inp )
    if hod == nil then hod = "Zruseno" end
    print( inp .. " = " .. hod, list ) 
  end 
  
  cont = message( "Prejte si hazet znovu?", "Kostka", mbox_buttons.yes_no, mbox_type.question )  
end  

print( "\n=== END ===========" )
print( )
