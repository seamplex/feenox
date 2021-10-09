function Div(element)
  if element.classes:includes('in-format') and
    element.classes:find_if(function (x) return FORMAT:match(x) end) then
      return element
  else
      return nil
  end
end
