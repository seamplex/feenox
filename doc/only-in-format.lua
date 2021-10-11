function Div(element)
  if element.classes:includes('only-in-format') then
    if element.classes:find_if(function (x) return FORMAT:match(x) end) then
      return nil
    else
      return {}
    end
  else
    return nil    
  end
end
