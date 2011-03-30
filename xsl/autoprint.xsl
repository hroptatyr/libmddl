<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:mddl="http://www.mddl.org/mddl/3.0-beta"
  xmlns:xlink="http://www.w3.org/1999/xlink"
  version="1.0">

  <xsl:output method="text"/>
  <xsl:param name="hdr"/>

  <xsl:include href="autospec-common.xsl"/>

  <xsl:template match="spec">
    <xsl:text>/* AUTO-GENERATED, DO NOT MODIFY */&#0010;&#0010;</xsl:text>

    <!-- decl -->
    <xsl:apply-templates mode="decl"/>

    <!-- def -->
    <xsl:apply-templates mode="def"/>
  </xsl:template>

  <xsl:template match="struct" mode="decl">
    <xsl:text>static void print_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(FILE *out, mddl_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>_t p, size_t indent)</xsl:text>
    <xsl:text>;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="slot" mode="decl">
    <xsl:text>static void print_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(FILE *out, </xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> p, size_t indent)</xsl:text>
    <xsl:text>;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct" mode="def">
    <xsl:text>static void&#0010;print_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(FILE *out, mddl_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>_t p, size_t indent)</xsl:text>
    <xsl:text>&#0010;{&#0010;</xsl:text>

    <xsl:text>&#0009;print_indent(out, indent);&#0010;</xsl:text>
    <xsl:text>&#0009;fputs("&lt;</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>", out);&#0010;</xsl:text>

    <xsl:apply-templates select="slot[@attr]" mode="attr"/>

    <xsl:text>&#0009;fputs("&gt;\n", out);&#0010;</xsl:text>

    <!-- all the rest -->
    <xsl:apply-templates/>

    <xsl:text>&#0009;print_indent(out, indent);&#0010;</xsl:text>
    <xsl:text>&#0009;fputs("&lt;/</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>&gt;\n", out);&#0010;</xsl:text>
    <xsl:text>}&#0010;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="slot[@attr]" mode="attr">
    <xsl:if test="@default">
      
    </xsl:if>
    <xsl:text>&#0009;print_attr_</xsl:text>
    <xsl:value-of select="@class"/>
    <xsl:text>(out, "</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>", p-&gt;</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>, </xsl:text>
    <xsl:choose>
      <xsl:when test="@default">
        <xsl:text>"</xsl:text>
        <xsl:value-of select="@default"/>
        <xsl:text>"</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>NULL</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>, 0);&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="slot[not(@attr)]">
    <xsl:text>&#0009;print_</xsl:text>
    <xsl:value-of select="@class"/>
    <xsl:text>(out, p-&gt;</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>, indent + 2</xsl:text>
    <xsl:text>);&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult = '*']">
    <!-- simple type first, check if print is needed -->
    <xsl:text>&#0009;for (size_t i = 0; i &lt; p-&gt;n</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>; i++) {&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;print_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(out, p-&gt;</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text> + i, indent + 2);&#0010;</xsl:text>

    <xsl:text>&#0009;}&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult != '*']">
    <xsl:text>&#0009;if (!__</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>_null_p(p-&gt;</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>)) {&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;print_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(out, p-&gt;</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>, indent + 2);&#0010;</xsl:text>

    <xsl:text>&#0009;}&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="decl"/>
  <xsl:template match="text()" mode="def"/>
  <xsl:template match="text()" mode="attr"/>
  <xsl:template match="*"/>
  <xsl:template match="*" mode="decl"/>
  <xsl:template match="*" mode="def"/>
  <xsl:template match="*" mode="attr"/>

</xsl:stylesheet>
