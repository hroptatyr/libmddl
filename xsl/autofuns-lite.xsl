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
    <xsl:choose>
      <xsl:when test="$hdr">
        <xsl:apply-templates mode="decl"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates mode="def"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="spec/struct" mode="decl">
    <xsl:apply-templates mode="decl">
      <xsl:with-param name="super" select="@slot"/>
    </xsl:apply-templates>

    <xsl:text>/* dtor */&#0010;</xsl:text>
    <xsl:text>DECLF void mddl_free_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(</xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text>*);</xsl:text>
    <xsl:text>&#0010;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult='*']" mode="decl">
    <xsl:param name="super"/>
    <xsl:variable name="retty">
      <xsl:text>mddl_</xsl:text>
      <xsl:value-of select="@slot"/>
      <xsl:text>_t</xsl:text>
    </xsl:variable>

    <xsl:text>DECLF </xsl:text>
    <xsl:value-of select="$retty"/>
    <xsl:text> mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_add_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_t to);&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult!='*']" mode="decl">
    <xsl:param name="super"/>
    <xsl:variable name="retty">
      <xsl:text>void</xsl:text>
    </xsl:variable>

    <xsl:text>DECLF </xsl:text>
    <xsl:value-of select="$retty"/>
    <xsl:text> mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_set_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_t to, </xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> from</xsl:text>
    <xsl:if test="@primary = 'no' or not(@primary)">
      <xsl:text>[</xsl:text>
      <xsl:value-of select="@mult"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
    <xsl:text>);&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="slot" mode="decl">
    <xsl:param name="super"/>
    <xsl:variable name="retty">
      <xsl:text>void</xsl:text>
    </xsl:variable>

    <xsl:text>DECLF </xsl:text>
    <xsl:value-of select="$retty"/>
    <xsl:text> mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_set_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_t to, </xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> from);&#0010;</xsl:text>
  </xsl:template>


  <xsl:template match="spec/struct" mode="def">
    <xsl:apply-templates mode="def">
      <xsl:with-param name="super" select="@slot"/>
    </xsl:apply-templates>

    <xsl:text>&#0010;/* dtor */&#0010;</xsl:text>
    <xsl:text>DEFUN void&#0010;mddl_free_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(</xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> *victim)&#0010;</xsl:text>
    <xsl:text>{&#0010;</xsl:text>
    <xsl:apply-templates mode="free">
      <xsl:with-param name="super" select="@slot"/>
    </xsl:apply-templates>
    <xsl:text>&#0009;return;&#0010;</xsl:text>
    <xsl:text>}&#0010;&#0010;</xsl:text>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult='*']" mode="def">
    <xsl:param name="super"/>
    <xsl:variable name="retty">
      <xsl:text>mddl_</xsl:text>
      <xsl:value-of select="@slot"/>
      <xsl:text>_t</xsl:text>
    </xsl:variable>

    <xsl:text>DEFUN </xsl:text>
    <xsl:value-of select="$retty"/>
    <xsl:text>&#0010;mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_add_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_t to)&#0010;</xsl:text>
    <xsl:text>{&#0010;</xsl:text>
    <xsl:text>&#0009;ADDF(</xsl:text>	
    <xsl:value-of select="@slot"/>
    <xsl:text>);&#0010;</xsl:text>
    <xsl:text>}&#0010;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult!='*']" mode="def">
    <xsl:param name="super"/>
    <xsl:variable name="retty">
      <xsl:text>void</xsl:text>
    </xsl:variable>

    <xsl:text>DEFUN </xsl:text>
    <xsl:value-of select="$retty"/>
    <xsl:text>&#0010;mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_set_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_t to, </xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> from</xsl:text>
    <xsl:if test="@primary = 'no' or not(@primary)">
      <xsl:text>[</xsl:text>
      <xsl:value-of select="@mult"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
    <xsl:text>)&#0010;</xsl:text>
    <xsl:text>{&#0010;</xsl:text>
    <xsl:text>&#0009;SET_</xsl:text>
    <xsl:choose>
      <xsl:when test="starts-with(@type, 'struct ')">
        <xsl:text>struct</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="@type"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>_F(</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>);&#0010;</xsl:text>
    <xsl:text>}&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="slot" mode="def">
    <xsl:param name="super"/>
    <xsl:variable name="retty">
      <xsl:text>void</xsl:text>
    </xsl:variable>

    <xsl:text>DEFUN </xsl:text>
    <xsl:value-of select="$retty"/>
    <xsl:text>&#0010;mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_set_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_t to, </xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> from)&#0010;</xsl:text>
    <xsl:text>{&#0010;</xsl:text>
    <xsl:text>&#0009;SET_</xsl:text>
    <xsl:choose>
      <xsl:when test="starts-with(@type, 'struct ')">
        <xsl:text>struct</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="@type"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>_F(</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>);&#0010;</xsl:text>
    <xsl:text>}&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult='*']" mode="free">
    <xsl:param name="super"/>
    <xsl:text>&#0009;if (victim-&gt;</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>) {&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;mddl_free_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(victim-&gt;</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>);&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;free(victim-&gt;</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>);&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;victim-&gt;n</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text> = 0;&#0010;</xsl:text>
    <xsl:text>&#0009;}&#0010;</xsl:text>
  </xsl:template>

  <!-- catch all -->
  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="decl"/>
  <xsl:template match="text()" mode="def"/>
  <xsl:template match="text()" mode="free"/>
  <xsl:template match="*"/>
  <xsl:template match="*" mode="decl"/>
  <xsl:template match="*" mode="def"/>
  <xsl:template match="*" mode="free"/>

</xsl:stylesheet>
