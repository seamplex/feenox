-- Creates an image identical to the one given
function Image (el)
  if      FORMAT == "latex" and el.attributes.width_latex ~= nil then
    el.attributes.width = el.attributes.width_latex
  elseif FORMAT == "html"  and el.attributes.width_html ~= nil then
    el.attributes.width = el.attributes.width_html
  elseif FORMAT == "texinfo"  and el.attributes.width_texinfo ~= nil then
    el.attributes.width = el.attributes.width_texinfo
  end
  
  return el
end
