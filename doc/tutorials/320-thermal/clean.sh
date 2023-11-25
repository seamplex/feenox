cat .gitignore | sed '/^#.*/ d' | sed '/^\s*$/ d' | sed 's/^/rm -rf /' | grep -v mp4 | bash
