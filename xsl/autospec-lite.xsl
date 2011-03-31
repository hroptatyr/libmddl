<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:mddl="http://www.mddl.org/mddl/3.0-beta"
  xmlns:xlink="http://www.w3.org/1999/xlink"
  version="1.0">

  <xsl:output method="text"/>

  <xsl:include href="autospec-common.xsl"/>

  <xsl:template match="spec">
    <xsl:text>/* AUTO-GENERATED, DO NOT MODIFY */&#0010;&#0010;</xsl:text>
    <xsl:text>#if !defined mddl_lite_h_&#0010;</xsl:text>
    <xsl:text>#define mddl_lite_h_&#0010;&#0010;</xsl:text>

    <xsl:apply-templates mode="tdef"/>
    <xsl:text>&#0010;</xsl:text>

    <xsl:apply-templates mode="udef"/>
    <xsl:text>&#0010;</xsl:text>

    <xsl:apply-templates mode="sdef"/>
    <xsl:text>&#0010;</xsl:text>

    <xsl:text>#endif  /* !mddl_lite_h_*/&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct" mode="tdef">
    <xsl:text>typedef </xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> *mddl_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>_t;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="union" mode="udef">
    <xsl:text>typedef </xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> {&#0010;</xsl:text>

    <xsl:apply-templates/>

    <xsl:text>} mddl_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>_t;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct" mode="sdef">
    <xsl:value-of select="@type"/>
    <xsl:text> {&#0010;</xsl:text>

    <xsl:apply-templates/>

    <xsl:text>};&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult='*']">
    <xsl:text>&#0009;size_t n</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>;&#0010;</xsl:text>
    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> *</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct">
    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>[</xsl:text>
    <xsl:value-of select="@mult"/>
    <xsl:text>]</xsl:text>
    <xsl:text>;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="slot">
    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>;&#0010;</xsl:text>
  </xsl:template>

  <!-- catch all -->
  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="udef"/>
  <xsl:template match="text()" mode="tdef"/>
  <xsl:template match="text()" mode="sdef"/>

</xsl:stylesheet>
