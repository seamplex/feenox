#!/bin/sh
# take the output of this script as a kate syntax definition file for FeenoX
#
# mkdir -p $HOME/.local/share/katepart5/syntax/
# ./syntax-kate.sh > $HOME/.local/share/katepart5/syntax/feenox.xml
#

. ./keywords.sh

cat << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!-- copy this file to \$HOME/.local/share/katepart5/syntax/ to get Kate Syntax Highlighting for FeenoX input files -->
<language name="FeenoX" version="1" kateversion="5.54.0" section="Scientific" extensions="*.fee" author="jeremy theler" license="GPL">
EOF

# primary keywords
echo "  <highlighting>"
echo "    <list name=\"prim_keywords\">"
for kw in ${UPPER}; do
  echo "      <item>${kw}</item>"
done
echo "    </list>"

# secondary keywords (TO-DO)
echo "    <list name=\"sec_keywords\">"
echo "    </list>"

# special variables
echo "    <list name=\"internals\">"
for kw in ${VARS}; do
  echo "      <item>${kw}</item>"
#   echo "      <item>${kw}_0</item>"
#   echo "      <item>${kw}_init</item>"
done
echo "    </list>"

# functions
echo "    <list name=\"functions\">"
for kw in ${FUNCS}; do
  echo "      <item>$kw</item>"
done
echo "    </list>"


# contexts
cat << EOF
    <contexts>
      <context attribute="Normal Text" lineEndContext="#stay" name="Normal">

<!-- do not detect strings (for now) -->
<!--         <DetectChar attribute="String" context="String" char="&quot;"/> -->
<!--         <DetectChar attribute="Normal Text" context="#stay"  char="&quot;"/> -->

        <keyword attribute="Keyword1" context="#stay" String="prim_keywords"/>
        <keyword attribute="Keyword2" context="#stay" String="sec_keywords"/>
        <keyword attribute="Identifier" context="#stay" String="internals"/>
        <keyword attribute="Function" context="#stay" String="functions"/>

        <DetectChar char="{" attribute="Operator" context="#stay" beginRegion="block" />
        <DetectChar char="}" attribute="Operator" context="#stay" endRegion="block" />

        <DetectChar char="=" attribute="Operator" context="#stay" />
        <DetectChar char="!" attribute="Operator" context="#stay" />
        <DetectChar char="+" attribute="Operator" context="#stay" />
        <DetectChar char="-" attribute="Operator" context="#stay" />
        <DetectChar char="/" attribute="Operator" context="#stay" />
        <DetectChar char="*" attribute="Operator" context="#stay" />
        <DetectChar char="^" attribute="Operator" context="#stay" />
<!--        <DetectChar char="<" attribute="Operator" context="#stay" />-->
<!--        <DetectChar char=">" attribute="Operator" context="#stay" />-->
        <DetectChar char="^" attribute="Operator" context="#stay" />
        <DetectChar char="(" attribute="Operator" context="#stay" />
        <DetectChar char=")" attribute="Operator" context="#stay" />

        <Float attribute="Types" context="#stay"/>
        <Int attribute="Types" context="#stay"/>
        <Detect2Chars char="\\" char1="#" attribute="Normal Text" context="#stay" />
        <DetectChar attribute="Comment" context="Comment" char="#"/>
      </context>
    
      <context name="Comment" attribute="Comment" lineEndContext="#pop"/>

      <context name="String" attribute="String" lineEndContext="#stay">
        <DetectChar char="&quot;" attribute="String" context="#pop"/>
      </context>

    </contexts>

    <itemDatas>
      <itemData name="Normal Text" defStyleNum="dsNormal"/>
      <itemData name="Keyword1" defStyleNum="dsKeyword"/>
      <itemData name="Keyword2" defStyleNum="dsKeyword" color="#003300"/>
      <itemData name="Operator" defStyleNum="dsOperator" color="#666666"/>
      <itemData name="Identifier" defStyleNum="dsOthers" color="#663333" italic="1"/>
      <itemData name="Function" defStyleNum="dsFunction" color="#006600" bold="1"/>
      <itemData name="Types" defStyleNum="dsDataType" color="#3333CC"/>
      <itemData name="String" defStyleNum="dsString" color="#666699"/>
      <itemData name="Comment" defStyleNum="dsComment"/>
    </itemDatas>
  </highlighting>

  <general>
    <comments>
      <comment name="singleLine" start="#" />
    </comments>
    <keywords casesensitive="1" />
  </general>
</language>
EOF
