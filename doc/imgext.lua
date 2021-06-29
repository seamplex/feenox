function Image(el)
  el.src = string.gsub(el.src, ".svg", ".pdf")
  return el
end
