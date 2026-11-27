function Link(link)
  if FORMAT ~= "gfm" then
    return
  end

  local path, suffix = link.target:match("^([^?#]*)(.*)$")
  if path:match("^%./") or path:match("^https://github%.com/seamplex/feenox/blob/main/") then
    link.target = path:gsub("%.md$", ".markdown") .. suffix
    return link
  end
end
